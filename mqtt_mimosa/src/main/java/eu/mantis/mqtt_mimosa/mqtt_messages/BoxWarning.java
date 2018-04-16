package eu.mantis.mqtt_mimosa.mqtt_messages;

public class BoxWarning {

  private String boxId;
  private String message;
  private String timestamp;

  public BoxWarning() {
  }

  public BoxWarning(String boxId, String message, String timestamp) {
    this.boxId = boxId;
    this.message = message;
    this.timestamp = timestamp;
  }

  public String getBoxId() {
    return boxId;
  }

  public void setBoxId(String boxId) {
    this.boxId = boxId;
  }

  public String getMessage() {
    return message;
  }

  public void setMessage(String message) {
    this.message = message;
  }

  public String getTimestamp() {
    return timestamp;
  }

  public void setTimestamp(String timestamp) {
    this.timestamp = timestamp;
  }

}
