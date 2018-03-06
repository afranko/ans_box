package eu.mantis.mqtt_mimosa;

class BoxMain {

  public static void main(String[] args) {
    new MyMqttClient().sendCommandToBroker();
  }

}
