package eu.mantis.mqtt_mimosa.mqtt_messages;

import java.util.ArrayList;
import java.util.List;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasValue {

  @SerializedName("values")
  @Expose
  private List<String> values = new ArrayList<>();
  @SerializedName("value")
  @Expose
  private float value;

  public List<String> getValues() {
    return values;
  }

  public void setValues(List<String> values) {
    this.values = values;
  }

  public float getValue() {
    return value;
  }

  public void setValue(float value) {
    this.value = value;
  }

}
