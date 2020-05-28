#include "FufSurge.h"

FufSurge::FufSurge(void)
{
}

/* This method creates a FUF/SURGE template for common or proper nouns, it receives its type, the lex, an integer 
 * representing the determiner (1: definite yes, 2: definite no, 3: determiner none), the parameters classifier and
 * number are optional and they contain the actual value if not null. Please be aware that if the last closing
 * parenthesis is missing - since we can have qualifier or possessor info before ending the cat common
 */
string FufSurge::realizeSubjectCommon(string noun, int determiner, string classifier, string number)
{
	string tempAgent;
	tempAgent.append("((cat common)(lex \\\"");
	tempAgent.append(noun);
	tempAgent.append("\\\")");
	if (determiner == 1)
		tempAgent.append("(definite yes)");
	else if (determiner == 2)
		tempAgent.append("(definite no)");
	else if (determiner == 3)
		tempAgent.append("(determiner none)");
	if (classifier.compare("") != 0)
	{
		tempAgent.append("(classifier === \\\"");
		tempAgent.append(classifier);
		tempAgent.append("\\\")");
	}
	if (number.compare("") != 0)
	{
		tempAgent.append("(number ");
		tempAgent.append(number);
		tempAgent.append(")");
	}
	return tempAgent;
}

// This method returns the string "It"
string FufSurge::realizeSubjectPronoun()
{
	string tempAgent;
	tempAgent.append("((cat pronoun)(person third))");
	return tempAgent;
}

// This method returns the string "This (noun)"
string FufSurge::realizeSubjectDistance(string noun)
{
	string tempAgent;
	tempAgent.append("((cat common)(distance near)(lex \\\"");
	tempAgent.append(noun);
	tempAgent.append("\\\"))");
	return tempAgent;
}

// This method returns the string "The first/second/third segment"
string FufSurge::realizeSubjectOrdinal(string position)
{
	string tempAgent;
	tempAgent.append("((cat common)(ordinal ((value ");
	tempAgent.append(position);
	tempAgent.append(")(digit no)))(definite yes)(lex segment))");
	return tempAgent;
}

string FufSurge::realizePP(string preposition)
{
	string tempTemplate;
	tempTemplate.append("((cat pp)(prep === ");
	tempTemplate.append(preposition);
	tempTemplate.append(")(np");
	return tempTemplate;
}

// it closes all the opened parenthesis
string FufSurge::realizeMeasure(string quantity, string unit)
{
	string tempTemplate;
	tempTemplate.append("((cat measure)(quantity ((value ");
	tempTemplate.append(quantity);
	tempTemplate.append(")))(unit");
	if (unit.compare("") == 0)
		tempTemplate.append("((gap yes))))");
	else
	{
		tempTemplate.append("((lex \\\"");
		tempTemplate.append(unit);
		tempTemplate.append("\\\"))))");
	}
	return tempTemplate;
}
 
// This method generates the clause information stopping at (partic(
// additional can add additional tuples to the clause (mood relative), for example.
string FufSurge::realizeClause(string process, string mode, string lex, string clauseAdditional, string procAdditional)
{
	string tempTemplate;
	tempTemplate.append("((cat clause)");
	if (clauseAdditional.compare("") != 0)
		tempTemplate.append(clauseAdditional);
	tempTemplate.append("(proc((type ");
	tempTemplate.append(process);
	tempTemplate.append(")");
	if (mode.compare("") != 0)
	{	
		tempTemplate.append("(mode ");
		tempTemplate.append(mode);
		tempTemplate.append(")");
	}	
	if (process.compare("material") == 0)
	{
		tempTemplate.append("(lex \\\"");
		tempTemplate.append(lex);
		tempTemplate.append("\\\")");
	}
	if (procAdditional.compare("") != 0)
		tempTemplate.append(procAdditional);
	tempTemplate.append("))");
	tempTemplate.append("(partic(");
	return tempTemplate;
}


