import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;

/**
 * The VisitManager class is responsible for managing a particular visit of a patient by 
 * arrival time.
 * 
 * @author Jurgen Aliaj
 * @version 1.0 Nov 8, 2014
 */
public class VisitManager implements Serializable {
	
	/**
	 * Map visits to store new visits by a patient.
	 */
	private Map<String, VisitRecord> visits;
	
	/**
	 * The VisitManager class is responsible for managing a particular visit of a patient by 
	 * arrival time.
	 */
	VisitManager() {
		visits = new TreeMap<String, VisitRecord>();
	}
	
	/**
	 * Method to add a patient's visit to the records.
	 * 
	 * @param arrival			Time of patient's arrival.
	 * @param visit				Object of type VisitRecord (map).
	 */
	public void addVisit(String arrival, VisitRecord visit) {
		visits.put(arrival, visit);
	}

}
