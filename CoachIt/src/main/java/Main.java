import static spark.Spark.*;

import java.io.IOException;
import java.io.StringWriter;
import java.net.URI;
import java.net.URISyntaxException;
import java.sql.Date;
import java.util.List;
import static java.lang.Math.toIntExact;

import org.sql2o.Sql2o;

import expDB.Club;
import expDB.Event;
import expDB.Person;
import expDB.Sql2oCom;
import handler.NewEventPayload;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;

import static spark.Spark.get;
import static spark.Spark.post;
import spark.Request;
import spark.Response;
import spark.Route;



public class Main {
	public static void main(String[] args) {
		
		// Hardcoding URL despite dynamic since it works
		//"ifnyuqhcpfmfcf:f3d5433440bbc1b70747dfb3ab9567a876e6854a0df9ae1ca0132733dd04888b@ec2-54-163-252-55.compute-1.amazonaws.com:5432/da7515ddprla0s"
		/*
		String username = "ifnyuqhcpfmfcf";
		String password = "f3d5433440bbc1b70747dfb3ab9567a876e6854a0df9ae1ca0132733dd04888b";
		String dbUrl = "jdbc:postgresql://" + "ec2-54-163-252-55.compute-1.amazonaws.com" + ':' + "5432" + "/da7515ddprla0s";
		Sql2o sql2o = new Sql2o(dbUrl, username, password);
		*/
		
		String username = "sql9164885";
		String password = "VbQKNcdHxa";
		String dbUrl = "jdbc:mysql://" + "sql9.freemysqlhosting.net" + "/sql9164885";
		Sql2o sql2o = new Sql2o(dbUrl, username, password);
		Sql2oCom model = new Sql2oCom(sql2o);

		// port(getHerokuAssignedPort());
		staticFiles.location("/public");

		model.setup();

		//sql2o.createQuery("create table person(	pid SERIAL,	name text NOT NULL, dateOfBirth date NOT NULL,	email varchar(255) unique NOT NULL, primary key (pid))").executeUpdate();
		List<Person> p = model.getPeopleAll();

        post("/Login", (req, res) -> {
        	Person person = model.getAdmin(req.queryParams("email"));
        	if(person==null){
            	res.redirect("/login.html");
        	}
        	req.session().attribute("personId", person.getPid());
        	res.redirect("/");
        	return "";
        });

        post("/CreateEvent", (req, res) -> {
        	String r = "";
        	if(req.session().attribute("personId")==null)
        	{
        		res.redirect("/");
        		return "";
        	}
        	int personId = req.session().attribute("personId");
        	int teamId = model.getClubByAdminId(personId).getCid();
        	int eid = model.createEvent(req.queryParams("location"), Date.valueOf(req.queryParams("date").replaceAll("/", "-"))
        						, req.queryParams("type"), Date.valueOf(req.queryParams("deadline").replaceAll("/", "-"))
        						, req.queryParams("title"), teamId, personId);
        	if(eid==-1){
        		res.redirect("/");
        		return"";
        	}
    		res.redirect("/event/" + eid);
        	return "";
        });
        
		get("/", (req,res) -> "Site Under Construction");

		get("/event/:eid", (req, res) -> {
			int eid = Integer.parseInt(req.params("eid"));
        	req.session().attribute("eid", eid);
			Event event = model.getEvent(eid);
			//res.status(200);
			//res.type("application/json");
			res.redirect("/event.html?"+event.Parameterize());
			return "";
		});
		
		//get events this current user is participating in 
		get("/events", (req, res) ->{
			int personId = req.session().attribute("personId");
			List<Event> events = model.getEventByPerson((int) personId);
			String s = "[";
			for (Event event: events){
				s = s.concat("{\"title\":\""+ event.getName()+"\", \"start\":\""+ event.getEventDate()
				+ "\", \"url\":\"event/" + event.getEid()+"\"},");
			}
			
			return s.substring(0, s.length()-1).concat("]");
		});

		get("/clubName", (req, res) ->{
			int personId = req.session().attribute("personId");
			Club club = model.getClubByAdminId(personId);
			return club.getName();
		});

		
		//get events this current user is participating in 
		get("/players", (req, res) ->{
			int personId = req.session().attribute("personId");
			List<Person> players = model.getPlayersByAdmin(personId);
			String s = "[";
			for (Person player: players){
				s = s.concat("{\"name\":\""+ player.getName()+"\", \"email\":\""+ player.getEmail()
				+ "\"},");
			}
			return s.substring(0, s.length()-1).concat("]");
		});
		get("/clubs", (req, res) -> {
			int personId = req.session().attribute("personId");
			int eid = req.session().attribute("eid");
			List<Club> clubs = model.getTeamsEventStatus(eid);
			String s = "[";
			for (Club club: clubs){
				s = s.concat("{\"name\":\""+ club.getName()+"\", \"status\":\""+ club.getStatus()
				+ "\", \"mine\":\"" + Boolean.toString(club.getAdmin()==personId) + "\"},");
			}
			return s.substring(0, s.length()-1).concat("]");
		});

		get("/playersEvent", (req, res) ->{
			int personId = req.session().attribute("personId");
			int eid = req.session().attribute("eid");
			List<Person> players = model.getPlayersEventStatus(personId,eid);
			String s = "[";
			for (Person player: players){
				s = s.concat("{\"name\":\""+ player.getName()+"\", \"staus\":\""+ player.getStatus()
				+ "\"} ,");
			}
			return s.substring(0, s.length()-1).concat("]");
		});

		
		
		
		
		
		/*post("/event", (request, response) -> {
		    try {
		        ObjectMapper mapper = new ObjectMapper();
		        NewEventPayload creation = mapper.readValue(request.body(), NewEventPayload.class);
		        if (!creation.isValid()) {
		            //response.status(HTTP_BAD_REQUEST);
		            return "";
		        }
		        java.sql.Date eDate = new java.sql.Date(creation.getEventDate().getTime());
		        java.sql.Date deadline = new java.sql.Date(creation.getDeadline().getTime());

		        int id = model.createEvent(creation.getLocation(), eDate, creation.getEventType(), deadline, 
		        		creation.getName(), (creation.getTeamCut()), (creation.getOrganiser()));
		        response.status(200);
		        response.type("application/json");
		        return id;
		    } catch (JsonParseException jpe) {
		        //response.status(HTTP_BAD_REQUEST);
		        return "";
		    }
		});
		*/
	}
	 public static String dataToJson(Object data) {
	        try {
	            ObjectMapper mapper = new ObjectMapper();
	            mapper.enable(SerializationFeature.INDENT_OUTPUT);
	            StringWriter sw = new StringWriter();
	            mapper.writeValue(sw, data);
	            return sw.toString();
	        } catch (IOException e){
	            throw new RuntimeException("IOException from a StringWriter?");
	        }
	    }

	private static int getHerokuAssignedPort() {
		ProcessBuilder processBuilder = new ProcessBuilder();
		if (processBuilder.environment().get("PORT") != null) {
			return Integer.parseInt(processBuilder.environment().get("PORT"));
		}
		return 4567;
	}
}
