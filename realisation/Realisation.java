/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * The realisation object keeps track of the features which define the reading level or reading difficulty associated with a proposition.
 */
package edu.udel.eecis.teahouse.sight;

import java.util.Map;


/**
 * @author Priscilla
 *
 */
public class Realisation {

	private String realisationType;
	private int sentences = 0;
	private int words = 0;
	private int avgWordLenght = 0;
	private int pronouns = 0;
	private int begPronouns = 0;
	private int articles = 0;
	private int begArticles = 0;
	private int relativeClause = 0;
	private int adjective = 0;
	private int adverb = 0;
	private int passive = 0;
	private int prepositions = 0;
	private int conjunctions = 0;
	private int avgNPLength = 0;
	private int avgVPLength = 0;
	
	/**
	 * @return the realizationType
	 */
	public String getRealisationType() {
		return realisationType;
	}

	/**
	 * @param realizationType the realizationType to set
	 */
	public void setRealisationType(String realisationType) {
		this.realisationType = realisationType;
	}

	/**
	 * @return the sentences
	 */
	public int getSentences() {
		return sentences;
	}

	/**
	 * @param sentences the sentences to set
	 */
	public void setSentences(int sentences) {
		this.sentences = sentences;
	}

	/**
	 * @return the words
	 */
	public int getWords() {
		return words;
	}

	/**
	 * @param words the words to set
	 */
	public void setWords(int words) {
		this.words = words;
	}

	/**
	 * @return the avgWordLenght
	 */
	public int getAvgWordLenght() {
		return avgWordLenght;
	}

	/**
	 * @param avgWordLenght the avgWordLenght to set
	 */
	public void setAvgWordLenght(int avgWordLenght) {
		this.avgWordLenght = avgWordLenght;
	}

	/**
	 * @return the pronouns
	 */
	public int getPronouns() {
		return pronouns;
	}

	/**
	 * @param pronouns the pronouns to set
	 */
	public void setPronouns(int pronouns) {
		this.pronouns = pronouns;
	}

	/**
	 * @return the begPronouns
	 */
	public int getBegPronouns() {
		return begPronouns;
	}

	/**
	 * @param begPronouns the begPronouns to set
	 */
	public void setBegPronouns(int begPronouns) {
		this.begPronouns = begPronouns;
	}

	/**
	 * @return the articles
	 */
	public int getArticles() {
		return articles;
	}

	/**
	 * @param articles the articles to set
	 */
	public void setArticles(int articles) {
		this.articles = articles;
	}

	/**
	 * @return the begArticles
	 */
	public int getBegArticles() {
		return begArticles;
	}

	/**
	 * @param begArticles the begArticles to set
	 */
	public void setBegArticles(int begArticles) {
		this.begArticles = begArticles;
	}

	/**
	 * @return the relativeClause
	 */
	public int getRelativeClause() {
		return relativeClause;
	}

	/**
	 * @param relativeClause the relativeClause to set
	 */
	public void setRelativeClause(int relativeClause) {
		this.relativeClause = relativeClause;
	}

	/**
	 * @return the adjective
	 */
	public int getAdjective() {
		return adjective;
	}

	/**
	 * @param adjective the adjective to set
	 */
	public void setAdjective(int adjective) {
		this.adjective = adjective;
	}

	/**
	 * @return the adverb
	 */
	public int getAdverb() {
		return adverb;
	}

	/**
	 * @param adverb the adverb to set
	 */
	public void setAdverb(int adverb) {
		this.adverb = adverb;
	}

	/**
	 * @return the passive
	 */
	public int getPassive() {
		return passive;
	}

	/**
	 * @param passive the passive to set
	 */
	public void setPassive(int passive) {
		this.passive = passive;
	}

	/**
	 * @return the prepositions
	 */
	public int getPrepositions() {
		return prepositions;
	}

	/**
	 * @param prepositions the prepositions to set
	 */
	public void setPrepositions(int prepositions) {
		this.prepositions = prepositions;
	}

	/**
	 * @return the conjunctions
	 */
	public int getConjunctions() {
		return conjunctions;
	}

	/**
	 * @param conjunctions the conjunctions to set
	 */
	public void setConjunctions(int conjunctions) {
		this.conjunctions = conjunctions;
	}

	/**
	 * @return the avgNPLength
	 */
	public int getAvgNPLength() {
		return avgNPLength;
	}

	/**
	 * @param avgNPLength the avgNPLength to set
	 */
	public void setAvgNPLength(int avgNPLength) {
		this.avgNPLength = avgNPLength;
	}

	/**
	 * @return the avgVPLength
	 */
	public int getAvgVPLength() {
		return avgVPLength;
	}

	/**
	 * @param avgVPLength the avgVPLength to set
	 */
	public void setAvgVPLength(int avgVPLength) {
		this.avgVPLength = avgVPLength;
	}

	public Realisation() {	
	}


	
}
