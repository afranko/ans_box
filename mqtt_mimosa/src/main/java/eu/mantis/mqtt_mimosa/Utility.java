package eu.mantis.mqtt_mimosa;

import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;

import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;

import com.google.gson.Gson;

public final class Utility {
	
	private static final Gson gson = new Gson();

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

		if (response == null || response.getStatus() == 500 || response.getStatus() == 404) {
			throw new Exception("Response is null, 500 (Internal Server Error) or 404 (Not Found).");
		}

		return response;
	}
	
	public static <T> T fromJson(String json, Class<T> parsedClass){
        return gson.fromJson(json, parsedClass);
    }

	public static <T> String toJson(T object){
		return gson.toJson(object);
	}
}
