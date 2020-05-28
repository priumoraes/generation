/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * This class is responsible for settign the appropriate reading level vocabulary for realization.
 */
package edu.udel.eecis.teahouse.sight;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeMap;

import net.didion.jwnl.JWNL;

import org.tartarus.snowball.ext.PorterStemmer;

/**
 * @author Priscilla
 *
 */
/**
 * @author Priscilla
 *
 */
public class VocabularyByGrade {

	static Set<String> vocab45 = new HashSet<String>();
	static Set<String> vocab68 = new HashSet<String>();
	static Set<String> vocab910 = new HashSet<String>();
	static Set<String> vocab11cc = new HashSet<String>();
	
	static Set<String> newVocab45 = new HashSet<String>();
	static Set<String> newVocab68 = new HashSet<String>();
	static Set<String> newVocab910 = new HashSet<String>();
	static Set<String> newVocab11cc = new HashSet<String>();
	
	Set<String> context = new HashSet<String>();
	
	Map<String, Integer> ngrams = new HashMap<String, Integer>();
	
	public static void main(String args[]) throws FileNotFoundException{
		try {
			List<Set<String>> allGradeVocabs = new ArrayList<Set<String>>();
			instantiateNewVocabulary45();
			instantiateNewVocabulary68();
			instantiateNewVocabulary910();
			instantiateNewVocabulary11cc();
			allGradeVocabs.add(getNewVocab45());
			allGradeVocabs.add(getNewVocab68());
			allGradeVocabs.add(getNewVocab910());
			allGradeVocabs.add(getNewVocab11cc());
			
			//defineLexiconByGradeLevel("consist", allGradeVocabs);
			for (String synonym : instantiateTempSynonymList("input/synonymsToExpand.txt")){
				System.out.println("Working on synonyms of "+synonym);
				//collectSynonyms(instantiateTempSynonymList("input/smallContext.txt"), synonym);
				getTopSynonyms(synonym);
				defineLexiconByGradeLevel(synonym, allGradeVocabs);
			}
				
			//getTopSynonyms("steepness");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public VocabularyByGrade() throws FileNotFoundException{
		try {
			instantiateVocabulary45();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			instantiateVocabulary68();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			instantiateVocabulary910();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			instantiateVocabulary11cc();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			instantiateContext();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

    }
    
	private static void instantiateVocabulary45() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/vocabularyGrade45.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))
            		vocab45.add(line.toLowerCase());
                line = br.readLine();
            }
        }
    }
    
    private static void instantiateVocabulary68() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/vocabularyGrade68.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))
            		vocab68.add(line.toLowerCase());
                line = br.readLine();
            }
        }
    }
    
    private static void instantiateVocabulary910()throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/vocabularyGrade910.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))
            		vocab910.add(line.toLowerCase());
                line = br.readLine();
            }
        }
    }
    
    private static void instantiateVocabulary11cc()throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/vocabularyGrade11cc.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))
            		vocab11cc.add(line.toLowerCase());
                line = br.readLine();
            }
        }
    }
    
    
  //----- new vocabulary definition starts here -----//
	private static void instantiateNewVocabulary45() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/newVocabularyGrade45.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	String[] words = line.split("\\s+");
            	for (int i = 0; i < words.length; i++) {
            	    // You may want to check for a non-word character before blindly
            	    // performing a replacement
            	    // It may also be necessary to adjust the character class
            	    words[i] = words[i].replaceAll("[^\\w]", "");
            	    newVocab45.add(words[i].toLowerCase());
            	}
//            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))          		
                line = br.readLine();
            }
        }
    }
	
	
	private static void instantiateNewVocabulary68() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/newVocabularyGrade68.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	String[] words = line.split("\\s+");
            	for (int i = 0; i < words.length; i++) {
            	    // You may want to check for a non-word character before blindly
            	    // performing a replacement
            	    // It may also be necessary to adjust the character class
            	    words[i] = words[i].replaceAll("[^\\w]", "");
            	    newVocab68.add(words[i].toLowerCase());
            	}
