/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * Generate sentence organizes propositions and performs the surface realisation.
 */
package edu.udel.eecis.teahouse.sight;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import simplenlg.framework.CoordinatedPhraseElement;
import simplenlg.framework.NLGElement;
import simplenlg.framework.NLGFactory;
import simplenlg.framework.PhraseElement;
import simplenlg.framework.StringElement;
import simplenlg.lexicon.Lexicon;
import simplenlg.realiser.english.Realiser;
import simplenlg.phrasespec.*;
import simplenlg.features.*;

/**
 * This class uses simpleNLG API for creating sentences of various types.
 * 
 * @author Priscilla
 *
 */
public class GenerateSentence {

	Lexicon lexicon = Lexicon.getDefaultLexicon();
    NLGFactory nlgFactory = new NLGFactory(lexicon);
    Realiser realiser = new Realiser(lexicon);
	
    public GenerateSentence(){ 	
    };
    
	/**
	 * Takes a list of modifiers (adjectives) and the head of the NP and 
	 * returns a NP (noun phrase).
	 * 
	 * @param preModifiers - list of pre modifiers - it can be empty
	 * 		  headNP - the head of the noun phrase
	 * @return a noun phrase
	 */
    public NPPhraseSpec generateNP(String determiner, List<String> preModifiers, String headNP){
    	NPPhraseSpec nounPhrase = nlgFactory.createNounPhrase();
    	if (determiner != null)
    	{	// set the determiner of the noun phrase
    		nounPhrase.setDeterminer(determiner);
    	// set the head of the noun phrase
    	}
    	nounPhrase.setHead(headNP);

    	if (preModifiers != null){
    		for (String word : preModifiers){
    			// adding adjectives to the front of the head NP
    			nounPhrase.addPreModifier(word);;
    		}
    	}

    	return nounPhrase;
    }
    
	/**
	 * Takes the head of the NP and booleans for modifiers and 
	 * returns a NP (noun phrase).
	 * 
	 * @param headNP - the head of the noun phrase
	 * 		  determiner - if a determiner is needed
	 * 		  mentioned - if the head being realised has been already mentioned
	 * @return a noun phrase
	 */
    public NPPhraseSpec generateNP(String headNP, boolean determiner, boolean mentioned){
    	NPPhraseSpec nounPhrase = nlgFactory.createNounPhrase();
    	if (determiner){
	    	if (!mentioned)
	    		// set the determiner to a or an
	    		nounPhrase.setDeterminer("a");
	    	else  nounPhrase.setDeterminer("the");
    	}
	    nounPhrase.setHead(headNP);
    	return nounPhrase;
    }
    
	/**
	 * Takes a preposition and a noun phrase and  
	 * returns a PP (prepositional phrase).
	 * 
	 * @param preposition
	 * 		  complement - the noun phrase of the PP
	 * @return a prepositional phrase (e.g. "to the pan")
	 */    
    public PPPhraseSpec generatePP(String preposition, NPPhraseSpec complement){
    	PPPhraseSpec ppPhrase = nlgFactory.createPrepositionPhrase(preposition, complement);
    	return ppPhrase;
    }
    
	/**
	 * Takes a preposition and a noun phrase and  
	 * returns a PP (prepositional phrase).
	 * 
	 * @param preposition
	 * 		  complement - the noun phrase of the PP
	 * @return a prepositional phrase (e.g. "to the pan")
	 */    
    public PPPhraseSpec generatePP(String preposition, NLGElement complement){
    	PPPhraseSpec ppPhrase = nlgFactory.createPrepositionPhrase(preposition, complement);
    	return ppPhrase;
    }
    
	/**
	 * Takes a preposition and a noun phrase and  
	 * returns a PP (prepositional phrase).
	 * 
	 * @param preposition
	 * 		  complement - the noun phrase of the PP
	 * @return a prepositional phrase (e.g. "to the pan")
	 */    
    public NPPhraseSpec generatePPasNP(String preposition, NPPhraseSpec nounPhrase){
    	nounPhrase.setPreModifier(preposition);
    	return nounPhrase;
    }
    
