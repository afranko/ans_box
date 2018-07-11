package eu.mantis.mqtt_mimosa;

import java.io.IOException;
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
    /*
     * Returns the Internet Protocol (IP) address of the client or
     * last proxy that sent the request. For HTTP servlets, same as
     * the value of the CGI variable REMOTE_ADDR.
     */
    String ip = sr.getRemoteAddr();
    System.out.println("Caller's IP address: " + ip);
  }

}
