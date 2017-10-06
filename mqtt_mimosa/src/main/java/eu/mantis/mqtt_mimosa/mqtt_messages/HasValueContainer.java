package eu.mantis.mqtt_mimosa.mqtt_messages;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasValueContainer {

  @SerializedName("hasValue")
  @Expose
  private HasValue hasValue;
  @SerializedName("uuid")
  @Expose
  private Uuid uuid;

  public HasValue getHasValue() {
    return hasValue;
  }

  public void setHasValue(HasValue hasValue) {
    this.hasValue = hasValue;
  }

  public Uuid getUuid() {
    return uuid;
  }

  public void setUuid(Uuid uuid) {
    this.uuid = uuid;
  }

}
