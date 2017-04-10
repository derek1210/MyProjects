package expDB;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.Date;

public class Event {
	private int eid;
	private String location;
	private Date eventDate;
	private String eventType;
	private Date deadline;
	private String name;
	private int teamCut;
	private int organiser;
	private String status; 
	
	//TODO: import Lombok / add to pom for @Data annotation use and remove everything below afterwards
	
	public int getEid() {
		return eid;
	}
	public void setEid(int eid) {
		this.eid = eid;
	}
	public String getLocation() {
		return location;
	}
	public void setLocation(String location) {
		this.location = location;
	}
	public Date getEventDate() {
		return eventDate;
	}
	public void setEventDate(Date eventDate) {
		this.eventDate = eventDate;
	}
	public String getEventType() {
		return eventType;
	}
	public void setEventType(String eventType) {
		this.eventType = eventType;
	}
	public Date getDeadline() {
		return deadline;
	}
	public void setDeadline(Date deadline) {
		this.deadline = deadline;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public Integer getTeamCut() {
		return teamCut;
	}
	public void setTeamCut(Integer teamCut) {
		this.teamCut = teamCut;
	}
	public Integer getOrganiser() {
		return organiser;
	}
	public void setOrganiser(Integer organiser) {
		this.organiser = organiser;
	}
	public String getStatus() {
		return status;
	}
	public void setStatus(String status) {
		this.status = status;
	}
	
	public String Parameterize(){
		try {
		String parm = "title=" + URLEncoder.encode(getName(), "UTF-8") +
				  "&date=" + getEventDate() +
				  "&loc=" + getLocation() +
				  "&type=" + getEventType() +
				  "&deadline=" + getDeadline();	
		return parm;
		} catch (UnsupportedEncodingException ignored) {
			return "";
		}
	}
}
