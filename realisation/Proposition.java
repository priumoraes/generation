/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * Proposition objects correspond to nodes in the graph and represent atomic pieces of information that become either phrases or sentences.
 */
package edu.udel.eecis.teahouse.sight;

import java.util.List;
import java.util.Map;
import java.util.Set;

import simplenlg.phrasespec.SPhraseSpec;


/**
 * @author Priscilla
 *
 */
public class Proposition {

	// Some of the proposition fields will be empty, depending on the proposition type
	private String type;
	private String propTemplate;
	private String description;
	private String detail;
	private String degree;
	private String membership;
	private String membershipCode;
	private String group;
	private List<List<String>> additionalInfo;
	private String value;
	private String valueDetail;	//can be used for units and scale
	private String segmentPosition;
	private boolean used;
	private boolean conveyable;
	private boolean isIndependent;
	private List<Realisation> allowedRealisations;
	//key is the part of speech (noun, adjective, conjunction) and value is a list of possibilities for that POS
	private Map<String, String> lexicon;
	private String currentRealisation;
	private SPhraseSpec currentSentenceObject;
	
	public Proposition(){
		
	}

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getDetail() {
		return detail;
	}

	public void setDetail(String detail) {
		this.detail = detail;
	}

	public String getDegree() {
		return degree;
	}

	public void setDegree(String degree) {
		this.degree = degree;
	}

	public String getMembership() {
		return membership;
	}

	public void setMembership(String membership) {
		this.membership = membership;
	}

	public List<List<String>> getAdditionalInfo() {
		return additionalInfo;
	}

	public void setAdditionalInfo(List<List<String>> additionalInfo) {
		this.additionalInfo = additionalInfo;
	}

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}

	public String getValueDetail() {
		return valueDetail;
	}

	public void setValueDetail(String valueDetail) {
		this.valueDetail = valueDetail;
	}

	public String getSegmentPosition() {
		return segmentPosition;
	}

	public void setSegmentPosition(String segmentPosition) {
		this.segmentPosition = segmentPosition;
	}

	public boolean isUsed() {
		return used;
	}

	public void setUsed(boolean used) {
		this.used = used;
	}

	public boolean isConveyable() {
		return conveyable;
	}

	public void setConveyable(boolean conveyable) {
		this.conveyable = conveyable;
	}

	public boolean isIndependent() {
		return isIndependent;
	}

	public void setIndependent(boolean isIndependent) {
		this.isIndependent = isIndependent;
	}

	public String getPropTemplate() {
		return propTemplate;
	}

	public void setPropTemplate(String propTemplate) {
		this.propTemplate = propTemplate;
	}

	public String getGroup() {
		return group;
	}

	public void setGroup(String group) {
		this.group = group;
	}

	public String getMembershipCode() {
		return membershipCode;
	}

	public void setMembershipCode(String membershipCode) {
		this.membershipCode = membershipCode;
	}

	public List<Realisation> getAllowedRealisations() {
		return allowedRealisations;
	}

	public void setAllowedRealisations(List<Realisation> allowedRealisations) {
		this.allowedRealisations = allowedRealisations;
	}

	public Map<String,String> getLexicon() {
		return lexicon;
	}

	public void setLexicon(Map<String,String> lexicon) {
		this.lexicon = lexicon;
	}

	/**
	 * @return the currentRealisation
	 */
	public String getCurrentRealisation() {
		return currentRealisation;
	}

	/**
	 * @param currentRealisation the currentRealisation to set
	 */
	public void setCurrentRealisation(String currentRealisation) {
		this.currentRealisation = currentRealisation;
	}

	/**
	 * @return the currentSentenceObject
	 */
	public SPhraseSpec getCurrentSentenceObject() {
		return currentSentenceObject;
	}

	/**
	 * @param currentSentenceObject the currentSentenceObject to set
	 */
	public void setCurrentSentenceObject(SPhraseSpec currentSentenceObject) {
		this.currentSentenceObject = currentSentenceObject;
	}


	
	
}
