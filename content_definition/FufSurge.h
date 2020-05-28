#pragma once
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class FufSurge
{
public:
	FufSurge(void);

	/* The following methods intend to modularize the FUF/Surge template creation */
	// realizationCAT will have only three parameters when it is a clause
	string realizeClause(string process, string mode, string lex, string clauseAdditional, string procAdditional);
	// if it is one of common or proper
	//string FufSurge::realizeCAT(string catType, string lex, int determiner, string classifier, string number, bool qualifier);
	// it just adds the preposition and (np( to the template -- the rest need to be added or got from CAT method
	string realizePP(string preposition);
	string realizeSubjectCommon(string noun, int determiner, string classifier, string number);
	string realizeSubjectPronoun();
	string realizeSubjectDistance(string noun);
	string realizeSubjectOrdinal(string position);
	string realizeMeasure(string quantity, string unit);

	/* Beginning graph sentences for level 0 */
	// It generates the first sentence of the Introduction of summaries level 0
	string sentence1IntroLevel0(bool bGraph_volatility, string sGraph_volatility, string graph_vol_intensity);
	// It generates the second sentence of the Introduction of summaries level 0
	string sentence2IntroLevel0(string noun);
	
	/* Intention sentences */
	// It generates the sentence of intention (with dates) for intention type 1 (RT, FT, ST)
	string sentenceIntentionType1IntroLevel0(string noun, string initialDate, string endDate, string subjectType);
	// It generates the first sentence of intention for type 1 (CHT, CTR, CTLS, CSCT)
	string sentence1IntentionType2IntroLevel0(string noun, string subjectType);
	// It generates the second sentence of intention for type 1 (CHT, CTR, CTLS, CSCT) WITHOUT TREND DATES
	string sentence2IntentionType2IntroLevel0(string noun, string subjectType, int intentionType, vector<pair<string,string>> trendVector);
	// It generates the second sentence of intention for type 1 (CHT, CTR, CTLS, CSCT) WITH TREND DATES
	string sentence3IntentionType2IntroLevel0(string noun, string subjectType, int intentionType, vector<pair<string,string>> trendVector);
	// It generates THE CIRCUMSTANCE PIECE of an FD when the graph has more trends than the intention
	string exceptionInTheGraph(string noun, string subjectType, string initialDate, string endDate);

	/* initial_value end_value maximum_value minimum_value */
	// It generates the INITIAL, END, MAXIMUM OR MINIMUM value of a TREND or GRAPH (these two only change the way I start the sentence - possessive pronoun)
	string individualValueTrendSentence1(string noun, string value, string unit, string subjectType, string classifier);
	string individualValueTrendSentence2(string value, string unit, string classifier);
	// It generates initial and end values or maximum and minimum value aggregated
	string aggregatedValueTrendSentence(bool clause, bool subject, string noun, string value1, string value2, string unit, string subjectType, string classifier1, string classifier2);
	
	/* volatility steepness */
	// It generates a single sentence for steepness. *** Object Position ***
	string steepnessSentenceObj(string noun, string subjectType, string value, string classifier);
	// It generates a single sentence for volatility for level 0. *** Object Position ***
	string volatilitySentence1Level0Obj(bool clause, bool partic, bool subject, string noun, string subjectType, string value1, string value2, string classifier);
	// It generates another single sentence for volatility for level 0. *** Object Position ***
	string volatilitySentence2Level0Obj(string noun, string subjectType, string classifier);
	
	/* rate_change absolute_change overall_period */
	// It generates a single sentence with rate, absolute and period of change.
	string rateAbsolutePeriodSentence(string noun, string behavior, string subjectType, string rate, string rateMeasure, string absolute, string unit, string period, string periodUnit);
	// It generates a single sentence with rate and absolute change.
	string rateAbsoluteSentence(string noun, string behavior, string subjectType, string rate, string rateMeasure, string absolute, string unit);
	// It generates either the rate_change or the absolute_change
	string overallChangeSentence(string noun, string behavior, string subjectType, string value, string measure);
	// It generates either the rate_change or the absolute_change + overall_period
	string overallChangePeriodSentence(string noun, string behavior, string subjectType, string value, string measure, string period, string periodUnit);

	~FufSurge(void);
};
