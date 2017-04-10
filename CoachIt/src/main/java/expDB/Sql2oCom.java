package expDB;

import java.sql.Date;
import java.time.LocalDate;
import java.util.List;

import org.sql2o.Connection;
import org.sql2o.Sql2o;

public class Sql2oCom implements Dbcoms{
	private Sql2o sql2o;
	
	public Sql2oCom(Sql2o sql2o){
		this.sql2o = sql2o;
	}

	@Override
	public int createPerson(String name, Date DoB, String email) {
		try(Connection con = sql2o.open()){
			int pid = (int) con.createQuery("insert into person(name, dateOfBirth, email) values(:name, :dateOfBirth, :email)")
			.addParameter("name", name)
			.addParameter("dateOfBirth", DoB)
			.addParameter("email", email)
			.executeUpdate()
			.getKey();
			con.close();
			return pid;
		}
		catch(Exception e){
			return -1;
		}
	}

	@Override
	public int createPlayer(int pid, int cid) {
		try(Connection con = sql2o.open()){
			con.createQuery("insert into player(pid, cid) values(:pid, :cid)")
			.addParameter("pid", pid)
			.addParameter("cid", cid)
			.executeUpdate();
			con.close();
		}
		return 0;
	}

	@Override
	public int createCoach(int pid, int cid) {
		try(Connection con = sql2o.open()){
			int ch = (int) con.createQuery("insert into coach(coachId, cid) values(:pid, :cid)")
			.addParameter("pid", pid)
			.addParameter("cid", cid)
			.executeUpdate()
			.getKey();
			con.close();
			return ch;
		}		
	}

	@Override
	public int createInvitation(int eid, int cid) {
		try(Connection con = sql2o.open()){
			con.createQuery("insert into invitation(eid, cid, status) values(:eid, :cid, 0)")
			.addParameter("eid", eid)
			.addParameter("cid", cid)
			.executeUpdate();
			con.close();
			return 0;
		}
	}

	@Override
	public int updateInvitation(int eid, int cid, int status) {
		try(Connection con = sql2o.open()){
			con.createQuery("update invitation set status=:status where eid=:eid and cid=:cid")
			.addParameter("eid", eid)
			.addParameter("cid", cid)
			.addParameter("status", status)
			.executeUpdate();
			con.close();
			// 1 = pending. Send invites & emails
			if(status == 1){
				sendPlayerInvites(cid, eid);
			}
			return status;
		}
		catch(Exception e){
			return -2;
		}
	}

	@Override
	public int createEvent(String location, Date date, String type, Date deadline, String name, int teamcut,
			int organiser) {
		try(Connection con = sql2o.open()){
			int eid = Math.toIntExact((long) con.createQuery("insert into event(location, eventDate, eventType, deadline, name, teamcut, organiser) values "
					+ "(:location, :eventDate, :eventType, :deadline, :name, :teamcut, :organiser)")
			.addParameter("location", location)
			.addParameter("eventDate", date)
			.addParameter("eventType", type)
			.addParameter("deadline", deadline)
			.addParameter("name", name)
			.addParameter("teamcut", teamcut)
			.addParameter("organiser", organiser)
			.executeUpdate()
			.getKey());
			//creating invitations opens a new con? needed?
			//Notify ALL CLUBS
			int res = 0;
			List<Integer> clubs = con.createQuery("select cid from club").executeAndFetch(Integer.class);
			con.close();
			for(int cid : clubs){
				res = createInvitation(eid, cid);
				if(res == -2) return -2;
			}
			//con.close();
			return eid;
		}
		catch(Exception e){
			return -1;
		}
	}

	@Override
	public int updateEvent(int eid, String location, Date date, String type, Date deadline, String name, int teamcut,
			int organiser) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int removeEvent(int eid) {
		try(Connection con = sql2o.open()){
			con.createQuery("delete from event where eid=:eid")
			.addParameter("eid", eid)
			.executeUpdate();
			return 0;
		}
	}
	@Override
	public int changeEventStatus(int eid, String status){
		try(Connection con = sql2o.open()){
			con.createQuery("update event set status=:status where eid=:eid")
			.addParameter("eid", eid)
			.addParameter("status", status)
			.executeUpdate();
			return 0;
		}
		
	}

