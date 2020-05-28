/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * Defines a node for the A* search algorithm.
 */
package edu.udel.eecis.teahouse.sight;

import java.util.List;
import java.util.Map;
import java.util.Stack;

import simplenlg.phrasespec.SPhraseSpec;

/**
 * @author Priscilla
 *
 */
public class SearchNode {

	private List<Proposition> unrealisedProps;
	private Map<String,Map<String,Map<String,Integer>>> sentenceMap;
	private Stack<Pair<String,SPhraseSpec>> propSentences;
	private boolean prune;
	
	
	public SearchNode(){}


	/**
	 * @return the unrealisedProps
	 */
	public List<Proposition> getUnrealisedProps() {
		return this.unrealisedProps;
	}


	/**
	 * @param unrealisedProps the unrealisedProps to set
	 */
	public void setUnrealisedProps(List<Proposition> unrealisedProps) {
		this.unrealisedProps = unrealisedProps;
	}


	/**
	 * @return the sentenceMap
	 */
	public Map<String, Map<String, Map<String, Integer>>> getSentenceMap() {
		return this.sentenceMap;
	}


	/**
	 * @param sentenceMap the sentenceMap to set
	 */
	public void setSentenceMap(
			Map<String, Map<String, Map<String, Integer>>> sentenceMap) {
		this.sentenceMap = sentenceMap;
	}


	/**
	 * @return the propSentences
	 */
	public Stack<Pair<String, SPhraseSpec>> getPropSentences() {
		return this.propSentences;
	}


	/**
	 * @param propSentences the propSentences to set
	 */
	public void setPropSentences(
			Stack<Pair<String, SPhraseSpec>> propSentences) {
		this.propSentences = propSentences;
	}


	/**
	 * @return the prune
	 */
	public boolean isPrune() {
		return this.prune;
	}


	/**
	 * @param prune the prune to set
	 */
	public void setPrune(boolean prune) {
		this.prune = prune;
	}
	
	
}
