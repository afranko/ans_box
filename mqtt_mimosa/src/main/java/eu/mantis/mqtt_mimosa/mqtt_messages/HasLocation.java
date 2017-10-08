package eu.mantis.mqtt_mimosa.mqtt_messages;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasLocation {

  @SerializedName("measLocId")
  @Expose
  private MeasLocId measLocId;

  public MeasLocId getMeasLocId() {
    return measLocId;
  }

  public void setMeasLocId(MeasLocId measLocId) {
    this.measLocId = measLocId;
  }

}
