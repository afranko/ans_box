package eu.mantis.mqtt_mimosa.mimosa_messages;

public class EventCharData {

  private String asset_org_site;
  private int asset_id;
  private String ev_db_site;
  private int ev_db_id;
  private int event_type_code;
  private String gmt_event_start;
  private String ec_db_site;
  private int ec_db_id;
  private int ec_type_code;
  private String eu_db_site;
  private int eu_db_id;
  private int eu_type_code;
  private String data_value;
  private String gmt_last_updated;

  public EventCharData() {
  }

  public EventCharData(String asset_org_site, int asset_id, String ev_db_site, int ev_db_id, int event_type_code, String gmt_event_start,
                       String ec_db_site, int ec_db_id, int ec_type_code, String eu_db_site, int eu_db_id, int eu_type_code, String data_value,
                       String gmt_last_updated) {
    this.asset_org_site = asset_org_site;
    this.asset_id = asset_id;
    this.ev_db_site = ev_db_site;
    this.ev_db_id = ev_db_id;
    this.event_type_code = event_type_code;
    this.gmt_event_start = gmt_event_start;
    this.ec_db_site = ec_db_site;
    this.ec_db_id = ec_db_id;
    this.ec_type_code = ec_type_code;
    this.eu_db_site = eu_db_site;
    this.eu_db_id = eu_db_id;
    this.eu_type_code = eu_type_code;
    this.data_value = data_value;
    this.gmt_last_updated = gmt_last_updated;
  }

  public String getAsset_org_site() {
    return asset_org_site;
  }

  public void setAsset_org_site(String asset_org_site) {
    this.asset_org_site = asset_org_site;
  }

  public int getAsset_id() {
    return asset_id;
  }

  public void setAsset_id(int asset_id) {
    this.asset_id = asset_id;
  }

  public String getEv_db_site() {
    return ev_db_site;
  }

  public void setEv_db_site(String ev_db_site) {
    this.ev_db_site = ev_db_site;
  }

  public int getEv_db_id() {
    return ev_db_id;
  }

  public void setEv_db_id(int ev_db_id) {
    this.ev_db_id = ev_db_id;
  }

  public int getEvent_type_code() {
    return event_type_code;
  }

  public void setEvent_type_code(int event_type_code) {
    this.event_type_code = event_type_code;
  }

  public String getGmt_event_start() {
    return gmt_event_start;
  }

  public void setGmt_event_start(String gmt_event_start) {
    this.gmt_event_start = gmt_event_start;
  }

  public String getEc_db_site() {
    return ec_db_site;
  }

  public void setEc_db_site(String ec_db_site) {
    this.ec_db_site = ec_db_site;
  }

  public int getEc_db_id() {
    return ec_db_id;
  }

  public void setEc_db_id(int ec_db_id) {
    this.ec_db_id = ec_db_id;
  }

  public int getEc_type_code() {
    return ec_type_code;
  }

  public void setEc_type_code(int ec_type_code) {
    this.ec_type_code = ec_type_code;
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

  public String getData_value() {
    return data_value;
  }

  public void setData_value(String data_value) {
    this.data_value = data_value;
  }

  public String getGmt_last_updated() {
    return gmt_last_updated;
  }

  public void setGmt_last_updated(String gmt_last_updated) {
    this.gmt_last_updated = gmt_last_updated;
  }

}
