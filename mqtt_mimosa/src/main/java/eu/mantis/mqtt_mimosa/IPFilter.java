package eu.mantis.mqtt_mimosa;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.container.ContainerRequestContext;
import javax.ws.rs.container.ContainerRequestFilter;
import javax.ws.rs.core.Context;
import javax.ws.rs.ext.Provider;

@Provider
public class IPFilter implements ContainerRequestFilter {

  @Context
  private HttpServletRequest sr;

  @Override
  public synchronized void filter(ContainerRequestContext request) throws IOException {
    System.out.println("New " + request.getMethod() + " request at: " + request.getUriInfo().getRequestUri().toString());
    String prettyJson = Utility.getRequestPayload(request.getEntityStream());
    System.out.println(prettyJson);

    InputStream in = new ByteArrayInputStream(prettyJson.getBytes("UTF-8"));
    request.setEntityStream(in);

    //always null atm :/
    if (sr != null) {
      System.out.println("Caller's address: " + sr.getRemoteAddr());
    }

  }

}
