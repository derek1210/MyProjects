package mail;
import java.io.IOException;

import com.sendgrid.Content;
import com.sendgrid.Email;
import com.sendgrid.Mail;
import com.sendgrid.Method;
import com.sendgrid.Request;
import com.sendgrid.Response;
import com.sendgrid.SendGrid;

import expDB.Club;
import expDB.Event;
import expDB.Person;

public class SendMail {
	private static final String MY_API_KEY = "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";
	
	/**
	 * Generates an email template in HTML given a person, event and club.
	 * @param person Person to whom the email is addressed.
	 * @param event Event that is used to fill in information in email.
	 * @param club Club that is used to generate proper response links in buttons.
	 * @return Valid HTML code represented as a string.
	 */
	public String createHTML(Person person, Event event, Club club){
		String instance = "<!DOCTYPE html>";
		instance += "<html><body><h1>Coachit</h1><br><h7>You have been invited to participate in the "
				+ event.getEventType() + " :</h7>";
		instance += "<h2>" + event.getName() + "</h2><h7>The event will occur on this date at this location:</h7>"
				+ "<h2>" + event.getEventDate() + "at" + event.getLocation() + "</h2><h7>";
		instance += " Please confirm your availability: </h7><br><br><table cellspacing=\"0\" cellpadding=\"0\"><tr><td align=\"center\" width=\"300\" height=\"40\" "
				+ "bgcolor=\"#008000\" style=\"-webkit-border-radius: 5px; -moz-border-radius: 5px; border-radius: 5px; color: #ffffff; display: block;\">"
				+ "<a href=\"localhost:4567/respond/player/" + person.getPid() + "/club/"
						+ club.getCid() + "/event/" + event.getEid() + "/status/accept\" "
				+ "style=\"font-size:16px; font-weight: bold; font-family: Helvetica, Arial, sans-serif; text-decoration: none; line-height:40px; "
				+ "width:100%; display:inline-block\"><span style=\"color: #FFFFFF\" onmouseover=\"my(this)\">Available</span></a></td>";
		instance += "<script>function my(x) {x.style.color=\"#000000\";}</script></tr></table><br> <br><table cellspacing=\"0\" "
				+ "cellpadding=\"0\"><tr><td align=\"center\" width=\"300\" height=\"40\" bgcolor=\"#FF0000\" style=\"-webkit-border-radius: "
				+ "5px; -moz-border-radius: 5px; border-radius: 5px; color: #ffffff; display: block;\">"
				+ "<a href=\"localhost:4567/respond/player/" + person.getPid() + "/club/"
						+ club.getCid() + "/event/" + event.getEid() + "/status/reject\" "
				+ "style=\"font-size:16px;font-weight: bold; font-family: Helvetica, Arial, sans-serif; text-decoration: none; line-height:40px; width:100%; "
				+ "display:inline-block\"><span style=\"color: #FFFFFF\" onmouseover=\"my(this)\">Decline</span></a></td></tr></table>";
		instance += "<br><br> <hr><h7>You are receiving this email because you are subscribed to the Coachit platform.</h7></body></html>";
		return instance;
	}
	/**
	 * Prints status 202 if the email has been successfully created.
	 * @param person Person used to extract personal information.
	 * @param event Event used to extract event information.
	 * @param club Club used to generate correct links on the buttons.
	 * @throws IOException Check email addresses or API KEY.
	 */
	public void sendEmail(Person person, Event event, Club club) throws IOException {
		
		String html_text = createHTML(person,event,club);
		Email from = new Email("app65233591@heroku.com");
		String subject = "Coach-it Invitation";
		Email to = new Email(person.getEmail());
		Content content = new Content("text/html", html_text);
		Mail mail = new Mail(from, subject, to, content);

		SendGrid sg = new SendGrid(MY_API_KEY);
		Request request = new Request();
		try {
			request.method = Method.POST;
			request.endpoint = "mail/send";
			request.body = mail.build();
			Response response = sg.api(request);
			System.out.println(response.statusCode);
			System.out.println(response.body);
			System.out.println(response.headers);
		} catch (IOException ex) {
			throw ex;
		}
	}
}
