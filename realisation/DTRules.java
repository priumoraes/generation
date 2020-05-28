/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * The A* search heuristic is determined by the output from a Decision Tree, which uses a Common Core Standards dataset annotated by reading level. The elements used to 
 * learn the different reading levels are the occurrences and positions of parts of speech, vocabulary sophistication and grammatical patterns.
 */
package edu.udel.eecis.teahouse.sight;


import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @author Priscilla
 *
 */
public class DTRules {

	private List<HashMap<String, Pair<Integer, Integer>>> grade45Rules = new ArrayList<HashMap<String,Pair<Integer,Integer>>>();
	private List<HashMap<String, Pair<Integer, Integer>>> grade68Rules = new ArrayList<HashMap<String,Pair<Integer,Integer>>>();
	private List<HashMap<String, Pair<Integer, Integer>>> grade910Rules = new ArrayList<HashMap<String,Pair<Integer,Integer>>>();
	private List<HashMap<String, Pair<Integer, Integer>>> grade11ccRules = new ArrayList<HashMap<String,Pair<Integer,Integer>>>();
	
	private Map<HashMap,Integer> confidence = new HashMap<HashMap,Integer>(); 

	public DTRules() throws IOException{

    	try(BufferedReader br = new BufferedReader(new FileReader("input/grade45Rules.txt"))) {
    		//System.out.println("--------------- Creating 4 5 rules -----------------------");
            String line = br.readLine();
            HashMap<String,Pair<Integer,Integer>> aMap = new HashMap<String,Pair<Integer,Integer>>();
            while (line != null) {
            	while ((line != "---") && (line != null)){
            		String[] constituents = line.split("\\s");
            		if ((constituents.length > 1))
	            	{
	            		Pair<Integer,Integer> aPair = new Pair<Integer,Integer>(0,0);	
	            		aPair.setFirst(Integer.valueOf(constituents[1]));
	            		aPair.setSecond(Integer.valueOf(constituents[2]));
	            		aMap.put(constituents[0], aPair);
	            		//System.out.println("Added "+constituents[0]+" to map.");
	            	}
            		else if (line.matches("\\d+"))
            		{
            			//System.out.println("Found a number, adding "+line+" to confidence map.");
            			confidence.put(aMap, Integer.valueOf(line));
            			grade45Rules.add(aMap);
            			aMap = new HashMap<String,Pair<Integer,Integer>>();
                    	//System.out.println("Size of list: "+grade45Rules.size());
            		}
            		line = br.readLine();
            	}
            	line = br.readLine();
            }
        }
    	setGrade45Rules(grade45Rules);
    	
    	try(BufferedReader br = new BufferedReader(new FileReader("input/grade68Rules.txt"))) {
    		//System.out.println("--------------- Creating 6 8 rules -----------------------");
            String line = br.readLine();
            HashMap<String,Pair<Integer,Integer>> aMap = new HashMap<String,Pair<Integer,Integer>>();
            while (line != null) {
            	while ((line != "---") && (line != null)){
            		String[] constituents = line.split("\\s");
            		if ((constituents.length > 1))
	            	{
	            		Pair<Integer,Integer> aPair = new Pair<Integer,Integer>(0,0);	
	            		aPair.setFirst(Integer.valueOf(constituents[1]));
	            		aPair.setSecond(Integer.valueOf(constituents[2]));
	            		aMap.put(constituents[0], aPair);
	            		//System.out.println("Added "+constituents[0]+" to map.");
	            	}
            		else if (line.matches("\\d+"))
            		{
            			//System.out.println("Found a number, adding "+line+" to confidence map.");
            			confidence.put(aMap, Integer.valueOf(line));
            			grade68Rules.add(aMap);
            			aMap = new HashMap<String,Pair<Integer,Integer>>();
                    	//System.out.println("Size of list: "+grade68Rules.size());
            		}
            		line = br.readLine();
            	}
            	line = br.readLine();
            }
        }
    	setGrade68Rules(grade68Rules);
    	
    	try(BufferedReader br = new BufferedReader(new FileReader("input/grade910Rules.txt"))) {
    		//System.out.println("--------------- Creating 9 10 rules -----------------------");
            String line = br.readLine();
            HashMap<String,Pair<Integer,Integer>> aMap = new HashMap<String,Pair<Integer,Integer>>();
            while (line != null) {
            	while ((line != "---") && (line != null)){
            		String[] constituents = line.split("\\s");
            		if ((constituents.length > 1))
	            	{
	            		Pair<Integer,Integer> aPair = new Pair<Integer,Integer>(0,0);	
	            		aPair.setFirst(Integer.valueOf(constituents[1]));
	            		aPair.setSecond(Integer.valueOf(constituents[2]));
	            		aMap.put(constituents[0], aPair);
	            		//System.out.println("Added "+constituents[0]+" to map.");
	            	}
            		else if (line.matches("\\d+"))
            		{
            			//System.out.println("Found a number, adding "+line+" to confidence map.");
            			confidence.put(aMap, Integer.valueOf(line));
            			grade910Rules.add(aMap);
            			aMap = new HashMap<String,Pair<Integer,Integer>>();
                    	//System.out.println("Size of list: "+grade910Rules.size());
            		}
            		line = br.readLine();
            	}	
            	line = br.readLine();
            }
        }
    	setGrade910Rules(grade910Rules);
    	
    	try(BufferedReader br = new BufferedReader(new FileReader("input/grade11ccRules.txt"))) {
    		//System.out.println("--------------- Creating 11 cc rules -----------------------");
            String line = br.readLine();
            HashMap<String,Pair<Integer,Integer>> aMap = new HashMap<String,Pair<Integer,Integer>>();
            while (line != null) {
            	while ((line != "---") && (line != null)){
            		String[] constituents = line.split("\\s");
            		if ((constituents.length > 1))
	            	{
	            		Pair<Integer,Integer> aPair = new Pair<Integer,Integer>(0,0);	
	            		aPair.setFirst(Integer.valueOf(constituents[1]));
	            		aPair.setSecond(Integer.valueOf(constituents[2]));
	            		aMap.put(constituents[0], aPair);
	            		//System.out.println("Added "+constituents[0]+" to map.");
	            	}
            		else if (line.matches("\\d+"))
            		{
            			//System.out.println("Found a number, adding "+line+" to confidence map.");
            			confidence.put(aMap, Integer.valueOf(line));
            			grade11ccRules.add(aMap);
            			aMap = new HashMap<String,Pair<Integer,Integer>>();
                    	//System.out.println("Size of list: "+grade11ccRules.size());
            		}
            		line = br.readLine();
            	}
            	line = br.readLine();
            }
        }
    	setGrade11ccRules(grade11ccRules);
    	
    	setConfidence(confidence);
	}

