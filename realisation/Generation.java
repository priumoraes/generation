/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * The Genration class is responsible for the core fucntions of the surface realizer.
 */
package edu.udel.eecis.teahouse.sight;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.Stack;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import simplenlg.phrasespec.SPhraseSpec;

import com.representqueens.lingua.en.Fathom;
import com.representqueens.lingua.en.Readability;

import edu.smu.tspell.wordnet.AdjectiveSynset;
import edu.smu.tspell.wordnet.AdverbSynset;
import edu.smu.tspell.wordnet.NounSynset;
import edu.smu.tspell.wordnet.Synset;
import edu.smu.tspell.wordnet.SynsetType;
import edu.smu.tspell.wordnet.VerbSynset;
import edu.smu.tspell.wordnet.WordNetDatabase;


/**
 * This is the main class. It reads a json file with propositions selected by PageRank and applies rules for
 * aggregating propositions based on the different reading levels
 * @author Priscilla
 *
 */
public class Generation {

	/**
	 * @param args
	 */

	// List of Proposition objects that were selected by PageRank
	public static List<Proposition> props = new ArrayList<Proposition>();
	
	//Lists that will hold propositions that belong to the same organization group
	public static List<Proposition> propsGroup1 = new ArrayList<Proposition>();
	public static List<Proposition> propsGroup2 = new ArrayList<Proposition>();
	public static List<Proposition> propsGroup3 = new ArrayList<Proposition>();
	
	//this map controls the lexical items complexities for the proposition types (the verbs that are the root predicates
	//in the sentences)
	public static Map<String,Map<String,List<String>>> verbLexicon = new HashMap<String,Map<String,List<String>>>();
	
	public static List<String> verbsSimpleShow = new ArrayList<String>();
	public static List<String> verbsAdvancedShow = new ArrayList<String>();
	public static List<String> verbsSimplePresent = new ArrayList<String>();
	public static List<String> verbsAdvancedPresent = new ArrayList<String>();
	public static List<String> verbsSimpleConsist = new ArrayList<String>();
	public static List<String> verbsAdvancedConsist = new ArrayList<String>();
	public static List<String> verbsSimpleDateChange = new ArrayList<String>();
	public static List<String> verbsAdvancedDateChange = new ArrayList<String>();
	public static List<String> verbsSimpleMaxMinDate = new ArrayList<String>();
	public static List<String> verbsAdvancedMaxMinDate = new ArrayList<String>();
	
	public static Map<String,List<String>> intermediateMapShow = new HashMap<String,List<String>>();
	public static Map<String,List<String>> intermediateMapPresent = new HashMap<String,List<String>>();
	public static Map<String,List<String>> intermediateMapConsist = new HashMap<String,List<String>>();
	public static Map<String,List<String>> intermediateMapDateChange = new HashMap<String,List<String>>();
	public static Map<String,List<String>> intermediateMapMaxMinDate = new HashMap<String,List<String>>();
	
	public static Map<String,Map<String,Map<String,Integer>>> currentSentenceMap = new HashMap<String,Map<String,Map<String,Integer>>>();
	public static Map<String,Map<String,Integer>> tempSemanticRole = new HashMap<String,Map<String,Integer>>();
	public static Map<String,Integer> tempSubject = new HashMap<String,Integer>();
	public static Map<String,Integer> tempObject = new HashMap<String,Integer>();
	
	public static Stack<Pair<String,SPhraseSpec>> propSentences = new Stack<Pair<String,SPhraseSpec>>();
	public static Stack<Pair<String,SPhraseSpec>> propSentences2 = new Stack<Pair<String,SPhraseSpec>>();
	public static Stack<Pair<String,SPhraseSpec>> propSentences3 = new Stack<Pair<String,SPhraseSpec>>();
	
	//These variables store the sentences in the right order, temporarily and finally respectively
	public static List<String> summary = new ArrayList<String>();
	public static List<String> finalSummary = new ArrayList<String>();
	
	public static void main(String[] args) throws IOException {

		//This map contains the variations for the root predicates of the propositions 
		//creating for "show"
		verbsSimpleShow.add("show");
		verbsSimpleShow.add("display");
		verbsAdvancedShow.add("depict");
		verbsAdvancedShow.add("portray");
		intermediateMapShow.put("simple", verbsSimpleShow);
		intermediateMapShow.put("advanced", verbsAdvancedShow);
		verbLexicon.put("show", intermediateMapShow);
		
		//creating for "present"
		verbsSimplePresent.add("present");
		//verbsSimple.add("display");
		verbsAdvancedPresent.add("introduce");
		//verbsAdvanced.add("illustrate");
		intermediateMapPresent.put("simple", verbsSimplePresent);
		intermediateMapPresent.put("advanced", verbsAdvancedPresent);
		verbLexicon.put("present", intermediateMapPresent);

		//creating for "consist"
		verbsSimpleConsist.add("consist of");
		//verbsSimple.add("display");
		verbsAdvancedConsist.add("contain");
		//verbsAdvanced.add("illustrate");
		intermediateMapConsist.put("simple", verbsSimpleConsist);
		intermediateMapConsist.put("advanced", verbsAdvancedConsist);
		verbLexicon.put("consist", intermediateMapConsist);
		
		//creating for "date_change"
		verbsSimpleDateChange.add("span over");
		verbsSimpleDateChange.add("take");
		verbsAdvancedDateChange.add("cover");
		verbsAdvancedDateChange.add("compass");
		
		intermediateMapDateChange.put("simple", verbsSimpleDateChange);
		intermediateMapDateChange.put("advanced", verbsAdvancedDateChange);
		verbLexicon.put("date_change", intermediateMapDateChange);
		
		//creating for "max_min_date"
		verbsSimpleMaxMinDate.add("happen");
		verbsAdvancedMaxMinDate.add("occur");
		intermediateMapMaxMinDate.put("simple", verbsSimpleMaxMinDate);
		intermediateMapMaxMinDate.put("advanced", verbsAdvancedMaxMinDate);
		verbLexicon.put("max_min_date", intermediateMapMaxMinDate);
		
		//System.out.println(verbLexicon);
		
		final double readingMeasure = 9.50;
		float fogMeasure;
		
		int localDisaggregateGroup1 = 0;
		int localDisaggregateGroup2 = 0;
		int localDisaggregateGroup3 = 0;
		
		int localSimplifyLexiconGroup1 = 0;
		int localSimplifyLexiconGroup2 = 0;
		int localSimplifyLexiconGroup3 = 0;
		
		PropRealisation aPropTemplate = new PropRealisation();
		VocabularyByGrade aVocab = new VocabularyByGrade();
		JSONParser parser = new JSONParser();  
		WordNetDatabase database = WordNetDatabase.getFileInstance();
		String sVocabulary = null;
		Set<String> setVocab = null;
		
		//-- Getting the right set of rules for the target reading level
		DTRules rules = new DTRules();
		List<HashMap<String, Pair<Integer, Integer>>> myRules = new ArrayList<HashMap<String,Pair<Integer,Integer>>>();
		if (readingMeasure < 6)
		{
			myRules = rules.getGrade45Rules();
			sVocabulary = "vocab45";
			setVocab = aVocab.getVocab45();
		}
		else if ((readingMeasure >= 6) && (readingMeasure < 9))
		{
			myRules = rules.getGrade68Rules();
			sVocabulary = "vocab68";
			setVocab = aVocab.getVocab68();
		}
		else if ((readingMeasure >= 9) && (readingMeasure < 11))
		{
			myRules = rules.getGrade910Rules();
			sVocabulary = "vocab910";
			setVocab = aVocab.getVocab910();
		}
		else
		{
			myRules = rules.getGrade11ccRules();
			sVocabulary = "vocab11cc";
			setVocab = aVocab.getVocab11cc();
		}
		
		System.out.println("Vocabulary 4-5: "+aVocab.getVocab45());
		System.out.println("Vocabulary 4-5 size: "+aVocab.getVocab45().size());
		
		System.out.println("Vocabulary 6-8: "+aVocab.getVocab68());
		System.out.println("Vocabulary 6-8 size: "+aVocab.getVocab68().size());
		
		System.out.println("Vocabulary 9-10: "+aVocab.getVocab910());
		System.out.println("Vocabulary 9-10 size: "+aVocab.getVocab910().size());
		
		System.out.println("Vocabulary 11-cc: "+aVocab.getVocab11cc());
		System.out.println("Vocabulary 11-cc size: "+aVocab.getVocab11cc().size());
		
		//lexicalContext holds all the synsets for all the default lexical items in the propositions
		Set<String> lexicalContext = new HashSet<String>();
		try {  
			// reading JSON files and instantiating objects
			Object obj = parser.parse(new FileReader("input/propositions.json"));  
		  
			JSONArray propositions = (JSONArray) obj;

			for(int i = 0; i < propositions.size(); i++){
				JSONObject innerObj = (JSONObject) propositions.get(i);

				Proposition aProposition = new Proposition();
				aProposition.setType(innerObj.get("Type").toString());
				aProposition.setPropTemplate(innerObj.get("PropTemplate").toString());
				aProposition.setDescription(innerObj.get("Description").toString());
				aProposition.setDetail(innerObj.get("Detail").toString());
				aProposition.setDegree(innerObj.get("Degree").toString());
				aProposition.setMembership(innerObj.get("Membership").toString());
				aProposition.setMembershipCode(innerObj.get("MembershipCode").toString());
				aProposition.setGroup(innerObj.get("Group").toString());
				aProposition.setValue(innerObj.get("Value").toString());
				aProposition.setValueDetail(innerObj.get("ValueDetail").toString());
				aProposition.setSegmentPosition(innerObj.get("SegmentPosition").toString());
				aProposition.setConveyable((boolean) innerObj.get("Conveyable"));
				aProposition.setIndependent((boolean) innerObj.get("Independent"));

				JSONArray additionalInfo = (JSONArray) innerObj.get("Additional");
				if (additionalInfo != null){
					List<List<String>> allAddInfo = new ArrayList<List<String>>();
					for(int j = 0; j < additionalInfo.size(); j++){
						List<String> addInfo = new ArrayList<String>();
						JSONObject innerObj2 = (JSONObject) additionalInfo.get(j);
						addInfo.add(innerObj2.get("Description").toString());
						addInfo.add(innerObj2.get("Begin").toString());
						addInfo.add(innerObj2.get("End").toString());
						allAddInfo.add(addInfo);
					}
					aProposition.setAdditionalInfo(allAddInfo);
				} else {
					aProposition.setAdditionalInfo(null);
				}
				JSONArray allowedRealisations = (JSONArray) innerObj.get("AllowedRealisations");
				if (allowedRealisations != null){
					List<Realisation> realisations = new ArrayList<Realisation>();
					for(int j = 0; j < allowedRealisations.size(); j++){
						Realisation aRealisation = new Realisation();
						JSONObject innerObj2 = (JSONObject) allowedRealisations.get(j);
						aRealisation.setRealisationType(innerObj2.get("Realization").toString());
						aRealisation.setSentences((int)(long)innerObj2.get("Sentences"));
						aRealisation.setWords((int)(long)innerObj2.get("Words"));
						aRealisation.setAvgWordLenght((int)(long)innerObj2.get("AvgWordLength"));
						aRealisation.setPronouns((int)(long)innerObj2.get("Pronouns"));
						aRealisation.setBegPronouns((int)(long)innerObj2.get("BegPronoun"));
						aRealisation.setArticles((int)(long)innerObj2.get("Article"));
						aRealisation.setBegArticles((int)(long)innerObj2.get("BegArticle"));
						aRealisation.setRelativeClause((int)(long)innerObj2.get("RelativeClause"));
						aRealisation.setAdjective((int)(long)innerObj2.get("Adjective"));
						aRealisation.setAdverb((int)(long)innerObj2.get("Adverb"));
						aRealisation.setPassive((int)(long)innerObj2.get("Passive"));
						aRealisation.setPrepositions((int)(long)innerObj2.get("Preposition"));
						aRealisation.setConjunctions((int)(long)innerObj2.get("Conjunction"));
						aRealisation.setAvgNPLength((int)(long)innerObj2.get("AvgNPLength"));
						aRealisation.setAvgVPLength((int)(long)innerObj2.get("AvgVPLength"));
						realisations.add(aRealisation);
					}
					aProposition.setAllowedRealisations(realisations);
				} else {
					aProposition.setAllowedRealisations(null);
				}
				aProposition.setUsed(false);
				
				JSONArray lexicon = (JSONArray) innerObj.get("Lexicon");
				if (lexicon != null){
					Map<String,String> allLexicon = new HashMap<String,String>();
					for(int j = 0; j < lexicon.size(); j++){
						JSONObject innerObj2 = (JSONObject) lexicon.get(j);
						String pos = returnLexiconPOS(innerObj2.get("POS").toString());
						//System.out.println("Returned POS: "+pos);
						String appropriateLexicon = null;
						if ((!(pos.equals("null"))) && (innerObj2.get("Word") != null))
						{	
							lexicalContext.addAll(returnLexiconNgrams(innerObj2.get("Word").toString(), pos, database));
							appropriateLexicon = returnAppropriateLexicon(innerObj2.get("Word").toString(), pos, database, setVocab, sVocabulary);
							allLexicon.put(innerObj2.get("POS").toString(),appropriateLexicon);
						}
					}
					aProposition.setLexicon(allLexicon);
				} else {
					aProposition.setLexicon(null);
				}

				//System.out.println("Lexicon map "+aProposition.getLexicon()+" of prop "+aProposition.getType());
				props.add(aProposition);
				
				if (aProposition.getType().equalsIgnoreCase("graph")){
					propsGroup3.add(aProposition);
				}
				if (aProposition.getGroup().equals("1")){
					propsGroup1.add(aProposition);
				} else if (aProposition.getGroup().equals("2")){
					propsGroup2.add(aProposition);
				} else {
					propsGroup3.add(aProposition);
				}
			}
			//Calling method that updates the propositions with new AllowedRealisation values
			updatePropositionAllowedRealisation(props);
			System.out.println("Complete set of ngrams: "+aVocab.getContext());
			aVocab.filterNgramsByContextPOSWordMatch(aVocab.getContext());
			aVocab.setNgrams(lexicalContext); //this is wrong as lexicalContext is not context.txt
			aVocab.filterNgramsByContext(aVocab.getContext());
			aVocab.compactThNgrams();
			getSynsets(props);
			searchRealisation(propsGroup1);
			
			
			//Calling method that returns true if the set of aggregated props match the rule for the target reading level
			checkReadabilityRuleMatch(propsGroup1, propsGroup2, propsGroup3, myRules.get(0));
			checkReadabilityRuleMatchGroup2(propsGroup2);
			checkReadabilityRuleMatchGroup3(propsGroup3);
			
  		} catch (FileNotFoundException e) {  
			e.printStackTrace();  
		} catch (IOException e) {  
			e.printStackTrace();  
		} catch (ParseException e) {  
			e.printStackTrace();  
		}  
		
		
		//Counting how many aggregations can be done in a group
		int aggregationProps1 = countAggregation(propsGroup1);
		int aggregationProps2 = countAggregationSpecial(propsGroup2);
		int aggregationProps3 = countAggregation(propsGroup3);
		
		System.out.println("Aggregations allowed on group 1: "+aggregationProps1);
		System.out.println("Aggregations allowed on group 2: "+aggregationProps2);
		System.out.println("Aggregations allowed on group 3: "+aggregationProps3);
		
		//System.out.println("Aggregating all propositions from group 1:");
		//System.out.println("Target grade level is "+readingMeasure+" or below.");
		
		Proposition tempProp1;
		Proposition tempProp2;
		
		
		//-- Testing WordNet for lexical item choice
		//System.out.println("Synonym: "+returnAppropriateLexicon("picture", "noun", database, aVocab.getVocab45(), "vocab45"));
		
		/**The following data structure stores the current state of a sentence. The first map has as key the sentence and as values other maps
		(currently only two) that will store the SUBJ and OBJ states. For that we want to keep a list of pairs that will keep track of the counts
		of ADJ (adjectives) and RC (relative clauses) used in each semantic role as well as the proposition number (1 or 2) that was realized as each
		semantic role.*/

		
		String realised = null;
		SPhraseSpec sentence = null;
	
		Pair<String,SPhraseSpec> tempPair;
		
		String tempPOS = null;
		String currentSentence = null;
		SPhraseSpec currentSentenceObject = null;
		
		callSearchRealisation();

	}

