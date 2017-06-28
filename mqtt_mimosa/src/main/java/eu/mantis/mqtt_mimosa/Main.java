package eu.mantis.mqtt_mimosa;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.internal.websocket.Base64;

import eu.mantis.mqtt_mimosa.mimosa_messages.TimeData;
import eu.mantis.mqtt_mimosa.mqtt_messages.SimpleCEPEvent;

public class Main implements MqttCallback {

	MqttClient client;
	public static final String TIME_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/time_data";

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
			client.subscribe("/movement");
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
			client.subscribe("/movement");
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken arg0) {
		// TODO Auto-generated method stub
	}

	@Override
	public void messageArrived(String topic, MqttMessage message) throws Exception {
		System.out.println("-------------------------------------------------");
		System.out.println("| Topic:" + topic);
		System.out.println("| Message: " + new String(message.getPayload()));
		System.out.println("-------------------------------------------------");
		
		sendCEPEventToMimosa(new String(message.getPayload()));
	}
	
	public void sendCEPEventToMimosa(String payload){
		SimpleCEPEvent event = Utility.fromJson(payload, SimpleCEPEvent.class);
		
		List<String> movementValues = new ArrayList<>();
		try {
			movementValues = event.getHasAttributes().get(0)
					.getHasValueContainers().get(0).getHasValue().getValues();
		} catch (Exception e) {
			e.printStackTrace();
		}
		StringBuilder sb = new StringBuilder();
		float numberOfSamples = 0;
		for(String sensorValues : movementValues){
			String[] splittedValues = sensorValues.split("-");
			for(int i = 0; i < splittedValues.length; i++){
				sb.append(splittedValues[i]);
				numberOfSamples++;
			}
		}
		String waveformData = Base64.encode(sb.toString());
		
		float measDuration = 0;
		try {
			measDuration = event.getHasAttributes().get(0).getHasValueContainers()
					.get(1).getHasValue().getValue();
			//converting to seconds
			measDuration = measDuration / 1000;
		} catch (Exception e) {
			e.printStackTrace();
		}
		String timeStamp = event.getHasTimestamp().getHasUTCDateTime().getValue();
		
		TimeData timeData = new TimeData("0000000000000000", 0, timeStamp, 0, "0000000000000000",
				0, 1, "S", 0, measDuration, numberOfSamples, "0000000000000000", 0, 0, "N", waveformData);
		
		try {
			Utility.sendPostRequest(TIME_DATA_URL, timeData);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
