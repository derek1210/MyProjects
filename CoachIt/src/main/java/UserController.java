import static spark.Spark.get;

import spark.Request;
import spark.Response;
import spark.Route;
import users.UserManagement;


public class UserController {
	public UserController(final UserManagement userService) {
	     
	    get("/users", new Route() {
	      @Override
	      public Object handle(Request request, Response response) {
	        // process request
	        return UserManagement.getAllUsers();
	      }
	    });
	     
	    // more routes
	  }

}
