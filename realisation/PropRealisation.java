/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * PropRealisation objects are responsible for the realization of each proposition (adjective phrase, prepositional phrase, verb phrase, noun phrase, etc).
 */
package edu.udel.eecis.teahouse.sight;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import simplenlg.features.Feature;
import simplenlg.features.Tense;
import simplenlg.framework.NLGElement;
import simplenlg.phrasespec.NPPhraseSpec;
import simplenlg.phrasespec.SPhraseSpec;

/**
 * @author Priscilla
 *
 */
public class PropRealisation {
	
	public PropRealisation(){
		
	}
	
	public enum PropositionType {SHOW, PRESENT, TYPE, VOLATILE, STEEP, VALUE, DATE, VALUE_CHANGE, DATE_CHANGE, MAX_MIN_VALUE, MAX_MIN_DATE};
	private PropositionType proposition;
	
	// Lexicons have a word and its "cost" based on difficulty
	private List<Pair<String,String>> predicateLexicon = new ArrayList<Pair<String,String>>();
	
//	public enum RealizationType {ADJ, RC, S};
//	private RealizationType realizationType;
	
	// rules will have an argument description (e.g. arg1) and a realization associated with it, which has costs
	// when creating new PropTemplates, specify the possible realizations an argument can assume
	//private Map<String, Realization> rules = new HashMap<String, Realization>();
	
	// key of first map is the realization type (S, ADJ, RC) and the value is another map that contains
	// argument description and realization
	private Map<String,Map<String,Realisation>> possibleRealizations;

	public List<Pair<String, String>> getPredicateLexicon() {
		return predicateLexicon;
	}

	public void setPredicateLexicon(List<Pair<String, String>> predicateLexicon) {
		this.predicateLexicon = predicateLexicon;
	}

	public Map<String, Map<String, Realisation>> getPossibleRealizations() {
		return possibleRealizations;
	}

	public void setPossibleRealizations(Map<String, Map<String, Realisation>> possibleRealizations) {
		this.possibleRealizations = possibleRealizations;
	}
	
	public PropositionType getProposition() {
		return proposition;
	}

	public void setProposition(PropositionType proposition) {
		this.proposition = proposition;
	}


	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition show
	 */
	public SPhraseSpec show(Proposition arg1, Proposition arg2, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.generateNP(arg2.getDescription(), true, false), "", "");
	    if (voice.equalsIgnoreCase("passive"))
	    	sentence.setFeature(Feature.PASSIVE, true);
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition show
	 */
	public SPhraseSpec showDetail(Proposition arg1, Proposition arg2, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
	    SPhraseSpec sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.generateNP(arg2.getDescription(), true, false), "", "");
    	sentence.addComplement(aSentence.generatePP("consisting of", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(0), true, false)));
    	sentence.addComplement(aSentence.generatePP("from", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(1), false, false)));
    	sentence.addComplement(aSentence.generatePP("to", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(2), false, false)));
    	sentence.addComplement(aSentence.generatePP("followed by", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(0), true, false)));
    	sentence.addComplement(aSentence.generatePP("through", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(2), false, false)));
    	if (voice.equalsIgnoreCase("passive"))
    		sentence.setFeature(Feature.PASSIVE, true);
    	return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec show(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		if (aggregationType.equalsIgnoreCase("conjunction")){
			return aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.generateNP(arg2.getDescription(), true, false), "", "present"), affectedRole);
		}
		else {
			return aSentence.appendRC(verbLexicon, aSentence.generateNP(arg2.getDescription(), true, false), arg1, affectedRole);
		}
	}
	
	//----------------------------- NEED TO CHECK SHOW DETAIL REGARDING VOICE AND USE OF PRONOUNS -------------------------------------------------------------------------//
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition show
	 */
	public SPhraseSpec showDetail(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = aSentence.generateSentence(verbLexicon, null, aSentence.generateNP(arg2.getDescription(), true, false), "", "");
	    sentence.addComplement(aSentence.generatePP("consisting of", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(0), true, false)));
	    sentence.addComplement(aSentence.generatePP("from", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(1), false, false)));
	    sentence.addComplement(aSentence.generatePP("to", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(2), false, false)));
	    sentence.addComplement(aSentence.generatePP("followed by", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(0), true, false)));
	    sentence.addComplement(aSentence.generatePP("through", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(2), false, false)));
