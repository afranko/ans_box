package eu.mantis.mqtt_mimosa;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;

import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public final class Utility {
	
	private static final Gson gson = new GsonBuilder().setPrettyPrinting().create();

	private Utility() {
	}

	public static <T> Response sendRequest(String URI, String method, T payload) throws Exception {

		Response response = null;
		ClientConfig configuration = new ClientConfig();
		configuration.property(ClientProperties.CONNECT_TIMEOUT, 30000);
		configuration.property(ClientProperties.READ_TIMEOUT, 30000);
		Client client = ClientBuilder.newClient(configuration);

		WebTarget target = client.target(UriBuilder.fromUri(URI).build());
		switch (method) {
		case "GET":
			response = target.request().header("Content-type", "application/json").get();
			break;
		case "POST":
			System.out.println("test1");
			response = target.request().header("Content-type", "application/json").post(Entity.json(payload));
			System.out.println("test2");
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

		if (response == null || response.getStatus() == 500 || response.getStatus() == 400 || response.getStatus() == 404) {
			throw new Exception("Response is null or the response status code is: 400, 404 or 500");
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
		int HttpResult = connection.getResponseCode(); 
		if (HttpResult == HttpURLConnection.HTTP_OK) {
		    BufferedReader br = new BufferedReader(
		            new InputStreamReader(connection.getInputStream(), "utf-8"));
		    String line = null;  
		    while ((line = br.readLine()) != null) {  
		        sb.append(line + "\n");  
		    }
		    br.close();
		}
		else {
			System.out.println(connection.getResponseMessage());
			throw new Exception(connection.getResponseMessage()); 
		}  
		
		System.out.println("Response to POST request: " + sb.toString());
	}
	
	public static <T> T fromJson(String json, Class<T> parsedClass){
        return gson.fromJson(json, parsedClass);
    }

	public static <T> String toJson(T object){
		return gson.toJson(object);
	}
}
