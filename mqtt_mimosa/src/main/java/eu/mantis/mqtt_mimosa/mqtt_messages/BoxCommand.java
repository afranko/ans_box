package eu.mantis.mqtt_mimosa.mqtt_messages;

public class BoxCommand {

  private String timestamp;

  public BoxCommand() {
  }

  public BoxCommand(String timestamp) {
    this.timestamp = timestamp;
  }

  public String getTimestamp() {
    return timestamp;
  }

  public void setTimestamp(String timestamp) {
    this.timestamp = timestamp;
  }

}
