package eu.mantis.mqtt_mimosa;

import java.util.ArrayList;
import java.util.List;

import java.util.concurrent.TimeUnit;
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
import org.eclipse.paho.client.mqttv3.util.Debug;

class Main implements MqttCallback {

  private MqttClient client;
  private Debug debug;
  private static final String TIME_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/time_data";
  private static final String NUM_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/num_data";
  private static final String MQTT_BROKER_URL = "tcp://mantis1.tmit.bme.hu:1883";

  private Main() {
  }

  public static void main(String[] args) {
    new Main().startListening();
  }

  private void startListening() {
    try {
      client = new MqttClient(MQTT_BROKER_URL, "MIMOSA_interface");
      client.connect();
      client.setCallback(this);
      client.subscribe("/environment");
      client.subscribe("/movement");
      //client.subscribe("/warning");
      //client.subscribe("#"); subscribe to all topics, and filter with switch/case in messageArrived()
      debug = client.getDebug();
      // message can be sent here to a topic
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker.");
    }
  }

  @Override
  public void connectionLost(Throwable arg0) {
    debug.dumpBaseDebug();
    debug.dumpClientDebug();
    debug.dumpSystemProperties();
    System.out.println("Connection lost: " + arg0.getMessage());
    try {
      client.connect();
      client.setCallback(this);
      client.subscribe("/environment");
      client.subscribe("/movement");
      //client.subscribe("/warning");
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker.");
    }
  }

  @Override
  public void deliveryComplete(IMqttDeliveryToken arg0) {
  }

  @Override
  public void messageArrived(String topic, MqttMessage message) throws Exception {
    String payload = new String(message.getPayload());

    System.out.println("-------------------------------------------------");
    System.out.println("New message arrived to topic: " + topic);
    System.out.println("Message: " + payload);
    System.out.println("-------------------------------------------------");

    if (topic.equals("/movement")) {
      sendMovementToMimosa(payload);
    } else {
      sendEnviromentMeasToMimosa(payload);
    }
  }

  private void sendMovementToMimosa(String payload) {
    MovementEvent event = Utility.fromJson(payload, MovementEvent.class);

    String measLocSite = event.getHasLocation().getMeasLocId().getValue();

    List<String> movementValues = new ArrayList<>();
    try {
      movementValues = event.getHasAttributes().get(0).getHasValueContainers().get(0).getHasValue().getValues();
    } catch (Exception e) {
      e.printStackTrace();
    }
    float[] ch1 = new float[movementValues.size()], ch2 = new float[movementValues.size()], ch3 = new float[movementValues
        .size()], ch4 = new float[movementValues.size()];
    int numberOfSamples = 0;
    for (String sensorValues : movementValues) {
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
      measDuration = event.getHasAttributes().get(0).getHasValueContainers().get(1).getHasValue().getValue();
      //converting to seconds
      measDuration = measDuration / 1000;
    } catch (Exception e) {
      e.printStackTrace();
    }

    String timeStamp = Utility.fixDateFormat(event.getHasTimestamp().getHasUTCDateTime().getValue());

    TimeData timeData1 = new TimeData(measLocSite, 0, timeStamp, 0, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData1);
    TimeData timeData2 = new TimeData(measLocSite, 0, timeStamp, 1, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData2);
    TimeData timeData3 = new TimeData(measLocSite, 0, timeStamp, 2, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData3);
    TimeData timeData4 = new TimeData(measLocSite, 0, timeStamp, 3, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData4);

    Utility.sendRequest(TIME_DATA_URL, "POST", timeData1);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData2);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData3);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData4);
    /*try {
      Utility.sendPostRequest(TIME_DATA_URL, timeData1);
      Utility.sendPostRequest(TIME_DATA_URL, timeData2);
      Utility.sendPostRequest(TIME_DATA_URL, timeData3);
      Utility.sendPostRequest(TIME_DATA_URL, timeData4);
    } catch (Exception e) {
      e.printStackTrace();
    }*/
  }

  private void sendEnviromentMeasToMimosa(String payload) {
    EnvironmentMeas measurement = Utility.fromJson(payload, EnvironmentMeas.class);

    String measLocSite = measurement.getHasLocation().getMeasLocId().getValue();

    String timeStamp = Utility.fixDateFormat(measurement.getHasTimestamp().getHasUTCDateTime().getValue());

    List<HasValueContainer> hasValueContainers = new ArrayList<>();
    try {
      hasValueContainers = measurement.getHasAttributes().get(0).getHasValueContainers();
    } catch (Exception e) {
      e.printStackTrace();
    }

    for (HasValueContainer valueContainer : hasValueContainers) {
      NumData numData = new NumData(measLocSite, 0, timeStamp, "0000000000000000", 5, Integer.valueOf(valueContainer.getUuid().getValue()),
                                    valueContainer.getHasValue().getValue());

      Utility.sendRequest(NUM_DATA_URL, "POST", numData);
      /*try {
        Utility.sendPostRequest(NUM_DATA_URL, numData);
      } catch (Exception e) {
        e.printStackTrace();
      }*/
    }
  }

}
