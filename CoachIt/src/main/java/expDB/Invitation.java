package expDB;


/**
 * 
 * @author chrion
 *Invitation for a club to an event.
 *
 *Status 
 *	-1: rejected
 *	 0: invited
 *	 1: pending
 *	 2: accepted
 */
public class Invitation {
	
	public int getEid() {
		return eid;
	}
	public void setEid(int eid) {
		this.eid = eid;
	}
	public int getCid() {
		return cid;
	}
	public void setCid(int cid) {
		this.cid = cid;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	private int eid;
	private int cid;
	private int status;
	
}
