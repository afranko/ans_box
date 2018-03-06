package eu.mantis.mqtt_mimosa.mqtt_messages;

//TODO rename this class when I know the purpose of it
public class BoxId {

  private String timestamp;

  public BoxId() {
  }

  public BoxId(String timestamp) {
    this.timestamp = timestamp;
  }

  public String getTimestamp() {
    return timestamp;
  }

  public void setTimestamp(String timestamp) {
    this.timestamp = timestamp;
  }

}
