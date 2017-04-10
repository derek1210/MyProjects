package expDB;

import java.sql.Date;
import java.time.LocalDate;
import java.util.List;

public interface Dbcoms{
		
	int createPerson(String name, Date DoB, String email);
	int createPlayer(int pid, int cid);
	int createCoach(int pid, int cid);
	
	int createInvitation(int eid, int cid);
	int updateInvitation(int eid, int cid, int status);
	int createEvent(String location, Date date, String type, Date deadline, String name, int teamcut, int organiser);
	int updateEvent(int eid, String location, Date date, String type, Date deadline, String name, int teamcut, int organiser);
	int removeEvent(int eid);
	
	List getEventsAll(int cid);
	List getClubsAll();
	List getPlayersAll();
	List getPeopleAll();
	List getCoachesAll();
	List getInvitationsAll();
	
	List getInvited(int eid);
	List getPlayersByClub(int cid);
	
	Person getPerson(String email);
	Person getAdmin(String email);
	Event getEvent(int eid);
	
	int createClub(String name, int admin);
	Event getEventbyName(String name);
	int updatePlayerInviteStatus(String status, int cid, int pid, int eid);
	int changeEventStatus(int eid, String status);
	int sendPlayerInvites(int cid, int eid);
	List getPlayerInviteByClub(int cid);
	Person getPerson(int pid);
	Club getClub(int cid);
	List<Person> getPlayersByAdmin(int admin);
	
}