// This method returns the string corresponding to aggregating graph_type + graph_volatility for summaries level 0
// "The image shows a line graph." or "The image shows a line graph with many ups and downs."
string FufSurge::sentence1IntroLevel0(bool bGraph_volatility, string sGraph_volatility, string graph_vol_intensity)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material) (lex show)))(partic((agent ");
	// adding the cat common for the agent
	sentence.append(realizeSubjectCommon("image", 1, "", ""));
	//closing the parenthesis of common and the agent
	sentence.append("))");
	sentence.append("(affected ");
	// adding the cat common for the affected (classifier is blank here because for volatility, for example,
	// it will be a circumstance since I will be using fluctuation or ups and downs, or peaks and valleys
	sentence.append(realizeSubjectCommon("line graph", 2, "", ""));
	//closing the parenthesis of the common, affected and partic
	sentence.append(")))");
	if (!(bGraph_volatility))
		//closing the parenthesis of the clause (no circumstance will be added)
		sentence.append("))");
	else
	{
		sentence.append("(circum((manner((cat pp)(prep === with)(np ");
		sentence.append(realizeSubjectCommon(sGraph_volatility, 1, graph_vol_intensity, "plural"));
		//closing the parenthesis of the circum and clause
		sentence.append(")))))))");
	}
	return sentence;
}

// This method returns the string corresponding to entity_description for summaries level 0
// "The line graph presents the number of annual difference from Seattle's 1899 sea level in inches".
string FufSurge::sentence2IntroLevel0(string noun)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material) (lex present)))(partic((agent");
	sentence.append(realizeSubjectCommon("line graph", 1, "", ""));
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(noun, 3, "", ""));
	sentence.append(")))))");
	return sentence;
}

/******************************** INTENTION SENTENCES ****************************************************/
// This method returns the string corresponding to intention type 1 (RT, FT, ST) for summaries level 0. For
// BF and BJ we will use the FufSurge atomic template for composed_trend. subjectType can be pronoun, common,
// distance or ordinal
// "It shows a rising trend from 1900 to 1928."
string FufSurge::sentenceIntentionType1IntroLevel0(string noun, string initialDate, string endDate, string subjectType)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex show)))(partic((agent");
	// deciding if calls method to return "It" or "The line graph"
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon("line graph", 1, "", ""));
		sentence.append(")");	// closing the paranthesis of common
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(noun, 2, "", ""));
	sentence.append(")(qualifier(((cat list)(distinct ~(((cat pp)(prep === from)(np");
	sentence.append(realizeSubjectCommon(initialDate, 3, "", ""));
	sentence.append(")))((cat pp)(prep === to)(np");
	sentence.append(realizeSubjectCommon(endDate, 3, "", ""));
	sentence.append(")))))))))))))");
	return sentence;
}

