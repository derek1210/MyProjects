package expDB;

import java.util.Date;

public class Person {
	private int pid;
	private String name;
	private Date dateOfBirth;
	private String email;
	private int status;

	public int getPid() {
		return pid;
	}
	public int getStatus() {
		return status;
	}
	public void setPid(int pid) {
		this.pid = pid;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public Date getDateOfBirth() {
		return dateOfBirth;
	}
	public void setDateOfBirth(Date dateOfBirth) {
		this.dateOfBirth = dateOfBirth;
	}
	public String getEmail() {
		return email;
	}
	public void setEmail(String email) {
		this.email = email;
	}
}
