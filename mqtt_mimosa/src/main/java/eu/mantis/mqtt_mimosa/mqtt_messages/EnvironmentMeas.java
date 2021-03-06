package eu.mantis.mqtt_mimosa.mqtt_messages;

import java.util.ArrayList;
import java.util.List;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class EnvironmentMeas {

  @SerializedName("hasAttributes")
  @Expose
  private List<HasAttribute> hasAttributes = new ArrayList<>();
  @SerializedName("hasEventDescriptionUuid")
  @Expose
  private HasEventDescriptionUuid hasEventDescriptionUuid;
  @SerializedName("hasLocation")
  @Expose
  private HasLocation hasLocation;
  @SerializedName("hasTimestamp")
  @Expose
  private HasTimestamp hasTimestamp;


  public EnvironmentMeas() {
  }

  public EnvironmentMeas(List<HasAttribute> hasAttributes, HasEventDescriptionUuid hasEventDescriptionUuid, HasLocation hasLocation,
                         HasTimestamp hasTimestamp) {
    super();
    this.hasAttributes = hasAttributes;
    this.hasEventDescriptionUuid = hasEventDescriptionUuid;
    this.hasLocation = hasLocation;
    this.hasTimestamp = hasTimestamp;
  }

  public List<HasAttribute> getHasAttributes() {
    return hasAttributes;
  }

  public void setHasAttributes(List<HasAttribute> hasAttributes) {
    this.hasAttributes = hasAttributes;
  }

  public HasEventDescriptionUuid getHasEventDescriptionUuid() {
    return hasEventDescriptionUuid;
  }

  public void setHasEventDescriptionUuid(HasEventDescriptionUuid hasEventDescriptionUuid) {
    this.hasEventDescriptionUuid = hasEventDescriptionUuid;
  }

  public HasLocation getHasLocation() {
    return hasLocation;
  }

  public void setHasLocation(HasLocation hasLocation) {
    this.hasLocation = hasLocation;
  }

  public HasTimestamp getHasTimestamp() {
    return hasTimestamp;
  }

  public void setHasTimestamp(HasTimestamp hasTimestamp) {
    this.hasTimestamp = hasTimestamp;
  }

}