	/**
	 * Takes a sentence and a pp (prepositional phrase)
	 * and returns the same sentence with the pp appended to it.
	 * 
	 * @param aSentence - the original sentence
	 * 		  pp - the pp to be appended to the sentence
	 * @return the new sentence with the pp appended (e.g. "boil water in a large pan")
	 */   
    public SPhraseSpec appendPP(SPhraseSpec aSentence, PPPhraseSpec pp){
    	NLGElement sentenceObject = aSentence.getObject();
    	if (sentenceObject != null)
    	{
    		((PhraseElement) sentenceObject).setComplement(pp);
    		aSentence.setObject(sentenceObject);
    	}
    	else {
//    		sentenceObject = aSentence.getSubject();
//    		((PhraseElement) sentenceObject).setComplement(pp);
//    		aSentence.setSubject(sentenceObject);
    		aSentence.setComplement(pp);
    	}
    	
    	return aSentence;
    	
    }
    
	/**
	 * Takes a sentence and a pp (prepositional phrase)
	 * and returns the same sentence with the pp appended to it.
	 * 
	 * @param aSentence - the original sentence
	 * 		  pp - the pp to be appended to the sentence
	 * @return the new sentence with the pp appended (e.g. "boil water in a large pan")
	 */   
    public NPPhraseSpec appendPP(NPPhraseSpec nounPhrase, PPPhraseSpec pp){
    	nounPhrase.setPostModifier(pp);
    	return nounPhrase;
    	
    }
    
	/**
	 * Takes a sentence and a pp (prepositional phrase)
	 * and returns the same sentence with the pp appended to it.
	 * 
	 * @param aSentence - the original sentence
	 * 		  pp - the pp to be appended to the sentence
	 * @return the new sentence with the pp appended (e.g. "boil water in a large pan")
	 */   
    public NPPhraseSpec appendPP(NLGElement nounPhrase, PPPhraseSpec pp){
    	((PhraseElement) nounPhrase).setPostModifier(pp);
    	return (NPPhraseSpec) nounPhrase;
    	
    }
    
	/**
	 * Takes an adjective, an adverb and a noun phrase and  
	 * returns a noun phrase with the adjective and (optional) adverb attached to it.
	 * 
	 * @param adjective - an adjective
	 * 		  adverb - an adverb
	 * 		  noun phrase - the noun phrase
	 * @return a noun phrase
	 */    
    public NPPhraseSpec appendAdj(String adjective, String adverb, NPPhraseSpec nounPhrase){
    	System.out.println("Receiving adverb and adjective: "+adverb+" "+adjective);
//    	nounPhrase.addFrontModifier(adjective);
//    	nounPhrase.addFrontModifier(adverb);
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	nounPhrase.setPreModifier(adjPhrase);
    	System.out.println("Returning: "+adjPhrase.getRealisation());
    	return nounPhrase;
    }
    public NPPhraseSpec appendAdj(String adjective, String adverb, NLGElement nounPhrase){
    	System.out.println("Receiving adverb and adjective: "+adverb+" "+adjective);
//    	nounPhrase.addFrontModifier(adjective);
//    	nounPhrase.addFrontModifier(adverb);
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	((PhraseElement) nounPhrase).setPreModifier(adjPhrase);
    	System.out.println("Returning: "+adjPhrase);
    	return (NPPhraseSpec) nounPhrase;
    }
    public NLGElement appendAdj(String adjective, String adverb, NLGElement nounPhrase){
    	//System.out.println("Receiving adverb and adjective: "+adverb+" "+adjective);
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	if (nounPhrase.getClass().getName().equals("simplenlg.phrasespec.NPPhraseSpec")){
    		if (adverb != null){
    			((NPPhraseSpec) nounPhrase).addPreModifier(adverb);
    		}
    		System.out.println("Came on the NPPhrase NLGElement for adjective");
    		((NPPhraseSpec) nounPhrase).addPreModifier(adjPhrase);
    		return nounPhrase;
    	} else{
    		if (adverb != null){
    			((CoordinatedPhraseElement) nounPhrase).addPreModifier(adverb);
    		}
    		((CoordinatedPhraseElement) nounPhrase).addPreModifier(adjPhrase);
    		System.out.println("Came on the else for adjective");
    		return nounPhrase;
    	}
    }
    
