
package eu.mantis.mqtt_mimosa.mqtt_messages;

import java.util.ArrayList;
import java.util.List;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class HasValue {

    @SerializedName("values")
    @Expose
    private List<String> values = new ArrayList<String>();
    @SerializedName("value")
    @Expose
    private int value;

    public List<String> getValues() {
        return values;
    }

    public void setValues(List<String> values) {
        this.values = values;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        this.value = value;
    }

}
