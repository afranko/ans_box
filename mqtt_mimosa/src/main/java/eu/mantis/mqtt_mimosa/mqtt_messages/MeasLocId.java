package eu.mantis.mqtt_mimosa.mqtt_messages;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class MeasLocId {

  @SerializedName("value")
  @Expose
  private String value;

  public String getValue() {
    return value;
  }

  public void setValue(String value) {
    this.value = value;
  }

}