	@Override
	public List<Event> getEventsAll(int cid) {
		// note: right now invitations are only associated with clubs so all invitations will be visible to everyone in an "invited" club for now
		try (Connection conn = sql2o.open()) {
            List<Event> events = conn.createQuery("select event.* from event inner join invitation ON (event.eid = invitation.eid) where cid=:cid")
            		.addParameter("cid", cid)
                    .executeAndFetch(Event.class);
            conn.close();
            return events;
        }
	
	}
	@Override
	public Club getClub(int cid) {
		try (Connection conn = sql2o.open()) {
            Club club = conn.createQuery("select * from club where cid=:cid")
            		.addParameter("cid", cid)
                    .executeAndFetchFirst(Club.class);
            conn.close();
            return club;
        }
	}
	@Override
	public List<Club> getClubsAll() {
		try (Connection conn = sql2o.open()) {
            List<Club> clubs = conn.createQuery("select * from club")
                    .executeAndFetch(Club.class);
            conn.close();
            return clubs;
        }
	}

	@Override
	public List<Person> getPlayersAll() {
		return null;
	}

	@Override
	public List<Person> getPeopleAll() {
		try (Connection conn = sql2o.open()) {
            List<Person> people = conn.createQuery("select * from person")
                    .executeAndFetch(Person.class);
            conn.close();
            return people;
        }
	}

	@Override
	public List<Person> getCoachesAll() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<String> getInvitationsAll() {
		// Is this the same as getEventsAll? (I wrote that method as the same thing as this)
		return null;
	}

	@Override
	public List<Invitation> getInvited(int eid) {
		try(Connection con = sql2o.open()){
			List<Invitation> invites = con.createQuery("select * from invitations where eid =: eid")
					.addParameter("eid", eid)
					.executeAndFetch(Invitation.class);
			con.close();
			return invites;
		}
		catch(Exception e){
			
		
			return null;
		}
	}

	@Override
	public List<Person> getPlayersByClub(int cid) {
		try (Connection conn = sql2o.open()) {
            List<Person> people = conn.createQuery("select person.* from person inner join player on (person.pid = player.playerid) where cid:=cid")
            		.addParameter("cid", cid)
                    .executeAndFetch(Person.class);
            conn.close();
            return people;
        }
	}
	
	@Override
	public List<Person> getPlayersByAdmin(int admin) {
		try (Connection conn = sql2o.open()) {
            List<Person> people = (List<Person>) conn.createQuery("select * from person where pid in(select p.pid from player as p where p.cid = (select c.cid from club as c where admin = :admin))")
            		.addParameter("admin", admin)
                    .executeAndFetch(Person.class);
            conn.close();
            return people;
        }
	}

	@Override
	public Person getPerson(String email) {
		try (Connection conn = sql2o.open()) {
            Person person = conn.createQuery("select * from person where email = :email")
            		.addParameter("email", email)
            		.executeAndFetchFirst(Person.class);
            conn.close();
            return person;
        }
	}
	
	@Override
	public Person getPerson(int pid) {
		try (Connection conn = sql2o.open()) {
            Person person = conn.createQuery("select * from person where pid = :pid")
            		.addParameter("pid", pid)
            		.executeAndFetchFirst(Person.class);
            conn.close();
            return person;
		}
  }
    
	public Person getAdmin(String email){
		try (Connection conn = sql2o.open()) {
            Person admins = conn.createQuery("select person.* from club inner join person on club.admin = person.pid where email=:email;")
            		.addParameter("email", email)
            		.executeAndFetchFirst(Person.class);
            conn.close();
            return admins;
        }
	}
	
	@Override
	public int createClub(String name, int admin) {
		try{
			Connection con = sql2o.beginTransaction();
			int cid = (int) con.createQuery("insert into club(name, admin) values(:name, :admin)")
			.addParameter("name", name)
			.addParameter("admin", admin)
			.executeUpdate()
			.getKey();
			con.close();
			return cid;
		}
		catch (Exception e){
			return -1;
		}
	}
	
