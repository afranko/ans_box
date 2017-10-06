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
  private Integer errorCode;
  @SerializedName("more_info")
  @Expose
  private String moreInfo;

  /**
   * No args constructor for use in serialization
   */
  public ErrorMessage() {
  }

  public ErrorMessage(Integer status, String message, Integer errorCode, String moreInfo) {
    this.status = status;
    this.message = message;
    this.errorCode = errorCode;
    this.moreInfo = moreInfo;
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

  public Integer getErrorCode() {
    return errorCode;
  }

  public void setErrorCode(Integer errorCode) {
    this.errorCode = errorCode;
  }

  public String getMoreInfo() {
    return moreInfo;
  }

  public void setMoreInfo(String moreInfo) {
    this.moreInfo = moreInfo;
  }

}
