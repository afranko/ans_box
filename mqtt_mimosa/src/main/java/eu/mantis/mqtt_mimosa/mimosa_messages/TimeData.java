package eu.mantis.mqtt_mimosa.mimosa_messages;


public class TimeData {
	
	private String meas_loc_site;
	private int meas_loc_id;
	private String gmt_event;
	private int ordering_seq;
	private String eu_db_site;
	private int eu_db_id;
	private int eu_type_code;
	private String secs_or_revs;
	private float off_secs_or_revs;
	private float max_secs_or_revs;
	private float number_of_samples;
	private String win_db_site;
	private int win_db_id;
	private int win_type_code;
	private String complex_yn;
	private String waveform_data; //should be base64 coded binary data
	
	public TimeData(){
	}
	
	public TimeData(String meas_loc_site, int meas_loc_id, String gmt_event, int ordering_seq, String eu_db_site,
			int eu_db_id, int eu_type_code, String secs_or_revs, float off_secs_or_revs, float max_secs_or_revs, 
			float number_of_samples, String win_db_site, int win_db_id, int win_type_code, String complex_yn, 
			String waveform_data) {
		super();
		this.meas_loc_site = meas_loc_site;
		this.meas_loc_id = meas_loc_id;
		this.gmt_event = gmt_event;
		this.ordering_seq = ordering_seq;
		this.eu_db_site = eu_db_site;
		this.eu_db_id = eu_db_id;
		this.eu_type_code = eu_type_code;
		this.secs_or_revs = secs_or_revs;
		this.off_secs_or_revs = off_secs_or_revs;
		this.max_secs_or_revs = max_secs_or_revs;
		this.number_of_samples = number_of_samples;
		this.win_db_site = win_db_site;
		this.win_db_id = win_db_id;
		this.win_type_code = win_type_code;
		this.complex_yn = complex_yn;
		this.waveform_data = waveform_data;
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

	public int getOrdering_seq() {
		return ordering_seq;
	}

	public void setOrdering_seq(int ordering_seq) {
		this.ordering_seq = ordering_seq;
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

	public String getSecs_or_revs() {
		return secs_or_revs;
	}

	public void setSecs_or_revs(String secs_or_revs) {
		this.secs_or_revs = secs_or_revs;
	}

	public float getOff_secs_or_revs() {
		return off_secs_or_revs;
	}

	public void setOff_secs_or_revs(float off_secs_or_revs) {
		this.off_secs_or_revs = off_secs_or_revs;
	}

	public float getMax_secs_or_revs() {
		return max_secs_or_revs;
	}

	public void setMax_secs_or_revs(float max_secs_or_revs) {
		this.max_secs_or_revs = max_secs_or_revs;
	}

	public float getNumber_of_samples() {
		return number_of_samples;
	}

	public void setNumber_of_samples(float number_of_samples) {
		this.number_of_samples = number_of_samples;
	}

	public String getWin_db_site() {
		return win_db_site;
	}

	public void setWin_db_site(String win_db_site) {
		this.win_db_site = win_db_site;
	}

	public int getWin_db_id() {
		return win_db_id;
	}

	public void setWin_db_id(int win_db_id) {
		this.win_db_id = win_db_id;
	}

	public int getWin_type_code() {
		return win_type_code;
	}

	public void setWin_type_code(int win_type_code) {
		this.win_type_code = win_type_code;
	}

	public String getComplex_yn() {
		return complex_yn;
	}

	public void setComplex_yn(String complex_yn) {
		this.complex_yn = complex_yn;
	}

	public String getWaveform_data() {
		return waveform_data;
	}

	public void setWaveform_data(String waveform_data) {
		this.waveform_data = waveform_data;
	}
	
}