	/**
	 * @return the grade45Rules
	 */
	List<HashMap<String, Pair<Integer, Integer>>> getGrade45Rules() {
		return grade45Rules;
	}

	/**
	 * @param grade45Rules the grade45Rules to set
	 */
	private void setGrade45Rules(
			List<HashMap<String, Pair<Integer, Integer>>> grade45Rules) {
		this.grade45Rules = grade45Rules;
	}

	/**
	 * @return the grade68Rules
	 */
	List<HashMap<String, Pair<Integer, Integer>>> getGrade68Rules() {
		return grade68Rules;
	}

	/**
	 * @param grade68Rules the grade68Rules to set
	 */
	private void setGrade68Rules(
			List<HashMap<String, Pair<Integer, Integer>>> grade68Rules) {
		this.grade68Rules = grade68Rules;
	}

	/**
	 * @return the grade910Rules
	 */
	List<HashMap<String, Pair<Integer, Integer>>> getGrade910Rules() {
		return grade910Rules;
	}

	/**
	 * @param grade910Rules the grade910Rules to set
	 */
	private void setGrade910Rules(
			List<HashMap<String, Pair<Integer, Integer>>> grade910Rules) {
		this.grade910Rules = grade910Rules;
	}

	/**
	 * @return the grade11ccRules
	 */
	List<HashMap<String, Pair<Integer, Integer>>> getGrade11ccRules() {
		return grade11ccRules;
	}

	/**
	 * @param grade11ccRules the grade11ccRules to set
	 */
	private void setGrade11ccRules(
			List<HashMap<String, Pair<Integer, Integer>>> grade11ccRules) {
		this.grade11ccRules = grade11ccRules;
	}

	/**
	 * @return the confidence
	 */
	public Map<HashMap, Integer> getConfidence() {
		return confidence;
	}

	/**
	 * @param confidence the confidence to set
	 */
	private void setConfidence(Map<HashMap, Integer> confidence) {
		this.confidence = confidence;
	}
	
	
	
}