//            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))          		
                line = br.readLine();
            }
        }
    }
	
	
	private static void instantiateNewVocabulary910() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/newVocabularyGrade910.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	String[] words = line.split("\\s+");
            	for (int i = 0; i < words.length; i++) {
            	    // You may want to check for a non-word character before blindly
            	    // performing a replacement
            	    // It may also be necessary to adjust the character class
            	    words[i] = words[i].replaceAll("[^\\w]", "");
            	    newVocab910.add(words[i].toLowerCase());
            	}
//            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))          		
                line = br.readLine();
            }
        }
    }
	
	
	private static void instantiateNewVocabulary11cc() throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/newVocabularyGrade11cc.txt"))) {
            String line = br.readLine();
            while (line != null) {
            	String[] words = line.split("\\s+");
            	for (int i = 0; i < words.length; i++) {
            	    // You may want to check for a non-word character before blindly
            	    // performing a replacement
            	    // It may also be necessary to adjust the character class
            	    words[i] = words[i].replaceAll("[^\\w]", "");
            	    newVocab11cc.add(words[i].toLowerCase());
            	}
//            	if ((!(line.matches("^\\d+(\\.\\d+)?"))) && (line.length() > 1))          		
                line = br.readLine();
            }
        }
    }
	//----- new vocabulary definition ends here -----//
	
    private void instantiateContext()throws FileNotFoundException, IOException{
    	try(BufferedReader br = new BufferedReader(new FileReader("input/context.txt"))) {
            String line = br.readLine();
            while (line != null) {
           		context.add(line.toLowerCase());
                line = br.readLine();
            }
        }
    }


	/**
	 * @return the vocab45
	 */
	public static Set<String> getVocab45() {
		return vocab45;
	}

	/**
	 * @param vocab45 the vocab45 to set
	 */
	public void setVocab45(Set<String> vocab45) {
		this.vocab45 = vocab45;
	}

	/**
	 * @return the vocab68
	 */
	public static Set<String> getVocab68() {
		return vocab68;
	}

	/**
	 * @param vocab68 the vocab68 to set
	 */
	public void setVocab68(Set<String> vocab68) {
		this.vocab68 = vocab68;
	}

	/**
	 * @return the vocab910
	 */
	public static Set<String> getVocab910() {
		return vocab910;
	}

	/**
	 * @param vocab910 the vocab910 to set
	 */
	public void setVocab910(Set<String> vocab910) {
		this.vocab910 = vocab910;
	}

	/**
	 * @return the vocab11cc
	 */
	public static Set<String> getVocab11cc() {
		return vocab11cc;
	}

	/**
	 * @param vocab11cc the vocab11cc to set
	 */
	public void setVocab11cc(Set<String> vocab11cc) {
		this.vocab11cc = vocab11cc;
	}

	/**
	 * @return the ngrams
	 */
	public Map<String, Integer> getNgrams() {
		return ngrams;
	}

	
	/**
	 * @return the context
	 */
	public Set<String> getContext() {
		return context;
	}

	/**
	 * @param context the context to set
	 */
	public void setContext(Set<String> context) {
		this.context = context;
	}

	
	/**
	 * @param tempSynonymList the tempSynonymList to set
	 * @throws IOException 
	 * @throws FileNotFoundException 
	 */
	public static Set<String> instantiateTempSynonymList(String fileName) throws FileNotFoundException, IOException {
		Set<String> tempSynonymList = new HashSet<String>();
    	try(BufferedReader br = new BufferedReader(new FileReader(fileName))) {
            String line = br.readLine();
            while (line != null) {
            	tempSynonymList.add(line.toLowerCase());
                line = br.readLine();
            }
        }
		return tempSynonymList;
	}
	
	//This method is for the th index only. It reads the original ngram format and compacts them the same
	//way the python script does
	public static void compactThNgrams() throws FileNotFoundException, IOException{
		File dir = new File("L:\\ngrams\\all\\thRenamed");
    	File[] directoryListing = dir.listFiles();
    	String[] tempNgram = null;
    	String[] tempNgramCounts = null;
    	
    	Map<String, Integer> thNgrams = new HashMap<String, Integer>();
    	

    	if (directoryListing != null) {
			for (File child : directoryListing) {
				File file1 = new File("L:\\ngrams\\th\\compacted\\"+child.getName());
				// if file doesnt exists, then create it
				if (!file1.exists()) {
					file1.createNewFile();
				}
				
				FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
				BufferedWriter bw1 = new BufferedWriter(fw1);
				
				System.out.println("Reading file "+child);
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
		            	tempNgram = line.split("\\t\\d{4}\\t");
		            	if (tempNgram.length > 1){
			            	tempNgramCounts = tempNgram[1].trim().split("\\s");

			            	if (thNgrams.containsKey(tempNgram[0].trim()))
			            		thNgrams.put(tempNgram[0].trim(), thNgrams.get(tempNgram[0].trim())+Integer.valueOf(tempNgramCounts[0].trim()));
			            	else
			            		thNgrams.put(tempNgram[0].trim(), Integer.valueOf(tempNgramCounts[0].trim()));
		            	}
		                line = br.readLine();
		            }
		        }
		    	for (String anEntry : thNgrams.keySet()){
		    		bw1.write(anEntry+" |count: "+thNgrams.get(anEntry));
		    		bw1.write("\n");
		    	}
		    	thNgrams.clear();
				bw1.close();
			}

    	}
	}


	//This method is just for creating one document for each ngram for indexing and searching using Lucene
	public void createOneNgramPerFile() throws FileNotFoundException, IOException{
		File dir = new File("L:\\ngrams\\all\\group1");
    	File[] directoryListing = dir.listFiles();

    	int counter = 0;

    	if (directoryListing != null) {
			for (File child : directoryListing) {
			
				System.out.println("Reading file "+child);
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
						File file1 = new File("L:\\ngrams\\lucene\\docs\\ngramsPerFile\\"+child.getName()+"_"+counter+".txt");
						// if file doesnt exists, then create it
						if (!file1.exists()) {
							file1.createNewFile();
						}
						
						FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
						BufferedWriter bw1 = new BufferedWriter(fw1);
						bw1.write(line);
						bw1.close();
						counter++;
						line = br.readLine();
		            }
		        }
		    	counter = 0;
			}

    	}
	}
	
	//This method is just for renaming the documents in a directory (since Lucene only indexes with a specific extension
	//and the file splitter software adds a number to the end of the file name)
	public static void renameDirFiles() throws FileNotFoundException, IOException{
		File dir = new File("L:\\ngrams\\th");
    	File[] directoryListing = dir.listFiles();

    	if (directoryListing != null) {
			for (File child : directoryListing) {	
				System.out.println("Reading file "+child);
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
					File file1 = new File("L:\\ngrams\\all\\thRenamed\\"+child.getName()+".txt");
					if(file1.exists()) throw new java.io.IOException("file exists");
					String line = br.readLine();
					FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
					
					BufferedWriter bw1 = new BufferedWriter(fw1);
					while (line != null) {
						bw1.write(line);
						bw1.write("\n");
						line = br.readLine();
					}
					bw1.close();
		        }
			}
    	}
	}
	
	/**
	 * @param lexicalContext - the set of default concepts that create our context.
	 * 	the main idea of this method is to go through all the Google 5-grams and return the
	 * 	count of the number of times 5-grams contained each word in the context and the synonyms
	 *  being analyzed. So, to define the set of synonyms that should be considered as a relevant synset
	 *  in our domain, I get each word of the synonymList and each word of the context set and check how
	 *  many 5-grams contain both (and the 5-grams counts). The output of this method is a map that 
	 *  contains as key the synonym and as value the total count of 5-grams with any other word from 
	 *  my context list. A threshold should be applied later in order to define the final synset. 
	 *  Due to performance issues, I am currently saving the results (synonyms found and counts) to
	 *  files. Then I can later read the files and compile the results.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public static void collectSynonyms(Set<String> lexicalContext, String concept) throws FileNotFoundException, IOException{
    	File dir = new File("L:\\ngrams\\lucene\\docs\\filtered_pos_wordmatch_smallContext");
    	File[] directoryListing = dir.listFiles();
    	String[] tempNgram = null;
    	
    	//loads the set of synonyms for a concept collected at thesaurus.com
    	Set<String> tempSynonyms = instantiateTempSynonymList("input/synonyms/"+concept+"_synonyms.txt");
    	String ngram = "";
    	String tempLexicon = "";
    	String tempSynonym = "";
    	
    	Map<String, Integer> ngrams = new HashMap<String, Integer>();
    	Map<String, Integer> foundSynonyms = new HashMap<String, Integer>();
    	
    	if (directoryListing != null) {
			for (File child : directoryListing) {			
				System.out.println("Reading file "+child);
				File file1 = new File("L:\\ngrams\\output\\filtered_pos_wordmatch_smallContext\\"+concept+"_synonyms\\"+child.getName());
				File file2 = new File("L:\\ngrams\\output\\filtered_pos_wordmatch_smallContext\\"+concept+"_ngrams\\"+child.getName());
				// if file doesnt exists, then create it
				if (!file1.exists()) {
					file1.createNewFile();
				}
				if (!file2.exists()) {
					file2.createNewFile();
				}
				FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
				BufferedWriter bw1 = new BufferedWriter(fw1);
				
				FileWriter fw2 = new FileWriter(file2.getAbsoluteFile());
				BufferedWriter bw2 = new BufferedWriter(fw2);
				
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
		            	tempNgram = line.split("\\|count:");
		            	//System.out.println("First element: "+tempNgram[0]+" and second element is "+tempNgram[1]);
		            	for (String lexicon : lexicalContext){
		            		tempLexicon = " "+lexicon+" ";
		            		for (String synonym : tempSynonyms){
		            			tempSynonym = " "+synonym+" ";
		            			ngram = " "+tempNgram[0].toLowerCase().trim()+" ";
		            			if ((ngram.contains(tempSynonym)) && (ngram.contains(tempLexicon)) && (!(synonym.equalsIgnoreCase(lexicon)))){
	            					bw2.write(line);
	            					bw2.write("\n");
		            				if (foundSynonyms.containsKey(synonym))
		            					foundSynonyms.put(synonym, foundSynonyms.get(synonym) + Integer.valueOf(tempNgram[1].trim()));
		            				else
		            					foundSynonyms.put(synonym, Integer.valueOf(tempNgram[1].trim()));
		            			}
		            		}
		            	}
		            				
		                line = br.readLine();
		            }
		    	}
		    	for (String anEntry : foundSynonyms.keySet()){
		    		bw1.write(anEntry+" "+foundSynonyms.get(anEntry));
		    		bw1.write("\n");
		    	}

				bw1.close();
				bw2.close();
				System.out.println("Number of found synonyms is: "+foundSynonyms.size());
				foundSynonyms.clear();
			}
    	}

	}
    
	
	/**
	 * @param lexicalContext - the name of the lexical item that we want to read the synonyms and
	 * 	the frequencies for. It will iterate over all the generated file counts and print 
	 * (for now) them ranked. It can also be changed to return the top n synonyms.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public void filterNgramsByContext(Set<String> lexicalContext) throws FileNotFoundException, IOException{
    	File dir = new File("L:\\ngrams\\lucene\\docs\\forFiltering");
    	File[] directoryListing = dir.listFiles();
    	String[] tempNgram = null;
    	String ngram = "";
    	String tempLexicon = "";
    	
    	if (directoryListing != null) {
			for (File child : directoryListing) {			
				System.out.println("Reading file "+child);
				File file1 = new File("L:\\ngrams\\lucene\\docs\\filtered\\"+child.getName());
				// if file doesnt exists, then create it
				if (!file1.exists()) {
					file1.createNewFile();
				}

				FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
				BufferedWriter bw1 = new BufferedWriter(fw1);
				
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
		            	//tempNgram = line.split("\\|count:");
		            	for (String lexicon : lexicalContext){
		            		//tempLexicon = lexicon;
	            			//tempLexicon = " "+tempLexicon+" ";
	            			//System.out.println(tempLexicon);
	            			if ((line.contains(lexicon))){
            					bw1.write(line);
            					bw1.write("\n");
	            			}
	            		}
		                line = br.readLine();
		            }		           				
		    	}

				bw1.close();
			}
    	}

	}
    
	
	/**
	 * @param lexicalContext - the name of the lexical item that we want to read the synonyms and
	 * 	the frequencies for. It will iterate over all the generated file counts and print 
	 * (for now) them ranked. It can also be changed to return the top n synonyms.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public void filterNgramsByContextPOSWordMatch(Set<String> lexicalContext) throws FileNotFoundException, IOException{
    	File dir = new File("L:\\ngrams\\lucene\\docs\\filtered");
    	File[] directoryListing = dir.listFiles();
    	String[] tempNgram = null;
    	String ngram = "";
    	String tempLexicon = "";
    	
    	if (directoryListing != null) {
			for (File child : directoryListing) {			
				System.out.println("Reading file "+child);
				File file1 = new File("L:\\ngrams\\lucene\\docs\\filtered_pos_wordmatch\\"+child.getName());
				// if file doesnt exists, then create it
				if (!file1.exists()) {
					file1.createNewFile();
				}

				FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
				BufferedWriter bw1 = new BufferedWriter(fw1);
				
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
		            	tempNgram = line.split(" ");
		            	for (String part : tempNgram){
		            		part = part.replaceAll("_[A-Z]+", "");
		            		ngram += part+" ";
		            		//System.out.println(ngram);
		            	}
		            	for (String lexicon : lexicalContext){
		            		tempLexicon = lexicon;
	            			tempLexicon = " "+tempLexicon+" ";
	            			//System.out.println(tempLexicon);
	            			if ((ngram.contains(tempLexicon))){
            					bw1.write(ngram);
            					bw1.write("\n");
	            			}
	            		}
		            	ngram = "";
		                line = br.readLine();
		            }		           				
		    	}

				bw1.close();
			}
    	}

	}

	/**
	 * @param concept - the name of the lexical item that we want to read the synonyms and
	 * 	the frequencies for. It will iterate over all the generated file counts and print 
	 * them to a file named concept.txt. It also prints another two files: one with the ranked
	 * list of stemmed concepts and their counts combined and the list of concepts that have their
	 * combined counts above the current defined threshold (which is the average of total counts and
	 * number of stemmed concepts). The final called concept_stemmed_aboveAvg.txt is printed to the
	 * same folder and shows only the stems above average, their counts, and the inflected term
	 * that originate from them.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public static void getTopSynonyms(String concept) throws FileNotFoundException, IOException{
    	File dir = new File("L:\\ngrams\\output\\filtered_pos_wordmatch_smallContext\\"+concept+"_synonyms");
    	File[] directoryListing = dir.listFiles();
    	String[] tempSynonymCount = null;
    	
    	Map<String, Integer> foundSynonyms = new HashMap<String, Integer>();
    	Map<String, Integer> stemmedSynonyms = new HashMap<String, Integer>();
    	
    	Map<String, Set<String>> stemmedToInflected = new HashMap<String, Set<String>>();
    	
    	int totalValue = 0;
    	
    	PorterStemmer stemmer = new PorterStemmer();
        
    	//Set<String> inflectedWords = new HashSet<String>();
    	
    	if (directoryListing != null) {
			for (File child : directoryListing) {			
				System.out.println("Reading file "+child);
				
		    	try(BufferedReader br = new BufferedReader(new FileReader(child))) {
		            String line = br.readLine();
		            while (line != null) {
          	
		            	//System.out.println("ngram: "+line);
		            	tempSynonymCount = line.split(" ");
		            	//System.out.println("Size of parts is: "+ tempSynonymCount.length);
		            	String word = "";
		            	String count = "";
		            	for (String parts : tempSynonymCount){
		             		if (parts.matches("\\d+")){
		             			//System.out.println("part is number: "+parts);
		             			count = parts;
		             		}
		            		else{
		            			//System.out.println("part is concept: "+parts);
		            			word += parts +" ";
		            		}
		            	}
		            	//adding synonyms inflected to foundSynonyms map
	            		if (foundSynonyms.containsKey(word.trim())){
		            		foundSynonyms.put(word.trim(), foundSynonyms.get(word.trim()) + Integer.valueOf(count.trim()));
		            	}
		            	else{
		            		foundSynonyms.put(word.trim(), Integer.valueOf(count.trim()));
		            		
		            	}
		            	
	            		//adding stemmed synonyms to stemmedSynonyms map
	            		//String[] stemmedParts = word.split(" ");
		    		   // stemmer.setCurrent(stemmedParts[0]);
		    		    stemmer.setCurrent(word.trim());
		    		    stemmer.stem();
		    		    if (stemmedToInflected.containsKey(stemmer.getCurrent())){
		    		    	Set<String> tempSet = stemmedToInflected.get(stemmer.getCurrent());
		    		    	tempSet.add(word.trim());
		    		    	stemmedToInflected.put(stemmer.getCurrent(), tempSet);
		    		    }
		    		    else{
		    		    	Set<String> tempSet = new HashSet<String>();
		    		    	tempSet.add(word.trim());
		    		    	stemmedToInflected.put(stemmer.getCurrent(), tempSet);
		    		    }
		    		    	
		    		    
		    		    System.out.println("Word "+word.trim()+" stems to "+stemmer.getCurrent());
	            		if (stemmedSynonyms.containsKey(stemmer.getCurrent())){
	            			System.out.println("Found element on stemmed map.");
	            			stemmedSynonyms.put(stemmer.getCurrent(), stemmedSynonyms.get(stemmer.getCurrent()) + Integer.valueOf(count.trim()));
	            			totalValue += Integer.valueOf(count.trim());
	            		}
		            	else{
		            		System.out.println("No element found on stemmed map.");
		            		stemmedSynonyms.put(stemmer.getCurrent(), Integer.valueOf(count.trim()));
		            		totalValue += Integer.valueOf(count.trim());
		            		
		            	}
		            		
	            		
	            		System.out.println("Stemmed map contains "+stemmedSynonyms.size()+" elements.");
		                line = br.readLine();
		            }
		    	}
			}
    	}
        ValueComparator bvc =  new ValueComparator(foundSynonyms);
        TreeMap<String,Integer> sorted_map = new TreeMap<String,Integer>(bvc);
        
        ValueComparator bvcStemmed =  new ValueComparator(stemmedSynonyms);
        TreeMap<String,Integer> sorted_stemmed_map = new TreeMap<String,Integer>(bvcStemmed);
        
        System.out.println("unsorted map: "+foundSynonyms);

        File file1 = new File("L:\\ngrams\\output\\rankedSynonyms\\"+concept+".txt");
		// if file doesnt exists, then create it
		if (!file1.exists()) {
			file1.createNewFile();
		}
		
        File file2 = new File("L:\\ngrams\\output\\rankedSynonyms\\"+concept+"_stemmed.txt");
		// if file doesnt exists, then create it
		if (!file2.exists()) {
			file2.createNewFile();
		}
		
        File file3 = new File("L:\\ngrams\\output\\rankedSynonyms\\"+concept+"_stemmed_aboveAvg.txt");
		// if file doesnt exists, then create it
		if (!file3.exists()) {
			file3.createNewFile();
		}
		
		FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
		BufferedWriter bw1 = new BufferedWriter(fw1);
		
		FileWriter fw2 = new FileWriter(file2.getAbsoluteFile());
		BufferedWriter bw2 = new BufferedWriter(fw2);
		
		FileWriter fw3 = new FileWriter(file3.getAbsoluteFile());
		BufferedWriter bw3 = new BufferedWriter(fw3);
		
        sorted_map.putAll(foundSynonyms);
    	for (String anEntry : sorted_map.keySet()){

    		bw1.write(anEntry + " " +foundSynonyms.get(anEntry));
			bw1.write("\n");
    		System.out.println("Synonym: " + anEntry + " has count: "+ foundSynonyms.get(anEntry));
    	}
    	bw1.close();
    	
    	sorted_map.clear();
    	
    	System.out.println("Total value is: " + totalValue);
    	System.out.println("Size of stemmed synonyms is: " +stemmedSynonyms.size());
    	System.out.println("Threshold is: " + totalValue/stemmedSynonyms.size());
    	
    	if (stemmedSynonyms.size() > 0){
	        
    		sorted_stemmed_map.putAll(stemmedSynonyms);
	    	for (String anEntry : sorted_stemmed_map.keySet()){
	
	    		bw2.write(anEntry + " " +stemmedSynonyms.get(anEntry));
				bw2.write("\n");
	    		System.out.println("Synonym stemmed: " + anEntry + " has count: "+ stemmedSynonyms.get(anEntry));
	    		if (stemmedSynonyms.get(anEntry) > totalValue/stemmedSynonyms.size()){
	    			bw3.write(anEntry + " " +stemmedSynonyms.get(anEntry));
					bw3.write("\n");
					for (String aValue : stemmedToInflected.get(anEntry)){
						bw3.write("--- "+aValue);
						bw3.write("\n");
		    			System.out.println("--- "+aValue);
					}
					System.out.println("Value "+stemmedSynonyms.get(anEntry)+" is above threshold of "+totalValue/stemmedSynonyms.size());
	    		}
//	    		bw3.write("-----");
//				bw3.write("\n");
	    	}
	    	bw2.close();
	    	bw3.close();
    	}
    	sorted_stemmed_map.clear();
    	
    	for (String anEntry : stemmedToInflected.keySet()){
    		System.out.println("Stemmed word "+ anEntry+" has inflected words:");
    		for (String aValue : stemmedToInflected.get(anEntry)){
    			System.out.println("--- "+aValue);
    		}
    	}
	}
	

	/**
	 * @param concept - This method receives a concept (e.g. trend) and generates a file called 
	 * concept_grade_lexicon.txt that contains the synonyms of the concept trend that are appropriate
	 * for each one of the grade levels (4_5, 6_8, 9_10, 11_cc). It reads the set of inflected
	 * words from above average count stems and checks which ones appeared on the grade vocabulary.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public static void defineLexiconByGradeLevel(String concept, List<Set<String>> vocabs) throws FileNotFoundException, IOException{
		File file = new File("L:\\ngrams\\output\\rankedSynonyms\\"+concept+"_stemmed_aboveAvg.txt");

		Set<String> grade45Words = new HashSet<String>();
		Set<String> grade68Words = new HashSet<String>();
		Set<String> grade910Words = new HashSet<String>();
		Set<String> grade11ccWords = new HashSet<String>();
		
    	String tempSynonym = "";
    	String tempStemmed = "";
    	
        File file1 = new File("L:\\ngrams\\output\\rankedSynonyms\\"+concept+"_grade_lexicon_noAvg2.txt");
		// if file doesnt exists, then create it
		if (!file1.exists()) {
			file1.createNewFile();
		}
		FileWriter fw1 = new FileWriter(file1.getAbsoluteFile());
		BufferedWriter bw1 = new BufferedWriter(fw1);
		
		bw1.write("vocabularyGrade45");
		bw1.write("\n");
    	Set<String> aVocab = vocabs.get(0);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line = br.readLine();
            while (line != null) {
            	if (line.startsWith("---")){
            		tempSynonym = line.split(" ")[1];
            		System.out.println("Temp synonym is "+ tempSynonym);
            	}
            	else{
            		tempStemmed = line.split(" ")[0];
            	}
            	for (String word : aVocab){
            		//System.out.println("---Word "+ word);
	            	if (word.equalsIgnoreCase(tempSynonym.trim())){
	            		System.out.println("Comparing word "+word+" on vocabularyGrade45 with synonym "+ tempSynonym);
	            		//grade45Words.add(tempStemmed);
	            		grade45Words.add(tempSynonym);
	            	}
            	}
            	line = br.readLine();
            }
    	}
		for (String word : grade45Words){
			bw1.write(word.trim());
    		bw1.write("\n");
		}
		bw1.write("\n");
		
    	bw1.write("vocabularyGrade68");
		bw1.write("\n");
    	aVocab = vocabs.get(1);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line = br.readLine();
            while (line != null) {
            	if (line.startsWith("---")){
            		tempSynonym = line.split(" ")[1];
            	}
            	else{
            		tempStemmed = line.split(" ")[0];
            	}
            	for (String word : aVocab){
	            	if (word.equalsIgnoreCase(tempSynonym.trim())){
	            		//grade68Words.add(tempStemmed);
	            		grade68Words.add(tempSynonym);
	            	}
            	}
            	line = br.readLine();
            }
    	}
		for (String word : grade68Words){
			bw1.write(word.trim());
    		bw1.write("\n");
		}
		bw1.write("\n");
		
		bw1.write("vocabularyGrade910");
		bw1.write("\n");
    	aVocab = vocabs.get(2);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line = br.readLine();
            while (line != null) {
            	if (line.startsWith("---")){
            		tempSynonym = line.split(" ")[1];
            	}
            	else{
            		tempStemmed = line.split(" ")[0];
            	}
            	for (String word : aVocab){
	            	if (word.equalsIgnoreCase(tempSynonym.trim())){
	            		//grade910Words.add(tempStemmed);
	            		grade910Words.add(tempSynonym);
	            	}
            	}
            	line = br.readLine();
            }
    	}
		for (String word : grade910Words){
			bw1.write(word.trim());
    		bw1.write("\n");
		}
		bw1.write("\n");
		
		bw1.write("vocabularyGrade11cc");
		bw1.write("\n");
    	aVocab = vocabs.get(3);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line = br.readLine();
            while (line != null) {
            	if (line.startsWith("---")){
            		tempSynonym = line.split(" ")[1];
            	}
            	else{
            		tempStemmed = line.split(" ")[0];
            	}
            	for (String word : aVocab){
	            	if (word.equalsIgnoreCase(tempSynonym.trim())){
	            		//grade11ccWords.add(tempStemmed);
	            		grade11ccWords.add(tempSynonym);
	            	}
            	}
            	line = br.readLine();
            }
    	}
		for (String word : grade11ccWords){
			bw1.write(word.trim());
    		bw1.write("\n");
		}
		bw1.write("\n");
		bw1.close();
	}
	/**
	 * @return the newVocab45
	 */
	public static Set<String> getNewVocab45() {
		return newVocab45;
	}
	/**
	 * @param newVocab45 the newVocab45 to set
	 */
	public static void setNewVocab45(Set<String> newVocab45) {
		VocabularyByGrade.newVocab45 = newVocab45;
	}
	/**
	 * @return the newVocab68
	 */
	public static Set<String> getNewVocab68() {
		return newVocab68;
	}
	/**
	 * @param newVocab68 the newVocab68 to set
	 */
	public static void setNewVocab68(Set<String> newVocab68) {
		VocabularyByGrade.newVocab68 = newVocab68;
	}
	/**
	 * @return the newVocab910
	 */
	public static Set<String> getNewVocab910() {
		return newVocab910;
	}
	/**
	 * @param newVocab910 the newVocab910 to set
	 */
	public static void setNewVocab910(Set<String> newVocab910) {
		VocabularyByGrade.newVocab910 = newVocab910;
	}
	/**
	 * @return the newVocab11cc
	 */
	public static Set<String> getNewVocab11cc() {
		return newVocab11cc;
	}
	/**
	 * @param newVocab11cc the newVocab11cc to set
	 */
	public static void setNewVocab11cc(Set<String> newVocab11cc) {
		VocabularyByGrade.newVocab11cc = newVocab11cc;
	}
	
	
}
