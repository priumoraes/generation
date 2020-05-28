#pragma once
#include <set>
#include "PageRank.h"
#include "FufSurge.h"


class Generation
{

public:
	PageRank myPageRank;
	FufSurge myFufSurge;

	char *linegraphKeyTemp;
	string mostImportantSegment; // the most important trend is stored in here and is used in the organization/focus shifting module
	bool exceptionOnGraphDates; // it its true if graph is bigger than intention (if it has additional trends)
	// it stores the ...?
	string surgeTemplateAux;
	vector<string> allSentences; // This vector stores the aggregated sentences to be realized in order

	// in order to aggregate sentences I need a fast way of checking which propositions were selected by PageRank
	// since I do not have a predefined set of propositions of each response. For this reason, I am creating this
	// set so I can fill it with all the propositions that there are, and read it when making aggragation decisions
	set<string> myGraphProps, myTrendProps_aux;
	set<string>::iterator myGraphProps_it, myTrendProps_aux_it;
	
	// For trends I need to store a set for each different segment. the key of the map is the segment position
	// of the trend in the graph
	map<string,set<string>> myTrendProps;
	map<string,set<string>>::iterator myTrendProps_it;
	
	// this set is responsible for keeping the ordering in a response. it is composed of two integers
	// the first is the proposition order and the second is the position of the proposition in the
	// myPageRank.finalRankedNodes vector
	map<int,int> myOrganizedSet;
	map<int,int>::iterator myOrganizedSet_it;

	// this map will allow us to quickly get the value of a proposition given its type (e.g. graph_type = position
	// of the graph_type proposition on myGraph vector, so I can get the value wherever it is inside the
	// proposition node). It will only contain the propositions selected for this response.
	// It is being filled in the same function where I fill the sets of prop types (fillPropositionsSelected())
	//map<string, int> myTypeValuePropMap;
	//map<string, int>::iterator myTypeValuePropMap_it;

	// Please note that this map contains PROBABLE comparable features, the ones that are really
	// comparable will be stored into comparableSets
	// key contains the type of proposition (volatility, steepness, etc)
	// value is a vector of pairs where the first element is the trend order number (so we know which 
	// trends have that proposition in the response) and the second is the position of that proposition
	// on myPageRank.finalRankedNodes (so we pass that information to myOrganizedSet)
	map<string,vector<pair<string,int>>> candidateComparableFeatures;
	map<string,vector<pair<string,int>>>::iterator candidateComparableFeatures_it;

	// this vector will be filled by the method checkIfComparable and it will have as its elements
	// the names of the groups that will be compared (the groups might be: volatiltiy, steepness, 
	// trend_overall_period_years, trend_overall_period_months, trend_overall_period_days, 
	// trend_absolute_change, trend_rate_change). Other features such as trend_descritption, 
	// initial and end dates and values won't be used in the comparision since they are identifiers 
	// of the trend, analytical attributes
	set<string> comparableSets;
	set<string>::iterator comparableSets_it;

	Generation(char* lineGraphKey);
	void addFufSurgeTemplates();
	void fillOrganizationStruct();
	void responseOrganization();
	void segmentOrganization(int curPosition, vector<string> trendOrder, string importantSegmentPos);
	void segmentComparisonOrganization(int curPosition, vector<string> trendOrder);
	void responseRealizationIndividualSentences();
	void responseRealizationAggregatedSentences(vector<string> allSentences);
	void fillPropositionsSelected();
	void callFunctionForTemplate(int templateNumber);

	// printing methods
	void printOrganizationStruct();
	void printFufSurgeTemplates();
	void printAggregatedFufSurgeTemplates();
	void printOrganizedSet();
	void printOrganizedSetFufSurgeTemplates();
	void printComparableFeaturesMap();
	void printComparableSets();
	void printGraphPropsSet();
	void printTrendPropsMap();

	int overallGraphOrganization(int numberOfSecondary, int maxPosition, int suggestionPos);

	bool checkIfComparable(vector<string> trendOrder);

	string getImportantSegmentPosition(string intention);
	
	vector<string> aggregateTemplate0();
	vector<string> aggregateTemplate1();
	vector<string> aggregateTemplate2();
	vector<string> aggregateTemplate3();
	vector<string> aggregateTemplate4();
	
	~Generation(void);

};