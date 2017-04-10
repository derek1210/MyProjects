package handler;

import java.util.Date;

import expDB.Sql2oCom;

interface Validable {
	   boolean isValid();
	}

public class NewEventPayload implements Validable {
	private String location;
	private Date eventDate;
	private String eventType;
	private Date deadline;
	private String name;
	private int teamCut;
	private int organiser;

	

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



	public int getTeamCut() {
		return teamCut;
	}



	public void setTeamCut(int teamCut) {
		this.teamCut = teamCut;
	}



	public int getOrganiser() {
		return organiser;
	}



	public void setOrganiser(int organiser) {
		this.organiser = organiser;
	}



	public boolean isValid() {
		return location != null && eventDate != null && name != null;
	}
}