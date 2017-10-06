package eu.mantis.mqtt_mimosa.mqtt_messages;

import java.util.List;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class BoxCommand {

  @SerializedName("hasAttributes")
  @Expose
  private List<HasAttribute> hasAttributes = null;
  @SerializedName("hasEventDescriptionUuid")
  @Expose
  private HasEventDescriptionUuid hasEventDescriptionUuid;
  @SerializedName("hasTimestamp")
  @Expose
  private HasTimestamp hasTimestamp;

  public BoxCommand() {
  }

  public BoxCommand(List<HasAttribute> hasAttributes, HasEventDescriptionUuid hasEventDescriptionUuid, HasTimestamp hasTimestamp) {
    super();
    this.hasAttributes = hasAttributes;
    this.hasEventDescriptionUuid = hasEventDescriptionUuid;
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

  public HasTimestamp getHasTimestamp() {
    return hasTimestamp;
  }

  public void setHasTimestamp(HasTimestamp hasTimestamp) {
    this.hasTimestamp = hasTimestamp;
  }

}
