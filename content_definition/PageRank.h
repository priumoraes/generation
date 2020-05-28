#pragma once
#include "DBConnection.h"

const float complementRelation		= 0.6f;
const float belongsToRelation		= 0.6f;
const float contrastRelation		= 0.4f;
const float redundancyRelation		= 0.1f;
const float sameMembershipRelation	= 0.1f;

const float penaltyFactor		= 4.00f;
const float boostFactor			= sqrt(float(penaltyFactor));

struct realization{
	string partOfSentence;  // not using partOfSentence nor partOfSpeech yet, see if it is necessary (maybe I will need some other field to identify diferent proposition purposes)
	string partOfSpeech;
	string fufSurgeFrame;
};

struct proposition{				// maybe add a field that will hold additional info such as degree of volatility, sustainability of small segments
	string type;				// field used to identify relationship between graphs (ex: Initial_Date)
	string description;			// actual value
	vector<string> membership;	// vector of unique identifiers of which the proposition belongs to (trend, composedTrend)
	string detail;				// this field can be used to store additional info pertaining to the proposition (it can be number of trends for a suggestion, the order of a trend or the degree of the volatility/steepness)
	bool conveyable;			// conveyable field: holds true if the proposition should be conveyed, false otherwise
	bool independent;			// independent propositions represent concepts that can be alone in a sentence (not needed to add membership info) - volatility is NOT an independent prop
	// myRealization is a vector because some propositions require more than one to be conveyed (composed_trend is an example)
	vector<pair<string, int>> terms; // a vector of different terms that can be used to describe a concept. The integer element can be 0 (easier) or 1 (harder)
	// the different term levels will be used in the aggregation step for generating summaries in different grading levels
	vector<realization> myRealization;		
	string segmentPosition;		// this is used to keep the appearance order of a segment in the graph, only some will have an actual value
	// this vector stores any particular additional info of a proposition type, graph_type will store trends info (ordered)for the whole graph, 
	// while composed_trend will store its trends in order. It has a pair of strings so we can add something that identifies the detailed
	// information (for ordered trends it brings the number of the trend segment in the graph)
	vector<pair<string,string>> additionalInfo;	
};

struct node{
	proposition myProposition;
	float *edge;			// it becomes an array of pointers to the weights
	bool selected;			// controls if a node was already selected for the current response
	int degree;
	int conceptRole;		// 1 - Entity; 2 - Feature
}; 


// auxiliary structure to keep track of the score with which a node was selected
struct nodeScore{
	node aNode;
	float score;
};

// grouped information about each entity (features are not represented here, they are grouped within the entity
// to which they belong
struct entity{
	bool special;			// special entities are Rising and Falling trends that are conceptually a BJ or BF respectivelly
	string description;		// Rising Trend, Falling Trend
	string key;				// specific key that identifies a unique segment (rising, falling or stable trend)
	int featureCount;		// number of features selected for the response that belong to a segment
	double totalWeight;		// total sum of weights of the propositions selected (see if I will need that)
	string segmentPosition;
};

// this structures holds grouped information from nodes selected by page rank for each response
// it is used in the step where we need to do focus shifting in the organization
struct responseComponents{
	pair<string, string> intention; //its category name and its unique description
	vector<pair<pair<string, string>,bool>> secondaryMessages;   //a secondary message is composed by pair category + unique description and stores a boolean for organization
	vector<entity> allEntities;
};

/* initialWeightMap is composed by the proposition type and the initial weight that will be assigned to it
 * by the priority vertex. Those weights are collected from either a previous computation or from the
 * experiment 
 */
typedef map<string,float> initialWeightMap;
typedef pair<string,string> propositionPair;
typedef map<propositionPair,string> relationWeightMap;
typedef map<int,int> selectionHistoryMap;	//node position in the graph is the key, the value is the number
											//of times this node was selected to be conveyed in a response

class PageRank
{

