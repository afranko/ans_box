package eu.mantis.mqtt_mimosa;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import eu.mantis.mqtt_mimosa.mimosa_messages.ErrorMessage;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Base64;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status.Family;
import javax.ws.rs.core.UriBuilder;
import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.jackson.JacksonFeature;

final class Utility {

  private static final Gson gson = new GsonBuilder().disableHtmlEscaping().setPrettyPrinting().create();
  //private static final Gson gson = new Gson();

  private Utility() {
  }

  static <T> Response sendRequest(String URI, String method, T payload) {
    System.out.println("Sending " + method + " request to: " + URI);
    if (payload != null) {
      System.out.println("The request payload:");
      System.out.println(gson.toJson(payload));
    }

    Response response;
    ClientConfig configuration = new ClientConfig();
    configuration.property(ClientProperties.CONNECT_TIMEOUT, 60000);
    configuration.property(ClientProperties.READ_TIMEOUT, 60000);
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
        throw new RuntimeException("Invalid method type was given to the Utility.sendRequest() method");
    }

    if (!(response.getStatusInfo().getFamily() == Family.SUCCESSFUL)) {
      ErrorMessage errorMessage;
      try {
        errorMessage = response.readEntity(ErrorMessage.class);
      } catch (RuntimeException e) {
        throw new RuntimeException("Unknown error occurred at " + URI);
      }
      if (errorMessage == null) {
        throw new RuntimeException("Unknown error occurred at " + URI);
      }
      else {
        System.out.println("Error occurred during the request at " + URI);
        System.out.println(gson.toJson(errorMessage));
        if (response.getStatus() == 409) {
          if ("sql error when trying to delete".equals(errorMessage.getMore_info())) {
            System.out.println("This measurement is already saved in the MIMOSA database!");
          } else if ("sql error when trying to insert".equals(errorMessage.getMore_info())) {
            System.out.println("New Measurement Location! Needs to be added to the site and meas_location tables first!\n\n");
          }
        }
      }
    } else {
      System.out.println("Publishing to the MIMOSA database was successful!");
    }

    return response;
  }

  static <T> void sendPostRequest(String URL, T payload) throws Exception {
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
    String line;
    while ((line = br.readLine()) != null) {
      sb.append(line).append("\n");
    }
    br.close();

    int HttpResult = connection.getResponseCode();
    if (HttpResult == HttpURLConnection.HTTP_OK || HttpResult == HttpURLConnection.HTTP_CREATED) {
      System.out.println("POST request was successful.\n");
    } else {
      if (HttpResult == HttpURLConnection.HTTP_CONFLICT) {
        System.out.println("This measurement is already saved in the MIMOSA database!");
      } else {
        System.out.println("POST request failed:" + sb.toString());
        System.out.println("Status code: " + HttpResult);
        //throw new Exception(sb.toString());
      }
    }
  }

  static <T> T fromJson(String json, Class<T> parsedClass) {
    return gson.fromJson(json, parsedClass);
  }

  static <T> String toJson(T object) {
    return gson.toJson(object);
  }

  static String encodeFloatArray(float[] values) {
    ByteBuffer buf = ByteBuffer.allocate(Float.SIZE / Byte.SIZE * values.length);
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.asFloatBuffer().put(values);
    return Base64.getEncoder().encodeToString(buf.array());
  }

  static String fixDateFormat(String timeStamp) {
    DateTimeFormatter from = DateTimeFormatter.ofPattern("yyyy.MM.dd.HH:mm:ss");
    DateTimeFormatter to = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss.SSS");
    //NOTE could convert date to GMT to comply with MIMOSA standard
    return LocalDateTime.parse(timeStamp, from).format(to);
  }

}
