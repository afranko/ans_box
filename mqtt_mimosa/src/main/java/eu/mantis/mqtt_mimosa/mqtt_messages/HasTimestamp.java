
package eu.mantis.mqtt_mimosa.mqtt_messages;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasTimestamp {

    @SerializedName("hasUTCDateTime")
    @Expose
    private HasUTCDateTime hasUTCDateTime;

    public HasUTCDateTime getHasUTCDateTime() {
        return hasUTCDateTime;
    }

    public void setHasUTCDateTime(HasUTCDateTime hasUTCDateTime) {
        this.hasUTCDateTime = hasUTCDateTime;
    }

}