	/* Example set of propositions currently used to describe a line graph. Since the graph has dynamicly
	defined size, this example assumes only one ComposedTrend (Suggestion) and one Trend.
	Additional information that will be computed are represented within a proposition by (<metric>).
	P0: (Priority Vertex) - this proposition is not conveyed
	P1: graph_type -- "This is a line graph."
	P2: entity_description -- "The graph is about <entity> (<scale>)." - Measurement axis descriptor
	P3: graph_volatility -- "The graph is (<degree>) volatile/smooth."
	P4: graph_overall_behaviour -- "The graph shows an overall (<graph_overall_behaviour>) increase/decrease."
	P5: graph_absolute_change -- "The graph changed by (<graph_absolute_change>) (<unit>)."
	P6: graph_rate_change -- "The graph (<graph_overall_behaviour>) (<graph_rate_change>) twice/1.5 times."
	P7: graph_overall_period_years -- "The overall graph period in years is (<graph_overall_period_years>)."
	P8: graph_overall_period_months -- "The overall graph period in months is (<graph_overall_period_months>)."
	P9: graph_overall_period_days -- "The overall graph period in days is (<graph_overall_period_days>)."
	P10: graph_initial_date -- "The graph initiates in/on/at (<graph_initial_date>)."
	P11: graph_end_date -- "The graph ends in/on/at (<graph_end_date>)."
	P12: graph_initial_value -- "The graph initiates in/on/at (<graph_initial_value>)."
	P13: graph_end_value -- "The graph ends in/on/at (<graph_end_value>)."
	P14: maximum_point_value -- "The maximum value in the graph is <maximum_value>."
	P15: minimum_point_value -- "The minimum value in the graph is <minimum_value>."
	P16: maximum_point_date -- "The maximum value in the graph occurs at <maximum_value_date>."
	P17: minimum_point_date -- "The minimum value in the graph occurs at <minimum_value_date>."
	P18: composed_trend -- "This graph shows a <composed_trend>."
	P19: trend_description -- "This is a <trend_description>." (Rising Trend, Falling Trend, 
							  Stable Trend, Point Correlation, Non sustained)
	P20: volatility -- "<trend_description> is (<degree>) volatile / smooth." - the characteristic will be
					   decided based on the metric calculation result.
	P21: steepness -- "<trend_description> is (<degree>) steep / flat." - the characteristic will be
					   decided based on the metric calculation result.
	P22: trend_initial_date -- "<trend_description> starts at <trend_initial_date>."
	P23: trend_end_date -- "<trend_description> ends at <trend_end_date>."
	P24: trend_initial_value -- "<trend_description> has <trend_initial_value> as its initial value."
	P25: trend_end_value -- "<trend_description> has <trend_end_value> as its end value."
	P26: trend_absolute_change -- "<trend_description> has a total increase/decrease of (<trend_absolute_change>)."
	P27: trend_rate_change -- "<trend_description> has a total increase/decrease of (<trend_rate_change>)."
	P28: trend_overall_period_years -- "<trend_description> is over the period of (<trend_overall_period_years>)."
	P29: trend_overall_period_months -- "<trend_description> is over the period of (<trend_overall_period_months>)."
	P30: trend_overall_period_days -- "<trend_description> is over the period of (<trend_overall_period_days>)."
	*/
	
public:

	PageRank::PageRank();
	
	DBConnection dbconnection;
	Helper myHelper;
	Input input;

	typedef vector<node> graph;
	typedef vector<nodeScore> vectorNodeScore;
	typedef nodeScore myNodeScore;

	vector<float> pageRankVector;
	vector<float> pageRankVectorCopy;
	vectorNodeScore rankedNodes;
	vectorNodeScore finalRankedNodes;

	int tempGraphSize;
	int totalNumberOfTrends;
	int numberOfIntentionTrends;
	
	graph myGraph;
	entity anEntity;
	responseComponents aResponse;

	map<string,string> trendNameMap;
	map<string,string>::iterator trendNameMap_it;

	initialWeightMap myInitialWeightMap;
	initialWeightMap::iterator initialWeightMap_it;

	relationWeightMap myRelationWeightMap;
	relationWeightMap::iterator myRelationWeightMap_it;

	selectionHistoryMap mySelectionHistoryMap;
	selectionHistoryMap::iterator mySelectionHistoryMap_it;

	void PageRank::startPageRank(char *linegraphKeyTemp, string minimumProbability, int gap);
	void PageRank::instantiateInitialWeightMap();
	void PageRank::instantiateRelationWeightMap();
	void PageRank::instantiateSelectionHistoryMap();
	void PageRank::instantiateGraph(PGconn *conn, PGresult *res, PGresult *resOrdTrend, int graphSize, PGresult *resAnnotationOnGraphic, char *linegraphKeyTemp);
	void PageRank::updateMembership(string composedTrend, string trend);
	void PageRank::adjustMembershipForBFandBJ();
	void PageRank::setInitialPropWeights();
	void PageRank::applyEdgeWeight();
	void PageRank::printGraphContent();
	void PageRank::boostBasedOnSuggestion();
	void PageRank::getDegree();

	void PageRank::setInitialPageRank();
	void PageRank::convergePageRank();
	void PageRank::printPageRankVector();
	void PageRank::runPageRank();
	void PageRank::updatePropositionsRelations(int nodePosition);
	void PageRank::connectAncestorsandDescendants();
	void PageRank::initialBoostingSpecialProps();
	void PageRank::compTrendRelationSetup();
	void PageRank::adjustStableTrendDescription();
	void PageRank::selectPropositions();
	void PageRank::initialSummaryPropositions(vectorNodeScore allPropositions, int index);
	void PageRank::printRankedNodes(vectorNodeScore nodes);
	void PageRank::printGraphNodes();
	void PageRank::printMap();
	void PageRank::printTrendNameMap();
	
	bool PageRank::checkIfSameMembership(node aNode1, node aNode2);
	bool PageRank::checkIfBelongs(node aNode, string ancestor);
	bool PageRank::compTrendRelationSetupHelper(string compTrend1, string compTrend2);

	string PageRank::getVolatilityDegree(string weight);
	string PageRank::getSteepnessDegree(string angle, string volatility);

	int PageRank::getRelationBetweenSuggestions(string intention, string additionalSuggestion);
	int PageRank::getNodePositionFromUniqueType(string nodeType);

	node PageRank::getCompTrendNodeFromDescription(string aCompTrend);
	vector<node> PageRank::getVectorOfTrendNodes(string aCompTrend);

	float PageRank::sumWeightsOfEdges(node aNode);
	//float PageRank::calculateImportanceScore(node aNode);
	float PageRank::calculateImportanceScore(node aNode);
	//void PageRank::cleanSelectedList();
	//void PageRank::cleanPageRank();

	PageRank::~PageRank(void);
};
