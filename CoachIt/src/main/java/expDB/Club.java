package expDB;

public class Club {
	private int cid;
	private String name;
	private int admin;
	private int status;
	
	//TODO: import Lombok / add to pom for @Data annotation use and remove everything below afterwards

	public int getCid() {
		return cid;
	}
	public int getStatus() {
		return status;
	}
	public void setCid(int cid) {
		this.cid = cid;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public Integer getAdmin() {
		return admin;
	}
	public void setAdmin(Integer admin) {
		this.admin = admin;
	}
}