	public void setup(){
		try(Connection con = sql2o.beginTransaction()){

			String query = "create table if not exists person(pid integer NOT NULL auto_increment, name text NOT NULL, dateOfBirth date NOT NULL, " +
					"email varchar(255) unique NOT NULL, primary key (pid))";
			con.createQuery(query).executeUpdate();
			query = "create table if not exists club(cid integer NOT NULL auto_increment, name varchar(255) unique NOT NULL, admin integer NOT NULL, primary key (cid), " + 
					"constraint admin2pid foreign key (admin) references person(pid))";
			con.createQuery(query).executeUpdate();
			query = "create table if not exists coach(coachID integer, cid integer, primary key (coachID, cid), constraint coach2person foreign key (coachID) "
					+ "references person(pid) on delete cascade, constraint coachclub2club foreign key (cid) references club(cid) on delete cascade)";
			con.createQuery(query).executeUpdate();
			query = "create table if not exists player(pid integer, cid integer, primary key(pid, cid), constraint player2person foreign key (pid) references person(pid) on delete cascade, " +
					"constraint playerclub2club foreign key (cid) references club(cid) on delete cascade)";
			con.createQuery(query).executeUpdate();
			query = "create table if not exists event(eid integer NOT NULL auto_increment, location text NOT NULL, eventDate date NOT NULL, eventType text NOT NULL, " +
					"deadline date NOT NULL, name text NOT NULL, teamCut integer NOT NULL, organiser integer NOT NULL, status varchar(255) NOT NULL DEFAULT '', primary key (eid))";
			con.createQuery(query).executeUpdate();
			query = "create table if not exists invitation(eid integer, cid integer, status integer NOT NULL, primary key (eid, cid), " +
					"constraint invitationevent2event foreign key (eid) references event(eid) on delete cascade, constraint invitationclub2club foreign key (cid) references club(cid) on delete cascade)"; 
			con.createQuery(query).executeUpdate();
			query = "create table if not exists playerInvite(pid integer, cid integer, eid integer, status varchar(255) NOT NULL DEFAULT '', primary key(pid, cid, eid), constraint playerEx foreign key(pid) references player(pid) on delete cascade, " +
					"constraint eventEx foreign key (eid) references event (eid) on delete cascade, constraint clubEx foreign key (cid) references club (cid) on delete cascade)";
			con.createQuery(query).executeUpdate();
			con.commit();
			con.close();
			
		}
	}

	@Override
	public Event getEvent(int eid) {
		try(Connection con = sql2o.open()){
			Event event = (Event) con.createQuery("select * from event where eid = :eid")
			.addParameter("eid", eid)
			.executeAndFetchFirst(Event.class);
			con.close();
			return event;
			
		}
		catch(Exception e){
		return null;
		}
	}
	@Override
	public Event getEventbyName(String name) {
		try(Connection con = sql2o.open()){
			Event event = (Event) con.createQuery("select * from event where name=:name")
			.addParameter("name", name)
			.executeAndFetch(Event.class);
			con.close();
			return event;
			
		}
		catch(Exception e){
		return null;
		}
	}
	
	
	public List<Event> getEventByPerson(int pid) {
		// note: right now invitations are only associated with clubs so all invitations will be visible to everyone in an "invited" club for now
		try (Connection conn = sql2o.open()) {
            List<Event> events = conn.createQuery("select * from event where eid in (select eid from invitation where cid in (select cid from club where admin = :pid))")
            		.addParameter("pid", pid)
                    .executeAndFetch(Event.class);
            conn.close();
            return events;
		}
     }

	public Club getClubByAdminId(int pid) {
		try (Connection conn = sql2o.open()) {
            Club club = conn.createQuery("select * from club where admin=:pid")
            		.addParameter("pid", pid)
            		.executeAndFetchFirst(Club.class);
            conn.close();
            return club;
        }
	}
		
	public List<Club> getClubByPersonId(int pid) {
		try (Connection conn = sql2o.open()) {
            List<Club> clubs = conn.createQuery("select club.* from club inner join coach on (club.cid = coach.cid) inner join player on (coach.coachID = player.pid) where pid=:pid")
            		.addParameter("pid", pid)
            		.executeAndFetch(Club.class);
            conn.close();
            return clubs;
        }
	}
		
