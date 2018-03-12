package eu.mantis.mqtt_mimosa;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

@Path("box")
public class BoxResource {

  @GET
  @Path("test")
  @Produces(MediaType.TEXT_PLAIN)
  public String getIt() {
    return "This is the MQTT_MIMOSA interface -> Box Resource.";
  }

  @GET
  public Response sendTimestamp() {
    int statusCode = new MyMqttClient().sendTimestampToBroker(null);
    return Response.status(statusCode).build();
  }

  @GET
  @Path("{boxName}")
  public Response sendTimestamp(@PathParam("boxName") String boxName) {
    if (boxName != null && !boxName.startsWith("/")) {
      boxName = "/".concat(boxName);
    }
    int statusCode = MimosaMain.client.sendTimestampToBroker(boxName);
    return Response.status(statusCode).build();
  }

}
