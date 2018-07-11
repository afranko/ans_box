package eu.mantis.mqtt_mimosa;

import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;
import org.glassfish.grizzly.utils.Exceptions;

@Provider
public class AppExceptionMapper implements ExceptionMapper<Throwable> {

  @Override
  public Response toResponse(Throwable ex) {
    ex.printStackTrace();
    return Response.status(500).entity(Exceptions.getStackTraceAsString(ex)).type("text/plain").build();
  }
}
