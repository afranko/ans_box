package eu.mantis.mqtt_mimosa;

import com.google.gson.JsonSyntaxException;
import eu.mantis.mqtt_mimosa.mimosa_messages.EventCharData;
import eu.mantis.mqtt_mimosa.mimosa_messages.NumData;
import eu.mantis.mqtt_mimosa.mimosa_messages.TimeData;
import eu.mantis.mqtt_mimosa.mqtt_messages.BoxCommand;
import eu.mantis.mqtt_mimosa.mqtt_messages.BoxWarning;
import eu.mantis.mqtt_mimosa.mqtt_messages.EnvironmentMeas;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValueContainer;
import eu.mantis.mqtt_mimosa.mqtt_messages.MovementEvent;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public final class MyMqttClient implements MqttCallback {

  private static final String MQTT_BROKER_URL = MimosaMain.getProp().getProperty("mqtt_broker_url");

  //MIMOSA REST interface URLs where the data should be posted
  private static final String TIME_DATA_URL = MimosaMain.getProp().getProperty("time_data_url");
  private static final String NUM_DATA_URL = MimosaMain.getProp().getProperty("num_data_url");
  private static final String WARNING_URL = MimosaMain.getProp().getProperty("warning_url");

  private static boolean WAITING_FOR_LAST_POSITION;
  private static String LAST_POSITION_TIMESTAMP;

  private static MqttClient client;

  {
    try {
      String clientId = MqttAsyncClient.generateClientId();
      System.out.println("Generated unique client ID for broker connection: " + clientId);
      client = new MqttClient(MQTT_BROKER_URL, clientId);
      client.connect();
      client.setCallback(this);
      client.subscribe("#");
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker. Caused by: " + e.getMessage());
    }
  }

  @Override
  public void connectionLost(Throwable throwable) {
    System.out.println("Connection lost with cause: " + throwable.getMessage());
    try {
      client.connect();
      client.setCallback(this);
      client.subscribe("#");
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not reconnect to the MQTT broker at: " + MQTT_BROKER_URL);
    }
  }

  @Override
  public void messageArrived(String topic, MqttMessage message) {
    String payload = new String(message.getPayload());

    System.out.println("-------------------------------------------------");
    System.out.println("New message arrived to topic: " + topic);
    System.out.println("Message: " + payload);
    System.out.println("-------------------------------------------------");

    switch (topic) {
      case "/environment":
        sendEnviromentMeasToMimosa(payload, false);
        break;
      case "/last":
        //Value: 0.0 - LOW position, 1.0 - HIGH position
        sendEnviromentMeasToMimosa(payload, true);
        break;
      case "/movement":
        sendMovementToMimosa(payload);
        break;
      case "/warning":
        sendWarningEventToMimosa(payload);
        break;
      default:
        break;
    }
  }

  @Override
  public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
  }

  private void sendEnviromentMeasToMimosa(String payload, boolean lastPos) {
    EnvironmentMeas measurement;
    try {
      measurement = Utility.fromJson(payload, EnvironmentMeas.class);
    } catch (JsonSyntaxException e) {
      System.out.println("JSON parser FAILED to parse Environment measurement. Caused by: " + e.getMessage());
      return;
    }

    String measLocSite = measurement.getHasLocation().getMeasLocId().getValue();

    String timestamp = measurement.getHasTimestamp().getHasUTCDateTime().getValue();
    if (lastPos && LAST_POSITION_TIMESTAMP != null && timestamp.equals(LAST_POSITION_TIMESTAMP)) {
      WAITING_FOR_LAST_POSITION = false;
    }
    timestamp = Utility.fixDateFormat(timestamp);

    List<HasValueContainer> hasValueContainers = new ArrayList<>();
    try {
      hasValueContainers = measurement.getHasAttributes().get(0).getHasValueContainers();
    } catch (Exception e) {
      e.printStackTrace();
    }

    for (HasValueContainer valueContainer : hasValueContainers) {
      NumData numData = new NumData(measLocSite, 0, timestamp, "0000000000000000", 5, Integer.valueOf(valueContainer.getUuid().getValue()),
                                    valueContainer.getHasValue().getValue());

      Utility.sendRequest(NUM_DATA_URL, "POST", numData);
    }
  }

  private void sendMovementToMimosa(String payload) {
    MovementEvent event;
    try {
      event = Utility.fromJson(payload, MovementEvent.class);
    } catch (JsonSyntaxException e) {
      System.out.println("JSON parser FAILED to parse Movement event. Caused by: " + e.getMessage());
      return;
    }

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

    String timestamp = Utility.fixDateFormat(event.getHasTimestamp().getHasUTCDateTime().getValue());

    TimeData timeData1 = new TimeData(measLocSite, 0, timestamp, 0, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData1);
    TimeData timeData2 = new TimeData(measLocSite, 0, timestamp, 1, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData2);
    TimeData timeData3 = new TimeData(measLocSite, 0, timestamp, 2, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData3);
    TimeData timeData4 = new TimeData(measLocSite, 0, timestamp, 3, "0000000000000000", 0, 1, "S", 0, measDuration, (float) numberOfSamples,
                                      "0000000000000000", 0, 0, "N", waveformData4);

    Utility.sendRequest(TIME_DATA_URL, "POST", timeData1);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData2);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData3);
    Utility.sendRequest(TIME_DATA_URL, "POST", timeData4);
  }

  private void sendWarningEventToMimosa(String payload) {
    BoxWarning warning;
    try {
      warning = Utility.fromJson(payload, BoxWarning.class);
    } catch (JsonSyntaxException e) {
      System.out.println("JSON parser FAILED to parse warning message coming from the box. Caused by: " + e.getMessage());
      return;
    }

    String timestamp = Utility.fixDateFormat(warning.getTimestamp());
    EventCharData charData = new EventCharData(warning.getBoxId(), 0, "0000000000000000", 1, 12, "2018-03-12 13:31:42.000", "0000000000000000", 5, 0,
                                               "0000000000000000", 0, 0, warning.getMessage(), timestamp);
    Utility.sendRequest(WARNING_URL, "POST", charData);
  }

  public int sendTimestampToBroker(String topic) {
    if (topic == null) {
      topic = "/ANS_MEAS_BOX_001";
    }

    DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy.MM.dd.HH:mm:ss");
    LAST_POSITION_TIMESTAMP = LocalDateTime.now().format(formatter);
    String payload = Utility.toJson(new BoxCommand(LAST_POSITION_TIMESTAMP));

    try {
      client.publish(topic, payload.getBytes(), 0, true);
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not publish the message to the /command topic!");
      return 500;
    }

    int seconds = 0;
    WAITING_FOR_LAST_POSITION = true;
    while (WAITING_FOR_LAST_POSITION) {
      //50 seconds passed...
      if (seconds > 49) {
        break;
      }
      try {
        Thread.sleep(1000);
      } catch (InterruptedException e) {
        e.printStackTrace();
        System.out.println("Waiting for last position message failed with Thread:InterruptedException");
      }
      seconds++;
    }
    if (WAITING_FOR_LAST_POSITION) {
      return 500;
    } else {
      return 200;
    }
  }

}