		//this variable will control inner aggregation - do not realise a proposition in a sentence by itself just because it could not find a sentence to aggregate it with
		boolean start = true;
		boolean sentenceFound = false;
		boolean canReduce = true;
		
		do {
			if (!(start)){
				//canReduce = reduceReadability(aggregationProps1, aggregationProps2, aggregationProps3);
				localDisaggregateGroup1 = disaggregateGroup1;
				localDisaggregateGroup2 = disaggregateGroup2;
				localDisaggregateGroup3 = disaggregateGroup3;
				
				localSimplifyLexiconGroup1 = simplifyLexiconGroup1;
				localSimplifyLexiconGroup2 = simplifyLexiconGroup2;
				localSimplifyLexiconGroup3 = simplifyLexiconGroup3;
				
				System.out.println("Simplifications for group 1: "+localSimplifyLexiconGroup1);
				System.out.println("Simplifications for group 2: "+localSimplifyLexiconGroup2);
				System.out.println("Simplifications for group 3: "+localSimplifyLexiconGroup3);
				
			}
			for (int i = 0; i < propsGroup1.size(); i++){
				tempProp1 = propsGroup1.get(i);
				for (int j = 0; j < propsGroup1.size(); j++){
					tempProp2 = propsGroup1.get(j);
					if ((tempProp1.getType().equals(tempProp2.getMembership())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						System.out.println("LocalDisaggregation1: "+localDisaggregateGroup1);
						if ((localDisaggregateGroup1 >= 0) || (propSentences.size() == 0))
						{
							//System.out.println("Realising sentence in group 1.");
							Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
							//aPair = generateSingleSentenceGroup1(tempProp2, localSimplifyLexiconGroup1, tempProp1, aPropTemplate);
							localDisaggregateGroup1--;
						}else{
							//System.out.println("Aggregating sentences in group 1.");
							while (!(propSentences.isEmpty())){
								
								System.out.println(propSentences.size());
								
								tempPair = propSentences.pop();
								currentSentence = tempPair.getFirst();
								currentSentenceObject = tempPair.getSecond();
								tempSemanticRole = currentSentenceMap.get(currentSentence);
								
								System.out.println(tempPair.getFirst());
								
								//checking in which semantic position (SUBJ or OBJ) tempProp1 is
								if (tempSemanticRole.get("SUBJ").containsKey(tempProp1.getType())){
									tempPOS = "SUBJ";
									sentenceFound = true;
								}
								else if (tempSemanticRole.get("OBJ").containsKey(tempProp1.getType())){
									tempPOS = "OBJ";
									sentenceFound = true;
								}
								if (sentenceFound)
									break;
							}

							if (tempProp2.getAllowedRealisations().contains("ADJ")){
								Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
								//aPair = generateAggregateAdjectiveGroup1(tempProp1, currentSentence, tempProp2, aPropTemplate, localSimplifyLexiconGroup1, currentSentenceObject, tempPOS);
		
							} else if (tempProp2.getAllowedRealisations().contains("RC")){
								Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
								//aPair = generateAggregateRCGroup1(tempPOS, currentSentence, tempProp2, localSimplifyLexiconGroup1, aPropTemplate, currentSentenceObject);
							}
						}
					}
					sentenceFound = false;
				}
			}
			
	
			for (int i = 0; i < propsGroup2.size(); i++){
				tempProp1 = propsGroup2.get(i);
				for (int j = 0; j < propsGroup2.size(); j++){
					tempProp2 = propsGroup2.get(j);
					if ((!(tempProp1.getType().equals(tempProp2.getType())) && (tempProp1.getDetail().equalsIgnoreCase(tempProp2.getMembershipCode())))){
						if ((propSentences2.size() == 0) || (localDisaggregateGroup2 >= 0)){
							//System.out.println("Realizing "+tempProp2.getType()+" as a sentence.");
							Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
							//aPair = generateSingleSentenceGroup2(tempProp1, tempProp2, localSimplifyLexiconGroup2, aPropTemplate);
							localDisaggregateGroup2--;
						}else{
							while (!(propSentences2.isEmpty())){
								
								tempPair = propSentences2.pop();
								currentSentence = tempPair.getFirst();
								currentSentenceObject = tempPair.getSecond();
								tempSemanticRole = currentSentenceMap.get(currentSentence);
								
								if (tempSemanticRole.get("SUBJ").containsKey(tempProp1.getType())){
									tempPOS = "SUBJ";
									sentenceFound = true;
								}
								else if (tempSemanticRole.get("SUBJ").containsKey(tempProp1.getType())){
									tempPOS = "OBJ";
									sentenceFound = true;
								}
								if (sentenceFound)
									break;
							}
							if (tempProp2.getAllowedRealisations().contains("ADJ")){
								Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
								//aPair = generateAggregateAdjectiveGroup2(tempProp2, tempPOS, currentSentence, currentSentenceObject, aPropTemplate, localSimplifyLexiconGroup2);
							}
									
							else if (tempProp2.getAllowedRealisations().contains("RC")){
								Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
								//aPair = generateAggregateRCGroup2(tempProp2, tempPOS, currentSentence, localSimplifyLexiconGroup2, aPropTemplate, currentSentenceObject);
							}
						}
					}
					sentenceFound = false;
				}
				propSentences2.clear();
			}


			for (int i = 0; i < propsGroup3.size(); i++){
				tempProp1 = propsGroup3.get(i);
				for (int j = 0; j < propsGroup3.size(); j++){
					tempProp2 = propsGroup3.get(j);
					if ((tempProp1.getType().equals(tempProp2.getMembership())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						if ((propSentences3.size() == 0) || (localDisaggregateGroup3 >= 0)){
							//System.out.println("Realizing "+tempProp2.getType()+" as a sentence.");
							Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
							//aPair = generateSingleSentenceGroup3(tempProp1, tempProp2, localSimplifyLexiconGroup3, aPropTemplate);
							localDisaggregateGroup3--;
						}else{
							while (!(propSentences3.isEmpty())){
								
								tempPair = propSentences3.pop();
								currentSentence = tempPair.getFirst();
								currentSentenceObject = tempPair.getSecond();
								tempSemanticRole = currentSentenceMap.get(currentSentence);
								
								if (tempSemanticRole.get("SUBJ").containsKey(tempProp1.getType())){
									tempPOS = "SUBJ";
									sentenceFound = true;
								}
								else if (tempSemanticRole.get("SUBJ").containsKey(tempProp1.getType())){
									tempPOS = "OBJ";
									sentenceFound = true;
								}
								if (sentenceFound)
									break;
								
								//only type and volatility accept ADJ for our case - just to keep the code cleaner and smaller, I will omit these options here, but
								// they should be added for a more general application of this code
								if (tempProp2.getAllowedRealisations().contains("RC")){
									Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
									//aPair = generateAggregateRCGroup3(tempProp2, tempPOS, currentSentence, localSimplifyLexiconGroup3, aPropTemplate, currentSentenceObject);
								}
							}
						}
						
					}
					sentenceFound = false;
				}
				propSentences3.clear();
			}
			//float kincaidMeasure = Readability.calcKincaid(Fathom.analyze(finalSummaryString));
			System.out.println();
			System.out.println();
			System.out.println("-----------------------------------------------------------");
			String finalSummaryString = "";
			for (String tempSentence : summary){
				if (currentSentenceMap.containsKey(tempSentence)){
					finalSummary.add(tempSentence);
					finalSummaryString += tempSentence+" ";
					System.out.println(tempSentence);
				}
			}
			fogMeasure = Readability.calcFog(Fathom.analyze(finalSummaryString));
			System.out.println();
			System.out.println();
			System.out.println("Fog grade level: "+fogMeasure);
			start = false;
			summary.clear();
			finalSummary.clear();
			currentSentenceMap.clear();
			propSentences.clear();
			propSentences2.clear();
			propSentences3.clear();
			for (Proposition aProp : propsGroup1){
				aProp.setUsed(false);
			}
			for (Proposition aProp : propsGroup2){
				aProp.setUsed(false);
			}
			for (Proposition aProp : propsGroup3){
				aProp.setUsed(false);
			}
		
		} while ((fogMeasure > readingMeasure) && (canReduce));
	}


	private static Pair<String,SPhraseSpec> generateSingleSentenceGroup1(Proposition tempProp2, int localSimplifyLexiconGroup1, Proposition tempProp1, PropRealisation aPropTemplate){
		
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		
		tempSubject.put(tempProp1.getType(), 0);
		tempSubject.put("NumberAdjectives", 0);
		tempSubject.put("NumberRelativeClauses", 0);
		tempObject.put(tempProp2.getType(), 0);
		tempObject.put("NumberAdjectives", 0);
		tempObject.put("NumberRelativeClauses", 0);
		tempSemanticRole.put("SUBJ", tempSubject);
		tempSemanticRole.put("OBJ", tempObject);
		String rootPredicate = "";
		String realised = null;
		
		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "show";
			if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
				sentence = aPropTemplate.showDetail(tempProp1, tempProp2, rootPredicate);
			else
				sentence = aPropTemplate.show(tempProp1, tempProp2, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("consist")){
			if (localSimplifyLexiconGroup1 > 0){
				rootPredicate = verbLexicon.get("consist").get("simple").get(0);
				localSimplifyLexiconGroup1--;
			}else{
				rootPredicate = verbLexicon.get("consist").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "consist";
			sentence = aPropTemplate.consist(tempProp1, tempProp2, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (localSimplifyLexiconGroup1 > 0){
				rootPredicate = verbLexicon.get("present").get("simple").get(0);
				localSimplifyLexiconGroup1--;
			}else{
				rootPredicate = verbLexicon.get("present").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			sentence = aPropTemplate.present(tempProp1, tempProp2, tempProp2.getValueDetail(), rootPredicate);
			tempProp2.setUsed(true);
     		realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			sentence = aPropTemplate.type(tempProp1, tempProp2);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			/* the lexical choices for volatility are tied to the reading level. Realised as a conjunction it is simple, and as an adjective or noun it is advanced.
			   However, since this is the sentence realisation, only conjunction and noun are allowed.
			   TODO: add randomness to the choice of allowed realisations given a complexity */
			String volatilityRealisation = "";
			String volatilityPOS = "";
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				volatilityPOS = "conjunction";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				volatilityPOS = "noun";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "noun";
			if (volatilityRealisation.equalsIgnoreCase(""))
				volatilityRealisation = "volatility";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			sentence = aPropTemplate.volatility(tempProp1, tempProp2, volatilityPOS, rootPredicate, volatilityRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
	}

	private static Pair<String,SPhraseSpec> generateAggregateAdjectiveGroup1(Proposition tempProp1, String currentSentence, Proposition tempProp2, PropRealisation aPropTemplate, int localSimplifyLexiconGroup1, SPhraseSpec currentSentenceObject, String tempPOS)
	{
		String tempPOS = null;
		GenerateSentence aSentence = new GenerateSentence();
		String realised = null;
		SPhraseSpec sentence = null;
		
		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		tempSubject.put("NumberAdjectives", tempSubject.get("NumberAdjectives") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);
		//call methods that will aggregate content of temProp2 with either the subject or the object of the sentence as an ADJ
		//then update currentSentenceMap by deleting copying content and deleting current sentence and adding the new sentence 
		//with updated values. Don't forget to set tempProp2.isUsed to true!
			
		//only type and volatility accept ADJ for our case - just to keep the code cleaner and smaller, I will omit these options here, but
		// they should be added for a more general application of this code
		if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			//Here we call the methods for aggregating tempProp2 as an adjective of the subject representing tempProp1 in the sentence
			sentence = aPropTemplate.typeADJ(currentSentenceObject, tempProp2, tempPOS);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			System.out.println(realised);
			//Add new sentence to the map --> See if we will need the old ones. If not, remove them from the map and only the remaining
			//ones will compose the final summary
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "volatile";
			String volatilityPOS = "adj";
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				volatilityPOS = "adj";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
				volatilityPOS = "adj";
				volatilityRealisation = tempProp2.getAdvancedLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			sentence = aPropTemplate.volatilityADJ(currentSentenceObject, tempProp2, tempPOS, volatilityRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			System.out.println(realised);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		currentSentenceMap.remove(currentSentence);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateAggregateRCGroup1(String tempPOS, String currentSentence, Proposition tempProp2, int localSimplifyLexiconGroup1, PropRealisation aPropTemplate, SPhraseSpec currentSentenceObject){
		boolean conjunction = false;
		SPhraseSpec sentence = null;
		String rootPredicate = "";
		String realised = null;
		GenerateSentence aSentence = new GenerateSentence();

		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		
		//checking if it is needed to add a new clause as a coordination instead of a regular relative clause
		if (tempSubject.get("NumberRelativeClauses") > 0)
			conjunction = true;
		tempSubject.put("NumberRelativeClauses", tempSubject.get("NumberRelativeClauses") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);
		//call method that will aggregate content of temProp2 with the subject of the sentence as a RC, then update currentSentenceMap by
		//deleting copying content and deleting current sentence and adding the new sentence with updated values. Don't forget
		//to set tempProp2.isUsed to true!

		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "show";
			if (conjunction)
				if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
					sentence = aPropTemplate.showDetail(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
				else
					sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
					sentence = aPropTemplate.showDetail(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
				else
					sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("consist")){
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("consist").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("consist").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "consist";
			if (conjunction)
				sentence = aPropTemplate.consist(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.consist(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
    		realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("present").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("present").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			if (conjunction)
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			if (conjunction)
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "conjunction");
			else
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "relativeClause");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "";
			String volatilityPOS = "";
			//here randomness will help getting varied summaries
			//TODO: add randomness
			if (localSimplifyLexiconGroup1 > 0){
				localSimplifyLexiconGroup1--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				volatilityPOS = "conjunction";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				volatilityPOS = "noun";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			if (conjunction)
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		currentSentenceMap.remove(currentSentence);
		System.out.println(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
		
	}

	private static Pair<String,SPhraseSpec> generateSingleSentenceGroup2(Proposition tempProp1, Proposition tempProp2, int localSimplifyLexiconGroup2, PropRealisation aPropTemplate){
		
		GenerateSentence aSentence = new GenerateSentence();
		String rootPredicate = "";
		SPhraseSpec sentence = null;
		String realised = null;
		
		tempSubject.put(tempProp1.getType(), 0);
		tempSubject.put("NumberAdjectives", 0);
		tempSubject.put("NumberRelativeClauses", 0);
		tempObject.put(tempProp2.getType(), 0);
		tempObject.put("NumberAdjectives", 0);
		tempObject.put("NumberRelativeClauses", 0);
		tempSemanticRole.put("SUBJ", tempSubject);
		tempSemanticRole.put("OBJ", tempObject);

		
		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
		}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
			}
			sentence = aPropTemplate.show(tempProp1, tempProp2, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("present").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("present").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			sentence = aPropTemplate.present(tempProp1, tempProp2, tempProp2.getValueDetail(), rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			sentence = aPropTemplate.type(tempProp1, tempProp2);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "";
			String volatilityPOS = "";
			//here randomness will help getting varied summaries
			//TODO: add randomness
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				volatilityPOS = "conjunction";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				volatilityPOS = "noun";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
    		sentence = aPropTemplate.volatility(tempProp1, tempProp2, volatilityPOS, rootPredicate, volatilityRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("steep")){
			String steepRealisation = "";
			String steepPOS = "";
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				//since steep has both adj and noun as both simple and advanced lexicon, randomness here will help with making a decision of which one to use. Or better, the complexity can decide!
				//TODO: use randomness for picking the part of speech (adj or noun)
				steepPOS = "noun";
				steepRealisation = tempProp2.getSimpleLexicon().get(steepPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				steepPOS = "adj";
				steepRealisation = tempProp2.getAdvancedLexicon().get(steepPOS).get(0);
			}
			if (steepPOS.isEmpty())
				steepPOS = "adj";
			if (steepRealisation.isEmpty())
				steepRealisation = "steep";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			sentence = aPropTemplate.steep(tempProp1, tempProp2, steepPOS, steepRealisation, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("value")){
			String valueRealisation = "";
			String valuePOS = "adj"; //for now it only has adj
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				valueRealisation = tempProp2.getSimpleLexicon().get(valuePOS).get(0);
			}else{
				valueRealisation = tempProp2.getAdvancedLexicon().get(valuePOS).get(0);
			}
			if (valueRealisation.equalsIgnoreCase(""))
				if (tempProp2.getType().contains("initial"))
					valueRealisation = "starting";
				else
					valueRealisation = "ending";
			sentence = aPropTemplate.value(tempProp1, tempProp2, valueRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("date")){
			String type;
			if (tempProp2.getType().contains("initial")){
				type = "start";
			}
			else {
				type = "end";
			}
			sentence = aPropTemplate.date(tempProp1, tempProp2, type);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		//realised = aSentence.realiser.realiseSentence(sentence);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences2.push(aPair);
		summary.add(realised);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateAggregateAdjectiveGroup2(Proposition tempProp2, String tempPOS, String currentSentence, SPhraseSpec currentSentenceObject, PropRealisation aPropTemplate, int localSimplifyLexiconGroup2){
		
		GenerateSentence aSentence = new GenerateSentence();
		String realised = null;
		SPhraseSpec sentence = null;
		
		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		tempSubject.put("NumberAdjectives", tempSubject.get("NumberAdjectives") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);
		String rootPredicate = "";
		//call methods that will aggregate content of temProp2 with either the subject or the object of the sentence as an ADJ
		//then update currentSentenceMap by deleting copying content and deleting current sentence and adding the new sentence 
		//with updated values. Don't forget to set tempProp2.isUsed to true!
			
		//only type and volatility accept ADJ for our case - just to keep the code cleaner and smaller, I will omit these options here, but
		// they should be added for a more general application of this code
		if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			//Here we call the methods for aggregating tempProp2 as an adjective of the subject representing tempProp1 in the sentence
			sentence = aPropTemplate.typeADJ(currentSentenceObject, tempProp2, tempPOS);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//Add new sentence to the map --> See if we will need the old ones. If not, remove them from the map and only the remaining
			//ones will compose the final summary
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "";
			String volatilityPOS = "adj";
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
    			volatilityRealisation = tempProp2.getAdvancedLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityRealisation.equalsIgnoreCase(""))
				volatilityRealisation = "volatile";
			sentence = aPropTemplate.volatilityADJ(currentSentenceObject, tempProp2, tempPOS, "volatile");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("steep")){
			String steepRealisation = "";
			String steepPOS = "adj";
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				steepRealisation = tempProp2.getSimpleLexicon().get(steepPOS).get(0);
			}else{
			    steepRealisation = tempProp2.getSimpleLexicon().get(steepPOS).get(0);
			}
			if (steepRealisation.isEmpty())
				steepRealisation = "steep";
			sentence = aPropTemplate.steepADJ(currentSentenceObject, tempProp2, tempPOS, steepRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		currentSentenceMap.remove(currentSentence);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences2.push(aPair);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateAggregateRCGroup2(Proposition tempProp2, String tempPOS, String currentSentence, int localSimplifyLexiconGroup2, PropRealisation aPropTemplate, SPhraseSpec currentSentenceObject){
		
		boolean conjunction = false;
		SPhraseSpec sentence = null;
		String rootPredicate = "";
		String realised = null;
		GenerateSentence aSentence = new GenerateSentence();
		
		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		if (tempSubject.get("NumberRelativeClauses") > 0)
			conjunction = true;
		tempSubject.put("NumberRelativeClauses", tempSubject.get("NumberRelativeClauses") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);

		//call method that will aggregate content of temProp2 with the subject of the sentence as a RC, then update currentSentenceMap by
		//deleting copying content and deleting current sentence and adding the new sentence with updated values. Don't forget
		//to set tempProp2.isUsed to true!
		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "show";
			if (conjunction)
				sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("present").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("present").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			if (conjunction)
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			if (conjunction)
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "conjunction");
			else
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "relativeClause");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "";
			String volatilityPOS = "";
			//here randomness will help getting varied summaries
			//TODO: add randomness
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				volatilityPOS = "conjunction";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				volatilityPOS = "noun";
				volatilityRealisation = tempProp2.getSimpleLexicon().get(volatilityPOS).get(0);
			}
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			if (conjunction)
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("steep")){
			String steepRealisation = "";
			String steepPOS = "";
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				rootPredicate = verbLexicon.get("show").get("simple").get(0);
				//since steep has both adj and noun as both simple and advanced lexicon, randomness here will help with making a decision of which one to use. Or better, the complexity can decide!
				//TODO: use randomness for picking the part of speech (adj or noun)
				steepPOS = "noun";
				steepRealisation = tempProp2.getSimpleLexicon().get(steepPOS).get(0);
			}else{
				rootPredicate = verbLexicon.get("show").get("advanced").get(0);
				steepPOS = "adj";
				steepRealisation = tempProp2.getSimpleLexicon().get(steepPOS).get(0);
			}
			if (steepPOS.equalsIgnoreCase(""))
				steepPOS = "adj";
			if (steepRealisation.isEmpty())
				steepRealisation = "steep";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			if (conjunction)
				sentence = aPropTemplate.steepRC(currentSentenceObject, tempProp2, steepPOS, tempPOS, steepRealisation, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.steepRC(currentSentenceObject, tempProp2, steepPOS, tempPOS, steepRealisation, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("value")){
			String valueRealisation = "";
			String valuePOS = "adj"; //for now it only has adj
			if (localSimplifyLexiconGroup2 > 0){
				localSimplifyLexiconGroup2--;
				valueRealisation = tempProp2.getSimpleLexicon().get(valuePOS).get(0);
			}else{
				valueRealisation = tempProp2.getAdvancedLexicon().get(valuePOS).get(0);
			}
			if (valueRealisation.equalsIgnoreCase(""))
				if (tempProp2.getType().contains("initial"))
					valueRealisation = "starting";
				else
					valueRealisation = "ending";
			if (conjunction)
				sentence = aPropTemplate.value(currentSentenceObject, tempProp2, tempPOS, "conjunction", valueRealisation);
			else
				sentence = aPropTemplate.value(currentSentenceObject, tempProp2, tempPOS, "relativeClause", valueRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("date")){
			//TODO: add lexical choice and fix type as done in value
			String type;
			if (tempProp2.getType().contains("initial")){
				type = "start";
			}
			else {
				type = "end";
			}
			if (conjunction)
				sentence = aPropTemplate.date(currentSentenceObject, tempProp2, type, tempPOS, "conjunction");
			else
				sentence = aPropTemplate.date(currentSentenceObject, tempProp2, type, tempPOS, "relativeClause");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		currentSentenceMap.remove(currentSentence);
		System.out.println(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences2.push(aPair);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateSingleSentenceGroup3(Proposition tempProp1, Proposition tempProp2, int localSimplifyLexiconGroup3, PropRealisation aPropTemplate){
		
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		String rootPredicate = "";
		String realised = null;
		
		tempSubject.put(tempProp1.getType(), 0);
		tempSubject.put("NumberAdjectives", 0);
		tempSubject.put("NumberRelativeClauses", 0);
		tempObject.put(tempProp2.getType(), 0);
		tempObject.put("NumberAdjectives", 0);
		tempObject.put("NumberRelativeClauses", 0);
		tempSemanticRole.put("SUBJ", tempSubject);
		tempSemanticRole.put("OBJ", tempObject);

		if (tempProp2.getPropTemplate().equalsIgnoreCase("max_min_value")){
			String maxMinRealisation = "";
			String maxMinPOS = "adj"; //for now it only has adj
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				maxMinRealisation = tempProp2.getSimpleLexicon().get(maxMinPOS).get(0);
			}else{
				maxMinRealisation = tempProp2.getAdvancedLexicon().get(maxMinPOS).get(0);
			}
			if (maxMinRealisation.equalsIgnoreCase(""))
				if (tempProp2.getType().contains("max"))
					maxMinRealisation = "maximum";
				else
					maxMinRealisation = "minimum";
			sentence = aPropTemplate.max_min_value(tempProp1, tempProp2, maxMinRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("max_min_date")){
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				rootPredicate = verbLexicon.get("max_min_date").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("max_min_date").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "occur";
			sentence = aPropTemplate.max_min_date(tempProp1, tempProp2, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("value_change")){
			//TODO: add lexical choice
			String type;
			if (tempProp2.getType().contains("increase")){
				type = "increase";
			}
			else {
				type = "decrease";
			}
			sentence = aPropTemplate.value_change(tempProp1, tempProp2, type);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("date_change")){
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				rootPredicate = verbLexicon.get("date_change").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("date_change").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "span";
			sentence = aPropTemplate.date_change(tempProp1, tempProp2, rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences3.push(aPair);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateAggregateRCGroup3(Proposition tempProp2, String tempPOS, String currentSentence, int localSimplifyLexiconGroup3, PropRealisation aPropTemplate, SPhraseSpec currentSentenceObject){
		
		boolean conjunction = false;
		SPhraseSpec sentence = null;
		String rootPredicate = "";
		String realised = null;
		GenerateSentence aSentence = new GenerateSentence();
		
		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		if (tempSubject.get("NumberRelativeClauses") > 0)
			conjunction = true;
		tempSubject.put("NumberRelativeClauses", tempSubject.get("NumberRelativeClauses") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);

		//call method that will aggregate content of temProp2 with the subject of the sentence as a RC, then update currentSentenceMap by
		//deleting copying content and deleting current sentence and adding the new sentence with updated values. Don't forget
		//to set tempProp2.isUsed to true!
		if (tempProp2.getPropTemplate().equalsIgnoreCase("max_min_value")){
			String maxMinRealisation = "";
			String maxMinPOS = "adj"; //for now it only has adj
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				maxMinRealisation = tempProp2.getSimpleLexicon().get(maxMinPOS).get(0);
			}else{
				maxMinRealisation = tempProp2.getAdvancedLexicon().get(maxMinPOS).get(0);
			}
			if (maxMinRealisation.equalsIgnoreCase(""))
				if (tempProp2.getType().contains("max"))
					maxMinRealisation = "maximum";
				else
					maxMinRealisation = "minimum";
			if (conjunction)
				sentence = aPropTemplate.max_min_value(currentSentenceObject, tempProp2, tempPOS, "conjunction", maxMinRealisation);
			else
				sentence = aPropTemplate.max_min_value(currentSentenceObject, tempProp2, tempPOS, "relativeClause", maxMinRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("max_min_date")){
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				rootPredicate = verbLexicon.get("max_min_date").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("max_min_date").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "occur";
			if (conjunction)
				sentence = aPropTemplate.max_min_date(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.max_min_date(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("value_change")){
			//TODO: add lexical choice
			String type;
			if (tempProp2.getType().contains("increase")){
				type = "increase";
			}
			else {
				type = "decrease";
			}
			if (conjunction)
				sentence = aPropTemplate.value_change(currentSentenceObject, tempProp2, tempPOS, type, "conjunction");
			else
				sentence = aPropTemplate.value_change(currentSentenceObject, tempProp2, tempPOS, type, "relativeClause");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("date_change")){
			if (localSimplifyLexiconGroup3 > 0){
				localSimplifyLexiconGroup3--;
				rootPredicate = verbLexicon.get("date_change").get("simple").get(0);
			}else{
				rootPredicate = verbLexicon.get("date_change").get("advanced").get(0);
			}
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "last";
			if (conjunction)
				sentence = aPropTemplate.date_change(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.date_change(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		currentSentenceMap.remove(currentSentence);
		System.out.println(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences3.push(aPair);
		return aPair;
	}
	

	/**
	 * @param propsGroup
	 * @return
	 */
	private static int countAggregationSpecial(List<Proposition> propsGroup) {
		int count = 0;
		Proposition tempProp1;
		Proposition tempProp2;
		for (int i = 0; i < propsGroup.size(); i++){
			tempProp1 = propsGroup.get(i);
			for (int j = 0; j < propsGroup.size(); j++){
				tempProp2 = propsGroup.get(j);
				if ((!(tempProp1.getType().equals(tempProp2.getType())) && (tempProp1.getDetail().equalsIgnoreCase(tempProp2.getMembershipCode())))){
					count++;
				}
			}
		}					
		return count;
	}

	/**
	 * @param propsGroup
	 * @return
	 */
	private static int countAggregation(List<Proposition> propsGroup) {
		int count = 0;
		Proposition tempProp1;
		Proposition tempProp2;
		for (int i = 0; i < propsGroup.size(); i++){
			tempProp1 = propsGroup.get(i);
			for (int j = 0; j < propsGroup.size(); j++){
				tempProp2 = propsGroup.get(j);
				if ((tempProp1.getType().equals(tempProp2.getMembership())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
					count++;
				}
			}
		}
		return count;
	}

	/**
	 * @param propsGroup - a group of propositions that can be aggregated
	 * @return groups - a list of pairs of propositions that can be aggregated, in order. The first element of the pair is the main entity and the
	 * second is the aggregated piece
	 */
	private static List<Pair<Proposition,Proposition>> groupAggregationsPairs(List<Proposition> propsGroup) {
		List<Pair<Proposition,Proposition>> groups = new ArrayList<Pair<Proposition,Proposition>>();
		Proposition tempProp1;
		Proposition tempProp2;
		for (int i = 0; i < propsGroup.size(); i++){
			tempProp1 = propsGroup.get(i);	
			for (int j = 0; j < propsGroup.size(); j++){
				tempProp2 = propsGroup.get(j);
				Pair<Proposition,Proposition> aPair = new Pair<Proposition,Proposition>();
				if (tempProp1.getType().equalsIgnoreCase("trend_description")){
					if ((tempProp1.getDetail().equals(tempProp2.getMembershipCode())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						aPair.setFirst(tempProp1);
						aPair.setSecond(tempProp2);
					}
				}
				else{
					if ((tempProp1.getType().equals(tempProp2.getMembership())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						aPair.setFirst(tempProp1);
						aPair.setSecond(tempProp2);
					}
				}
				if (aPair.getFirst() != null)
					groups.add(aPair);
			}

		}
		for (Pair<Proposition,Proposition> aPropPair : groups){
			System.out.println("This is a pair:");
			System.out.println(aPropPair.getFirst().getType());
			System.out.println(aPropPair.getSecond().getType());
			System.out.println("------------------------------");
		}
		return groups;
	}
	
	private static List<List<Proposition>> groupAggregationsLists(List<Proposition> propsGroup) {
		List<List<Proposition>> groups = new ArrayList<List<Proposition>>();
		Proposition tempProp1;
		Proposition tempProp2;
		boolean firstElement = true;
		for (int i = 0; i < propsGroup.size(); i++){
			tempProp1 = propsGroup.get(i);
			List<Proposition> aList = new ArrayList<Proposition>();
			for (int j = 0; j < propsGroup.size(); j++){
				tempProp2 = propsGroup.get(j);
				if (tempProp1.getType().equalsIgnoreCase("trend_description")){
					if ((tempProp1.getDetail().equals(tempProp2.getMembershipCode())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						if (firstElement){
							aList.add(tempProp1);
							firstElement = false;
						}
						aList.add(tempProp2);
					}
				}
				else{
					if ((tempProp1.getType().equals(tempProp2.getMembership())) && (!(tempProp1.getType().equals(tempProp2.getType())))){
						if (firstElement){
							aList.add(tempProp1);
							firstElement = false;
						}
						aList.add(tempProp2);
					}
				}
			}
			if (!(aList.isEmpty()))
				groups.add(aList);
			firstElement = true;

		}
		for (List<Proposition> aPropList : groups){
			System.out.println("This is a list:");
			for (Proposition aProp : aPropList){
				System.out.println(aProp.getType());	
			}
			System.out.println("------------------------------");
		}
		return groups;
	}
	
	/**
	 * @param word
	 * @param pos
	 * @param database
	 * @param aVocab
	 * @param vocab
	 * @return
	 */
	private static Set<String> returnLexiconNgrams(String word, String pos, WordNetDatabase database){
		
		NounSynset nounSynset;
		VerbSynset verbSynset;
		AdjectiveSynset adjectiveSynset;
		AdverbSynset adverbSynset;
		
		VocabularyByGrade aVocabulary = null;
		try {
			aVocabulary = new VocabularyByGrade();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Synset[] synsets = null;
		Set<String> tempSynsets = new HashSet<String>();
		
		if (pos.equalsIgnoreCase("adjective"))
			synsets = database.getSynsets(word, SynsetType.ADJECTIVE);
		else if (pos.equalsIgnoreCase("verb"))
			synsets = database.getSynsets(word, SynsetType.VERB);
		else if (pos.equalsIgnoreCase("adverb"))
			synsets = database.getSynsets(word, SynsetType.ADVERB);
		else
			synsets = database.getSynsets(word, SynsetType.NOUN);
		
		
		if (pos.equalsIgnoreCase("noun"))
		{
			//System.out.println("Getting noun synsets");
			for (int i = 0; i < synsets.length; i++) { 
				nounSynset = (NounSynset)(synsets[i]);
				//System.out.println("There are "+nounSynset.getWordForms().length+" in the noun synset.");
			    for (String aNoun : nounSynset.getWordForms())
			    {
			    	//System.out.println(aNoun);
				    tempSynsets.add(aNoun);
			    }
			}
			//System.out.println("All noun synsets are: "+tempSynsets);
			//System.out.println("Stric noun synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("verb"))
		{
			//System.out.println("Getting verb synsets");
			for (int i = 0; i < synsets.length; i++) { 
				verbSynset = (VerbSynset)(synsets[i]);
			    //System.out.println("There are "+verbSynset.getWordForms().length+" in the verb synset.");
			    for (String aVerb : verbSynset.getWordForms())
			    {
			    	//System.out.println(aVerb);
			    	tempSynsets.add(aVerb);
			    }
			}
			//System.out.println("All verb synsets are: "+tempSynsets);
			//System.out.println("Stric verb synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("adjective"))
		{
			//System.out.println("Getting adjective synsets");
			for (int i = 0; i < synsets.length; i++) { 
				adjectiveSynset = (AdjectiveSynset)(synsets[i]);
				//System.out.println("There are "+adjectiveSynset.getWordForms().length+" in the adjective synset.");
			    for (String anAdjective : adjectiveSynset.getWordForms())
			    {
			    	//System.out.println(anAdjective);
			    	tempSynsets.add(anAdjective);
			    }
			}
			//System.out.println("All adjectives synsets are: "+tempSynsets);
			//System.out.println("Stric verb synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("adverb"))
		{
			//System.out.println("Getting adverb synsets");
			for (int i = 0; i < synsets.length; i++) { 
				adverbSynset = (AdverbSynset)(synsets[i]);
				//System.out.println("There are "+adverbSynset.getWordForms().length+" in the adverb synset.");
			    for (String anAdverb : adverbSynset.getWordForms())
			    {
			    	//System.out.println(anAdverb);
			    	tempSynsets.add(anAdverb);
			    }
			}
		}
		return tempSynsets;
		
	}
	
	
	/**
	 * @param word
	 * @param pos
	 * @param database
	 * @param aVocab
	 * @param vocab
	 * @return
	 */
	private static String returnAppropriateLexicon(String word, String pos, WordNetDatabase database, Set<String> aVocab, String vocab){
		
		NounSynset nounSynset;
		VerbSynset verbSynset;
		AdjectiveSynset adjectiveSynset;
		AdverbSynset adverbSynset;
		
		VocabularyByGrade aVocabulary = null;
		try {
			aVocabulary = new VocabularyByGrade();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Synset[] synsets = null;
		Set<String> tempSynsets = new HashSet<String>();
		
		if (pos.equalsIgnoreCase("adjective"))
			synsets = database.getSynsets(word, SynsetType.ADJECTIVE);
		else if (pos.equalsIgnoreCase("verb"))
			synsets = database.getSynsets(word, SynsetType.VERB);
		else if (pos.equalsIgnoreCase("adverb"))
			synsets = database.getSynsets(word, SynsetType.ADVERB);
		else
			synsets = database.getSynsets(word, SynsetType.NOUN);
		
		
		if (pos.equalsIgnoreCase("noun"))
		{
			//System.out.println("Getting noun synsets");
			for (int i = 0; i < synsets.length; i++) { 
				nounSynset = (NounSynset)(synsets[i]);
				//System.out.println("There are "+nounSynset.getWordForms().length+" in the noun synset.");
			    for (String aNoun : nounSynset.getWordForms())
			    {
			    	//System.out.println(aNoun);
				    tempSynsets.add(aNoun);
			    }
			}
			//System.out.println("All noun synsets are: "+tempSynsets);
			//System.out.println("Stric noun synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("verb"))
		{
			//System.out.println("Getting verb synsets");
			for (int i = 0; i < synsets.length; i++) { 
				verbSynset = (VerbSynset)(synsets[i]);
			    //System.out.println("There are "+verbSynset.getWordForms().length+" in the verb synset.");
			    for (String aVerb : verbSynset.getWordForms())
			    {
			    	//System.out.println(aVerb);
			    	tempSynsets.add(aVerb);
			    }
			}
			//System.out.println("All verb synsets are: "+tempSynsets);
			//System.out.println("Stric verb synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("adjective"))
		{
			//System.out.println("Getting adjective synsets");
			for (int i = 0; i < synsets.length; i++) { 
				adjectiveSynset = (AdjectiveSynset)(synsets[i]);
				//System.out.println("There are "+adjectiveSynset.getWordForms().length+" in the adjective synset.");
			    for (String anAdjective : adjectiveSynset.getWordForms())
			    {
			    	//System.out.println(anAdjective);
			    	tempSynsets.add(anAdjective);
			    }
			}
			//System.out.println("All adjectives synsets are: "+tempSynsets);
			//System.out.println("Stric verb synsets are: "+returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc()));
		}
		else if (pos.equalsIgnoreCase("adverb"))
		{
			//System.out.println("Getting adverb synsets");
			for (int i = 0; i < synsets.length; i++) { 
				adverbSynset = (AdverbSynset)(synsets[i]);
				//System.out.println("There are "+adverbSynset.getWordForms().length+" in the adverb synset.");
			    for (String anAdverb : adverbSynset.getWordForms())
			    {
			    	//System.out.println(anAdverb);
			    	tempSynsets.add(anAdverb);
			    }
			}
		}
		List<String> restrictSynset = new ArrayList<String>();
		if (tempSynsets == null)
			return word;
		else
		{
			restrictSynset = returnStrictSynset(vocab, tempSynsets, aVocabulary.getVocab45(), aVocabulary.getVocab68(), aVocabulary.getVocab910(), aVocabulary.getVocab11cc());
			//System.out.println(restrictSynset);
			if (restrictSynset.size() == 0)
				return word;
			else if (restrictSynset.size() == 1)
				return restrictSynset.get(0);
			else
			{
				//System.out.println("Size of restrict set is "+restrictSynset.size());
				Random rand = new Random(); 
				int position = rand.nextInt(restrictSynset.size());
				return restrictSynset.get(position);
			}
		}
		
	}

	/**
	 * 
	 * @param vocabTarget - a string with the grade vocabulary name - used for comparison
	 * @param oldSynset - the synset acquired from a word
	 * @param vocab45 - original vocabulary for 4th and 5th grades
	 * @param vocab68 - original vocabulary for 6th through 8th grades
	 * @param vocab910 - original vocabulary for 9th and 10th grades
	 * @param vocab11cc - original vocabulary for 11th and up grades
	 * @return a new synset with only terms that appear at a higher grade vocabulary but
	 * 		not at the 4th and 5th vocabulary
	 */
	private static List<String> returnStrictSynset(String vocabTarget, Set<String> oldSynset, Set<String> vocab45, Set<String> vocab68, Set<String> vocab910, Set<String> vocab11cc){
		List<String> newSynset = new ArrayList<String>();
		if (vocabTarget.equalsIgnoreCase("vocab68"))
		{
			for (String term : oldSynset)
				if (vocab68.contains(term) && (!(vocab45.contains(term))))
					newSynset.add(term);
		}
		else if (vocabTarget.equalsIgnoreCase("vocab910"))
		{
			for (String term : oldSynset)
				if (vocab910.contains(term) && (!(vocab45.contains(term))))
					newSynset.add(term);
		}
		else if (vocabTarget.equalsIgnoreCase("vocab11cc"))
		{
			for (String term : oldSynset)
				if (vocab11cc.contains(term) && (!(vocab45.contains(term))))
					newSynset.add(term);
		}
		else
		{
			for (String term : oldSynset)
				if (vocab45.contains(term))
					newSynset.add(term);
		}
		return newSynset;
	}
	
	/**
	 * @param pos
	 * @return
	 */
	private static String returnLexiconPOS(String pos){
		if (pos.contains("noun"))
			return "noun";
		else if (pos.contains("adj"))
			return "adjective";
		else if (pos.contains("verb"))
			return "verb";
		else if (pos.contains("adv"))
			return "adverb";
		else
			return "null";
	}
	
	/**
	 * @param originalProps
	 * It updates the counts for the features of the DT with the new words that were returned from the lexicon choice phase
	 */
	private static void updatePropositionAllowedRealisation(List<Proposition> originalProps){
		int newWords = 0;
		int newAvgWordLength = 0;
		int newAvgNPLength = 0;
		int newAvgVPLength = 0;
				
		for (Proposition aProp : originalProps){
			//First complete the lexicon with values in the prop structure
			if ((aProp.getType().equalsIgnoreCase("graph_volatility")) || (aProp.getType().equalsIgnoreCase("volatility"))){
				Map<String,String> tempLexicon = new HashMap<String,String>();
				tempLexicon = aProp.getLexicon();
				tempLexicon.put("noun_object", aProp.getDegree());
				aProp.setLexicon(tempLexicon);
			}
			if (aProp.getType().equalsIgnoreCase("entity_description")){
				Map<String,String> tempLexicon = new HashMap<String,String>();
				String tempNounObject = null;
				tempNounObject = aProp.getDescription()+" in "+aProp.getValueDetail();
				tempLexicon = aProp.getLexicon();
				tempLexicon.put("noun_object", tempNounObject);
				aProp.setLexicon(tempLexicon);
			}
			if (aProp.getType().equalsIgnoreCase("composed_trend")){
				Map<String,String> tempLexicon = new HashMap<String,String>();
				String tempNounObject = null;
				for (List<String> aList : aProp.getAdditionalInfo()){
					for (String element : aList){
						tempNounObject += element+" ";
					}
				}
				tempNounObject = aProp.getDescription()+" consisting of a "+tempNounObject+ "from to followed by a until";
				tempLexicon = aProp.getLexicon();
				tempLexicon.put("noun_object", tempNounObject);
				aProp.setLexicon(tempLexicon);
			}
			if ((aProp.getType().equalsIgnoreCase("overall_period_years")) || 
				(aProp.getType().equalsIgnoreCase("overall_period_months")) ||
				(aProp.getType().equalsIgnoreCase("overall_period_days"))){
				Map<String,String> tempLexicon = new HashMap<String,String>();
				String tempNounObject = null;
				tempLexicon = aProp.getLexicon();
				tempNounObject = aProp.getValue()+" "+aProp.getValueDetail();
				tempLexicon.put("noun_object", tempNounObject);
				aProp.setLexicon(tempLexicon);
			}
			if ((aProp.getType().equalsIgnoreCase("maximum_point_date")) || (aProp.getType().equalsIgnoreCase("minimum_point_date"))){
				Map<String,String> tempLexicon = new HashMap<String,String>();
				tempLexicon = aProp.getLexicon();
				tempLexicon.put("noun_object", aProp.getValue());
				aProp.setLexicon(tempLexicon);
			}
			for (Realisation aReal : aProp.getAllowedRealisations()){
				newWords = 0;
				newAvgWordLength = 0;
				newAvgNPLength = 0;
				newAvgVPLength = 0;
				
				newWords = aReal.getWords();
				newAvgWordLength = aReal.getAvgWordLenght();
				newAvgNPLength = aReal.getAvgNPLength();
				newAvgVPLength = aReal.getAvgVPLength();
				//System.out.println("---------------------------"+aProp.getLexicon().values());
				//System.out.println("For prop "+aProp.getType()+" and realization "+aReal.getRealisationType()+" there are "+aReal.getWords()+" words.");
				for (String aLexicon : aProp.getLexicon().keySet()){
					System.out.println(aLexicon);
					newWords += 1;
					if (newAvgWordLength == 0)
						newAvgWordLength = aProp.getLexicon().get(aLexicon).length();
					else
						newAvgWordLength = (aProp.getLexicon().get(aLexicon).length() + newAvgWordLength)/2;
					if (!(aLexicon.contains("verb")))
						if (newAvgNPLength == 0)
							newAvgNPLength = aProp.getLexicon().get(aLexicon).length();
						else
							newAvgNPLength = (aProp.getLexicon().get(aLexicon).length() + newAvgNPLength)/2;
					else if (!(aLexicon.contains("subject")))
						if (newAvgVPLength == 0)
							newAvgVPLength = aProp.getLexicon().get(aLexicon).length();
						else
							newAvgVPLength = (aProp.getLexicon().get(aLexicon).length() + newAvgVPLength)/2;
				}
				aReal.setWords(newWords);
				aReal.setAvgNPLength(newAvgNPLength);
				aReal.setAvgVPLength(newAvgVPLength);
				aReal.setAvgWordLenght(newAvgWordLength);
			}
		}
		
		for (Proposition aProp : originalProps){
			System.out.println("For proposition "+aProp.getType());
			for (Realisation aReal : aProp.getAllowedRealisations()){
				System.out.println("Realisation type: "+aReal.getRealisationType());
				System.out.println("Words: "+aReal.getWords());
				System.out.println("AvgWordLength: "+aReal.getAvgWordLenght());
				System.out.println("AvgNPLength: "+aReal.getAvgNPLength());
				System.out.println("AvgVPLength: "+aReal.getAvgVPLength());
			}
		}
	}
	
	/**
	 * @param props1 - list of propositions of group 1
	 * @param props2 - list of propositions of group 2
	 * @param props3 - list of propositions of group 3
	 * @param aRuleGroup - one set of rules for the target reading level
	 * @return
	 */
	private static List<Map<Pair<Proposition,Proposition>, Map<String,String>>> checkReadabilityRuleMatch(List<Proposition> props1,
			List<Proposition> props2, List<Proposition> props3, Map<String, Pair<Integer, Integer>> aRuleGroup){
		//This structure stores something like:
		//key: graph, entity_description
		//value: key: "Realisation" value: "RC"
		//		 key: "Voice" value: "Active"
		//       key: "RefExpression" value: "Pronoun"/"Description"
		// there will be one for each organization group
		List<Map<Pair<Proposition,Proposition>, Map<String,String>>> allAggregationGroups = new ArrayList<Map<Pair<Proposition,Proposition>, Map<String,String>>>();
		Map<Pair<Proposition,Proposition>, Map<String,String>> aggregationsGroup1 = new HashMap<Pair<Proposition,Proposition>, Map<String,String>>();
		Map<Pair<Proposition,Proposition>, Map<String,String>> aggregationsGroup2 = new HashMap<Pair<Proposition,Proposition>, Map<String,String>>();
		Map<Pair<Proposition,Proposition>, Map<String,String>> aggregationsGroup3 = new HashMap<Pair<Proposition,Proposition>, Map<String,String>>();
		
		int Sentences = 0;
		int Words = 0;
		int AvgWordLength = 0;
		int Pronouns = 0;
		int BegPronoun = 0;
		int Article = 0;
		int BegArticle = 0;
		int RelativeClause = 0;
		int Adjective = 0;
		int Adverb = 0;
		int Passive = 0;
		int Preposition = 0;
		int Conjunction = 0;
		int AvgNPLength = 0;
		int AvgVPLength = 0;
		
		//Get list of pairs of propositions that can be aggregated for each group
		List<List<Proposition>> aggregatedGroups1 = groupAggregationsLists(props1);
		List<List<Proposition>> aggregatedGroups2 = groupAggregationsLists(props2);
		List<List<Proposition>> aggregatedGroups3 = groupAggregationsLists(props3);
		
		//this variable will hold the main entity having aggregation to it, so that we don't count its
		//values twice
		String currentMainEntity = "";
		boolean addMainEntity = true;
		boolean matches = true;
		
		//Iterate over rules
		for (Map.Entry<String, Pair<Integer,Integer>> aRule : aRuleGroup.entrySet()){
			for (List<Proposition> aPropList : aggregatedGroups1){
				for (int i = 0; i < aPropList.size(); i++){
					if (possibleToRealiseAs(aPropList.get(i),"ADJ")){
						Sentences += avgParagLengthSent(aPropList.get(i),"ADJ");
						Words += avgSentLengthWord(aPropList.get(i),"ADJ");
						AvgWordLength += avgWordLengthChar(aPropList.get(i),"ADJ");
						Pronouns += percPronouns(aPropList.get(i),"ADJ");
						BegPronoun += begSentPronoun(aPropList.get(i),"ADJ");
						Article += numberArticles(aPropList.get(i),"ADJ");
						BegArticle += begSentArticle(aPropList.get(i),"ADJ");
						RelativeClause += numberRelativeClauses(aPropList.get(i),"ADJ");
						Adjective += numberAdjectives(aPropList.get(i),"ADJ");
						Adverb += numberAdverbs(aPropList.get(i),"ADJ");
						Passive += percPassiveSent(aPropList.get(i),"ADJ");
						Preposition += percPrepositions(aPropList.get(i),"ADJ");
						Conjunction += percConjunctions(aPropList.get(i),"ADJ");
						AvgNPLength += avgNounPhraseLength(aPropList.get(i),"ADJ");
						AvgVPLength += avgVerbPhraseLength(aPropList.get(i),"ADJ");
					}
				}
				if (aRule.getKey().equalsIgnoreCase("avgParagLengthSent")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("avgSentLengthWord")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("avgWordLengthChar")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("percPronouns")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("begSentPronoun")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("numberArticles")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("begSentArticle")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("numberRelativeClauses")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("numberAdjectives")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("numberAdverbs")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("percPassiveSent")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("percPrepositions")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("percConjunctions")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("avgNounPhraseLength")){
					
				}
				else if (aRule.getKey().equalsIgnoreCase("avgVerbPhraseLength")){
					
				}
			}
		}
			
		for (List<Proposition> aProp : aggregatedGroups1){
			for (Proposition aProp : aPropList){
				for (Realisation aRealisation : aProp.getAllowedRealisations())
					Sentences += 
			}
		}
		return allAggregationGroups;
	}
	
	
	private static int avgParagLengthSent(Proposition prop, String realisation){
		int sentences = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				sentences = aRealisation.getSentences();
		return sentences;
	}
	
	private static int avgSentLengthWord(Proposition prop, String realisation){
		int words = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				words = aRealisation.getWords();
		return words;
	}
	
	private static int avgWordLengthChar(Proposition prop, String realisation){
		int characters = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				characters = aRealisation.getAvgWordLenght();
		return characters;
	}
	
	private static int percPronouns(Proposition prop, String realisation){
		int pronouns = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				pronouns = aRealisation.getPronouns();
		return pronouns;
	}
	
	private static int begSentPronoun(Proposition prop, String realisation){
		int begPronouns = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				begPronouns = aRealisation.getBegPronouns();
		return begPronouns;
	}
	
	private static int numberArticles(Proposition prop, String realisation){
		int articles = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				articles = aRealisation.getArticles();
		return articles;
	}
	
	private static int begSentArticle(Proposition prop, String realisation){
		int begArticles = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				begArticles = aRealisation.getBegArticles();
		return begArticles;
	}
	
	private static int numberRelativeClauses(Proposition prop, String realisation){
		int relative = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				relative = aRealisation.getRelativeClause();
		return relative;
	}
	
	private static int numberAdjectives(Proposition prop, String realisation){
		int adjectives = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				adjectives = aRealisation.getAdjective();
		return adjectives;
	}
	
	private static int numberAdverbs(Proposition prop, String realisation){
		int adverbs = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				adverbs = aRealisation.getAdverb();
		return adverbs;
	}
	
	private static int percPassiveSent(Proposition prop, String realisation){
		int passive = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				passive = aRealisation.getPassive();
		return passive;
	}
	
	private static int percPrepositions(Proposition prop, String realisation){
		int preposition = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				preposition = aRealisation.getPrepositions();
		return preposition;
	}
	
	private static int percConjunctions(Proposition prop, String realisation){
		int conjunction = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				conjunction = aRealisation.getConjunctions();
		return conjunction;
	}
	
	private static int avgNounPhraseLength(Proposition prop, String realisation){
		int nounPhrase = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				nounPhrase = aRealisation.getAvgNPLength();
		return nounPhrase;
	}
	
	private static int avgVerbPhraseLength(Proposition prop, String realisation){
		int verbPhrase = 0;
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				verbPhrase = aRealisation.getAvgVPLength();
		return verbPhrase;
	}
	
	/**
	 * @param prop - a Proposition object
	 * @param realisation - a string representing a realisation (one of "S", "ADJ", "RC")
	 * @return true if realisation is in the set of possible realisations of prop
	 */
	private static boolean possibleToRealiseAs(Proposition prop, String realisation){
		for (Realisation aRealisation : prop.getAllowedRealisations())
			if (aRealisation.getRealisationType().equalsIgnoreCase(realisation))
				return true;
		return false;
	}
	
	private static boolean checkReadabilityRuleMatchGroup2(List<Proposition> props2){
		int Sentences = 0;
		int Words = 0;
		int AvgWordLength = 0;
		int Pronouns = 0;
		int BegPronoun = 0;
		int Article = 0;
		int BegArticle = 0;
		int RelativeClause = 0;
		int Adjective = 0;
		int Adverb = 0;
		int Passive = 0;
		int Preposition = 0;
		int Conjunction = 0;
		int AvgNPLength = 0;
		int AvgVPLength = 0;
		
		int aggregationProps2 = countAggregation(props2);
		//List<Pair<Proposition,Proposition>> aggregatedGroups = groupAggregationsPairs(props2);
		for (Proposition aProp : props){
			
		}
		return true;
	}
	
	private static boolean checkReadabilityRuleMatchGroup3(List<Proposition> props3){
		int Sentences = 0;
		int Words = 0;
		int AvgWordLength = 0;
		int Pronouns = 0;
		int BegPronoun = 0;
		int Article = 0;
		int BegArticle = 0;
		int RelativeClause = 0;
		int Adjective = 0;
		int Adverb = 0;
		int Passive = 0;
		int Preposition = 0;
		int Conjunction = 0;
		int AvgNPLength = 0;
		int AvgVPLength = 0;
		
		int aggregationProps3 = countAggregation(props3);
		//List<Pair<Proposition,Proposition>> aggregatedGroups = groupAggregations(props3);
		for (Proposition aProp : props){
			
		}
		return true;
	}
	
	private static void callSearchRealisation(){
		SearchNode node = new SearchNode();
		node.setPropSentences(propSentences);
		node.setSentenceMap(currentSentenceMap);
		//start testing with props from group one.
		//TODO:change propsGroup1 to all props after testing
		node.setUnrealisedProps(propsGroup1);
		node.setPrune(false);
		
		List<SearchNode> nodes = new ArrayList<SearchNode>();
		nodes.add(node);
		
		nodes = searchRealisation(nodes, false);
	}

	/**
	 * Recursive method that receives a group of propositions and controls which operators
	 * should be applied to a given proposition. Kathy and I have decided the sentences will
	 * be generated on active and passive voice always using anaphora. Pronouns will be dealt
	 * with later.
	 * @param list of search nodes and boolean to control recursion
	 * @return list of search nodes and boolean to control recursion
	 */
	private static List<SearchNode> searchRealisation(List<SearchNode> nodes, boolean finished){
		boolean continueSearch = false;
		Pair<String,SPhraseSpec> sentence = null;
		System.out.println("Here starts search algorithm ........................");
		
		//create a new list of SearchNodes
		List<SearchNode> newNodes = new ArrayList<SearchNode>();
		
		//continue on the recursion
		while (!finished){
			for (SearchNode node : nodes){
				List<Proposition> props1 = node.getUnrealisedProps();
				//need to check if we need to continue expanding this node
				if (props.size() > 1)
					continueSearch = true;
				//block for realizing sentences using active voice and anaphora
				for (int i=0; i<props1.size(); i++){
					//each proposition will generate a new node which will contain the realised proposition (current)
					//and a list of unrealised ones. Each node keeps the data structures for map and stack for tracing
					//realisations and aggregations
					SearchNode aNode = new SearchNode();
					
					Proposition prop1 = props.get(i);
					System.out.println("Node chosen: "+prop1.getType());
					sentence = applyOperatorSentence(props.get(i), prop1, "active", "anaphora");
					System.out.println("Realising sentence: "+sentence.getFirst());
					//prop1.setUsed(true);
		
					List<Proposition> unrealisedProps = new ArrayList<Proposition>();
					for (int j=0; j<props1.size(); j++){
						if (!(props.get(j).isUsed()) && (props.get(j) != props.get(i)))
							unrealisedProps.add(props.get(j));
					}
					aNode.setUnrealisedProps(unrealisedProps);
					aNode.setPropSentences(propSentences);
					aNode.setSentenceMap(currentSentenceMap);
					aNode.setPrune(false);
					
					newNodes.add(aNode);
					for (int j=1; i<props1.size(); j++){
						Proposition prop2 = chooseNode(props1);
						sentence = applyOperatorAggregation(prop1, prop2, "ADJ");
						System.out.println("Realising aggregation "+sentence.getSecond());
						prop2.setUsed(true);
					}
					
		
				}
				
				//generating passive sentences using anaphora
				for (int i=0; i<props1.size(); i++){
					//each proposition will generate a new node which will contain the realised proposition (current)
					//and a list of unrealised ones. Each node keeps the data structures for map and stack for tracing
					//realisations and aggregations
					SearchNode aNode = new SearchNode();
					
					Proposition prop1 = props.get(i);
					System.out.println("Node chosen: "+prop1.getType());
					sentence = applyOperatorSentence(props.get(i), prop1, "passive", "anaphora");
					System.out.println("Realising sentence: "+sentence.getFirst());
					//prop1.setUsed(true);
		
					List<Proposition> unrealisedProps = new ArrayList<Proposition>();
					for (int j=0; j<props1.size(); j++){
						if (!(props.get(j).isUsed()) && (props.get(j) != props.get(i)))
							unrealisedProps.add(props.get(j));
					}
					aNode.setUnrealisedProps(unrealisedProps);
					aNode.setPropSentences(propSentences);
					aNode.setSentenceMap(currentSentenceMap);
					aNode.setPrune(false);
					
					nodes.add(aNode);
		
				}
				
				
				System.out.println("There are "+nodes.size()+" nodes in this phase. Printing sentences....");
		//		for (SearchNode aNode : nodes){
		//			System.out.println("Node: "+aNode.getSentenceMap())
		//		}
			}
		}
		return searchRealisation(newNodes, continueSearch);
		
	}
	
	/**
	 * Method that returns the next unrealised proposition
	 * @param props - list of propositions in a group
	 * @return the first proposition found in the list of propositions that hasn't been realised yet
	 */
	private static Proposition chooseNode(List<Proposition> props){
		Pair<String,SPhraseSpec> partialState = null;
		PropRealisation aPropTemplate = new PropRealisation();
		for (Proposition aProp : props){
			if (!(aProp.isUsed()))
				return aProp;
		}
		return null;
	}
	
	/**
	 * This method applies the generate sentence operator
	 * @param prop1 - proposition of the main entity
	 * @param prop2 - proposition to be aggregated to prop1
	 * @param voice - one of "active" | "passive"
	 * @param referringExpression - use "pronoun" if you want the sentence to start with a pronoun
	 * @return the pair for the final sentence - the sentence string and SPHraseSpec object
	 */
	private static Pair<String,SPhraseSpec> applyOperatorSentence(Proposition prop1, Proposition prop2, String voice, String referringExpression){
		Pair<String,SPhraseSpec> partialState = null;
		PropRealisation aPropTemplate = new PropRealisation();
		partialState = generateSearchSentenceGroup1(prop1, prop1, aPropTemplate, voice, referringExpression);
		return partialState;
	}
	
	/**
	 * This method applies the generate aggregation operator
	 * @param prop1 - proposition of the main entity
	 * @param prop2 - proposition to be aggregated to prop1
	 * @param aggregationType - one of "ADJ" | "RC"
	 * @param voice - one of "active" | "passive" - used in some special realisation cases
	 * @return the pair for the final sentence - the sentence string and SPHraseSpec object
	 */
	private static Pair<String,SPhraseSpec> applyOperatorAggregation(Proposition prop1, Proposition prop2, String aggregationType){
		Pair<String,SPhraseSpec> partialState = null;
		PropRealisation aPropTemplate = new PropRealisation();
			Pair<String,SPhraseSpec> currentPartialState = propSentences.pop();
		if (aggregationType.equalsIgnoreCase("ADJ"))
			partialState = generateSearchAdjectiveGroup1(prop1, currentPartialState.getFirst(), prop2, aPropTemplate, currentPartialState.getSecond(), "ADJ");
		else if (aggregationType.equalsIgnoreCase("RC"))
			partialState = generateSearchRCGroup1("RC", currentPartialState.getFirst(), prop2, aPropTemplate, currentPartialState.getSecond());

		return partialState;
	}
	
	private static Pair<String,SPhraseSpec> generateSearchSentenceGroup1(Proposition tempProp1, Proposition tempProp2, 
			PropRealisation aPropTemplate, String voice, String referringExpression){
	
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		//int localSimplifyLexiconGroup1 = 2;
		
		tempSubject.put(tempProp1.getType(), 0);
		tempSubject.put("NumberAdjectives", 0);
		tempSubject.put("NumberRelativeClauses", 0);
		tempObject.put(tempProp2.getType(), 0);
		tempObject.put("NumberAdjectives", 0);
		tempObject.put("NumberRelativeClauses", 0);
		tempSemanticRole.put("SUBJ", tempSubject);
		tempSemanticRole.put("OBJ", tempObject);
		String rootPredicate = "";
		String realised = null;
		
		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "show";
			if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
				sentence = aPropTemplate.showDetail(tempProp1, tempProp2, rootPredicate, voice, referringExpression);
			else
				sentence = aPropTemplate.show(tempProp1, tempProp2, rootPredicate, voice, referringExpression);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("consist")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "consist";
			sentence = aPropTemplate.consist(tempProp1, tempProp2, rootPredicate, voice, referringExpression);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			sentence = aPropTemplate.present(tempProp1, tempProp2, tempProp2.getValueDetail(), rootPredicate, voice, referringExpression);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			sentence = aPropTemplate.type(tempProp1, tempProp2, voice, referringExpression);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			/* the lexical choices for volatility are tied to the reading level. Realised as a conjunction it is simple, and as an adjective or noun it is advanced.
			   However, since this is the sentence realisation, only conjunction and noun are allowed.
			   TODO: add randomness to the choice of allowed realisations given a complexity */
			String volatilityRealisation = "";
			String volatilityPOS = "";

			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "noun";
			if (volatilityRealisation.equalsIgnoreCase(""))
				volatilityRealisation = "volatility";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			sentence = aPropTemplate.volatility(tempProp1, tempProp2, volatilityPOS, rootPredicate, volatilityRealisation, voice, referringExpression);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
	}

	private static Pair<String,SPhraseSpec> generateSearchAdjectiveGroup1(Proposition tempProp1, String currentSentence, Proposition tempProp2, PropRealisation aPropTemplate, SPhraseSpec currentSentenceObject, String tempPOS)
	{
		String tempPOS = null;
		GenerateSentence aSentence = new GenerateSentence();
		String realised = null;
		SPhraseSpec sentence = null;
		
		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		tempSubject.put("NumberAdjectives", tempSubject.get("NumberAdjectives") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);
		
		//call methods that will aggregate content of temProp2 with either the subject or the object of the sentence as an ADJ
		//then update currentSentenceMap by deleting copying content and deleting current sentence and adding the new sentence 
		//with updated values. Don't forget to set tempProp2.isUsed to true!
			
		//only type and volatility accept ADJ for our case - just to keep the code cleaner and smaller, I will omit these options here, but
		// they should be added for a more general application of this code
		if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			//Here we call the methods for aggregating tempProp2 as an adjective of the subject representing tempProp1 in the sentence
			sentence = aPropTemplate.typeADJ(currentSentenceObject, tempProp2, tempPOS);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			System.out.println(realised);
			//Add new sentence to the map --> See if we will need the old ones. If not, remove them from the map and only the remaining
			//ones will compose the final summary
		//	currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "volatile";
			String volatilityPOS = "adj";

			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			sentence = aPropTemplate.volatilityADJ(currentSentenceObject, tempProp2, tempPOS, volatilityRealisation);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			System.out.println(realised);
		//	currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		//currentSentenceMap.remove(currentSentence);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
	}
	
	private static Pair<String,SPhraseSpec> generateSearchRCGroup1(String tempPOS, String currentSentence, Proposition tempProp2, 
			PropRealisation aPropTemplate, SPhraseSpec currentSentenceObject){
		boolean conjunction = false;
		SPhraseSpec sentence = null;
		String rootPredicate = "";
		String realised = null;
		GenerateSentence aSentence = new GenerateSentence();

		//This block updates the values of the inner maps that control the aggregations done so far
		tempSemanticRole = currentSentenceMap.get(currentSentence);
		tempSubject = tempSemanticRole.get(tempPOS);
		
		//checking if it is needed to add a new clause as a coordination instead of a regular relative clause
		if (tempSubject.get("NumberRelativeClauses") > 0)
			conjunction = true;
		tempSubject.put("NumberRelativeClauses", tempSubject.get("NumberRelativeClauses") + 1);
		tempSemanticRole.put(tempPOS, tempSubject);
		
		//call method that will aggregate content of temProp2 with the subject of the sentence as a RC, then update currentSentenceMap by
		//deleting copying content and deleting current sentence and adding the new sentence with updated values. Don't forget
		//to set tempProp2.isUsed to true!

		if (tempProp2.getPropTemplate().equalsIgnoreCase("show")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "show";
			if (conjunction)
				if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
					sentence = aPropTemplate.showDetail(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
				else
					sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				if (tempProp2.getType().equalsIgnoreCase("composed_trend"))
					sentence = aPropTemplate.showDetail(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
				else
					sentence = aPropTemplate.show(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("consist")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "consist";
			if (conjunction)
				sentence = aPropTemplate.consist(currentSentenceObject, tempProp2, tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.consist(currentSentenceObject, tempProp2, tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("present")){
			if (rootPredicate.equalsIgnoreCase(""))
				rootPredicate = "present";
			if (conjunction)
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.present(currentSentenceObject, tempProp2, tempProp2.getValueDetail(), tempPOS, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("type")){
			if (conjunction)
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "conjunction");
			else
				sentence = aPropTemplate.typeRC(currentSentenceObject, tempProp2, tempPOS, "relativeClause");
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//currentSentenceMap.put(realised, tempSemanticRole);
		} else if (tempProp2.getPropTemplate().equalsIgnoreCase("volatility")){
			String volatilityRealisation = "";
			String volatilityPOS = "";
			//here randomness will help getting varied summaries
			//TODO: add randomness
			if (volatilityPOS.equalsIgnoreCase(""))
				volatilityPOS = "adj";
			if (volatilityRealisation.isEmpty())
				volatilityRealisation = "volatile";
			if (rootPredicate.isEmpty())
				rootPredicate = "show";
			if (conjunction)
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "conjunction", rootPredicate);
			else
				sentence = aPropTemplate.volatilityRC(currentSentenceObject, tempProp2, volatilityPOS, tempPOS, volatilityRealisation, "relativeClause", rootPredicate);
			tempProp2.setUsed(true);
			realised = aSentence.realiser.realiseSentence(sentence);
			//currentSentenceMap.put(realised, tempSemanticRole);
		}
		summary.add(realised);
		//currentSentenceMap.remove(currentSentence);
		System.out.println(realised);
		Pair<String,SPhraseSpec> aPair = new Pair<String,SPhraseSpec>(realised, sentence);
		propSentences.push(aPair);
		return aPair;
		
	}
}
