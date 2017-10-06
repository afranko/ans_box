package eu.mantis.mqtt_mimosa.mimosa_messages;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class ErrorMessage {

  @SerializedName("status")
  @Expose
  private Integer status;
  @SerializedName("message")
  @Expose
  private String message;
  @SerializedName("error_code")
  @Expose
  private Integer error_code;
  @SerializedName("more_info")
  @Expose
  private String more_info;

  /**
   * No args constructor for use in serialization
   */
  public ErrorMessage() {
  }

  public ErrorMessage(Integer status, String message, Integer error_code, String more_info) {
    this.status = status;
    this.message = message;
    this.error_code = error_code;
    this.more_info = more_info;
  }

  public Integer getStatus() {
    return status;
  }

  public void setStatus(Integer status) {
    this.status = status;
  }

  public String getMessage() {
    return message;
  }

  public void setMessage(String message) {
    this.message = message;
  }

  public Integer getError_code() {
    return error_code;
  }

  public void setError_code(Integer error_code) {
    this.error_code = error_code;
  }

  public String getMore_info() {
    return more_info;
  }

  public void setMore_info(String more_info) {
    this.more_info = more_info;
  }

}
