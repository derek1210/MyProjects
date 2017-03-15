import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;

/**
 * The VisitRecord class is responsible for storing a patient's visit record in a map based
 * on the date of the visit. Information stored: vital signs/symptoms.
 * 
 * @author Jurgen Aliaj
 * @version 1.0 Nov 8, 2014
 */
public class VisitRecord implements Serializable {
	
	/**
	 * Map vitals stores a patient's visit record based on the date of visit.
	 */
	private Map<String, TreeMap<String, Double>> vitals;
	
	/**
	 * The VisitRecord class is responsible for storing a patient's visit record in a map based
	 * on the date of the visit. Information stored: vital signs/symptoms.
	 */
	VisitRecord(){
		this.vitals = new TreeMap<String, TreeMap<String, Double>>();
	}
	
	/**
	 * This method updates the vital signs and symptoms of a particular patient.
	 * 
	 * @param time				Date and time of visit.
	 * @param systolic			Systolic blood pressure info.
	 * @param diastolic			Diastolic blood pressure info.
	 * @param temp				Patient's temperature.
	 * @param bpm				Patient's heart rate.
	 * @return 					void.
	 */
	public void updateVitals(String time, double systolic, double diastolic, double temp, double bpm){
		this.vitals.put(time, new TreeMap<String, Double>());
		this.vitals.get(time).put("Systolic Blood Pressure", systolic);
		this.vitals.get(time).put("Diastolic Blood Pressure", diastolic);
		this.vitals.get(time).put("Temperature", temp);
		this.vitals.get(time).put("Heart Rate", bpm);
	}
	
	/**
	 * Getter method for returning the vital signs and symptoms of a patient based on
	 * visit date.
	 * 
	 * @param 					Date of visit.
	 * @return					Returns a map of vital signs and symptoms.
	 */
	public Map<String, Double> getVitals(String time) {
		return this.vitals.get(time);
	}
	
	// Overload
	/**
	 * Getter method for returning the vital signs and symptoms of a patient (all visits).
	 * 
	 * @return					Returns a map of visit date + map vital signs and symptoms.
	 */
	public Map<String, TreeMap<String, Double>> getVitals() {
		return this.vitals;
	}
	
	/**
	 * Main method for testing out the VisitRecord class.
	 */
	public static void main (String[] args) {
		VisitRecord k = new VisitRecord();
		k.updateVitals("12 pm", 90, 110, 37, 75);
		System.out.println(k.getVitals("12 pm"));
	}
	
}

