package eu.mantis.mqtt_mimosa;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import eu.mantis.mqtt_mimosa.mqtt_messages.BoxCommand;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasAttribute;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasEventDescriptionUuid;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasTimestamp;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasUTCDateTime;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValue;
import eu.mantis.mqtt_mimosa.mqtt_messages.HasValueContainer;
import eu.mantis.mqtt_mimosa.mqtt_messages.Uuid;

class MqttMain implements MqttCallback {

  private MqttClient client;
  private static final String MQTT_BROKER_URL = "tcp://mantis1.tmit.bme.hu:1883";

  private MqttMain() {
  }

  public static void main(String[] args) {
    new MqttMain().startListening();
  }

  private void startListening() {
    try {
      client = new MqttClient(MQTT_BROKER_URL, "MIMOSA_interface");
      client.connect();
      client.setCallback(this);

      String payload = compileBoxCommand();
      client.publish("/command", payload.getBytes(), 0, true);
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker.");
    }
  }

  @Override
  public void connectionLost(Throwable arg0) {
    System.out.println("Connection lost: " + arg0.getMessage());
    try {
      client.connect();
      client.setCallback(this);
    } catch (MqttException e) {
      e.printStackTrace();
      System.out.println("Could not connect to the MQTT broker.");
    }
  }

  @Override
  public void deliveryComplete(IMqttDeliveryToken arg0) {
    System.out.println("Message delivered! Press Ctrl+C to exit.");
  }

  @Override
  public void messageArrived(String arg0, MqttMessage arg1) throws Exception {
    // TODO Auto-generated method stub

  }

  private String compileBoxCommand() {
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
    String timeStamp = df.format(now);

    HasUTCDateTime hasUTCDateTime = new HasUTCDateTime();
    hasUTCDateTime.setFormat("DATETIME");
    hasUTCDateTime.setValue(timeStamp);

    HasTimestamp hasTimestamp = new HasTimestamp();
    hasTimestamp.setHasUTCDateTime(hasUTCDateTime);

    List<HasAttribute> hasAttributes = new ArrayList<>();
    hasAttributes.add(hasAttribute);

    BoxCommand boxCommand = new BoxCommand();
    boxCommand.setHasAttributes(hasAttributes);
    boxCommand.setHasEventDescriptionUuid(hasEventDescriptionUuid);
    boxCommand.setHasTimestamp(hasTimestamp);

    return Utility.toJson(boxCommand);
  }

}
