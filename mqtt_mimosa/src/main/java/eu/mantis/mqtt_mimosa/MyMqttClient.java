package eu.mantis.mqtt_mimosa;

import com.google.gson.JsonSyntaxException;
import eu.mantis.mqtt_mimosa.mimosa_messages.NumData;
import eu.mantis.mqtt_mimosa.mimosa_messages.TimeData;
import eu.mantis.mqtt_mimosa.mqtt_messages.BoxCommand;
import eu.mantis.mqtt_mimosa.mqtt_messages.BoxId;
import eu.mantis.mqtt_mimosa.mqtt_messages.EnvironmentMeas;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasAttribute;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasEventDescriptionUuid;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasTimestamp;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasUTCDateTime;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValue;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValueContainer;
import eu.mantis.mqtt_mimosa.mqtt_messages.MovementEvent;
import eu.mantis.mqtt_mimosa.mqtt_messages.Uuid;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public final class MyMqttClient implements MqttCallback {

  private static final String TIME_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/time_data";
  private static final String NUM_DATA_URL = "http://mantis1.tmit.bme.hu:8081/mirei/num_data";
  private static final String WARNING_URL = "http://mantis1.tmit.bme.hu:8081/mirei/REST/asset_event"; //COULD CHANGE
  private static final String MQTT_BROKER_URL = "tcp://mantis1.tmit.bme.hu:1883";

  private static MqttClient client;

  static {
    try {
      client = new MqttClient(MQTT_BROKER_URL, "MIMOSA_interface");
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker.");
    }
  }

  @Override
  public void connectionLost(Throwable throwable) {
    System.out.println("Connection lost with cause: " + throwable.getMessage());
    try {
      client.connect();
      client.setCallback(this);
      client.subscribe("/environment");
      client.subscribe("/movement");
      client.subscribe("/warning");
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not reconnect to the MQTT broker.");
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
      case "/movement":
        sendMovementToMimosa(payload);
        break;
      case "/environment":
        sendEnviromentMeasToMimosa(payload);
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
    String[] topics = iMqttDeliveryToken.getTopics();
    if (topics != null && topics[0].equals("/command")) {
      System.out.println("Command message delivered! Shutting down...");
      System.exit(0);
    }
  }

  private void sendMovementToMimosa(String payload) {
    MovementEvent event;
    try {
      event = Utility.fromJson(payload, MovementEvent.class);
    } catch (JsonSyntaxException e) {
      System.out.println("JSON parser FAILED to parse Movement event.");
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
    EnvironmentMeas measurement;
    try {
      measurement = Utility.fromJson(payload, EnvironmentMeas.class);
    } catch (JsonSyntaxException e) {
      System.out.println("JSON parser FAILED to parse Environment measurement.");
      return;
    }

    String measLocSite = measurement.getHasLocation().getMeasLocId().getValue();

    String timestamp = Utility.fixDateFormat(measurement.getHasTimestamp().getHasUTCDateTime().getValue());

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
      /*try {
        Utility.sendPostRequest(NUM_DATA_URL, numData);
      } catch (Exception e) {
        e.printStackTrace();
      }*/
    }
  }

  private void sendWarningEventToMimosa(String payload) {

  }

  public int sendTimestampToBroker(String topic) {
    if (topic == null) {
      topic = "/ANS_MEAS_BOX_001";
    }

    DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy.MM.dd.HH:mm:ss");
    String timestamp = LocalDateTime.now().format(formatter);
    String payload = Utility.toJson(new BoxId(timestamp));

    try {
      client.publish(topic, payload.getBytes(), 0, true);
      return 200;
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not publish the message to the /command topic!");
      return 500;
    }
  }

  //TODO ask if this needs to be updated or not
  public void sendCommandToBroker() {
    if (!client.isConnected()) {
      startClient();
    }

    List<String> values = new ArrayList<>();
    values.add("doSpotMove");
    HasValue hasValue = new HasValue();
    hasValue.setValues(values);

    Uuid uuid = new Uuid();
    uuid.setValue("3820");

    HasValueContainer hasValueContainer = new HasValueContainer();
    hasValueContainer.setHasValue(hasValue);
    hasValueContainer.setUuid(uuid);
    List<HasValueContainer> hasValueContainers = new ArrayList<>();
    hasValueContainers.add(hasValueContainer);

    HasAttribute hasAttribute = new HasAttribute();
    hasAttribute.setHasAttributeClazzType("SD");
    hasAttribute.setHasName("cmd");
    hasAttribute.setHasValueContainers(hasValueContainers);

    HasEventDescriptionUuid hasEventDescriptionUuid = new HasEventDescriptionUuid();
    hasEventDescriptionUuid.setValue("-4788471454629101380");

    DateFormat df = new SimpleDateFormat("yyyy.MM.dd. HH:mm:ss");
    Date now = Calendar.getInstance().getTime();
    String timestamp = df.format(now);

    HasUTCDateTime hasUTCDateTime = new HasUTCDateTime();
    hasUTCDateTime.setFormat("DATETIME");
    hasUTCDateTime.setValue(timestamp);

    HasTimestamp hasTimestamp = new HasTimestamp();
    hasTimestamp.setHasUTCDateTime(hasUTCDateTime);

    List<HasAttribute> hasAttributes = new ArrayList<>();
    hasAttributes.add(hasAttribute);

    BoxCommand boxCommand = new BoxCommand();
    boxCommand.setHasAttributes(hasAttributes);
    boxCommand.setHasEventDescriptionUuid(hasEventDescriptionUuid);
    boxCommand.setHasTimestamp(hasTimestamp);

    String payload = Utility.toJson(boxCommand);

    try {
      client.publish("/command", payload.getBytes(), 0, true);
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not publish the message to the /command topic!");
    }
  }

  void startClient() {
    try {
      client.connect();
      client.setCallback(this);
      client.subscribe("/environment");
      client.subscribe("/movement");
      client.subscribe("/warning");
    } catch (MqttException e) {
      e.printStackTrace();
    }
  }

}
