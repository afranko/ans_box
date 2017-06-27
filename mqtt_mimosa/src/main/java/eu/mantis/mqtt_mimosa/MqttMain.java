package eu.mantis.mqtt_mimosa;

import java.util.List;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import com.google.gson.Gson;

import eu.mantis.mqtt_mimosa.mqtt_messages.HasAttribute;
import eu.mantis.mqtt_mimosa.mqtt_messages.SimpleCEPEvent;

public class MqttMain implements MqttCallback {

	MqttClient client;

	public MqttMain() {
	}

	public static void main(String[] args) {
		new MqttMain().startListening();
	}

	public void startListening() {
		try {
			client = new MqttClient("tcp://mantis1.tmit.bme.hu:80", "MIMOSA_interface");
			client.connect();
			client.setCallback(this);
			client.subscribe("/movement");
			//client.subscribe("#"); és messageArived-ban ifekkel vagy switch-el filterelni

			// message can be sent here to the topic
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
		//TODO conversion of the object to the proper MIMOSA payload
		
		//Utility.sendRequest(URI, "POST", payload);
	}

}
