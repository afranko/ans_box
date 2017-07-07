package eu.mantis.mqtt_mimosa;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Base64;
import java.util.Date;

import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;

import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.jackson.JacksonFeature;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public final class Utility {
	
	private static final Gson gson = new GsonBuilder().disableHtmlEscaping().setPrettyPrinting().create();
	//private static final Gson gson = new Gson();

	private Utility() {
	}

	public static <T> Response sendRequest(String URI, String method, T payload) throws Exception {

		Response response = null;
		ClientConfig configuration = new ClientConfig();
		configuration.property(ClientProperties.CONNECT_TIMEOUT, 30000);
		configuration.property(ClientProperties.READ_TIMEOUT, 30000);
		Client client = ClientBuilder.newClient(configuration);
		client.register(JacksonFeature.class);

		WebTarget target = client.target(UriBuilder.fromUri(URI).build());
		switch (method) {
		case "GET":
			response = target.request().header("Content-type", "application/json").get();
			break;
		case "POST":
			response = target.request().header("Content-type", "application/json").post(Entity.json(payload));
			break;
		case "PUT":
			response = target.request().header("Content-type", "application/json").put(Entity.json(payload));
			break;
		case "DELETE":
			response = target.request().header("Content-type", "application/json").delete();
			break;
		default:
			throw new Exception("Invalid method type was given to the Utility.sendRequest() method");
		}

		if (response.getStatus() == 500 || response.getStatus() == 400 || response.getStatus() == 404 || response.getStatus() == 409) {
			throw new Exception("Response status code is: 400, 404, 409 or 500");
		}
		else{
			System.out.println("POST was successful");
		}

		return response;
	}
	
	public static <T> void sendPostRequest(String URL, T payload) throws Exception{
		System.out.println("Sending POST request to: " + URL);
		URL url = new URL(URL);
		String jsonPayload = gson.toJson(payload);
		System.out.println(jsonPayload);

		HttpURLConnection connection = (HttpURLConnection) url.openConnection();
		connection.setDoOutput(true);
		connection.setDoInput(true);
		connection.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
		connection.setRequestProperty("Accept", "application/json");
		connection.setRequestMethod("POST");
		
		OutputStreamWriter wr = new OutputStreamWriter(connection.getOutputStream());
		wr.write(jsonPayload);
		wr.close();
		
		StringBuilder sb = new StringBuilder();  
		BufferedReader br = new BufferedReader(new InputStreamReader(connection.getInputStream(), "utf-8"));
	    String line = null;  
	    while ((line = br.readLine()) != null) {  
	        sb.append(line + "\n");  
	    }
	    br.close();
	    
		int HttpResult = connection.getResponseCode();
		if (HttpResult == HttpURLConnection.HTTP_OK || HttpResult == HttpURLConnection.HTTP_CREATED) {
			System.out.println("POST request was successful.\n");
		}
		else {
			System.out.println("POST request failed:" + sb.toString());
			throw new Exception(sb.toString()); 
		}  
	}
	
	public static <T> T fromJson(String json, Class<T> parsedClass){
        return gson.fromJson(json, parsedClass);
    }

	public static <T> String toJson(T object){
		return gson.toJson(object);
	}
	
	public static String encodeFloatArray(float[] values){
		ByteBuffer buf = ByteBuffer.allocate(Float.SIZE / Byte.SIZE * values.length);
		buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.asFloatBuffer().put(values);
        return Base64.getEncoder().encodeToString(buf.array());
	}
	
	public static String fixDateFormat(String timeStamp){
		Date date = new Date();
		try {
			date = new SimpleDateFormat("yyyy.MM.dd.HH:mm:ss").parse(timeStamp);
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(date);
	}
}
