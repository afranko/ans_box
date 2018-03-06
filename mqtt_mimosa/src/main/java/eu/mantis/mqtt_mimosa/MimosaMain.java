package eu.mantis.mqtt_mimosa;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.ServiceConfigurationError;
import javax.ws.rs.ProcessingException;
import javax.ws.rs.core.UriBuilder;
import org.glassfish.grizzly.http.server.HttpServer;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.server.ResourceConfig;

class MimosaMain {

  private static final String BASE_URI = "http://0.0.0.0:8200";

  private static HttpServer server;

  public static void main(String[] args) throws IOException {
    new MyMqttClient().startClient();
    server = startServer();

    System.out.println("Type \"stop\" to shutdown the server...");
    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    String input = "";
    while (!input.equals("stop")) {
      input = br.readLine();
    }
    br.close();
    shutdown();
  }

  private static HttpServer startServer() throws IOException {
    final ResourceConfig config = new ResourceConfig();
    config.registerClasses(BoxResource.class);

    URI uri = UriBuilder.fromUri(BASE_URI).build();
    final HttpServer server = GrizzlyHttpServerFactory.createHttpServer(uri, config);
    server.getServerConfiguration().setAllowPayloadForUndefinedHttpMethods(true);
    server.start();
    System.out.println("Started insecure server at: " + BASE_URI);
    return server;
  }

  private static void shutdown() {
    if (server != null) {
      server.shutdownNow();
    }
    System.out.println("Server stopped!");
  }

}
