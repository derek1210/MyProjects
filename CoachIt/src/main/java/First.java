import static spark.Spark.*;

public class First {
    public static void main(String[] args) {
    	staticFiles.location("/public");
        get("/hello", (req, res) -> "Hello World");
        post("/CreateEvent", (req, res) -> req.params());
        post("/Login", (req, res) -> {
        	req.session().attribute("email", req.queryParams("email"));
        	res.redirect("/");
        	return "";
        });
        post("/Signup", (req, res) -> {
        	req.session().attribute("email", req.queryParams("email"));
        	res.redirect("/");
        	return "";
        });
    }
}