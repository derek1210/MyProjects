

import java.io.IOException;

/**
 * The PatientArrivalQueue extends the abstract class PatientQueue, and prioritizes patients
 * by their arrival time.
 * 
 * @author Jurgen Aliaj
 * @version 1.0 Nov 8, 2014
 */
public class PatientArrivalQueue extends PatientQueue {
	
	/**
	 * The PatientArrivalQueue extends the abstract class PatientQueue, and prioritizes patients
	 * by their arrival time.
	 * 
	 * @param filename			Directory of file from which the patient data will be read.
	 */
	PatientArrivalQueue(String filename) throws IOException {
		super(filename);
	}

	@Override
	/**
	 * Implementation of the abstract method comparePatients.
	 * 
	 * @param a				First patient for comparison.
	 * @param b				Second patient for comparison.
	 * @return <to be implemented>
	 */
	public int comparePatients(PatientRecord a, PatientRecord b) {
		return 0; // We'll implement this in later stages of the project
	}
	
}