	/**
	 * Takes an adjective, an adverb and a sentence and  
	 * returns a sentence with the adjective and (optional) adverb attached to the subject of the sentence.
	 * 
	 * @param adjective
	 * 		  adverb
	 * 		  sentence
	 * @return a sentence
	 */    
    public SPhraseSpec appendAdjToSubject(String adjective, String adverb, SPhraseSpec sentence){
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	NLGElement subject = sentence.getSubject();
    	((PhraseElement) subject).setPreModifier(adjPhrase);
    	sentence.setSubject(subject);
    	return sentence;
    }
   
	/**
	 * Takes an adjective, an adverb and a sentence and  
	 * returns a sentence with the adjective and (optional) adverb attached to the object of the sentence.
	 * 
	 * @param adjective
	 * 		  adverb
	 * 		  sentence
	 * @return a sentence
	 */    
    public SPhraseSpec appendAdjToObject(String adjective, String adverb, SPhraseSpec sentence){
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	NLGElement object = sentence.getObject();
    	((PhraseElement) object).setPreModifier(adjPhrase);
    	sentence.setObject(object);
    	return sentence;
    }

	/**
	 * Takes an adjective, an adverb and a sentence and  
	 * returns a sentence with the adjective and (optional) adverb attached to the object of the sentence.
	 * 
	 * @param adjective
	 * 		  adverb
	 * 		  sentence
	 * 		  affectedRole - the semantic role to which the adjective should be attached to.
	 * @return a sentence
	 */  
    public SPhraseSpec appendADJ(String adjective, String adverb, SPhraseSpec sentence, String affectedRole){
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	if (adverb != null){
    		adjPhrase.setPreModifier(adverb);
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
//	    	NLGElement subject = sentence.getSubject();
//	    	((PhraseElement) subject).setPreModifier(adjPhrase);
//	    	sentence.setSubject(subject);
    		NPPhraseSpec subject = (NPPhraseSpec) sentence.getSubject();
	    	((PhraseElement) subject).setPreModifier(adjPhrase);
	    	sentence.setSubject(subject);   
    	} else {
	    	NLGElement object = sentence.getObject();
    		((PhraseElement) object).setPreModifier(adjPhrase);
	    	sentence.setObject(object);
    	}
    	return sentence;
    }
    public SPhraseSpec appendADJ(String adjective, String adverb, SPhraseSpec sentence, String affectedRole){
    	AdjPhraseSpec adjPhrase = nlgFactory.createAdjectivePhrase();
    	adjPhrase.setAdjective(adjective);
    	List<NLGElement> preModifiers = new ArrayList<NLGElement>();
//    	if (adverb != null){
//    		adjPhrase.setPreModifier(adverb);
//    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
	    	NLGElement subject = sentence.getSubject();
	    	preModifiers = ((PhraseElement) subject).getPreModifiers();
	    	//preModifiers.add(adjPhrase);
	    	System.out.println("FrontModifiers: "+preModifiers);
	    	if (adverb != null){
        		((PhraseElement) subject).addPreModifier(adverb);
        	}
	    	((PhraseElement) subject).addPreModifier(adjective);
	    	if (!(preModifiers.isEmpty())){
	    		for (NLGElement aModifier : preModifiers){
	    			((PhraseElement) subject).addPreModifier(aModifier);
	    		}
	    	}else{
	    		((PhraseElement) subject).setPreModifier(adjPhrase);
	    	}
	    	//((PhraseElement) subject).setFrontModifier(preModifiers);
	    	sentence.setSubject(subject);    		
    	} else {
	    	NLGElement object = sentence.getObject();
	    	preModifiers = ((PhraseElement) object).getPreModifiers();
	    	System.out.println("FrontModifiers: "+preModifiers);
	    	if (adverb != null){
        		((PhraseElement) object).addPreModifier(adverb);
        	}
	    	((PhraseElement) object).addPreModifier(adjective);
	    	//preModifiers.addAll((Collection<? extends NLGElement>) adjPhrase);
	    	if (!(preModifiers.isEmpty())){
	    		for (NLGElement aModifier : preModifiers){
	    			((PhraseElement) object).addPreModifier(aModifier);
	    		}
	    	}else{
	    		((PhraseElement) object).setPreModifier(adjPhrase);
	    	}
	    	sentence.setObject(object);
    	}
    	return sentence;
    }
    /**
	 * Takes the headVP for the relative clause and the object of the VP.
     * @param headVP
     * @param object
     * @return 
     */
    public SPhraseSpec generateRC(String headVP, NPPhraseSpec object){
    	SPhraseSpec aClause = nlgFactory.createClause();
    	aClause.setVerb(headVP);
    	aClause.setObject(object);
    	aClause.setFeature(Feature.COMPLEMENTISER, "which");
    	return aClause;
    	
    }
 
    /**
	 * Takes the headVP for the relative clause, the object of the VP and the sentence to which the RC should be attached.
     * @param headVP
     * @param object
     * @param sentence
     * @param affectedRole - subject or object (the one that should be modified by the relative clause)
     * @return sentence with relative clause attached to the object
     */
    public SPhraseSpec appendRC(String headVP, NPPhraseSpec object, SPhraseSpec sentence, String affectedRole){
    	SPhraseSpec aClause = nlgFactory.createClause();
    	aClause.setVerb(headVP);
    	if (object != null)
    	{
    		aClause.setObject(object);
    		aClause.setFeature(Feature.COMPLEMENTISER, "which");
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		NLGElement sentenceSubject = sentence.getSubject();
	    	((PhraseElement) sentenceSubject).setComplement(aClause);
	    	sentence.setSubject((NLGElement) sentenceSubject);
    	}
    	else {
	    	NLGElement sentenceObject = sentence.getObject();
	    	((PhraseElement) sentenceObject).setComplement(aClause);
	    	sentence.setObject((NLGElement) sentenceObject);
    	}
    	return sentence;
    	
    }
    
    public SPhraseSpec appendRC(String headVP, NLGElement object, SPhraseSpec sentence, String affectedRole){
    	SPhraseSpec aClause = nlgFactory.createClause();
    	aClause.setVerb(headVP);
    	if (object != null)
    	{
    		aClause.setObject(object);
    		aClause.setFeature(Feature.COMPLEMENTISER, "which");
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		NLGElement sentenceSubject = sentence.getSubject();
	    	((PhraseElement) sentenceSubject).setComplement(aClause);
	    	sentence.setSubject((NLGElement) sentenceSubject);
    	}
    	else {
	    	NLGElement sentenceObject = sentence.getObject();
	    	((PhraseElement) sentenceObject).setComplement(aClause);
	    	sentence.setObject((NLGElement) sentenceObject);
    	}
    	return sentence;
    	
    }
    
    /**
	 * Takes the headVP for the relative clause, the object of the VP and the sentence to which the RC should be attached.
     * @param headVP
     * @param object
     * @param sentence
     * @return sentence with relative clause attached to the object
     */
    public SPhraseSpec appendRCToSubject(String headVP, NPPhraseSpec object, SPhraseSpec sentence){
    	SPhraseSpec aClause = nlgFactory.createClause();
    	aClause.setVerb(headVP);
    	aClause.setObject(object);
    	NLGElement sentenceSubject = sentence.getSubject();
    	((PhraseElement) sentenceSubject).setComplement(aClause);
    	sentence.setSubject(sentenceSubject);
    	return sentence;
    	
    }
    
	/**
	 * Takes a noun phrase and a relative clause
	 * and returns the same sentence with relative clause appended to it.
	 * 
	 * @param aSentence - the original sentence
	 * 		  pp - the pp to be appended to the sentence
	 * @return the new sentence with the pp appended (e.g. "boil water in a large pan")
	 */   
    public NPPhraseSpec appendRC(NPPhraseSpec aNounPhrase, SPhraseSpec relativeClause){
    	aNounPhrase.addComplement(relativeClause);
    	return aNounPhrase;
    	
    }
    
	/**
	 * Takes a noun phrase and a relative clause
	 * and returns the same sentence with relative clause appended to it.
	 * 
	 * @param aSentence - the original sentence to which the coordination will be added
	 * 		  newElement - the element that contains the additional coordination (it is being tested for NPPhraseSpec and SPhraseSpec)
	 * @return the new sentence with the coordination added to the object
	 */   
    public SPhraseSpec appendCoordination(SPhraseSpec aSentence, NLGElement newElement, String affectedRole){
    	CoordinatedPhraseElement newCoordination = new CoordinatedPhraseElement();
    	List<NLGElement> modifiers = new ArrayList<NLGElement>();
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		NLGElement sentenceSubject = aSentence.getSubject();
    		if (sentenceSubject.getClass().getName().equals("simplenlg.framework.CoordinatedPhraseElement")){
    			modifiers = ((CoordinatedPhraseElement) sentenceSubject).getPostModifiers();
    		}else{
    			modifiers = ((NPPhraseSpec) sentenceSubject).getPostModifiers();
    		}
	    	if (!(modifiers.isEmpty())){
	    		newCoordination.addCoordinate(modifiers);
	    	} else {
	    		newCoordination.addCoordinate(aSentence.getSubject());
	    	}
    	} else {
    		newCoordination.addCoordinate(aSentence.getObject());
    	}
    	if (newElement.getClass().getName().equals("simplenlg.phrasespec.SPhraseSpec")){
    		newCoordination.addCoordinate(((SPhraseSpec) newElement).getVerbPhrase());
    	} else if (newElement.getClass().getName().equals("simplenlg.phrasespec.NPPhraseSpec")){
    		newCoordination.addCoordinate(newElement);
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		aSentence.setSubject(newCoordination);
    	}
    	else {
    		aSentence.setObject(newCoordination);
    	}
    	aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	return aSentence;
    	
    }
    
	/**
	 * Takes a noun phrase and a relative clause
	 * and returns the same sentence with relative clause appended to it.
	 * 
	 * @param aSentence - the original sentence to which the coordination will be added
	 * 		  newElement - the element that contains the additional coordination (it is being tested for NPPhraseSpec and SPhraseSpec)
	 * @return the new sentence with the coordination added to the object
	 */   
    public SPhraseSpec appendCoordination(SPhraseSpec aSentence, NLGElement newElement, String affectedRole){
    	CoordinatedPhraseElement newCoordination = new CoordinatedPhraseElement();
    	List<NLGElement> modifiers = new ArrayList<NLGElement>();
    	NLGElement head = null;
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		NLGElement sentenceSubject = aSentence.getSubject();
    		head = ((PhraseElement) sentenceSubject).getHead();
	    	modifiers = ((NPPhraseSpec) sentenceSubject).getPostModifiers();
	    	if (!(modifiers.isEmpty())){
	    		newCoordination.addCoordinate(modifiers);
	    	//} else {
	    	//	newCoordination.addCoordinate(aSentence.getSubject());
	    	}
    	} else {
    		NLGElement sentenceObject = aSentence.getObject();
    		head = ((PhraseElement) sentenceObject).getHead();
	    	modifiers = ((NPPhraseSpec) sentenceObject).getPostModifiers();
	    	if (!(modifiers.isEmpty())){
	    		newCoordination.addCoordinate(modifiers);
	    	//} else {
	    	//	newCoordination.addCoordinate(aSentence.getObject());
	    	}
    		//newCoordination.addCoordinate(aSentence.getObject());
    	}
    	if (newElement.getClass().getName().equals("simplenlg.phrasespec.SPhraseSpec")){
    		newCoordination.addCoordinate(((SPhraseSpec) newElement).getVerbPhrase());
    	} else if (newElement.getClass().getName().equals("simplenlg.phrasespec.NPPhraseSpec")){
    		newCoordination.addCoordinate(newElement);
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		((NPPhraseSpec) head).addPostModifier(newCoordination);
    		aSentence.setSubject(head);
    		//aSentence.setSubject(newCoordination);
    	}
    	else {
    		((StringElement) head).s.addPostModifier(newCoordination);
    		aSentence.setObject(head);
    		//aSentence.setObject(newCoordination);
    	}
    	//aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	return aSentence;
    	
    }
    
	/**
	 * Takes a noun phrase and a relative clause
	 * and returns the same sentence with relative clause appended to it.
	 * 
	 * @param aSentence - the original sentence to which the coordination will be added
	 * 		  newElement - the element that contains the additional coordination (it is being tested for NPPhraseSpec and SPhraseSpec)
	 * @return the new sentence with the coordination added to the object
	 */   
    public SPhraseSpec appendCoordination(SPhraseSpec aSentence, NLGElement newElement, String affectedRole){
    	CoordinatedPhraseElement newCoordination = new CoordinatedPhraseElement();
    	newCoordination.setFeature(Feature.CONJUNCTION, "and");
    	List<NLGElement> modifiers = new ArrayList<NLGElement>();
    	NLGElement sentenceSubject = null;
    	NLGElement sentenceObject = null;
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		sentenceSubject = aSentence.getSubject();
	    	modifiers = ((NPPhraseSpec) sentenceSubject).getPostModifiers();
	    	if (!(modifiers.isEmpty()))//{
	    		newCoordination.addCoordinate(modifiers);
//	    	} else {
//	    		newCoordination.addCoordinate(aSentence.getSubject());
//	    	}	
    	} else {
    		sentenceObject = aSentence.getObject();
	    	modifiers = ((NPPhraseSpec) sentenceObject).getPostModifiers();
	    	if (!(modifiers.isEmpty()))//{
	    		newCoordination.addCoordinate(modifiers);
    		//newCoordination.addCoordinate(aSentence.getObject());
    	}
    	if (newElement.getClass().getName().equals("simplenlg.phrasespec.SPhraseSpec")){
    		newCoordination.addCoordinate(((SPhraseSpec) newElement).getVerbPhrase());
    		
    	} else if (newElement.getClass().getName().equals("simplenlg.phrasespec.NPPhraseSpec")){
    		newCoordination.addCoordinate(newElement);
    		//newCoordination.setFeature(Feature.CONJUNCTION, "and");
    	}
    	if (affectedRole.equalsIgnoreCase("SUBJ")){
    		((PhraseElement) sentenceSubject).setPostModifier(newCoordination);
    		aSentence.setSubject(sentenceSubject);
    	}
    	else {
    		((PhraseElement) sentenceObject).setPostModifier(newCoordination);
    		aSentence.setObject(sentenceObject);
    	}
    	return aSentence;
    	
    }
    
	/**
	 * Takes a verb, an NP in the role of subject and a NP in the role of object  
	 * returns a sentence.
	 * 
	 * @param headVP - the verb of the phrase
	 * 		  subject - the noun phrase playing the subject of the sentence
	 * 		  object - the noun phrase playing the object of the sentence
	 * 		  type - option is "interrogative"
	 * 		  tense - one of (present | past)
	 * @return aSentence (e.g. "boil water")
	 */   
    public SPhraseSpec generateSentence(String headVP, NPPhraseSpec subject, NPPhraseSpec object, String type, String tense){
    	SPhraseSpec aSentence = nlgFactory.createClause();
    	aSentence.setVerb(headVP);
    	aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	if (type == "interrogative")
    	{
    		aSentence.setFeature(Feature.INTERROGATIVE_TYPE, InterrogativeType.YES_NO);
    	}
    	if (tense.equalsIgnoreCase("past")){
    		aSentence.setFeature(Feature.TENSE, Tense.PAST);
    	}
    	if (subject == null)
    		aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	else{
    		aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    		aSentence.setSubject(subject);
    	}
    	if (object != null)
    		aSentence.setObject(object);
    	aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	//aSentence.setFeature(Feature.PASSIVE, true);
    	return aSentence;
    }
    
	/**
	 * Takes a verb, an NP in the role of subject and a NP in the role of object  
	 * returns a sentence.
	 * 
	 * @param headVP - the verb of the phrase
	 * 		  subject - the noun phrase playing the subject of the sentence
	 * 		  object - the noun phrase playing the object of the sentence
	 * 		  type - option is "interrogative"
	 * 		  tense - one of (present | past)
	 * @return aSentence (e.g. "boil water")
	 */   
    public SPhraseSpec generateSentence(String headVP, NPPhraseSpec subject, NLGElement object, String type, String tense){
    	SPhraseSpec aSentence = nlgFactory.createClause();
    	aSentence.setVerb(headVP);
    	aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	if (type == "interrogative")
    	{
    		aSentence.setFeature(Feature.INTERROGATIVE_TYPE, InterrogativeType.YES_NO);
    	}
    	if (tense.equalsIgnoreCase("past")){
    		aSentence.setFeature(Feature.TENSE, Tense.PAST);
    	}
    	if (subject == null)
    		aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	else{
    		aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    		aSentence.setSubject(subject);
    	}
    	if (object != null)
    		aSentence.setObject(object);
    	aSentence.setFeature(Feature.NUMBER, NumberAgreement.SINGULAR);
    	//aSentence.setFeature(Feature.PASSIVE, true);
    	return aSentence;
    }

}


