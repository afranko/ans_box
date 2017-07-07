package eu.mantis.mqtt_mimosa;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import eu.mantis.mqtt_mimosa.mimosa_messages.NumData;
import eu.mantis.mqtt_mimosa.mimosa_messages.TimeData;
import eu.mantis.mqtt_mimosa.mqtt_messages.EnvironmentMeas;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValueContainer;
import eu.mantis.mqtt_mimosa.mqtt_messages.MovementEvent;

public class Main implements MqttCallback {

	MqttClient client;
	public static final String TIME_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/time_data";
	public static final String NUM_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/num_data";

	public Main() {
	}

	public static void main(String[] args) {
		new Main().startListening();
	}

	public void startListening() {
		try {
			client = new MqttClient("tcp://mantis1.tmit.bme.hu:80", "MIMOSA_interface");
			client.connect();
			client.setCallback(this);
			client.subscribe("/environment");
			client.subscribe("/movement");
			//client.subscribe("/warning");
			//client.subscribe("#"); subscribe to all topics, and filter with switch/case in messageArrived()

			// message can be sent here to a topic
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void connectionLost(Throwable arg0) {
		System.out.println("Connection lost: " + arg0.getMessage());
		try {
			client.connect();
			client.setCallback(this);
			client.subscribe("/environment");
			client.subscribe("/movement");
			//client.subscribe("/warning");
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken arg0) {
	}

	@Override
	public void messageArrived(String topic, MqttMessage message) throws Exception {
		String payload = new String(message.getPayload());
		
		//System.out.println("-------------------------------------------------");
		System.out.println("| New message arrived to topic: " + topic);
		//System.out.println("| Message: " + payload);
		//System.out.println("-------------------------------------------------");
		
		if(topic.equals("/movement")){
			sendMovementToMimosa(payload);
		}
		else{
			sendEnviromentMeasToMimosa(payload);
		}
	}
	
	public void sendMovementToMimosa(String payload){
		MovementEvent event = Utility.fromJson(payload, MovementEvent.class);
		
		String measLocSite = event.getHasLocation().getMeasLocId().getValue();
		
		List<String> movementValues = new ArrayList<>();
		try {
			movementValues = event.getHasAttributes().get(0)
					.getHasValueContainers().get(0).getHasValue().getValues();
		} catch (Exception e) {
			e.printStackTrace();
		}
		float[] ch1 = new float[movementValues.size()], ch2 = new float[movementValues.size()], ch3 = new float[movementValues.size()], ch4 = new float[movementValues.size()];
		int numberOfSamples = 0;
		for(String sensorValues : movementValues){
			String[] splittedValues = sensorValues.split("-");
			ch1[numberOfSamples] = Float.valueOf(splittedValues[0]);
			ch2[numberOfSamples] = Float.valueOf(splittedValues[1]);
			ch3[numberOfSamples] = Float.valueOf(splittedValues[2]);
			ch4[numberOfSamples] = Float.valueOf(splittedValues[3]);
			numberOfSamples++;
		}
		String waveformData1 = Utility.encodeFloatArray(ch1);
		String waveformData2 = Utility.encodeFloatArray(ch2);
		String waveformData3 = Utility.encodeFloatArray(ch3);
		String waveformData4 = Utility.encodeFloatArray(ch4);

		float measDuration = 0;
		try {
			measDuration = event.getHasAttributes().get(0).getHasValueContainers()
					.get(1).getHasValue().getValue();
			//converting to seconds
			measDuration = measDuration / 1000;
		} catch (Exception e) {
			e.printStackTrace();
		}

		String timeStamp = Utility.fixDateFormat(event.getHasTimestamp().getHasUTCDateTime().getValue());
		
		TimeData timeData1 = new TimeData(measLocSite, 0, timeStamp, 0, "0000000000000000",
				0, 1, "S", 0, measDuration, (float) numberOfSamples, "0000000000000000", 0, 0, "N", waveformData1);
		TimeData timeData2 = new TimeData(measLocSite, 0, timeStamp, 1, "0000000000000000",
				0, 1, "S", 0, measDuration, (float) numberOfSamples, "0000000000000000", 0, 0, "N", waveformData2);
		TimeData timeData3 = new TimeData(measLocSite, 0, timeStamp, 2, "0000000000000000",
				0, 1, "S", 0, measDuration, (float) numberOfSamples, "0000000000000000", 0, 0, "N", waveformData3);
		TimeData timeData4 = new TimeData(measLocSite, 0, timeStamp, 3, "0000000000000000",
				0, 1, "S", 0, measDuration, (float) numberOfSamples, "0000000000000000", 0, 0, "N", waveformData4);
		
		try {
			Utility.sendPostRequest(TIME_DATA_URL, timeData1);
			Utility.sendPostRequest(TIME_DATA_URL, timeData2);
			Utility.sendPostRequest(TIME_DATA_URL, timeData3);
			Utility.sendPostRequest(TIME_DATA_URL, timeData4);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void sendEnviromentMeasToMimosa(String payload){
		EnvironmentMeas measurement = Utility.fromJson(payload, EnvironmentMeas.class);
		
		String measLocSite = measurement.getHasLocation().getMeasLocId().getValue();
		
		String timeStamp = Utility.fixDateFormat(measurement.getHasTimestamp().getHasUTCDateTime().getValue());
		
		List<HasValueContainer> hasValueContainers = new ArrayList<>();
		try {
			hasValueContainers = measurement.getHasAttributes().get(0).getHasValueContainers();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		for(HasValueContainer valueContainer : hasValueContainers){
			NumData numData = new NumData(measLocSite, 0, timeStamp, "0000000000000000", 5, 
					Integer.valueOf(valueContainer.getUuid().getValue()),(float) valueContainer.getHasValue().getValue());
			try {
				Utility.sendPostRequest(NUM_DATA_URL,  numData);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		
	}
	
}
