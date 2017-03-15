

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 * The PatientRecord class stores information about a particular patient.
 * Attributes such as name, birthdate, and healthcard number.
 * As well as save a particular visit from a patient to a text file.
 * 
 * @author Raymond Zheng
 * @author Derek Wong
 * @version 1.0 Nov 8, 2014
 */
public class PatientRecord {
	
	/**
	 * saveLocation is the directory in which the text file outputs will be stored.
	 */
	private String saveLocation;
	
	/**
	 * Object of type VisitManager
	 */
	private VisitManager visits;
	
	/**
	 * name is an array that stores the patient's name.
	 */
	private String[] name; //first last
	
	/**
	 * birthDate is an array that stores the patient's date of birth.
	 */
	private String[] birthDate; //yyyy mm dd
	
	/**
	 * hcNum is the health card number of the patient.
	 */
	int hcNum; //health card number
	
	/**
	 * The PatientRecord class stores information about a particular patient.
	 * Attributes such as name, birthdate, and healthcard number.
	 * As well as save a particular visit from a patient to a text file.
	 */
	PatientRecord(String[] name, String[] birthDate, int hcNum) {
		this.visits = loadVisits(); //cloning to avoid aliasing
		this.name = name.clone(); //...
		this.birthDate = birthDate.clone(); //...
		this.hcNum = hcNum;
		this.saveLocation = "src/e1soln/visits_" + this.hcNum + ".bin";
	}
	
	/**
	 * The toString() method is the overwritten toString() method that is inherited from 
	 * the Object class.
	 * 
	 * @return 				Representation of the patient's name/birthdate/health card num
	 */
	public String toString() {
		return name[0] + " " + name[1] + ", " + birthDate[0] + "-" + birthDate[1] + "-" + birthDate[2] + " " + hcNum;
	}
	
	/**
	 * The addVisit method adds a new visit for a patient to be stored later.
	 * 
	 * @param arrival		Date and time of the patient's arrival
	 * @param visit			Object of type VisitRecord
	 * @return 				void
	 */
	public void addVisit(String arrival, VisitRecord visit) {
		visits.addVisit(arrival, visit);
		saveVisits();
	}
	
	/**
	 * The saveVisits method is responsible for saving a patient's visit to a text file.
	 * 
	 * @return				void
	 */
	private void saveVisits() {
		try {
			File file = new File(saveLocation);
			file.createNewFile();
		} catch (IOException ex) {
			System.err.println(ex);
		}
        ObjectOutputStream oos = null;
        try {
            oos = new ObjectOutputStream(new FileOutputStream(saveLocation));
            oos.writeObject(this.visits);
            oos.close();
        } catch (Exception ex) {
            System.err.println(ex);
        }
    }
	
	/**
	 * The loadVisits loads up a particular visit record from a patient stored on a text file.
	 * 
	 * @return				Object of type VisitManager
	 */
	private VisitManager loadVisits() {
        ObjectInputStream ois = null;
        VisitManager v = new  VisitManager();
        try {
            ois = new ObjectInputStream(new FileInputStream(saveLocation));
            v = (VisitManager) ois.readObject();
            ois.close();
        } catch (Exception ex) {
            System.err.println(ex);
        }
        return v;
    }
	
}
