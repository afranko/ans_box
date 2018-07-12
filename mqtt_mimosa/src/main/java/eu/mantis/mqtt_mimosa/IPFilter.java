package eu.mantis.mqtt_mimosa;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.time.LocalDateTime;
import javax.ws.rs.container.ContainerRequestContext;
import javax.ws.rs.container.ContainerRequestFilter;
import javax.ws.rs.ext.Provider;

@Provider
public class IPFilter implements ContainerRequestFilter {

  @Override
  public synchronized void filter(ContainerRequestContext request) throws IOException {
    System.out.println(
        "New " + request.getMethod() + " request at: " + request.getUriInfo().getRequestUri().toString() + " (and the time is " + LocalDateTime.now()
            + ")");
    String prettyJson = Utility.getRequestPayload(request.getEntityStream());
    System.out.println(prettyJson);

    InputStream in = new ByteArrayInputStream(prettyJson.getBytes("UTF-8"));
    request.setEntityStream(in);
  }

}
