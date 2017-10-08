package eu.mantis.mqtt_mimosa.mimosa_messages;

public class NumData {

  private String meas_loc_site;
  private int meas_loc_id;
  private String gmt_event;
  private String eu_db_site;
  private int eu_db_id;
  private int eu_type_code;
  private float data_value;

  public NumData() {
  }

  public NumData(String meas_loc_site, int meas_loc_id, String gmt_event, String eu_db_site, int eu_db_id, int eu_type_code, float data_value) {
    super();
    this.meas_loc_site = meas_loc_site;
    this.meas_loc_id = meas_loc_id;
    this.gmt_event = gmt_event;
    this.eu_db_site = eu_db_site;
    this.eu_db_id = eu_db_id;
    this.eu_type_code = eu_type_code;
    this.data_value = data_value;
  }

  public String getMeas_loc_site() {
    return meas_loc_site;
  }

  public void setMeas_loc_site(String meas_loc_site) {
    this.meas_loc_site = meas_loc_site;
  }

  public int getMeas_loc_id() {
    return meas_loc_id;
  }

  public void setMeas_loc_id(int meas_loc_id) {
    this.meas_loc_id = meas_loc_id;
  }

  public String getGmt_event() {
    return gmt_event;
  }

  public void setGmt_event(String gmt_event) {
    this.gmt_event = gmt_event;
  }

  public String getEu_db_site() {
    return eu_db_site;
  }

  public void setEu_db_site(String eu_db_site) {
    this.eu_db_site = eu_db_site;
  }

  public int getEu_db_id() {
    return eu_db_id;
  }

  public void setEu_db_id(int eu_db_id) {
    this.eu_db_id = eu_db_id;
  }

  public int getEu_type_code() {
    return eu_type_code;
  }

  public void setEu_type_code(int eu_type_code) {
    this.eu_type_code = eu_type_code;
  }

  public float getData_value() {
    return data_value;
  }

  public void setData_value(float data_value) {
    this.data_value = data_value;
  }


}
