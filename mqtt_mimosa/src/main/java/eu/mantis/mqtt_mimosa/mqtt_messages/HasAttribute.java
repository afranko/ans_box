package eu.mantis.mqtt_mimosa.mqtt_messages;

import java.util.ArrayList;
import java.util.List;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasAttribute {

  @SerializedName("hasAttributeClazzType")
  @Expose
  private String hasAttributeClazzType;
  @SerializedName("hasName")
  @Expose
  private String hasName;
  @SerializedName("hasValueContainers")
  @Expose
  private List<HasValueContainer> hasValueContainers = new ArrayList<>();

  public String getHasAttributeClazzType() {
    return hasAttributeClazzType;
  }

  public void setHasAttributeClazzType(String hasAttributeClazzType) {
    this.hasAttributeClazzType = hasAttributeClazzType;
  }

  public String getHasName() {
    return hasName;
  }

  public void setHasName(String hasName) {
    this.hasName = hasName;
  }

  public List<HasValueContainer> getHasValueContainers() {
    return hasValueContainers;
  }

  public void setHasValueContainers(List<HasValueContainer> hasValueContainers) {
    this.hasValueContainers = hasValueContainers;
  }

}