//		if (voice.equalsIgnoreCase("passive"))
//			sentence.setFeature(Feature.PASSIVE, true);
	    if (aggregationType.equalsIgnoreCase("conjunction")){
		    return aSentence.appendCoordination(arg1, sentence, affectedRole);
		}
		else
		{
			NPPhraseSpec object = aSentence.generateNP(arg2.getDescription(), true, false);
			object.addComplement(aSentence.generatePP("consisting of", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(0), true, false)));
			object.addComplement(aSentence.generatePP("from", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(1), false, false)));
			object.addComplement(aSentence.generatePP("to", aSentence.generateNP(arg2.getAdditionalInfo().get(0).get(2), false, false)));
			object.addComplement(aSentence.generatePP("followed by", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(0), true, false)));
			object.addComplement(aSentence.generatePP("through", aSentence.generateNP(arg2.getAdditionalInfo().get(1).get(2), false, false)));
			return aSentence.appendRC(verbLexicon, object, arg1, affectedRole);
		}
		
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition show
	 */
	public SPhraseSpec consist(Proposition arg1, Proposition arg2, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.generateNP(arg2.getDescription(), true, false), "", "");
	    if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
	    return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec consist(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		if (aggregationType.equalsIgnoreCase("conjunction")){
			return aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.generateNP(arg2.getDescription(), true, false), "", "present"), affectedRole);
		}
		else {
			return aSentence.appendRC(verbLexicon, aSentence.generateNP(arg2.getDescription(), true, false), arg1, affectedRole);
		}
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments) and a string
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param arg3 - unit and scale of the MAD
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition present
	 */
	public SPhraseSpec present(Proposition arg1, Proposition arg2, String arg3, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.generateNP(arg2.getDescription(), false, false), "", "");
	    sentence.addComplement(aSentence.generatePP("in", aSentence.generateNP(null, null, arg3)));
	    if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
	    return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that presents a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param arg3 - unit and scale of the MAD
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec present(SPhraseSpec arg1, Proposition arg2, String arg3, String affectedRole, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		if (aggregationType.equalsIgnoreCase("conjunction")){
			sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.generateNP(arg2.getDescription(), false, false), "", "present"), affectedRole);
			sentence.addComplement(aSentence.generatePP("in", aSentence.generateNP(null, null, arg3)));
		} else {
			sentence = aSentence.appendRC(verbLexicon, aSentence.generateNP(arg2.getDescription(), false, false), arg1, affectedRole);
			sentence.addComplement(aSentence.generatePP("in", aSentence.generateNP(null, null, arg3)));
		}
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @return a complete sentence for the proposition type
	 */
	public SPhraseSpec type(Proposition arg1, Proposition arg2, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = aSentence.generateSentence("be", subject, aSentence.generateNP(arg2.getDescription(), false, false), "", "");
	    if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
	    return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec typeADJ(SPhraseSpec arg1, Proposition arg2, String affectedRole){
		GenerateSentence aSentence = new GenerateSentence();
	    return aSentence.appendADJ(arg2.getDetail(), null, arg1, affectedRole);
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec typeADJ(SPhraseSpec arg1, Proposition arg2, String affectedRole){
		GenerateSentence aSentence = new GenerateSentence();
		String lexicon = "line";
		NPPhraseSpec tempNP = null;
		String realised = null;
		if(affectedRole == "SUBJ"){
			//System.out.println("Trying to get subject "+arg1.getSubject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getSubject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			realised = aSentence.realiser.realiseSentence(arg1.getSubject());
			System.out.println("Subject in typeADJ: "+realised);
			arg1.setSubject(aSentence.appendAdj(lexicon, null, (NLGElement) arg1.getSubject()));
		}
		else
		{
			//System.out.println("Trying to get object "+arg1.getObject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getObject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			realised = aSentence.realiser.realiseSentence(arg1.getObject());
			tempNP = (NPPhraseSpec) arg1.getObject();
			//tempNP.addComplement((NLGElement) tempNP.getPostModifiers());
			System.out.println("Object in typeADJ: "+realised);
			realised = aSentence.realiser.realiseSentence(tempNP);
			System.out.println("tempNP Object in typeADJ: "+realised);
			arg1.setObject(aSentence.appendAdj(lexicon, null, (NLGElement) arg1.getObject()));
		}
			
		//aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
		return arg1;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec typeRC(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType){
		GenerateSentence aSentence = new GenerateSentence();
		if (aggregationType.equalsIgnoreCase("conjunction")){
			return aSentence.appendCoordination(arg1, aSentence.generateSentence("be", null, aSentence.generateNP(arg2.getDescription(), true, false), "", "present"), affectedRole);
		} else{
			return aSentence.appendRC("be", aSentence.generateNP(arg2.getDescription(), true, false), arg1, affectedRole);
		}
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param style - one of (noun | adj | conjunction) and it changes the construction of the sentence itself
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @param lexicon - will be passed because it can vary based on the reading level (can be only of type ADJ, e.g. volatile | jagged)
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey volatility. See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon (be, show)
	public SPhraseSpec volatility(Proposition arg1, Proposition arg2, String style, String verbLexicon, String lexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = null;
		String degree = "";
		//String description = "";
		if (style.equalsIgnoreCase("noun")){
			//description = "volatility";	//here can also be "fluctuation", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "much";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "little";
			}
			sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "");
		} else if (style.equalsIgnoreCase("adj")){
			//description = "volatile";	//here can also be "jagged", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "highly";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = null;
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "slightly";
			}
			if (degree != null){
				sentence = aSentence.generateSentence("be", subject, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "");
			} else {
				sentence = aSentence.generateSentence("be", subject, aSentence.generateNP(lexicon, false, false), "", "");
			}
		} else if (style.equalsIgnoreCase("conjunction")){
			//description = "ups and downs";	//here can also be "jagged", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "many";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "few";
			}
			sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "");
		}
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates an adjective appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param lexicon - will be passed because it can vary based on the reading level (can be only of type ADJ, e.g. volatile | jagged)
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec volatilityADJ(SPhraseSpec arg1, Proposition arg2, String affectedRole, String lexicon){
		GenerateSentence aSentence = new GenerateSentence();
		String degree = "";
		if (arg2.getDegree().equalsIgnoreCase("high")){
			degree = "highly";
		} else if (arg2.getDegree().equalsIgnoreCase("regular")){
			degree = null;
		} else if (arg2.getDegree().equalsIgnoreCase("low")){
			degree = "slightly";
		}
		//return aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
		if(affectedRole == "SUBJ"){
			System.out.println("Trying to get subject "+arg1.getSubject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getSubject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			arg1.setSubject(aSentence.appendAdj(lexicon, degree, (NLGElement) arg1.getSubject()));
		}
		else
		{
			System.out.println("Trying to get object "+arg1.getObject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getObject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			arg1.setObject(aSentence.appendAdj(lexicon, degree, (NLGElement) arg1.getObject()));
		}
			
		//aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
		return arg1;
	}
	
	public SPhraseSpec volatilityADJ(SPhraseSpec arg1, Proposition arg2, String affectedRole, String lexicon){
		GenerateSentence aSentence = new GenerateSentence();
		String degree = "";
		if (arg2.getDegree().equalsIgnoreCase("high")){
			degree = "highly";
		} else if (arg2.getDegree().equalsIgnoreCase("regular")){
			degree = null;
		} else if (arg2.getDegree().equalsIgnoreCase("low")){
			degree = "slightly";
		}
		//return aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
		if(affectedRole == "SUBJ"){
			System.out.println("Trying to get subject "+arg1.getSubject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getSubject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			arg1.setSubject(aSentence.appendADJ(lexicon, degree, arg1, "SUBJ"));
		}
		else
		{
			System.out.println("Trying to get subject "+arg1.getObject()+ " of sentence: "+arg1);
			//NLGElement aNounPhrase = arg1.getObject();
			//aSentence.appendAdj(lexicon, degree, aNounPhrase);
			arg1.setObject(aSentence.appendADJ(lexicon, degree, arg1, "OBJ"));
		}
			
		//aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
		return arg1;
	}
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param style - one of (noun | adj | conjunction) and it changes the construction of the sentence itself
	 * @param lexicon - will be passed because it can vary based on the reading level (can be of types NOUN, ADJ, CONJUNCTION e.g. volatile | jagged)
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec volatilityRC(SPhraseSpec arg1, Proposition arg2, String affectedRole, String style, String lexicon, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		String degree = "";
		//String description = "";
		if (style.equalsIgnoreCase("noun")){
			//description = "volatility";	//here can also be "fluctuation", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "much";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "little";
			}
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC(verbLexicon, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), arg1, affectedRole);
		} else if (style.equalsIgnoreCase("adj")){
			//description = "volatile";	//here can also be "jagged", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "highly";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = null;
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "slightly";
			}
			if (degree != null){
				if (aggregationType.equalsIgnoreCase("conjunction"))
					sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("be", null, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "present"), affectedRole);
				else
					sentence = aSentence.appendRC("be", aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), arg1, affectedRole);
			}
			else {
				if (aggregationType.equalsIgnoreCase("conjunction"))
					sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("be", null, aSentence.generateNP(lexicon, false, false), "", "present"), affectedRole);
				else
					sentence = aSentence.appendRC("be", aSentence.generateNP(lexicon, false, false), arg1, affectedRole);
			}
		} else if (style.equalsIgnoreCase("conjunction")){
			//description = "ups and downs";	//here can also be "peaks and valleys", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "many";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "few";
			}
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC(verbLexicon, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), arg1, affectedRole);
		}
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param style - one of (noun | adj) and it changes the construction of the sentence itself
	 * @param lexicon - will be passed because it can vary based on the reading level
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey volatility. See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon (be, show)
	public SPhraseSpec steep(Proposition arg1, Proposition arg2, String style, String lexicon, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence = null;
		String degree = "";
		//String description = "";
		if (style.equalsIgnoreCase("noun")){
			//description = "steepness";	//here can also be "sharpness", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "much";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "little";
			}
			sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "");
		} else if (style.equalsIgnoreCase("adj")){
			//description = "steep";	//here can also be "sharp", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "highly";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = null;
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "slightly";
			}
			if (degree != null){
				sentence = aSentence.generateSentence("be", subject, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "");
			}
			else {
				sentence = aSentence.generateSentence("be", subject, aSentence.generateNP(lexicon, false, false), "", "");
			}
		}
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
	    return sentence;
	}
	
	/**
	 * Generates an adjective appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows a ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param lexicon - will be passed because it can vary based on the reading level
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec steepADJ(SPhraseSpec arg1, Proposition arg2, String affectedRole, String lexicon){
		GenerateSentence aSentence = new GenerateSentence();
		String degree = "";
		//String description = "steep"; //here can also be "sharp", see where the control of the lexical choice will take place 
		if (arg2.getDegree().equalsIgnoreCase("high")){
			degree = "highly";
		} else if (arg2.getDegree().equalsIgnoreCase("regular")){
			degree = null;
		} else if (arg2.getDegree().equalsIgnoreCase("low")){
			degree = "slightly";
		}
	    return aSentence.appendADJ(lexicon, degree, arg1, affectedRole);
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param style - one of (noun | adj) and it changes the construction of the sentence itself
	 * @param lexicon - will be passed because it can vary based on the reading level
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec steepRC(SPhraseSpec arg1, Proposition arg2, String affectedRole, String style, String lexicon, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		String degree = "";
		//String description = "";
		if (style.equalsIgnoreCase("noun")){
			//description = "steepness";	//here can also be "sharpness", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "much";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = "some";
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "little";
			}
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC(verbLexicon, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), arg1, affectedRole);
		} else if (style.equalsIgnoreCase("adj")){
			//description = "steep";	//here can also be "sharp", see where the control of the lexical choice will take place
			if (arg2.getDegree().equalsIgnoreCase("high")){
				degree = "highly";
			} else if (arg2.getDegree().equalsIgnoreCase("regular")) {
				degree = null;
			} else if (arg2.getDegree().equalsIgnoreCase("low")) {
				degree = "slightly";
			}
			if (degree != null){
				if (aggregationType.equalsIgnoreCase("conjunction"))
					sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("be", null, aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), "", "present"), affectedRole);
				else
					sentence = aSentence.appendRC("be", aSentence.appendAdj(degree, null, aSentence.generateNP(lexicon, false, false)), arg1, affectedRole);
			} else {
				if (aggregationType.equalsIgnoreCase("conjunction"))
					sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("be", null, aSentence.generateNP(lexicon, false, false), "", "present"), affectedRole);
				else
					sentence = aSentence.appendRC("be", aSentence.generateNP(lexicon, false, false), arg1, affectedRole);
			}
		}
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param type - one of (start | end)
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec value(Proposition arg1, Proposition arg2, String type){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence;
		if (type.equalsIgnoreCase("start")){
			sentence = aSentence.generateSentence("have", aSentence.generateNP(arg1.getDescription(), true, true), aSentence.appendAdj("starting", null, aSentence.generateNP("value", true, false)), "", "");
		} else {
			sentence = aSentence.generateSentence("have", aSentence.generateNP(arg1.getDescription(), true, true), aSentence.appendAdj("ending", null, aSentence.generateNP("value", true, false)), "", "");
		}
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param lexicon - defines the type of the value (start or end) ans, at the same time, the level of complexity based on the lexical choice.
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec value(Proposition arg1, Proposition arg2, String lexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		sentence = aSentence.generateSentence("have", subject, aSentence.appendAdj(lexicon, null, aSentence.generateNP("value", true, false)), "", "");
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param type - type; one of (start | end)
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec value(SPhraseSpec arg1, Proposition arg2, String affectedRole, String type, String aggregationType){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		NPPhraseSpec nounPhrase;
		if (type.equalsIgnoreCase("start")){
			nounPhrase = aSentence.appendAdj("starting", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		} else {
			nounPhrase = aSentence.appendAdj("ending", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		}
	    return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param lexicon - defines the type of the value (start or end) ans, at the same time, the level of complexity based on the lexical choice.
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec value(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String lexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		NLGElement nounPhrase;
		nounPhrase = aSentence.appendAdj(lexicon, null, aSentence.generateNP("value", true, false));
		nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (aggregationType.equalsIgnoreCase("conjunction"))
			sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
		else
			sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param type - one of (start | end)
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec date(Proposition arg1, Proposition arg2, String type, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		if (type.equalsIgnoreCase("start")){
			sentence = aSentence.generateSentence("have", subject, aSentence.appendAdj("starting", null, aSentence.generateNP("date", true, false)), "", "");
		} else {
			sentence = aSentence.generateSentence("have", subject, aSentence.appendAdj("ending", null, aSentence.generateNP("date", true, false)), "", "");
		}
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param type - one of (start | end)
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec date(SPhraseSpec arg1, Proposition arg2, String affectedRole, String type, String aggregationType){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		NLGElement nounPhrase;
		if (type.equalsIgnoreCase("start")){
			nounPhrase = aSentence.appendAdj("starting", null, aSentence.generateNP("date", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		} else {
			nounPhrase = aSentence.appendAdj("ending", null, aSentence.generateNP("date", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		}
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param type - one of (increase | decrease)
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec value_change(Proposition arg1, Proposition arg2, String type, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		if (type.equalsIgnoreCase("increase")){
			sentence = aSentence.generateSentence("increase", subject, null, "", "past");
		} else {
			sentence = aSentence.generateSentence("decrease", subject, null, "", "past");
		}
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("by", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param type - one of (increase | decrease)
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec value_change(SPhraseSpec arg1, Proposition arg2, String affectedRole, String type){
		GenerateSentenceImpl aSentence = new GenerateSentenceImpl();
		SPhraseSpec sentence;
		if (type.equalsIgnoreCase("increase")){
			sentence = aSentence.appendRC("increase", null, arg1, affectedRole);
			sentence = aSentence.appendPP(sentence, aSentence.generatePP("by", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			sentence.setFeature(Feature.TENSE, Tense.PAST);
		} else {
			sentence = aSentence.appendRC("decrease", null, arg1, affectedRole);
			sentence = aSentence.appendPP(sentence, aSentence.generatePP("by", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			sentence.setFeature(Feature.TENSE, Tense.PAST);
		}
	    return sentence;
	}
	public SPhraseSpec value_change(SPhraseSpec arg1, Proposition arg2, String affectedRole, String type, String aggregationType){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		NLGElement nounPhrase;
		if (type.equalsIgnoreCase("increase")){
			nounPhrase = aSentence.appendAdj("increasing", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("show", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("show", nounPhrase, arg1, affectedRole);
		} else {
			nounPhrase = aSentence.appendAdj("decreasing", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("show", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("show", nounPhrase, arg1, affectedRole);
		}
	    return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec date_change(Proposition arg1, Proposition arg2, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence;
		sentence = aSentence.generateSentence(verbLexicon, aSentence.generateNP(arg1.getDescription(), true, true), null, "", "");
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("over", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec date_change(Proposition arg1, Proposition arg2, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false)), "", "");
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec date_change(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregateType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		if (aggregateType.equalsIgnoreCase("conjunction"))
			return aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false)), "", "present"), affectedRole);
		else
			return aSentence.appendRC(verbLexicon, aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false)), arg1, affectedRole);
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param type - one of (maximum | minimum)
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec max_min_value(Proposition arg1, Proposition arg2, String type){
		GenerateSentence aSentence = new GenerateSentence();
    	SPhraseSpec sentence;
		if (type.equalsIgnoreCase("maximum")){
			sentence = aSentence.generateSentence("have", aSentence.generateNP(arg1.getDescription(), true, true), aSentence.appendAdj("maximum", null, aSentence.generateNP("value", true, false)), "", "");
		} else {
			sentence = aSentence.generateSentence("have", aSentence.generateNP(arg1.getDescription(), true, true), aSentence.appendAdj("minimum", null, aSentence.generateNP("value", true, false)), "", "");
		}
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		return sentence;
	}
	
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param lexicon - defined by the type and the reading level
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec max_min_value(Proposition arg1, Proposition arg2, String lexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		NPPhraseSpec nounPhrase = aSentence.generateNP("value", true, true);
		nounPhrase.addPreModifier(lexicon);
		//sentence = aSentence.generateSentence("have", aSentence.generateNP(arg1.getDescription(), true, true), aSentence.appendAdj(lexicon, null, aSentence.generateNP("value", true, true)), "", "");
		sentence = aSentence.generateSentence("have", subject, nounPhrase, "", "");
		sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param type - type; one of (maximum | minimum)
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec max_min_value(SPhraseSpec arg1, Proposition arg2, String affectedRole, String type, String aggregationType){
		GenerateSentence aSentence = new GenerateSentence();
		SPhraseSpec sentence = null;
		NPPhraseSpec nounPhrase;
		if (type.equalsIgnoreCase("maximum")){
			nounPhrase = aSentence.appendAdj("maximum", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		} else {
			nounPhrase = aSentence.appendAdj("minimum", null, aSentence.generateNP("value", true, false));
			nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
			if (aggregationType.equalsIgnoreCase("conjunction"))
				sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
			else
				sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
		}
	    return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param lexicon - defined by the type and the reading level
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	// same here, for this method I can have the root predicate "have" or the root predicate ("start" | "end")
	public SPhraseSpec max_min_value(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String lexicon){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec nounPhrase = aSentence.generateNP("value", true, true);
		nounPhrase.addPreModifier(lexicon);
		SPhraseSpec sentence = null;
		//NLGElement nounPhrase;
		//nounPhrase = aSentence.appendAdj(lexicon, null, aSentence.generateNP("value", true, false));
		nounPhrase = aSentence.appendPP(nounPhrase, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (aggregationType.equalsIgnoreCase("conjunction"))
			sentence = aSentence.appendCoordination(arg1, aSentence.generateSentence("have", null, nounPhrase, "", "present"), affectedRole);
		else
			sentence = aSentence.appendRC("have", nounPhrase, arg1, affectedRole);
	    return sentence;
	}
	/**
	 * Generates a sentence when called with two propositions (or arguments)
	 * 
	 * @param arg1 - subject proposition
	 * @param arg2 - object proposition
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence for the proposition type
	 */
	// This method will need to be changed since there are other ways to convey value (based on the root predicate). See where the rules go
	// maybe I want to receive a code for a rule that will get the right parts of a proposition and also receive the predicate lexicon
	public SPhraseSpec max_min_date(Proposition arg1, Proposition arg2, String verbLexicon, String voice, String referringExpression){
		GenerateSentence aSentence = new GenerateSentence();
		NPPhraseSpec subject = aSentence.generateNP(arg1.getDescription(), true, true);
		if (referringExpression.equalsIgnoreCase("pronoun"))
			subject.setFeature(Feature.PRONOMINAL, true);
		SPhraseSpec sentence;
		sentence = aSentence.generateSentence(verbLexicon, subject, aSentence.generatePPasNP("on", aSentence.generateNP(arg2.getValue(), false, false)), "", "");
		//sentence = aSentence.appendPP(sentence, aSentence.generatePP("of", aSentence.appendAdj(arg2.getValue(), null, aSentence.generateNP(arg2.getValueDetail(), false, false))));
		if (voice.equalsIgnoreCase("passive"))
			sentence.setFeature(Feature.PASSIVE, true);
		return sentence;
	}
	
	/**
	 * Generates a relative clause appended to the noun phrase when called with a NP and a proposition
	 * 
	 * @param arg1 - sentence to which the relative clause "that shows ..." should be attached to
	 * @param arg2 - proposition that will be attached to the sentence - the object of the relative clause
	 * @param affectedRole - to which part of the sentence (subject or object) the relative clause should be attached to
	 * @param aggregationType - for sentences that can be aggregated as a relative clause, in case there exists already one,
	 * 	they will be added as a coordination on the existing relative clause
	 * @param verbLexicon - which verb should be used according to readability constraints
	 * @return a complete sentence with a relative clause attached to the affectedRole (subject or object)
	 */
	public SPhraseSpec max_min_date(SPhraseSpec arg1, Proposition arg2, String affectedRole, String aggregationType, String verbLexicon){
		GenerateSentence aSentence = new GenerateSentence();
		if (aggregationType.equalsIgnoreCase("conjunction"))
			return aSentence.appendCoordination(arg1, aSentence.generateSentence(verbLexicon, null, aSentence.generatePPasNP("on", aSentence.generateNP(arg2.getValue(), false, false)), "", "present"), affectedRole);
		else
			return aSentence.appendRC(verbLexicon, aSentence.generatePPasNP("on", aSentence.generateNP(arg2.getValue(), false, false)), arg1, affectedRole);
	}
}