// This method returns the string corresponding to the first sentence of intention type 2 and 3 
// (CHT, CTR, CTLS, CSCT) for summaries level 0.
// "It shows a trend that changes." or "It shows a trend that changes and returns."
string FufSurge::sentence1IntentionType2IntroLevel0(string noun, string subjectType)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex show)))(partic((agent");
	// deciding if calls method to return "It" or "The line graph"
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon("line graph", 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(noun, 2, "", ""));
	sentence.append(")))))");
	return sentence;
}


// This method returns the string corresponding to the second sentence of intention type 2 and 3 
// (CHT, CTR, CTLS, CSCT) for summaries level 0 (without the dates on it). noun is the description of the intention
// when needed; subjectType is one of: pronoun, common, distance, ordinal
// "It consists of a relatively stable trend followed by a rising trend.
// or "It consists of a relatively stable trend, a falling trend and 
// a rising trend."
string FufSurge::sentence2IntentionType2IntroLevel0(string noun, string subjectType, int intentionType, vector<pair<string,string>> trendVector)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex consist)))(partic((agent");
	// deciding if calls method to return "It" or "The changing trend"
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	if (intentionType == 2)
	{
		sentence.append("((cat pp)(prep === of)(np");
		sentence.append(realizeSubjectCommon(trendVector[0].first, 2, "", ""));
		sentence.append(")(qualifier((cat pp)(prep === \\\"followed by\\\")(np");
		sentence.append(realizeSubjectCommon(trendVector[2].first, 2, "", ""));
		sentence.append(")))))))))))");
	}
	else if (intentionType == 3)
	{
		sentence.append("((complex conjunction)(distinct ~(((cat pp)(prep === of)(np");
		sentence.append(realizeSubjectCommon(trendVector[0].first, 2, "", ""));
		sentence.append(")))((cat pp)(prep === \\\"followed by\\\")(np");
		sentence.append(realizeSubjectCommon(trendVector[2].first, 2, "", ""));
		sentence.append(")))");
		sentence.append(realizeSubjectCommon(trendVector[4].first, 2, "", ""));
		sentence.append("))))))))");
	}
	return sentence;
}


// This method returns the string corresponding to the second sentence of intention type 2 and 3 
// (CHT, CTR, CTLS, CSCT) for summaries level 0, although it has the dates on it.
// "It consists of a relatively stable trend from 1900 to 1928 followed by a rising trend until 2003.
// or "It consists of a relatively stable trend from 1900 to 1928, a falling trend until 2000 and 
// a rising trend through 2003."
string FufSurge::sentence3IntentionType2IntroLevel0(string noun, string subjectType, int intentionType, vector<pair<string,string>> trendVector)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex consist)))(partic((agent");
	// deciding if calls method to return "It" or "The changing trend"
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	if (intentionType == 2)
	{
		sentence.append("((cat pp)(prep === of)(np");
		sentence.append(realizeSubjectCommon(trendVector[0].first, 2, "", ""));
		sentence.append(")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np");
		sentence.append(realizeSubjectCommon(trendVector[1].first, 3, "", ""));
		sentence.append(")))((cat pp)(prep === to)(np");
		sentence.append(realizeSubjectCommon(trendVector[1].second, 3, "", ""));
		sentence.append(")))((cat pp)(prep === \\\"followed by\\\")(np");
		sentence.append(realizeSubjectCommon(trendVector[2].first, 2, "", ""));
		sentence.append(")))((cat pp)(prep === through)(np");
		sentence.append(realizeSubjectCommon(trendVector[3].second, 2, "", ""));
		sentence.append(")))))))))))))");
	}
	else if (intentionType == 3)
	{
		sentence.append("((complex conjunction)(distinct ~(((cat pp)(prep === of)(np");
		sentence.append(realizeSubjectCommon(trendVector[0].first, 2, "", ""));
		sentence.append(")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np");
		sentence.append(realizeSubjectCommon(trendVector[1].first, 3, "", ""));
		sentence.append(")))((cat pp)(prep === to)(np");
		sentence.append(realizeSubjectCommon(trendVector[1].second, 3, "", ""));
		sentence.append("))))))))");
		sentence.append(realizeSubjectCommon(trendVector[2].first, 2, "", ""));
		sentence.append(")(qualifier((cat pp)(prep === until)(np");
		sentence.append(realizeSubjectCommon(trendVector[3].second, 3, "", ""));
		sentence.append(")))))");
		sentence.append(realizeSubjectCommon(trendVector[4].first, 2, "", ""));
		sentence.append(")(qualifier((cat pp)(prep === through)(np");
		sentence.append(realizeSubjectCommon(trendVector[5].second, 3, "", ""));
		sentence.append("))))))))))))");
	}
	return sentence;
}


string FufSurge::exceptionInTheGraph(string noun, string subjectType, string initialDate, string endDate)
{
	string sentence;
	sentence.append("(circum ((concession((cat pp)(position front)(prep === although)(mood concession-mood)(np ((cat clause) "
					"(proc((type material)(lex span)))(partic((agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon(noun,1,"",""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizePP("from"));
	sentence.append(realizeSubjectCommon(initialDate,3,"",""));
	sentence.append(")(qualifier");
	sentence.append(realizePP("to"));
	sentence.append(realizeSubjectCommon(endDate,3,"",""));
	sentence.append("))))))");
	return sentence;
}

/*********************************** END OF INTENTION SENTENCES ****************************************/

/***************************************** TREND SENTENCES *********************************************/

// This method generates: "It has a starting/ending/maximum/minimum value of 1.97 inches." 
// or "The rising trend has a starting/ending/maximum/minimum value of 1.97 inches." 
// (rising trend would be then passed on the noun parameter)
string FufSurge::individualValueTrendSentence1(string noun, string value, string unit, string subjectType, string classifier)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex have)))(partic((agent");
	// deciding if calls method to return "It" or "The rising trend"
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon("value", 2, classifier, ""));
	sentence.append(")(qualifier((cat pp)(prep === of)(np ((cat measure)(quantity((value ");
	sentence.append(value);
	if (unit.compare("") == 0)
		sentence.append(")))(unit((gap yes))))))))))))");
	else
	{
		sentence.append(")))(unit((lex \\\"");
		sentence.append(unit);
		sentence.append("\\\"))))))))))))");
	}
	return sentence;
}

// This method generates: "Its starting/ending/maximum/minimum value is of 1.97 inches." 
string FufSurge::individualValueTrendSentence2(string value, string unit, string classifier)
{
	string sentence;
	sentence.append("((cat clause)(proc((type ascriptive)))(partic((identified");
	sentence.append(realizeSubjectCommon("value", 0, classifier, ""));
	sentence.append(")(possessor((cat personal-pronoun)(person third)(number singular)(gender neuter)))))(identifier((cat pp)"
		 "(prep === of)(np((cat measure)(quantity ((value");
	sentence.append(value);
	if (unit.compare("") == 0)
		sentence.append(")))(unit((gap yes))))))))))");
	else
	{
		sentence.append(")))(unit((lex \\\"");
		sentence.append(unit);
		sentence.append("\\\"))))))))))");
	}
	return sentence;
}

// This method generates: "It has a starting/maximum value of 1.97 inches and a(n)
// ending/minimum value of 2.04 inches." or "The rising trend has a starting/maximum value of 1.97 inches and a(n)
// ending/minimum value of 2.04 inches."
// (rising trend would be then passed on the noun parameter)
string FufSurge::aggregatedValueTrendSentence(bool clause, bool subject, string noun, string value1, string value2, string unit, string subjectType, string classifier1, string classifier2)
{
	string sentence;
	if (clause)
		sentence.append("((cat clause)(proc((type possessive)(mode attributive)))");
	if (subject)
	{
		sentence.append("(partic((carrier");
		// deciding if calls method to return "It" or "The rising trend"
		if (subjectType.compare("pronoun") == 0)
			sentence.append(realizeSubjectPronoun());
		else
		{
			sentence.append(realizeSubjectCommon(noun, 1, "", ""));
			sentence.append(")");
		}
		sentence.append(")");
	}
	sentence.append("(attribute((complex conjunction)(distinct ~(");
	sentence.append(realizeSubjectCommon("value", 2, classifier1, ""));
	sentence.append("(qualifier((cat pp)(prep === of)(np((cat measure)(quantity((value ");
	sentence.append(value1);
	if (unit.compare("") == 0)
		sentence.append(")))(unit((gap yes))))))))");
	else
	{
		sentence.append(")))(unit((lex \\\"");
		sentence.append(unit);
		sentence.append("\\\"))))))))");
	}
	sentence.append(realizeSubjectCommon("value", 2, classifier2, ""));
	sentence.append("(qualifier((cat pp)(prep === of)(np ((cat measure)(quantity((value ");
	sentence.append(value2);
	if (unit.compare("") == 0)
		sentence.append(")))(unit((gap yes)))))))))))))");
	else
	{
		sentence.append(")))(unit((lex \\\"");
		sentence.append(unit);
		sentence.append("\\\")))))))))))))");
	}
	if (subject)
		sentence.append("))");
	if (clause)
		sentence.append(")");
	return sentence;
}


// This method generates: "The rising trend is very/a little bit sharp/steep.". The word steep or sharp should be passed by the value
// parameter. It can be used for any level since either sharp or steep can be given.
string FufSurge::steepnessSentenceObj(string noun, string subjectType, string value, string classifier)
{
	string sentence;
	sentence.append("((cat clause)(proc((type ascriptive)))(partic((identified");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(identifier");
	sentence.append(realizeSubjectCommon(value, 3, "", classifier));
	sentence.append(")))))");
	return sentence;
}

// This method generates: "It has many ups and downs/peaks and valleys." Those will be passed by parameters
// value1 and value2 respectivelly. The intensity (some, many) will be passed by parameter classifier
string FufSurge::volatilitySentence1Level0Obj(bool clause, bool partic, bool subject, string noun, string subjectType, string value1, string value2, string classifier)
{
	string sentence;
	if (clause)
		sentence.append("((cat clause)(proc((type material)(lex have)))");
	if (partic)
	{
		sentence.append("(partic((agent");
		if (!(subject))
			sentence.append("((gap yes))");
		else
		{
			if (subjectType.compare("pronoun") == 0)
				sentence.append(realizeSubjectPronoun());
			else if (subjectType.compare("distance") == 0)
				sentence.append(realizeSubjectDistance(noun));
			else
			{
				sentence.append(realizeSubjectCommon(noun, 1, "", ""));
				sentence.append(")");
			}
		}
	}
	if (partic)
		sentence.append(")");
	sentence.append("(affected((complex conjunction)(distinct ~(");
	sentence.append(realizeSubjectCommon(value1, 2, classifier, "plural"));
	sentence.append(")");
	sentence.append(realizeSubjectCommon(value2, 2, "", "plural"));
	sentence.append(")))))");
	if (partic)
		sentence.append("))");
	if (clause)
		sentence.append(")");
	return sentence;
}

// This method generates: "It shows much fluctuation." The intensity (some, much) will be passed by parameter classifier
string FufSurge::volatilitySentence2Level0Obj(string noun, string subjectType, string classifier)
{
	string sentence;
	sentence.append("((cat clause)(proc((type material)(lex show)))(partic((agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon("fluctuation", 3, "", classifier));
	sentence.append(")))))");
	return sentence;
}


// This method generates: "<The line graph| This trend | It> shows an increase of <rate_change> percent, 
// which represents a total of <absolute_change>, over the period of <overall_change> <year/month/day>.
string FufSurge::rateAbsolutePeriodSentence(string noun, string behavior, string subjectType, string rate, string rateMeasure, string absolute, string unit, string period, string periodUnit)
{
	string sentence;
	sentence.append(realizeClause("material","","show", "", ""));
	sentence.append("(agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(behavior, 2, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeSubjectCommon(rateMeasure, 3, rate, "")); 	
	sentence.append("(qualifier((cat clause)(mood relative)(restrictive no)(proc((type material)(lex represent))) "
					"(scope {^ partic agent})(partic((affected");
	sentence.append(realizeSubjectCommon("total", 2, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(absolute, unit));
	sentence.append(")))))))))))))))(circum((time");
	sentence.append(realizePP("over"));
	sentence.append(realizeSubjectCommon("period", 1, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(period, periodUnit));
	sentence.append("))))))))))");
	return sentence;
}


// This method generates: "<The line graph| This trend | It> shows an increase of <rate_change> percent, 
// which represents a total of <absolute_change>.
string FufSurge::rateAbsoluteSentence(string noun, string behavior, string subjectType, string rate, string rateMeasure, string absolute, string unit)
{
	string sentence;
	sentence.append(realizeClause("material","","show","",""));
	sentence.append("(agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(behavior, 2, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeSubjectCommon(rateMeasure, 3, rate, "")); 	
	sentence.append("(qualifier((cat clause)(mood relative)(restrictive no)(proc((type material)(lex represent))) "
					"(scope {^ partic agent})(partic((affected");
	sentence.append(realizeSubjectCommon("total", 2, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(absolute, unit));
	sentence.append("))))))))))))))))))))))");
	return sentence;
}


// This method generates either a rate_change sentence or an absolute_change sentence. 
string FufSurge::overallChangeSentence(string noun, string behavior, string subjectType, string value, string measure)
{
	string sentence;
	sentence.append(realizeClause("material","","show","",""));
	sentence.append("(agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(behavior, 2, "overall", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(value, measure));
	sentence.append("))))))))");
	return sentence;
}


// This method generates either a rate_change sentence or an absolute_change sentence + overall_period.
// It shows an overall increase/decrease of <rate/absolute> unit over the eriod of <value> <years/months/days>
string FufSurge::overallChangePeriodSentence(string noun, string behavior, string subjectType, string value, string measure, string period, string periodUnit)
{
	string sentence;
	sentence.append(realizeClause("material","","show","",""));
	sentence.append("(agent");
	if (subjectType.compare("pronoun") == 0)
		sentence.append(realizeSubjectPronoun());
	else if (subjectType.compare("distance") == 0)
		sentence.append(realizeSubjectDistance(noun));
	else
	{
		sentence.append(realizeSubjectCommon(noun, 1, "", ""));
		sentence.append(")");
	}
	sentence.append(")(affected");
	sentence.append(realizeSubjectCommon(behavior, 2, "overall", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(value, measure));
	sentence.append(")))))(circum((time");
	sentence.append(realizePP("over"));
	sentence.append(realizeSubjectCommon("period", 1, "", ""));
	sentence.append("(qualifier");
	sentence.append(realizePP("of"));
	sentence.append(realizeMeasure(period, periodUnit));
	sentence.append("))))))))))");
	return sentence;
}

/************************************** END OF TREND SENTENCES *****************************************/


FufSurge::~FufSurge(void)
{
}
