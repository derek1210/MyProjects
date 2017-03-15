import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Comparator;
import java.util.PriorityQueue;
import java.util.Queue;
import java.util.Scanner;

/**
 * The abstract PetientQueue class is for arranging patients who have arrived in a priority 
 * queue.
 * 
 * @author Jurgen Aliaj
 * @author Alex Chernous
 * @version 1.0 Nov 8, 2014
 */
public abstract class PatientQueue {
	
	/**
	 * Queue of type PatientRecord to store new patients.
	 */
	protected Queue<PatientRecord> patients;
	
	/**
	 * 
	 */
	Comparator<PatientRecord> comparator = new Comparator<PatientRecord>() {
		
		@Override
		/**
		 * 
		 */
		public int compare(PatientRecord a, PatientRecord b) {
			return comparePatients(a, b);
		}
	};
	
	/**
	 * The abstract PetientQueue class is for arranging patients who have arrived in a priority 
	 * queue.
	 * Populates the patient queue from a text file.
	 * 
	 * @param path			Directory of file from which the patient data will be read.
	 * @throws IOException.
	 */
	PatientQueue(String path) throws IOException {
		this.patients = new PriorityQueue<PatientRecord>(50, comparator);
		File file = new File(path);
		//populates from file, otherwise a new file is created
		if (file.exists()) 
			this.populate(path);
		else
			file.createNewFile();
	}
	
	/**
	 * Populates patient data from existing text file using a scanner.
	 * 
	 * @param path			Directory of file.
	 */
	protected void populate(String path) throws FileNotFoundException {
		Scanner scanner = new Scanner(new FileInputStream(path));
		String[] patientData;
		//populates string array from a file using a scanner
		while(scanner.hasNextLine()) {
			patientData = scanner.nextLine().split(",");
			int healthCardNumber = Integer.parseInt(patientData[0]);
			String[] name = patientData[1].split(" ");
			String[] birthDate = patientData[2].split("-");
			this.add(new PatientRecord(name, birthDate, healthCardNumber));
		}
		scanner.close();	
	}
	
	/**
	 * Method that adds a patient Object to the patients Queue.
	 * 
	 * @return				void.
	 */
	public void add(PatientRecord patient) {
		this.patients.add(patient);
	}
	
	/**
	 * The toString() method is the overwritten toString() method that is inherited from 
	 * the Object class.
	 * 
	 * @return displayPatients		The representation of the patient queue.
	 */
	public String toString() {
		String displayPatients = "";
		//loops through patient queue and represents info on screen
		for(PatientRecord i: this.patients)
			displayPatients += i.toString() + "\n";
		return displayPatients;
	}
	
	/**
	 * An abstract method for comparing two patients.
	 * 
	 * @param a				First patient for comparison.
	 * @param b				Second patient for comparison.
	 * @return				Abstract, no return.
	 */
	public abstract int comparePatients(PatientRecord a, PatientRecord b);
	
	/**
	 * Linear search function for finding a patient's record based on their healthcard number.
	 * 
	 * @param healthNum		The healthcard number of a particular patient.
	 * @return i			Returns the Object PatientRecord.
	 */
	public <PatientRecord> findPatient(int healthNum){
		//loops to find patient based on health card number
		for(PatientRecord i: this.patients){
			if (i.hcNum == healthNum)
					return i;
		}
		return null
	}
	
}