	/*		
	// NOTE: To use this, add status in the database for event (currently there is none). Otherwise, will fail
	@Override
	public int updateEventStatus(String status, int eid) {
		try(Connection con = sql2o.open()){
			con.createQuery("update event set status = :status where eid =: eid;")
			.addParameter("status", status)
			.addParameter("eid", eid)
			.executeUpdate();
			return 0;
		}
		catch(Exception e){
			return -2;
		}
	}
	*/
		
	// Player Invitation Functions
	@Override
	/** 
	 * (int cid, int eid)
	 * Send player invites to all players belonging to club with cid = cid to event with eid = eid.
	 */
	public int sendPlayerInvites(int cid, int eid){
		try(Connection con = sql2o.open()){
			Event event = getEvent(eid);
			Club club = getClub(cid);
			List<Integer> players = con.createQuery("select pid from player where cid=:cid").executeAndFetch(Integer.class);
			for(int pid : players){
				con.createQuery("insert into playerInvite(pid, cid, eid, status) values (:pid, :cid, :eid, 'pending')")
				.addParameter("pid", pid)
				.addParameter("cid", cid)
				.addParameter("eid", eid)
				.executeUpdate();
				
				Person person = getPerson(pid);
				//Send email
			}
			con.close();

		}

		return 0;
	}
	
	
	@Override
	public int updatePlayerInviteStatus(String status, int cid, int pid, int eid) {
		try(Connection con = sql2o.open()){
			con.createQuery("update playerInvite set status = :status where cid=:cid and pid=:pid and eid=:eid;")
			.addParameter("status", status)
			.addParameter("pid", pid)
			.addParameter("cid", cid)
			.addParameter("eid", eid)
			.executeUpdate();
			return 0;
		}
		catch(Exception e){
			return -2;
		}
	}
	
	@Override
	public List<PlayerInvite> getPlayerInviteByClub(int cid){
		try(Connection con = sql2o.open()){
			List<PlayerInvite> invites = con.createQuery("select * from playerInvite where cid=:cid")
			.addParameter("cid", cid)
			.executeAndFetch(PlayerInvite.class);
			con.close();
			return invites;
		}
		catch(Exception e){
			return null;
		}
		
	}
	
		
	/*
	// NOTE: To use this, add status in the database for Club (currently there is none). Otherwise, will fail
	@Override
	public int updateClubStatus(String status, int cid) {
		try(Connection con = sql2o.open()){
			con.createQuery("update club set status = :status where cid =: cid;")
			.addParameter("status", status)
			.addParameter("cid", cid)
			.executeUpdate();
			return 0;
		}
		catch(Exception e){
			return -2;
		}
	}

 	*/
		
	public void deletePlayerByEmail(String email){
		try(Connection con = sql2o.open()){
			con.createQuery("delete from person where email=:email;")
			.addParameter("email", email)
			.executeUpdate();
		}
		catch(Exception e){		
			
		}
	}
		
	public void deletePlayerById(int pid){
		try(Connection con = sql2o.open()){
			con.createQuery("delete from person where pid=:pid;")
			.addParameter("pid", pid)
			.executeUpdate();
		}
		catch(Exception e){		
			
		}
	}
	/*
	 * players of an admin.
	 */

	public List<Club> getTeamsEventStatus(int eid) {
		try (Connection conn = sql2o.open()) {
            List<Club> clubs = (List<Club>) conn.createQuery("select c.*,i.status from club as c join invitation as i on c.cid=i.cid where i.eid = :eid")
            		.addParameter("eid", eid)
            		.executeAndFetch(Club.class);
            conn.close();
            return clubs;
        }
	}

	public List<Person> getPlayersEventStatus(int admin, int eid) {
		try (Connection conn = sql2o.open()) {
            List<Person> people = (List<Person>) conn.createQuery("select p.*,i.status from person join playerInvite as i on p.pid=i.pid as p where p.pid in(select p.pid from player as p where p.cid = (select c.cid from club as c where admin = :admin))where i.eid = :eid")
            		.addParameter("eid", eid)
            		.addParameter("admin", admin)
                    .executeAndFetch(Person.class);
            conn.close();
            return people;
        }
	}
	
}
