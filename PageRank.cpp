#include "PageRank.h"

using namespace std;

PageRank::PageRank()
{
	totalNumberOfTrends = 0;
	//numberOfIntentionTrends = 0;
}


void PageRank::startPageRank(char *linegraphKeyTemp, string minimumProbability, int gap)
{
	PGconn *conn = dbconnection.InitiateConn();
	//char *linegraphKeyTemp = "73";
	//string minimumProbability = "0.1";
	
	// Calls function from DBConnection that gets all the information from a line graph
	PGresult *res = dbconnection.getLinegraphInformation(conn, linegraphKeyTemp, minimumProbability);
	
	// Calls function from DBConnection that gets all the information from a line graph ordered by trendName
	PGresult *resOrdTrend = dbconnection.getLGInfoOrderedTrend(conn, linegraphKeyTemp, minimumProbability);

	// Calls function from DBConnection that gets all trend ID's in the linegraph that contain an annotation
	PGresult *resAnnotationOnGraphic = dbconnection.getAnnotationOnTrend(conn, linegraphKeyTemp);

	/* The number of tuples represent the number of combinations ComposedTrend - Trend
	 * Since we need to keep track of proposition membership, we need to have all the 
	 * combinations in the graph
	 */
	int numberOfTuples = PQntuples(res);

	// Number of Composed Trends in a line graph
	int numberOfComposedTrends = dbconnection.getNumberOfComposedTrends(conn, linegraphKeyTemp, minimumProbability);
	//int numberOfComposedTrends = 1;  //just for testing running only on the intention (graph L2)

	// Number of Trends in a line graph (not being used to define Graph Size)
	int numberOfTrends = dbconnection.getNumberOfTrends(conn, linegraphKeyTemp, minimumProbability);
	//int numberOfTrends = 2;          //just for testing running only on the intention (graph L2)

	int graphSize = 18 + (numberOfComposedTrends) + (12 * numberOfTrends);
	tempGraphSize = graphSize - 1;
	
	initialWeightMap_it = myInitialWeightMap.begin();

	/* Setup Maps with initial weights */
	PageRank::instantiateInitialWeightMap();
	PageRank::instantiateRelationWeightMap();

	/* Making graph of sixe graphSize */
	myGraph.resize(graphSize);

	/* Calling function that instantiates nodes in the graph */
	PageRank::instantiateGraph(conn, res, resOrdTrend, graphSize, resAnnotationOnGraphic, linegraphKeyTemp);
	
	/* Calling function that adjusts the descritpion of a Stable Trend to say it is relatively stable
	   trend if its trend_absolute_change or its trend_rate_change are different from zero */
	PageRank::adjustStableTrendDescription();

	//setting up relationship between composed trends (can only be done after instantiating the graph)
	if (numberOfComposedTrends > 1)
	{
	//	cout << "------- Calling CompTrendRelationSetup -------------" << endl;
		PageRank::compTrendRelationSetup();	//This is for setting the relationship between composed trend dynamically (only when I have more than 1)
	}

	/* Giving weights to the edges from the priority-vertex to all the other nodes in the graph 
	   It checks the weights on myInitialWeightMap */
	PageRank::setInitialPropWeights();
	
	// deleting non-sustained membership for BF and BJ (it was messing organization)
	//PageRank::adjustMembershipForBFandBJ();

	// applies the weights of the relationships between propositions
	PageRank::applyEdgeWeight();

	// it links the nodes that are related to each other
	PageRank::connectAncestorsandDescendants();

	// it boosts all the connections of propositions that have been boosted on the initial weight 
	// examples are: volatility, steepness, annotation, etc (it is currently doing for everybody that
	// had its initial weight changed)
	PageRank::initialBoostingSpecialProps();

	// it boosts all the propositions with their correspondent suggestions' probability weights
	PageRank::boostBasedOnSuggestion();

	//initiates the history map with 0's
	PageRank::instantiateSelectionHistoryMap();

	//populates the pageRankVector with values from the link from the priority vertex to the node
	PageRank::setInitialPageRank();

	//runs one iteration of PageRank
	//runPageRank();
	
	//calls method that runs pageRank and converges it
	PageRank::convergePageRank();

	//PageRank::printGraphContent();

	// gets nodes connectivity degrees
	PageRank::getDegree();

	//runs a number of iterations of pagerank selecting the highest ranked proposition and updating the graph
	//weights. It also stores the chosen proposition in the rankedNodes vector
	PageRank::selectPropositions();

	
	PageRank::initialSummaryPropositions(rankedNodes, gap);

	//cout << "Printing Initial Summary propositions:" << endl;
	//PageRank::printRankedNodes(finalRankedNodes);

	//printMap();
	//dbconnection.CloseConn(conn);
}

/* This method prints the content of myGraph 
 */
void PageRank::printGraphContent()
{
	//cout << "Start printing Graph" << endl;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		cout << myGraph[i].myProposition.type << " " << myGraph[i].myProposition.description << " " << myGraph[i].myProposition.detail << " " << myGraph[i].myProposition.segmentPosition << " ";
		for (int j = 0; j < int(myGraph.size()); j++)
		{
			printf("%4.4f  ", myGraph[i].edge[j]);
		}
		printf("\n");
	}
	printf("\n");
}



/* This map populates myInitialWeightMap with all the initial weights from the humam subject
 * experiment. It will be read by the pagerank algorithm when initializing the graph.
 * those weights are the relationship weights between the priority vertex and all the other
 * proposition types (when existent)
 */
void PageRank::instantiateInitialWeightMap()
{
	// priority_vertex
	pair<string,float> aPair0 ("priority_vertex", 0.00f);
	myInitialWeightMap.insert(aPair0);

	// graph_type proposition
	pair<string,float> aPair1 ("graph_type", 0.90f);
	myInitialWeightMap.insert(aPair1);
	
	// entity_description proposition
	pair<string,float> aPair2 ("entity_description", 0.90f);
	myInitialWeightMap.insert(aPair2);

	// graph_volatility proposition
	pair<string,float> aPair33 ("graph_volatility", 0.5f);
	myInitialWeightMap.insert(aPair33);

	// maximum_point_value proposition
	pair<string,float> aPair3 ("maximum_point_value", 0.29f);
	myInitialWeightMap.insert(aPair3);

	// minimum_point_value proposition
	pair<string,float> aPair4 ("minimum_point_value", 0.12f);
	myInitialWeightMap.insert(aPair4);

	// composed_trend proposition
	pair<string,float> aPair5 ("composed_trend", 1.00f);
	myInitialWeightMap.insert(aPair5);
	
	// comptrend_composition proposition
	//pair<string,float> aPair6 ("comptrend_composition", 0.00f);
	//myInitialWeightMap.insert(aPair6);

	// trend_description proposition
	pair<string,float> aPair7 ("trend_description", 0.9f);
	myInitialWeightMap.insert(aPair7);

	// volatility proposition
	pair<string,float> aPair8 ("volatility", 0.5f);
	myInitialWeightMap.insert(aPair8);

	// steepness proposition
	pair<string,float> aPair9 ("steepness", 0.18f);
	myInitialWeightMap.insert(aPair9);
	
	// trend_initial_date proposition
	pair<string,float> aPair10 ("trend_initial_date", 0.66f);
	myInitialWeightMap.insert(aPair10);

	// trend_end_date proposition
	pair<string,float> aPair11 ("trend_end_date", 0.55f);
	myInitialWeightMap.insert(aPair11);

	// trend_initial_value proposition
	pair<string,float> aPair12 ("trend_initial_value", 0.51f);
	myInitialWeightMap.insert(aPair12);

	// trend_end_value proposition
	pair<string,float> aPair13 ("trend_end_value", 0.48f);
	myInitialWeightMap.insert(aPair13);
	
	// trend_rate_change proposition
	pair<string,float> aPair14 ("trend_rate_change", 0.36f);
	myInitialWeightMap.insert(aPair14);

	// trend_absolute_change proposition
	pair<string,float> aPair15 ("trend_absolute_change", 0.36f);
	myInitialWeightMap.insert(aPair15);

	// trend_overall_period_years proposition
	pair<string,float> aPair16 ("trend_overall_period_years", 0.11f);
	myInitialWeightMap.insert(aPair16);

	// maximum_point_date proposition
	pair<string,float> aPair17 ("maximum_point_date", 0.29f);
	myInitialWeightMap.insert(aPair17);

	// minimum_point_date proposition
	pair<string,float> aPair18 ("minimum_point_date", 0.12f);
	myInitialWeightMap.insert(aPair18);

	//Proposition added in the newest version

	// graph_overall_behaviour proposition
	pair<string,float> aPair19 ("graph_overall_behaviour", 0.50f);
	myInitialWeightMap.insert(aPair19);

	// graph_absolute_change proposition
	pair<string,float> aPair20 ("graph_absolute_change", 0.50f);
	myInitialWeightMap.insert(aPair20);

	// graph_rate_change proposition
	pair<string,float> aPair21 ("graph_rate_change", 0.50f);
	myInitialWeightMap.insert(aPair21);

	// graph_overall_period_years proposition
	pair<string,float> aPair23 ("graph_overall_period_years", 0.50f);
	myInitialWeightMap.insert(aPair23);

	// graph_overall_period_months proposition
	pair<string,float> aPair24 ("graph_overall_period_months", 0.50f);
	myInitialWeightMap.insert(aPair24);

	// graph_overall_period_days proposition
	pair<string,float> aPair25 ("graph_overall_period_days", 0.50f);
	myInitialWeightMap.insert(aPair25);

	// graph_initial_date proposition
	pair<string,float> aPair26 ("graph_initial_date", 0.50f);
	myInitialWeightMap.insert(aPair26);

	// graph_end_date proposition
	pair<string,float> aPair27 ("graph_end_date", 0.50f);
	myInitialWeightMap.insert(aPair27);

	// graph_initial_value proposition
	pair<string,float> aPair28 ("graph_initial_value", 0.50f);
	myInitialWeightMap.insert(aPair28);

	// graph_end_value proposition
	pair<string,float> aPair29 ("graph_end_value", 0.50f);
	myInitialWeightMap.insert(aPair29);

	// trend_overall_period_months proposition
	pair<string,float> aPair31 ("trend_overall_period_months", 0.11f);
	myInitialWeightMap.insert(aPair31);

	// trend_overall_period_days proposition
	pair<string,float> aPair32 ("trend_overall_period_days", 0.11f);
	myInitialWeightMap.insert(aPair32);
}



/* This map populates myRelationWeightMap with all the proposition pair followed by their 
 * respective relationship type. Current types of relationships are: 
 *	- complementRelation: (additional information regarding a proposition -- Rising Volatile Trend)
 *  - belongsToRelation : (all the detail information about a Composed Trend belongs to it)
 *  - contrastRelation  : (Initial and End Points of a trend)
 *	- redundancyRelation: (work as a repeller assigning a low weight to the edge)
 */
void PageRank::instantiateRelationWeightMap()
{
	/* Complement relations (current weight set to 0.6) */

	/* !!! Testing to add this relation to see if talking about a trend would implicitly making it talk about
	the other one */
	// trend_description - trend_description
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_description")), "complementRelation"));

	// graph_type - entity_description
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "entity_description")), "complementRelation"));

	// graph_type - graph_volatility
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_volatility")), "complementRelation"));

	// graph_type - composed_trend
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "composed_trend")), "complementRelation"));

	// graph_type - maximum_point_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "maximum_point_value")), "complementRelation"));

	// graph_type - minimum_point_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "minimum_point_value")), "complementRelation"));

	// graph_type - maximum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "maximum_point_date")), "complementRelation"));

	// graph_type - minimum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "minimum_point_date")), "complementRelation"));

	// graph_type - graph_overall_behaviour pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_overall_behaviour")), "complementRelation"));

	// graph_type - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_absolute_change")), "complementRelation"));

	// graph_type - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_rate_change")), "complementRelation"));

	// graph_type - graph_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_overall_period_years")), "complementRelation"));

	// graph_type - graph_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_overall_period_months")), "complementRelation"));

	// graph_type - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_overall_period_days")), "complementRelation"));

	// graph_type - graph_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_initial_date")), "complementRelation"));

	// graph_type - graph_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_end_date")), "complementRelation"));

	// graph_type - graph_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_initial_value")), "complementRelation"));

	// graph_type - graph_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "graph_end_value")), "complementRelation"));

	// graph_overall_behaviour - graph_volatility
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_behaviour", "graph_volatility")), "complementRelation"));

	// graph_overall_behaviour - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_behaviour", "graph_absolute_change")), "complementRelation"));

	// graph_overall_behaviour - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_volatility", "graph_rate_change")), "complementRelation"));

	// graph_volatility - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_volatility", "graph_absolute_change")), "complementRelation"));

	// graph_volatility - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_behaviour", "graph_rate_change")), "complementRelation"));

	// graph_initial_date - graph_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_initial_date", "graph_overall_period_years")), "complementRelation"));

	// graph_initial_date - graph_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_initial_date", "graph_overall_period_months")), "complementRelation"));

	// graph_initial_date - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_initial_date", "graph_overall_period_days")), "complementRelation"));

	// graph_initial_value - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_initial_value", "graph_absolute_change")), "complementRelation"));

	// graph_initial_value - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_initial_value", "graph_rate_change")), "complementRelation"));

	// entity_description - maximum_point_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "maximum_point_value")), "complementRelation"));

	// entity_description - minimum_point_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "minimum_point_value")), "complementRelation"));

	// entity_description - maximum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "maximum_point_date")), "complementRelation"));

	// entity_description - minimum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "minimum_point_date")), "complementRelation"));

	// entity_description - composed_trend pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "composed_trend")), "complementRelation"));

	// entity_description - graph_volatility
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_volatility")), "complementRelation"));

	// entity_description - graph_overall_behaviour pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_overall_behaviour")), "complementRelation"));

	// entity_description - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_absolute_change")), "complementRelation"));

	// entity_description - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_rate_change")), "complementRelation"));

	// entity_description - graph_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_overall_period_years")), "complementRelation"));

	// entity_description - graph_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_overall_period_months")), "complementRelation"));

	// entity_description - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_overall_period_days")), "complementRelation"));

	// entity_description - graph_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_initial_date")), "complementRelation"));

	// entity_description - graph_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_end_date")), "complementRelation"));

	// entity_description - graph_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_initial_value")), "complementRelation"));

	// entity_description - graph_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "graph_end_value")), "complementRelation"));

	// entity_description - trend_description pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_description")), "complementRelation"));

	// entity_description - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "volatility")), "complementRelation"));

	// entity_description - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "steepness")), "complementRelation"));

	// entity_description - trend_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_initial_date")), "complementRelation"));

	// entity_description - trend_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_end_date")), "complementRelation"));

	// entity_description - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_initial_value")), "complementRelation"));

	// entity_description - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_end_value")), "complementRelation"));

	// entity_description - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_absolute_change")), "complementRelation"));

	// entity_description - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_rate_change")), "complementRelation"));

	// entity_description - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_overall_period_years")), "complementRelation"));
	
	// entity_description - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_overall_period_months")), "complementRelation"));
	
	// entity_description - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("entity_description", "trend_overall_period_days")), "complementRelation"));
	
	// composed_trend - graph_volatility
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "graph_volatility")), "complementRelation"));
	
	// trend_description - graph_volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "graph_volatility")), "complementRelation"));

	// trend_description - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "volatility")), "complementRelation"));

	// trend_description - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "steepness")), "complementRelation"));

	// trend_description - trend_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_initial_date")), "complementRelation"));

	// trend_description - trend_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_end_date")), "complementRelation"));

	// trend_description - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_initial_value")), "complementRelation"));

	// trend_description - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_end_value")), "complementRelation"));

	// trend_description - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_absolute_change")), "complementRelation"));

	// trend_description - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_rate_change")), "complementRelation"));

	// trend_description - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_overall_period_years")), "complementRelation"));

	// trend_description - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_overall_period_months")), "complementRelation"));
	
	// trend_description - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_description", "trend_overall_period_days")), "complementRelation"));
	
	// trend_initial_date - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_initial_value")), "complementRelation"));

	// trend_end_date - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_end_value")), "complementRelation"));

	// trend_initial_date - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_overall_period_years")), "complementRelation"));

	// trend_initial_date - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_overall_period_months")), "complementRelation"));

	// trend_initial_date - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_overall_period_days")), "complementRelation"));

	// trend_initial_date - trend_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_end_date")), "complementRelation"));

	// trend_initial_value - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_absolute_change")), "complementRelation"));

	// trend_initial_value - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_rate_change")), "complementRelation"));

	// trend_initial_value - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_end_value")), "complementRelation"));

	// maximum_point_value - maximum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("maximum_point_value", "maximum_point_date")), "complementRelation"));

	// minimum_point_value - minimum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("minimum_point_value", "minimum_point_date")), "complementRelation"));

	// volatility - steepness
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("volatility", "steepness")), "complementRelation"));

	/* Contrast relations (current weight set to 0.4) */

	// maximum_point_value - minimum_point_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("maximum_point_value", "minimum_point_value")), "contrastRelation"));

	// maximum_point_date - minimum_point_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("maximum_point_date", "minimum_point_date")), "contrastRelation"));

	// trend_end_value - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "trend_absolute_change")), "contrastRelation"));

	// trend_end_value - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "trend_rate_change")), "contrastRelation"));

	// trend_end_date - trend_overall_period_years pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_years")), "contrastRelation"));

	// trend_end_date - trend_overall_period_months pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_months")), "contrastRelation"));

	// trend_end_date - trend_overall_period_days pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_days")), "contrastRelation"));

	/* Redundancy relations (current weight set to 0.2) */

	// trend_absolute_change - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_absolute_change", "trend_rate_change")), "redundancyRelation"));

	// graph_absolute_change - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_absolute_change", "graph_rate_change")), "redundancyRelation"));

	// trend_overall_period_years - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_overall_period_years", "trend_overall_period_months")), "redundancyRelation"));

	// trend_overall_period_years - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_overall_period_years", "trend_overall_period_days")), "redundancyRelation"));

	// trend_overall_period_months - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_overall_period_months", "trend_overall_period_days")), "redundancyRelation"));

	// trend_end_date - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_years")), "redundancyRelation"));

	// trend_end_date - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_months")), "redundancyRelation"));

	// trend_end_date - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_days")), "redundancyRelation"));

	// trend_end_value - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_absolute_change")), "redundancyRelation"));

	// trend_end_value - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_rate_change")), "redundancyRelation"));

	// graph_overall_period_years - graph_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_period_years", "graph_overall_period_months")), "redundancyRelation"));

	// graph_overall_period_years - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_period_years", "graph_overall_period_days")), "redundancyRelation"));

	// graph_overall_period_months - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_overall_period_months", "graph_overall_period_days")), "redundancyRelation"));

	// graph_end_date - graph_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_end_date", "graph_overall_period_years")), "redundancyRelation"));

	// graph_end_date - graph_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_end_date", "graph_overall_period_months")), "redundancyRelation"));

	// graph_end_date - graph_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_end_date", "graph_overall_period_days")), "redundancyRelation"));

	// graph_end_value - graph_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_end_value", "graph_absolute_change")), "redundancyRelation"));

	// graph_end_value - graph_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_end_value", "graph_rate_change")), "redundancyRelation"));

	// graph_volatility - volatility pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_volatility", "volatility")), "redundancyRelation"));

	/* Added 2/25/2013 - check if it will work. It is needed because the intention already brings the dates on it */
	// composed_trend - graph_initial_date
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "graph_initial_date")), "redundancyRelation"));

	// composed_trend - graph_end_date
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "graph_end_date")), "redundancyRelation"));

	// composed_trend - trend_initial_date
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_initial_date")), "redundancyRelation"));

	// composed_trend - trend_end_date
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_end_date")), "redundancyRelation"));

	// composed_trend - trend_overall_period_years
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_years")), "redundancyRelation"));

	// composed_trend - trend_overall_period_months
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_months")), "redundancyRelation"));

	// composed_trend - trend_overall_period_days
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_days")), "redundancyRelation"));

	/* BelongsTo relation (current weight set to the same as complementRelation) */

	// graph_type - trend_description pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_description")), "belongsToRelation"));

	// graph_type - volatility pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "volatility")), "belongsToRelation"));

	// graph_type - steepness pair
	//myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "steepness")), "belongsToRelation"));

	// graph_type - trend_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_initial_date")), "belongsToRelation"));

	// graph_type - trend_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_end_date")), "belongsToRelation"));

	// graph_type - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_initial_value")), "belongsToRelation"));

	// graph_type - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_end_value")), "belongsToRelation"));

	// graph_type - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_absolute_change")), "belongsToRelation"));

	// graph_type - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_rate_change")), "belongsToRelation"));

	// graph_type - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_overall_period_years")), "belongsToRelation"));
	
	// graph_type - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_overall_period_months")), "belongsToRelation"));
	
	// graph_type - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("graph_type", "trend_overall_period_days")), "belongsToRelation"));
	
	// composed_trend - trend_description pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_description")), "belongsToRelation"));

	// composed_trend - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "volatiltiy")), "belongsToRelation"));

	// composed_trend - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "steepness")), "belongsToRelation"));

	// composed_trend - trend_initial_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_initial_date")), "belongsToRelation"));

	// composed_trend - trend_end_date pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_end_date")), "belongsToRelation"));

	// composed_trend - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_initial_value")), "belongsToRelation"));

	// composed_trend - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_end_value")), "belongsToRelation"));

	// composed_trend - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_absolute_change")), "belongsToRelation"));

	// composed_trend - trend_rate_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_rate_change")), "belongsToRelation"));

	// composed_trend - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_years")), "belongsToRelation"));

	// composed_trend - trend_overall_period_months pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_months")), "belongsToRelation"));

	// composed_trend - trend_overall_period_days pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("composed_trend", "trend_overall_period_days")), "belongsToRelation"));

	// ----- Extra relation for connecting the features withing a trend (after 10/15/12 meeting) - connectivity problem ------
	// sameMembershipRelation (0.1 weight) -- also trying with complementRelation
/*
	// trend_initial_date - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "volatility")), "complementRelation"));

	// trend_initial_date - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "steepness")), "complementRelation"));

	// trend_initial_date - trend_end_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_end_value")), "complementRelation"));

	// trend_initial_date - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_percentual_change")), "complementRelation"));
	
	// trend_initial_date - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_absolute_change")), "complementRelation"));

	// trend_initial_date - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_date", "trend_overall_period_years")), "complementRelation"));
	
	// trend_end_date - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "volatility")), "complementRelation"));

	// trend_end_date - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "steepness")), "complementRelation"));

	// trend_end_date - trend_initial_value pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_initial_value")), "complementRelation"));

	// trend_end_date - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_percentual_change")), "complementRelation"));
	
	// trend_end_date - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_absolute_change")), "complementRelation"));

	// trend_end_date - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_date", "trend_overall_period_years")), "complementRelation"));

	// trend_initial_value - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "volatility")), "complementRelation"));

	// trend_initial_value - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "steepness")), "complementRelation"));

	// trend_initial_value - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_percentual_change")), "complementRelation"));
	
	// trend_initial_value - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_absolute_change")), "complementRelation"));

	// trend_initial_value - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_initial_value", "trend_overall_period_years")), "complementRelation"));

	// trend_end_value - volatility pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "volatility")), "complementRelation"));

	// trend_end_value - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "steepness")), "complementRelation"));

	// trend_end_value - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "trend_percentual_change")), "complementRelation"));
	
	// trend_end_value - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "trend_absolute_change")), "complementRelation"));

	// trend_end_value - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_end_value", "trend_overall_period_years")), "complementRelation"));

	// volatility - steepness pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("volatility", "steepness")), "complementRelation"));

	// volatility - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("volatility", "trend_percentual_change")), "complementRelation"));
	
	// volatility - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("volatility", "trend_absolute_change")), "complementRelation"));

	// volatility - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("volatility", "trend_overall_period_years")), "complementRelation"));

	// steepness - trend_percentual_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("steepness", "trend_percentual_change")), "complementRelation"));
	
	// steepness - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("steepness", "trend_absolute_change")), "complementRelation"));

	// steepness - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("steepness", "trend_overall_period_years")), "complementRelation"));

	// trend_percentual_change - trend_absolute_change pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_percentual_change", "trend_absolute_change")), "complementRelation"));

	// trend_percentual_change - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_percentual_change", "trend_overall_period_years")), "complementRelation"));

	// trend_absolute_change - trend_overall_period_years pair
	myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> ("trend_absolute_change", "trend_overall_period_years")), "complementRelation"));
*/
}



/*  Initial weights and relationship weights are stored on the maps myInitialWeightMap
 *  and myRelationWeightMap and are either provided by the relationship between propositions
 *  or by the initial importance of conveying a proposition based on an humam subject experiment.
 *  Certain types of propositions such as volatility, for example, have a initial weight 
 *  assigned by the importance given by the humam subjects which is boosted by the calculation of
 *  the volatility metric (the more volatile it is a trend, more probable to talk about it).
 *  Notice: The volatility and steepness metrics calculation will also define the degree of them,
 *  changing the proposition to convey that a Trend is "slightly volatile" or "highly volatile"
 */
void PageRank::instantiateGraph(PGconn *conn, PGresult *res, PGresult *resOrdTrend, int graphSize, PGresult *resAnnotationOnGraphic, char *linegraphKeyTemp)
{
	PGresult *resTrends;
	string currentComposedTrend, intention;
	int trendException = 0;
	int tempYear1 = 0;
	int tempYear2 = 0;
	int tempMonth1 = 0;
	int tempMonth2 = 0;
	int tempDay1 = 0;
	int tempDay2 = 0;
	int tempDateDiffYear = 0;
	int tempDateDiffMonth = 0;
	int tempDateDiffDay = 0;
	// these variables are controlling the number of rising, falling, etc
	int numberOfRT = 1;
	int numberOfFT = 1;
	int numberOfST = 1;
	int numberOfBJ = 1;
	int numberOfBF = 1;
	stringstream result, tempResult, tempDateResult, sYears, sTempValue (stringstream::in | stringstream::out);
	int currentNumberOfTrends = 0;
	// the following two variables will take care of turning trend_description of Rising, Falling and Stable
	// that are from unique simple intentions RT, FT and ST respectively to NOT be conveyed
	int numberOfSuggestions = 0;
	bool simpleIntention = false;
	int numberOfTuples = PQntuples(res);
	float *myEdge;
	int graph_it = 0;
	double initialGraphValue = atof(PQgetvalue(res, 0, 27));
	double endGraphValue = atof(PQgetvalue(res, 0, 28));
	double initialTrendValue;
	double endTrendValue;
	double tempValue;
	proposition aProposition;
	dateDetail myDateDetail;
	string initialDate, endDate, scaleUnit, tempAnnotationText;

	//getting date details concerning the whole graph
	initialDate = PQgetvalue(res, 0, 29);
	endDate = PQgetvalue(res, 0, 30);
	myDateDetail = myHelper.getPeriods(initialDate, endDate);

	//getting sclae and unit when they exist and setting it in the detail of all propositions that are about value
	scaleUnit = PQgetvalue(res, 0, 32);		// scale comes first
	if (scaleUnit.size() > 0)
	{
		scaleUnit.append(" ");
		scaleUnit.append(PQgetvalue(res, 0, 31));	// unit comes second (ex: million dollars)
	}
	else
		scaleUnit.append(PQgetvalue(res, 0, 31));	// storing only unit (ex: dollars)

	// Adding PRIORITY_VERTEX node
	aProposition.description		= "priority_vertex";
	aProposition.membership.push_back("line graph");	//I cannot change that without changing the fixed reference regarding composed trends' membership
	aProposition.type				= "priority_vertex";
	aProposition.detail				= "";
	aProposition.conveyable			= false;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= true;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 0;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_type (aProposition.membership is the same)
	aProposition.description		= "line graph";
	aProposition.type				= "graph_type";
	aProposition.detail				= scaleUnit; //stores the scale and unit of the measurement axis descriptor
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;
	//the following vector contains pairs of trends description and position in the graph, ordered by appearance on it
	resTrends = dbconnection.getTrends(conn, linegraphKeyTemp);
	for (int a = 0; a < PQntuples(resTrends); a++)
	{
		aProposition.additionalInfo.push_back(pair<string,string> (PQgetvalue(resTrends, a, 0), PQgetvalue(resTrends, a, 1)));
	}

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 1;						// node is about an entity
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	aProposition.additionalInfo.clear();		// cleaning up vector of trends

	// Adding proposition for entity_description (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 0);  //the measurement axis descriptor extraction is still being implemented
	aProposition.type				= "entity_description";
	aProposition.detail				= PQgetvalue(res, 0, 32); //stores the scale of the measurement axis descriptor
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;						// node is about an entity's feature
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_volatility (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 1);
	aProposition.type				= "graph_volatility";
	aProposition.detail				= PageRank::getVolatilityDegree(PQgetvalue(res, 0, 1));
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;
	aProposition.terms.push_back(pair<string,int>("ups and downs",0));
	aProposition.terms.push_back(pair<string,int>("fluctuation",0));
	aProposition.terms.push_back(pair<string,int>("fluctuation",1));
	aProposition.terms.push_back(pair<string,int>("volatility",1));

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_overall_behaviour (aProposition.membership is the same)
	if (initialGraphValue > endGraphValue)
		aProposition.description = "decrease";	//tells if it increases or decreases
	else if (initialGraphValue < endGraphValue)
		aProposition.description = "increase";
	else
		aProposition.description = "no change";
	aProposition.type				= "graph_overall_behaviour";
	aProposition.detail				= "";			//no idea what to put in here
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;
	aProposition.additionalInfo.push_back(pair<string,string> (aProposition.description, "overall_behaviour"));

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_absolute_change (aProposition.membership is the same)
	result << abs(initialGraphValue - endGraphValue);			//tells how much is the overall change
	if (abs(initialGraphValue - endGraphValue) > 0)
		aProposition.conveyable			= true;
	else
		aProposition.conveyable			= false;
	aProposition.description		= result.str();	
	aProposition.type				= "graph_absolute_change";
	aProposition.detail				= scaleUnit;	//stores the scale and unit of the measurement axis descriptor (percentage, millions);
	aProposition.segmentPosition	= -1;
	//aProposition.additionInfo has same info from graph_overall_behaviour

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	result.str("");
	// Adding proposition for graph_rate_change (aProposition.membership is the same)
	if (initialGraphValue == 0 || endGraphValue == 0)
	{
		result << 0.00;
		aProposition.conveyable			= false;
	}
	else if (initialGraphValue > endGraphValue)
	{
		result << (ceil(abs(initialGraphValue / endGraphValue) * 100)) / 100;
		aProposition.conveyable			= true;
	}
	else if (initialGraphValue < endGraphValue)
	{
		result << (ceil(abs(endGraphValue / initialGraphValue) * 100)) / 100;
		aProposition.conveyable			= true;
	}
	else
	{
		result << 0.00;
		aProposition.conveyable			= false;
	}
	aProposition.description		= result.str();
	aProposition.type				= "graph_rate_change";
	aProposition.detail				= scaleUnit;	//stores the scale and unit of the measurement axis descriptor (percentage, millions);
	aProposition.segmentPosition	= -1;
	//aProposition.additionInfo has same info from graph_overall_behaviour

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	aProposition.additionalInfo.clear(); //cleaning up the info about overall_behaviour of the graph

	// Adding proposition for graph_overall_period_years (aProposition.membership is the same)
	sYears << myDateDetail.numberYears;
	aProposition.description		= sYears.str();	//tell the overall period in years
	aProposition.type				= "graph_overall_period_years";
	aProposition.segmentPosition	= -1;
	if (myDateDetail.numberYears == 0 && (myDateDetail.numberDays > 0 || myDateDetail.numberMonths > 0))
	{
		aProposition.detail				= "";
		aProposition.conveyable			= false;
	}
	else if ((myDateDetail.numberYears > 0) &&
		(myDateDetail.initialMonth.compare("0") == 0) &&
		(myDateDetail.endMonth.compare("0") == 0))
	{
		aProposition.detail				= "year";
		aProposition.conveyable			= true;
	}
	else
	{
		aProposition.detail				= "";
		aProposition.conveyable			= true;
	}

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_overall_period_months (aProposition.membership is the same)
	tempDateDiffYear = myDateDetail.numberYears;
	if (myDateDetail.numberMonths == 0)
		tempDateDiffMonth = tempDateDiffYear * 12;
	else if (myDateDetail.numberMonths > 0)
		tempDateDiffMonth = myDateDetail.numberMonths + tempDateDiffYear * 12;
	else if (myDateDetail.numberMonths < 0)
		tempDateDiffMonth = (tempDateDiffYear * 12) - (12 - myDateDetail.numberMonths);
	else
		tempDateDiffMonth = 0;
	if (tempDateDiffMonth > 0 && tempDateDiffMonth < 60)
	{
		tempDateResult << tempDateDiffMonth;
		aProposition.description = tempDateResult.str();
		aProposition.conveyable	= true;		//only convey it if it has years and the difference is greater than 0
	}
	else if (tempDateDiffMonth > 60)
	{
		tempDateResult << tempDateDiffMonth;
		aProposition.description	= tempDateResult.str();
		aProposition.conveyable		= false;
	}
	else
	{
		aProposition.description	= "";
		aProposition.conveyable		= false;
	}
	aProposition.type				= "graph_overall_period_months";
	aProposition.segmentPosition	= -1;
	if ((myDateDetail.numberMonths > 0) || 
		((myDateDetail.numberDays == 0) && 
		 (myDateDetail.initialMonth.compare("0") != 0)&& 
		 (myDateDetail.endMonth.compare("0") != 0)))
		aProposition.detail				= "month";
	else
		aProposition.detail				= "";

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	tempDateResult.str("");
	// Adding proposition for graph_overall_period_days (aProposition.membership is the same)
	if (myDateDetail.numberDays == 0)
		tempDateDiffDay = tempDateDiffMonth * 30;
	else if (myDateDetail.numberDays > 0)
		tempDateDiffDay = (myDateDetail.numberDays) + tempDateDiffMonth * 30;
	else if (myDateDetail.numberDays < 0)
		tempDateDiffDay = (tempDateDiffMonth * 30) - abs(myDateDetail.numberDays);
	else
		tempDateDiffDay = 0;
	if (tempDateDiffDay > 0 && tempDateDiffDay <= 365) //only convey it if it is greater than zero and less than 365
	{
		tempDateResult << tempDateDiffDay;
		aProposition.description	= tempDateResult.str();
		aProposition.conveyable		= true;		
	}
	else if (tempDateDiffDay > 365)
	{
		tempDateResult << tempDateDiffDay;				//keep number of days anyway
		aProposition.description	= tempDateResult.str();
		aProposition.conveyable		= false;
	}
	else
	{
		aProposition.description	= "";
		aProposition.conveyable		= false;
	}
	aProposition.type				= "graph_overall_period_days";
	aProposition.segmentPosition	= -1;
	if ((myDateDetail.initialDay.compare("0") != 0) && (myDateDetail.endDay.compare("0") != 0))
		aProposition.detail				= "day";
	else
		aProposition.detail				= "";

	
	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_initial_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 29);	//tell the overall initial date
	aProposition.type				= "graph_initial_date";
	aProposition.detail				= "";			//no idea either
	aProposition.segmentPosition	= -1;
	if (aProposition.description.compare("Undefined") == 0)
		aProposition.conveyable			= false;
	else
		aProposition.conveyable			= true;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_end_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 30);	//tell the overall end date
	aProposition.type				= "graph_end_date";
	aProposition.detail				= "";			//no idea either
	aProposition.segmentPosition	= -1;
	if (aProposition.description.compare("Undefined") == 0)
		aProposition.conveyable			= false;
	else
		aProposition.conveyable			= true;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_initial_value (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 27);	//tell the overall initial value
	aProposition.type				= "graph_initial_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
					initialGraphValue = atof(aProposition.description.c_str());
					break;
				}
				else
					continue;
			}
			else
				continue;
		}
	}
	// rounding it to 2 decimal places
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description = sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for graph_end_value (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 28);	//tell the overall end value
	aProposition.type				= "graph_end_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
					endGraphValue = atof(aProposition.description.c_str());
					break;
				}
				else
					continue;
			}
		}
	}
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description = sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
	// Since the initial or end values could have changed due to annotation issues, we need to change the
	// value of the graph_absolute_value node (it is position 5 in the graph)
	tempResult << abs(initialGraphValue - endGraphValue);
	myGraph[5].myProposition.description = tempResult.str();
	tempResult.str("");

	// Adding proposition for maximum_point_value (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 2);
	aProposition.type				= "maximum_point_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
					break;
				}
				else
					continue;
			}
			else
				continue;
		}
	}
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description 	= sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for minimum_point_value (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 3);
	aProposition.type				= "minimum_point_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
					break;
				}
				else
					continue;
			}
			else
				continue;
		}
	}
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description 	= sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for maximum_point_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 4);
	aProposition.type				= "maximum_point_date";
	aProposition.detail				= "";
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for minimum_point_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(res, 0, 5);
	aProposition.type				= "minimum_point_date";
	aProposition.detail				= "";
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= -1;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;


	/* Starting block of insertion of composed trend's propositions */

	currentComposedTrend			= PQgetvalue(res, 0, 7);

	// Adding the first proposition for composed_trend
	aProposition.description		= PQgetvalue(res, 0, 6);		//name of category (CHT, FT, etc)
	aProposition.membership.push_back(currentComposedTrend);		//adding detailed description
	aProposition.type				= "composed_trend";
	aProposition.detail				= PQgetvalue(res, 0, 8);		//probability value from Bayesian Network
	if ((aProposition.description.compare("BF") == 0) || (aProposition.description.compare("BJ") == 0)
		|| (aProposition.description.compare("RT") == 0) || (aProposition.description.compare("FT") == 0)
		|| (aProposition.description.compare("ST") == 0))
		aProposition.segmentPosition	= PQgetvalue(res, 0, 10);
	else
		aProposition.segmentPosition	= -1;
	// increasing the number of suggestions (in the trend description we will only turn it NOT conveyable
	// if we only have one intention and that is either RT, FT, or ST)
	numberOfSuggestions++;
	if ((aProposition.description.compare("RT") == 0) || (aProposition.description.compare("FT") == 0) ||
		(aProposition.description.compare("ST") == 0))
		simpleIntention					= true;
	
	// For composed Trends the additional info vector is storing two elements for each trend. The first is the
	// trend_description together with the trend order number; the second is the initial date together with the 
	// end date of the trend. This information will be used when conveying the intention proposition
	resTrends = dbconnection.getTrends(conn, linegraphKeyTemp, currentComposedTrend);
	for (int b = 0; b < PQntuples(resTrends); b++)
	{
		aProposition.additionalInfo.push_back(pair<string,string> (PQgetvalue(resTrends, b, 0), PQgetvalue(resTrends, b, 1)));
		aProposition.additionalInfo.push_back(pair<string,string> (PQgetvalue(resTrends, b, 2), PQgetvalue(resTrends, b, 3)));
	}
	PQclear(resTrends);
	
	intention = PQgetvalue(res, 0, 29);
	if (intention.compare("true") == 0)
		numberOfIntentionTrends = PQntuples(resTrends);

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 1;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	//aProposition.membership.clear();	// cleaning the vector up
	aProposition.additionalInfo.clear();

	for (int i = 0; i < numberOfTuples; i++)
	{
		// Checks if the composed trend at this row (tuple number) is already inserted
		if (currentComposedTrend.compare(PQgetvalue(res, i, 7)) == 0)
		{
			currentNumberOfTrends++; // this controls the number of trend for comptrend_composition proposition
			//cout << "This is the number of trend: " << currentNumberOfTrends << endl;
		} 
		else
		{
			aProposition.membership.clear();

			// getting current composed trend description
			currentComposedTrend			= PQgetvalue(res, i, 7);
			currentNumberOfTrends			= 1;

			// Adding proposition for composed_trend if the composed trend isn't already inserted
			aProposition.description		= PQgetvalue(res, i, 6);
			aProposition.membership.push_back("line graph");
			aProposition.membership.push_back(currentComposedTrend);
			aProposition.type				= "composed_trend";
			aProposition.detail				= PQgetvalue(res, i, 8);
			if ((aProposition.description.compare("BF") == 0) || (aProposition.description.compare("BJ") == 0)
				|| (aProposition.description.compare("RT") == 0) || (aProposition.description.compare("FT") == 0)
				|| (aProposition.description.compare("ST") == 0))
				aProposition.segmentPosition	= PQgetvalue(res, i, 10);
			else
				aProposition.segmentPosition	= -1;
			numberOfSuggestions++;
			simpleIntention = false;
			
			resTrends = dbconnection.getTrends(conn, linegraphKeyTemp, currentComposedTrend);
			for (int c = 0; c < PQntuples(resTrends); c++)
			{
				aProposition.additionalInfo.push_back(pair<string,string> (PQgetvalue(resTrends, c, 0), PQgetvalue(resTrends, c, 1)));
				aProposition.additionalInfo.push_back(pair<string,string> (PQgetvalue(resTrends, c, 2), PQgetvalue(resTrends, c, 3)));
			}
			PQclear(resTrends);

			intention = PQgetvalue(res, i, 29);
			if (intention.compare("true") == 0)
				numberOfIntentionTrends = PQntuples(resTrends);


			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 1;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;
			
			aProposition.additionalInfo.clear();
		}
		
	}

	aProposition.membership.clear();

	char trendPosition [1];
	// Getting trend information from first tuple (from original query results, ordered by composed trend first
	string currentTrend = PQgetvalue(resOrdTrend, 0, 9);
	currentTrend.append(PQgetvalue(resOrdTrend, 0, 10));
	currentComposedTrend = PQgetvalue(resOrdTrend, 0, 7);
	// trendNameMap is being used by the organization module, since the trend might change its name from rising
	// trend to second rising trend, for example
	trendNameMap.insert(pair<string,string>(currentTrend,PQgetvalue(resOrdTrend, 0, 9)));
	
	// Adding proposition for first trend_description 
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 9);
	aProposition.type				= "trend_description";
	aProposition.membership.push_back(currentComposedTrend);    // adds composed_trend to the membership vector
	aProposition.membership.push_back(currentTrend);			// adds trend_description to the membership vector		
	aProposition.detail				= currentTrend;				// stores unique identifier of trend
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);

	//increasing the value of variable that stores the total number of trend in the graph
	totalNumberOfTrends++;

	//getting exceptions in trend behavior
	//trendException = dbconnection.getExceptionInTrend(conn, linegraphKeyTemp, PQgetvalue(resOrdTrend, 0, 9), PQgetvalue(resOrdTrend, 0, 10));
	//cout << "Number of exceptions: " << trendException << " for trend " << PQgetvalue(resOrdTrend, 0, 9) << endl;
	//if (trendException > 0)
	//	aProposition.additionalInfo.push_back(pair<string,string>("exception", "exception"));

	// a trend_description proposition will NOT be conveyable if it is a Rising, Falling, or Stable trend
	// and it belongs to a simple and unique suggestion (RT, FT, or ST)
	if (((aProposition.description.compare("rising trend") == 0) || 
		 (aProposition.description.compare("falling trend") == 0) ||
		 (aProposition.description.compare("stable trend") == 0)) &&
		(numberOfSuggestions == 1) && (simpleIntention))
		aProposition.conveyable = false;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 1;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	//aProposition.additionalInfo.clear();  // cleaning up the vector of exceptions in a trend

	// Adding proposition for volatility (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 11);
	aProposition.type				= "volatility";
	//aProposition.membership.push_back(currentTrend);						// adds trend_description to the membership vector
	aProposition.detail				= PageRank::getVolatilityDegree(PQgetvalue(resOrdTrend, 0, 11));
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	aProposition.terms.push_back(pair<string,int>("ups and downs",0));
	aProposition.terms.push_back(pair<string,int>("peaks and valeys",0));
	aProposition.terms.push_back(pair<string,int>("fluctuation",1));
	aProposition.terms.push_back(pair<string,int>("volatility",1));

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for steepness (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 12);
	aProposition.type				= "steepness";
	aProposition.detail				= PageRank::getSteepnessDegree(PQgetvalue(resOrdTrend, 0, 12), PQgetvalue(resOrdTrend, 0, 11));	// calling function that returns the degree ("slight", "steep")
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	aProposition.terms.push_back(pair<string,int>("sharp",0));
	aProposition.terms.push_back(pair<string,int>("steep",1));

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for trend_initial_value (aProposition.membership is the same)
	initialTrendValue = atof(PQgetvalue(resOrdTrend, 0, 13));
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 13);
	aProposition.type				= "trend_initial_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
				//	cout << "Comparing: " << PQgetvalue(resAnnotationOnGraphic, i, 3) << endl;
				//	cout << "With the value before: " << aProposition.description << endl;
				//	cout << "Content of annotation field: " << PQgetvalue(resAnnotationOnGraphic, i, 2) << endl;
				//	cout << "Value before: " << aProposition.description << endl;
				//	cout << "Got annotated trend_initial_value" << endl;
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
				//	cout << "Value: " << aProposition.description << endl;
					initialTrendValue = atof(aProposition.description.c_str());
					break;
				}
				else
					continue;
			}
			else 
				continue;
		}
	}
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description = sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	
	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for trend_end_value (aProposition.membership is the same)
	endTrendValue = atof(PQgetvalue(resOrdTrend, 0, 14));
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 14);
	aProposition.type				= "trend_end_value";
	aProposition.detail				= "";
	if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
	{
		for (int i = 0; i < PQntuples(resAnnotationOnGraphic); i++)
		{
			if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, i, 3)) == 0)
			{
				tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, i, 2);
				if (tempAnnotationText.size() > 0)
				{
				//	cout << "Comparing: " << PQgetvalue(resAnnotationOnGraphic, i, 3) << endl;
				//	cout << "With the value before: " << aProposition.description << endl;
				//	cout << "Content of annotation field: " << PQgetvalue(resAnnotationOnGraphic, i, 2) << endl;
				//	cout << "Value before: " << aProposition.description << endl;
				//	cout << "Got annotated trend_end_value" << endl;
					aProposition.detail	= "annotated";
					aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, i, 2));
				//	cout << "Value: " << aProposition.description << endl;
					endTrendValue = atof(aProposition.description.c_str());
					break;
				}
				else
					continue;
			}
			else
				continue;
		}
	}
	if (aProposition.detail.compare("annotated") != 0)
	{
		tempValue = atof(aProposition.description.c_str());
		tempValue = tempValue * 100;
		tempValue = ceil(tempValue);
		tempValue = tempValue / 100;
		sTempValue << tempValue;
		aProposition.description 	= sTempValue.str();
		sTempValue.str("");
	}
	aProposition.conveyable			= true;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for trend_initial_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 15);
	aProposition.type				= "trend_initial_date";
	aProposition.detail				= PQgetvalue(resOrdTrend, 0, 15);		// TODO: maybe store the date type (100, 110, 111)
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	if (aProposition.description.compare("Undefined") == 0)
		aProposition.conveyable			= false;
	else
		aProposition.conveyable			= true;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	// Adding proposition for trend_end_date (aProposition.membership is the same)
	aProposition.description		= PQgetvalue(resOrdTrend, 0, 16);
	aProposition.type				= "trend_end_date";
	aProposition.detail				= PQgetvalue(resOrdTrend, 0, 16);		// TODO: maybe store the date type (100, 110, 111)
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	if (aProposition.description.compare("Undefined") == 0)
		aProposition.conveyable			= false;
	else
		aProposition.conveyable			= true;

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	result.str("");
	tempResult.str("");
	// Adding proposition for trend_absolute_change (aProposition.membership is the same)
	tempResult << abs(initialTrendValue - endTrendValue);
	if (initialTrendValue > endTrendValue)
	{
		aProposition.additionalInfo.push_back(pair<string,string> ("decrease", "trend_behaviour"));
		aProposition.conveyable			= true;
	}
	else if (initialTrendValue < endTrendValue)
	{
		aProposition.additionalInfo.push_back(pair<string,string> ("increase", "trend_behaviour"));
		aProposition.conveyable			= true;
	}
	else
	{
		aProposition.additionalInfo.push_back(pair<string,string> ("no change", "trend_behaviour"));
		aProposition.conveyable			= false;
	}
	aProposition.description		= tempResult.str();
	aProposition.type				= "trend_absolute_change";
	aProposition.detail				= scaleUnit; // storing the measurement axis unit for now
	//aProposition.conveyable			= true;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	result.str("");
	tempResult.str("");
	// Adding proposition for trend_rate_change (aProposition.membership is the same)
	if (initialTrendValue == 0 || endGraphValue == 0)
	{
		result << 0.00;
		aProposition.conveyable			= false;
	}
	else if (initialTrendValue > endTrendValue)
	{
		result << (ceil(abs(initialTrendValue / endTrendValue) * 100));
		aProposition.conveyable			= true;
	}
	else if (initialTrendValue < endTrendValue)
	{
		result << (ceil(abs(endTrendValue / initialTrendValue) * 100));
		aProposition.conveyable			= true;
	}
	else
	{
		result << 0.00;
		aProposition.conveyable			= false;
	}	
	aProposition.description		= result.str();
	aProposition.type				= "trend_rate_change";
	aProposition.detail				= scaleUnit;
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	// aProposition.additionalInfo will keep the same info from trend_absolute_change

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;
	aProposition.additionalInfo.clear();	//cleaning up info about trend_behaviour (increase, decrease, etc)

	tempDateResult.str("");
	// Adding proposition for trend_overall_period_years (aProposition.membership is the same)
	tempYear1 = atoi(PQgetvalue(resOrdTrend, 0, 19));
	tempYear2 = atoi(PQgetvalue(resOrdTrend, 0, 20));
	tempMonth1 = atoi(PQgetvalue(resOrdTrend, 0, 21));
	tempMonth2 = atoi(PQgetvalue(resOrdTrend, 0, 22));
	tempDay1 = atoi(PQgetvalue(resOrdTrend, 0, 23));
	tempDay2 = atoi(PQgetvalue(resOrdTrend, 0, 24));
	if ((tempYear1 > 0) && (tempYear2 > 0))
	{
		tempDateDiffYear = tempYear2 - tempYear1;
		tempDateResult << tempDateDiffYear;
		aProposition.description = tempDateResult.str();
		if (tempDateDiffYear > 0)
			aProposition.conveyable	= true;		//only convey it if it has years and the difference is greater than 0
		else
			aProposition.conveyable = false;
	}
	else
	{
		aProposition.description	= "";
		aProposition.conveyable		= false;
	}
	aProposition.type				= "trend_overall_period_years";
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	if ((tempMonth1 == 0) && (tempMonth2 == 0))
		aProposition.detail			= "year";
	else
		aProposition.detail			= "";

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	tempDateResult.str("");
	// Adding proposition for trend_overall_period_months (aProposition.membership is the same)
	if ((tempMonth1 == 0) && (tempMonth2 == 0))
		tempDateDiffMonth = tempDateDiffYear * 12;
	else if ((tempMonth1 > 0) && (tempMonth2 > 0) && (tempMonth2 > tempMonth1))
		tempDateDiffMonth = (tempMonth2 - tempMonth1) + tempDateDiffYear * 12;
	else if ((tempMonth1 > 0) && (tempMonth2 > 0) && (tempMonth2 < tempMonth1))
		tempDateDiffMonth = (tempDateDiffYear * 12) - (tempMonth1 - tempMonth2);
	else
		tempDateDiffMonth = 0;
	if (tempDateDiffMonth > 0 && tempDateDiffMonth < 60)
	{
		tempDateResult << tempDateDiffMonth;
		aProposition.description = tempDateResult.str();
		aProposition.conveyable	= true;		//only convey it if it has years and the difference is greater than 0
	}
	else
	{
		aProposition.description	= "";
		aProposition.conveyable		= false;
	}
	aProposition.type				= "trend_overall_period_months";
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	if(((tempMonth1 > 0) || (tempMonth2 > 0)) &&
		((tempDay1 <= 1) && (tempDay2 <= 1)))
		aProposition.detail				= "month";
	else
		aProposition.detail				= "";

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	tempDateResult.str("");
	// Adding proposition for trend_overall_period_days (aProposition.membership is the same)
	if (((tempDay1 == 0) && (tempDay2 == 0)) || (tempDay1 == tempDay2))
		tempDateDiffDay = tempDateDiffMonth * 30;
	else if ((tempDay1 > 0) && (tempDay2 > 0) && (tempDay2 > tempDay1))
		tempDateDiffDay = (tempDay2 - tempDay1) + tempDateDiffMonth * 30;
	else if ((tempDay1 > 0) && (tempDay2 > 0) && (tempDay2 < tempDay1))
		tempDateDiffDay = (tempDateDiffMonth * 30) - (tempDay1 - tempDay2);
	else
		tempDateDiffDay = 0;
	if (tempDateDiffDay > 0 && tempDateDiffDay <= 365) //only convey it if it is greater than zero and less than 365
	{
		tempDateResult << tempDateDiffDay;
		aProposition.description	= tempDateResult.str();
		aProposition.conveyable		= true;		
	}
	else if (tempDateDiffDay > 365)
	{
		tempDateResult << tempDateDiffDay;				//keep number of days anyway
		aProposition.description	= tempDateResult.str();
		aProposition.conveyable		= false;
	}
	else
	{
		aProposition.description	= "";
		aProposition.conveyable		= false;
	}
	aProposition.type				= "trend_overall_period_days";
	aProposition.segmentPosition	= PQgetvalue(resOrdTrend, 0, 10);
	if ((tempDay1 > 1) && (tempDay2 > 1))
		aProposition.detail				= "day";
	else
		aProposition.detail				= "";

	myGraph[graph_it].myProposition = aProposition;
	myGraph[graph_it].selected		= false;
	myGraph[graph_it].degree		= 0;
	myGraph[graph_it].conceptRole	= 2;
	myEdge							= new float [graphSize];
	myGraph[graph_it].edge			= myEdge;
	graph_it++;

	aProposition.membership.clear();
	for (int i = 0; i < numberOfTuples; i++)
	{
		/*  currentTrend will be used as the membership for all the trend children propositions 
		 *	[not like that right now!!! (which will also carry the membership information regarding the composed trends which
		 *	their trend-parent belongs to)]
		 */
		
		string tempCurrentTrend = PQgetvalue(resOrdTrend, i, 9);
		tempCurrentTrend.append(PQgetvalue(resOrdTrend, i, 10));
		string tempCurrentComposedTrend = PQgetvalue(resOrdTrend, i, 7);

		// Checks if the trend at this row (tuple number) is already inserted
		if (currentTrend.compare(tempCurrentTrend) == 0)
		{
			// calling method that will update the membership vector of the trend
			PageRank::updateMembership(tempCurrentComposedTrend, tempCurrentTrend);
			//myGraph[i].myProposition.membership.push_back(composedTrend);
			//cout << "This trend was already inserted" << endl;
		} 
		else
		{
			currentTrend = PQgetvalue(resOrdTrend, i, 9);
			currentTrend.append(PQgetvalue(resOrdTrend, i, 10));
			currentComposedTrend = PQgetvalue(resOrdTrend, i, 7);
			trendNameMap.insert(pair<string,string>(currentTrend,PQgetvalue(resOrdTrend, i, 9)));

			// Adding proposition for new trend_description
			aProposition.description = PQgetvalue(resOrdTrend, i, 9);
			aProposition.membership.push_back(tempCurrentComposedTrend);	// add the composedTrend to the vector
			aProposition.membership.push_back(tempCurrentTrend);			// add trend_description to vector
			aProposition.type = "trend_description";
			aProposition.detail = tempCurrentTrend;							// stores unique identifier of trend
			aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);

			//increasing the value of variable that stores the total number of trend in the graph
			totalNumberOfTrends++;

			//getting exceptions in trend behavior
			//trendException = 0;
			//trendException = dbconnection.getExceptionInTrend(conn, linegraphKeyTemp, PQgetvalue(resOrdTrend, i, 9), PQgetvalue(resOrdTrend, i, 10));
			//cout << "Number of exceptions: " << trendException << "for trend " << PQgetvalue(resOrdTrend, i, 9) << endl;
			//if (trendException > 0)
			//	aProposition.additionalInfo.push_back(pair<string,string>("exception", "exception"));

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 1;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;
		
			//aProposition.additionalInfo.clear();

			// Adding proposition for volatility (aProposition.membership is the same)
			aProposition.description = PQgetvalue(resOrdTrend, i, 11);
			aProposition.type = "volatility";
			//aProposition.membership.push_back(tempCurrentTrend);			// add trend_description to vector
			aProposition.detail = PageRank::getVolatilityDegree(PQgetvalue(resOrdTrend, i, 11));
			aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			aProposition.terms.push_back(pair<string,int>("ups and downs",0));
			aProposition.terms.push_back(pair<string,int>("peaks and valeys",0));
			aProposition.terms.push_back(pair<string,int>("fluctuation",1));
			aProposition.terms.push_back(pair<string,int>("volatility",1));

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			// Adding proposition for steepness (aProposition.membership is the same)
			aProposition.description = PQgetvalue(resOrdTrend, i, 12);
			aProposition.type = "steepness";
			aProposition.detail = PageRank::getSteepnessDegree(PQgetvalue(resOrdTrend, i, 12), PQgetvalue(resOrdTrend, i, 11));	// calling function that returns the degree ("slight", "steep")
			aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			aProposition.terms.push_back(pair<string,int>("sharp",0));
			aProposition.terms.push_back(pair<string,int>("steep",1));
			
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;
		
			// Adding proposition for trend_initial_value (aProposition.membership is the same)
			initialTrendValue = atof(PQgetvalue(resOrdTrend, i, 13));
			aProposition.description		= PQgetvalue(resOrdTrend, i, 13);
			aProposition.type				= "trend_initial_value";
			aProposition.detail				= "";
			if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
			{
				for (int j = 0; j < PQntuples(resAnnotationOnGraphic); j++)
				{
					if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, j, 3)) == 0)
					{
						tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, j, 2);
						if (tempAnnotationText.size() > 0)
						{
						//	cout << "Comparing: " << PQgetvalue(resAnnotationOnGraphic, j, 3) << endl;
						//	cout << "With the value before: " << aProposition.description << endl;
						//	cout << "Content of annotation field: " << PQgetvalue(resAnnotationOnGraphic, j, 2) << endl;
						//	cout << "Got annotated trend_initial_value" << endl;
							aProposition.detail	= "annotated";
							aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, j, 2));
						//	cout << "Value: " << aProposition.description << endl;
							initialTrendValue = atof(aProposition.description.c_str());
							break;
						}
						else
							continue;
					}	
					else
						continue;
				}
			}
			if (aProposition.detail.compare("annotated") != 0)
			{
				tempValue = atof(aProposition.description.c_str());
				tempValue = tempValue * 100;
				tempValue = ceil(tempValue);
				tempValue = tempValue / 100;
				sTempValue << tempValue;
				aProposition.description 	= sTempValue.str();
				sTempValue.str("");
			}
			aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			// Adding proposition for trend_end_value (aProposition.membership is the same)
			endTrendValue = atof(PQgetvalue(resOrdTrend, i, 14));
			aProposition.description		= PQgetvalue(resOrdTrend, i, 14);
			aProposition.type				= "trend_end_value";
			aProposition.detail				= "";

			if (PQresultStatus(resAnnotationOnGraphic) == PGRES_TUPLES_OK)		// Stores "annotated if the point is annotated"
			{
				for (int j = 0; j < PQntuples(resAnnotationOnGraphic); j++)
				{
					if (aProposition.description.compare(PQgetvalue(resAnnotationOnGraphic, j, 3)) == 0)
					{
						tempAnnotationText = PQgetvalue(resAnnotationOnGraphic, j, 2);
						if (tempAnnotationText.size() > 0)
						{
						//	cout << "Comparing: " << PQgetvalue(resAnnotationOnGraphic, j, 3) << endl;
						//	cout << "With the value before: " << aProposition.description << endl;
						//	cout << "Content of annotation field: " << PQgetvalue(resAnnotationOnGraphic, j, 2) << endl;
						//	cout << "Value before: " << aProposition.description << endl;
						//	cout << "Got annotated trend_end_value" << endl;
							aProposition.detail	= "annotated";
							aProposition.description = input.extractDigits(PQgetvalue(resAnnotationOnGraphic, j, 2));
						//	cout << "Value: " << aProposition.description << endl;
							endTrendValue = atof(aProposition.description.c_str());
							break;
						}
						else
							continue;
					}
					else
						continue;
				}
			}
			if (aProposition.detail.compare("annotated") != 0)
			{
				tempValue = atof(aProposition.description.c_str());
				tempValue = tempValue * 100;
				tempValue = ceil(tempValue);
				tempValue = tempValue / 100;
				sTempValue << tempValue;
				aProposition.description 	= sTempValue.str();
				sTempValue.str("");
			}
			aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			// Adding proposition for trend_initial_date (aProposition.membership is the same)
			aProposition.description		= PQgetvalue(resOrdTrend, i, 15);
			aProposition.type				= "trend_initial_date";
			aProposition.detail				= "";			// TODO: maybe store the date type (100, 110, 111)
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			if (aProposition.description.compare("Undefined") == 0)
				aProposition.conveyable			= false;
			else
				aProposition.conveyable			= true;

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			// Adding proposition for trend_end_date (aProposition.membership is the same)
			aProposition.description		= PQgetvalue(resOrdTrend, i, 16);
			aProposition.type				= "trend_end_date";
			aProposition.detail				= "";			// TODO: maybe store the date type (100, 110, 111)
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			if (aProposition.description.compare("Undefined") == 0)
				aProposition.conveyable			= false;
			else
				aProposition.conveyable			= true;

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			result.str("");
			tempResult.str("");
			// Adding proposition for trend_absolute_change (aProposition.membership is the same)
			if (initialTrendValue > endTrendValue)
			{
				aProposition.additionalInfo.push_back(pair<string,string> ("decrease", "trend_behaviour"));
				aProposition.conveyable			= true;
			}
			else if (initialTrendValue < endTrendValue)
			{
				aProposition.additionalInfo.push_back(pair<string,string> ("increase", "trend_behaviour"));
				aProposition.conveyable			= true;
			}
			else
			{
				aProposition.additionalInfo.push_back(pair<string,string> ("no change", "trend_behaviour"));
				aProposition.conveyable			= false;
			}
			tempResult << abs(initialTrendValue - endTrendValue);
			aProposition.description		= tempResult.str();
			aProposition.type				= "trend_absolute_change";
			aProposition.detail				= scaleUnit;
			//aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
		
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			result.str("");
			tempResult.str("");
			// Adding proposition for trend_rate_change (aProposition.membership is the same)
			if (initialTrendValue == 0 || endGraphValue == 0)
			{
				result << 0.00;
				aProposition.conveyable			= false;
			}
			else if (initialTrendValue > endTrendValue)
			{
				result << (ceil(abs(initialTrendValue / endTrendValue) * 100));
				aProposition.conveyable			= true;
			}
			else if (initialTrendValue < endTrendValue)
			{
				result << (ceil(abs(endTrendValue / initialTrendValue) * 100));
				aProposition.conveyable			= true;
			}
			else
			{
				result << 0.00;
				aProposition.conveyable			= false;
			}	
			aProposition.description		= result.str();
			aProposition.type				= "trend_rate_change";
			aProposition.detail				= scaleUnit;
			//aProposition.conveyable			= true;
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
		
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;
			aProposition.additionalInfo.clear();

			tempDateResult.str("");
			// Adding proposition for trend_overall_period_years (aProposition.membership is the same)
			tempYear1 = atoi(PQgetvalue(resOrdTrend, i, 19));
			tempYear2 = atoi(PQgetvalue(resOrdTrend, i, 20));
			tempMonth1 = atoi(PQgetvalue(resOrdTrend, i, 21));
			tempMonth2 = atoi(PQgetvalue(resOrdTrend, i, 22));
			tempDay1 = atoi(PQgetvalue(resOrdTrend, i, 23));
			tempDay2 = atoi(PQgetvalue(resOrdTrend, i, 24));
			if ((tempYear1 > 0) && (tempYear2 > 0))
			{
				tempDateDiffYear = tempYear2 - tempYear1;
				tempDateResult << tempDateDiffYear;
				aProposition.description = tempDateResult.str();
				if (tempDateDiffYear > 0)
					aProposition.conveyable	= true;		//only convey it if it has years
				else
					aProposition.conveyable	= false;
			}
			else
			{
				aProposition.description	= "";
				aProposition.conveyable		= false;
			}
			aProposition.type				= "trend_overall_period_years";
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			if ((tempMonth1 == 0) && (tempMonth2 == 0))
				aProposition.detail				= "year";
			else
				aProposition.detail				= "";

			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			tempDateResult.str("");
			// Adding proposition for trend_overall_period_months (aProposition.membership is the same)
			if ((tempMonth1 == 0) && (tempMonth2 == 0))
				tempDateDiffMonth = tempDateDiffYear * 12;
			else if ((tempMonth1 > 0) && (tempMonth2 > 0) && (tempMonth2 > tempMonth1))
				tempDateDiffMonth = (tempMonth2 - tempMonth1) + tempDateDiffYear * 12;
			else if ((tempMonth1 > 0) && (tempMonth2 > 0) && (tempMonth2 < tempMonth1))
				tempDateDiffMonth = (tempDateDiffYear * 12) - (tempMonth1 - tempMonth2);
			else
				tempDateDiffMonth = 0;
			if (tempDateDiffMonth > 0 && tempDateDiffMonth < 60)
			{
				tempDateResult << tempDateDiffMonth;
				aProposition.description = tempDateResult.str();
				aProposition.conveyable	= true;		//only convey it if it has years and the difference is greater than 0
			}
			else if (tempDateDiffMonth > 60)
			{
				tempDateResult << tempDateDiffMonth;
				aProposition.description	= tempDateResult.str();
				aProposition.conveyable		= false;
			}
			else
			{
				aProposition.description	= "";
				aProposition.conveyable		= false;
			}
			aProposition.type				= "trend_overall_period_months";
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			if (((tempMonth1 > 0) || (tempMonth2 > 0)) &&
				((tempDay1 <= 1) && (tempDay2 <= 1)))
				aProposition.detail				= "month";
			else
				aProposition.detail				= "";
		
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			tempDateResult.str("");
			// Adding proposition for trend_overall_period_days (aProposition.membership is the same)
			if (((tempDay1 == 0) && (tempDay2 == 0)) || (tempDay1 == tempDay2))
				tempDateDiffDay = tempDateDiffMonth * 30;
			else if ((tempDay1 > 0) && (tempDay2 > 0) && (tempDay2 > tempDay1))
				tempDateDiffDay = (tempDay2 - tempDay1) + tempDateDiffMonth * 30;
			else if ((tempDay1 > 0) && (tempDay2 > 0) && (tempDay2 < tempDay1))
				tempDateDiffDay = (tempDateDiffMonth * 30) - (tempDay1 - tempDay2);
			else
				tempDateDiffDay = 0;
			if (tempDateDiffDay > 0 && tempDateDiffDay <= 365) //only convey it if it is greater than zero and less than 365
			{
				tempDateResult << tempDateDiffDay;
				aProposition.description	= tempDateResult.str();
				aProposition.conveyable		= true;		
			}
			else if (tempDateDiffDay > 365)
			{
				tempDateResult << tempDateDiffDay;				//keep number of days anyway
				aProposition.description	= tempDateResult.str();
				aProposition.conveyable		= false;
			}
			else
			{
				aProposition.description	= "";
				aProposition.conveyable		= false;
			}
			aProposition.type				= "trend_overall_period_days";
			aProposition.segmentPosition	= PQgetvalue(resOrdTrend, i, 10);
			if ((tempDay1 > 1) && (tempDay2 > 1))
				aProposition.detail				= "day";
			else
				aProposition.detail				= "";
		
			myGraph[graph_it].myProposition = aProposition;
			myGraph[graph_it].selected		= false;
			myGraph[graph_it].degree		= 0;
			myGraph[graph_it].conceptRole	= 2;
			myEdge							= new float [graphSize];
			myGraph[graph_it].edge			= myEdge;
			graph_it++;

			aProposition.membership.clear();
		}
	}
	// The following piece is turning off the propositions that talk about some of the overall graph
	// features that are redundant when the graph only has one trend
	cout << "Total number of trends in the graph: " << currentNumberOfTrends << endl;
	if (currentNumberOfTrends == 1)
	{
		for (int k = 0; k < int(myGraph.size()); k++)
		{
			if (//(myGraph[k].myProposition.type.compare("graph_volatility") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_overall_behaviour") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_absolute_change") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_rate_change") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_initial_date") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_initial_value") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_end_date") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_end_value") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_overall_period_years") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_overall_period_months") == 0) ||
				(myGraph[k].myProposition.type.compare("graph_overall_period_days") == 0))
				myGraph[k].myProposition.conveyable = false;
		}
	}
}



/* This method is responsible for checking if the graph has a trend that is considered stable.
 * If it finds it, it checks if its absolute or its rate chenge is different from zero. If it
 * is different, it changes its description to Relatively Stable Trend 
 */
void PageRank::adjustStableTrendDescription()
{
	string trendOrder;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.description.compare("stable trend") == 0)
		{
			trendOrder = myGraph[i].myProposition.segmentPosition;
			for (int j = 0; j < int(myGraph.size()); j++)
			{
				if (((myGraph[j].myProposition.type.compare("trend_rate_change") == 0) ||
					(myGraph[j].myProposition.type.compare("trend_absolute_change") == 0)) &&
					(myGraph[j].myProposition.segmentPosition.compare(trendOrder) == 0) &&
					(myGraph[j].myProposition.description.compare("0.00") != 0))
					myGraph[i].myProposition.description = "relatively stable trend";
				else
					continue;
			}
		}
	}
}



/* This method is responsible for applying initial weights to the edges on myGraph based on 
 * initialWeightMap. Those weights are the ones that belong to the edges between the priority vertex and
 * each proposition.
 */
void PageRank::setInitialPropWeights()
{
	initialWeightMap_it = myInitialWeightMap.begin();
	float tempValue;
	/* setting initial weights based myInitialWeightMap and in specific calculations of certain features such as
	 * volatility, steepness, annotations, etc. */
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		// boosting key propositions about the overall graph
		if (myGraph[i].myProposition.type.compare("graph_type") == 0)
		{
			initialWeightMap_it = myInitialWeightMap.find("graph_type");
			tempValue = (initialWeightMap_it->second * boostFactor);
			//tempValue = initialWeightMap_it->second;
			myGraph[0].edge[i] = tempValue;
			myGraph[i].edge[0] = tempValue;
		}
		else if (myGraph[i].myProposition.type.compare("entity_description") == 0)
		{
			initialWeightMap_it = myInitialWeightMap.find("entity_description");
			tempValue = (initialWeightMap_it->second * boostFactor);
			//tempValue = initialWeightMap_it->second;
			myGraph[0].edge[i] = tempValue;
			myGraph[i].edge[0] = tempValue;
		}
		else if (myGraph[i].myProposition.type.compare("graph_volatility") == 0)
		{
			if (myGraph[i].myProposition.detail.compare("highly volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("graph_volatility");
				tempValue = (4 * (boostFactor * (initialWeightMap_it->second + (initialWeightMap_it->second * atof(myGraph[i].myProposition.description.c_str())))));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("graph_volatility");
				tempValue = (3 * (boostFactor * (initialWeightMap_it->second + (initialWeightMap_it->second * atof(myGraph[i].myProposition.description.c_str())))));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("slightly volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("graph_volatility");
				tempValue = (2 * (boostFactor * (initialWeightMap_it->second + (initialWeightMap_it->second * atof(myGraph[i].myProposition.description.c_str())))));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
			{
				initialWeightMap_it = myInitialWeightMap.find("graph_volatility");
				tempValue = initialWeightMap_it->second;
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
		}
		//if the measurement axis descriptor already shows the numbers in percentage, I want to decrease the initial weight of the proposition
		else if (myGraph[i].myProposition.type.compare("graph_rate_change") == 0)
		{
			if (myGraph[i].myProposition.detail.compare("percentage") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("graph_rate_change");
				tempValue = (initialWeightMap_it->second / penaltyFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
				initialWeightMap_it = myInitialWeightMap.find("graph_rate_change");
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
		}
		// boosting trend_description propositions in general (the ones that belong to the intention
		// will be boosted more later)
		else if (myGraph[i].myProposition.type.compare("trend_description") == 0)
		{
			initialWeightMap_it = myInitialWeightMap.find("trend_description");
			tempValue = (4 * initialWeightMap_it->second * boostFactor);
			//tempValue = (initialWeightMap_it->second * boostFactor);
			myGraph[0].edge[i] = tempValue;
			myGraph[i].edge[0] = tempValue;
		}
		//if the measurement axis descriptor already shows the numbers in percentage, I want to decrease the initial weight of the proposition
		else if (myGraph[i].myProposition.type.compare("trend_rate_change") == 0)
		{
			initialWeightMap_it = myInitialWeightMap.find("trend_rate_change");
			if (myGraph[i].myProposition.detail.compare("percentage") == 0)
			{
				tempValue = (initialWeightMap_it->second / penaltyFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
		}
		else if ((myGraph[i].myProposition.type.compare("trend_overall_period_years") == 0) ||
				 (myGraph[i].myProposition.type.compare("trend_overall_period_months") == 0) ||
				 (myGraph[i].myProposition.type.compare("trend_overall_period_days") == 0))
		{
			initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
			if ((myGraph[i].myProposition.type.compare("trend_overall_period_years") == 0) &&
				(myGraph[i].myProposition.detail.compare("year") == 0))
			{
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.type.compare("trend_overall_period_months") == 0) 
				if ((myGraph[i].myProposition.detail.compare("month") == 0) &&
					((atoi(myGraph[i].myProposition.description.c_str())%12) != 0))
				{
					tempValue = (initialWeightMap_it->second * boostFactor);
					myGraph[0].edge[i] = tempValue;
					myGraph[i].edge[0] = tempValue;
				}
				else
				{
					tempValue = (initialWeightMap_it->second / penaltyFactor);
					myGraph[0].edge[i] = tempValue;
					myGraph[i].edge[0] = tempValue;
				}
			else if ((myGraph[i].myProposition.type.compare("trend_overall_period_days") == 0) &&
					 (myGraph[i].myProposition.detail.compare("day") == 0))
			{
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
			{
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
			}
		}
		else if ((myGraph[i].myProposition.type.compare("graph_overall_period_years") == 0) ||
				 (myGraph[i].myProposition.type.compare("graph_overall_period_months") == 0) ||
				 (myGraph[i].myProposition.type.compare("graph_overall_period_days") == 0))
		{
			initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
			if ((myGraph[i].myProposition.type.compare("graph_overall_period_years") == 0) &&
				(myGraph[i].myProposition.detail.compare("year") == 0))
			{
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if ((myGraph[i].myProposition.type.compare("graph_overall_period_months") == 0) &&
					 (myGraph[i].myProposition.detail.compare("month") == 0))
			{
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if ((myGraph[i].myProposition.type.compare("graph_overall_period_days") == 0) &&
					 (myGraph[i].myProposition.detail.compare("day") == 0))
			{
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
			{
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
			}
		}
		else if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			// composed_trends have their initial weights set up to be their probabilities from the BN
			tempValue = atof(myGraph[i].myProposition.detail.c_str());
			myGraph[0].edge[i] = tempValue;
			myGraph[i].edge[0] = tempValue;
		}
		else if (myGraph[i].myProposition.type.compare("volatility") == 0)
		{
			if (myGraph[i].myProposition.detail.compare("highly volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("volatility");
				tempValue = (4 * (initialWeightMap_it->second * boostFactor));
				//tempValue = (2 * (initialWeightMap_it->second * boostFactor));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("volatility");
				tempValue = (3 * (initialWeightMap_it->second * boostFactor));
				//tempValue = (1.8 * initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("slightly volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("volatility");
				tempValue = (2 * (initialWeightMap_it->second * boostFactor));
				//tempValue = (1.5 * initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("slightly volatile") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("volatility");
				tempValue = (initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			//else if ((myGraph[i].myProposition.detail.compare("relatively smooth") == 0) ||
			//		(myGraph[i].myProposition.detail.compare("smooth") == 0))
			//{
			//	initialWeightMap_it = myInitialWeightMap.find("volatility");
			//	tempValue = initialWeightMap_it->second + (initialWeightMap_it->second * atof(myGraph[i].myProposition.description.c_str()));
			//	myGraph[0].edge[i] = tempValue;
			//	myGraph[i].edge[0] = tempValue;
			//}
			else
			{
				initialWeightMap_it = myInitialWeightMap.find("volatility");
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
				//myGraph[i].myProposition.conveyable = false;		//do not say anything about volatility (when it is not found "-1")
			}
		}
		// please note that the multiplication for boosting in steepness is different because this proposition
		// is not strongly related to other propositions -- see if that is a good way of solving it
		else if (myGraph[i].myProposition.type.compare("steepness") == 0)
		{
			if ((myGraph[i].myProposition.detail.compare("very steep") == 0) ||
				(myGraph[i].myProposition.detail.compare("almost flat") == 0))
			{
				initialWeightMap_it = myInitialWeightMap.find("steepness");
				tempValue = (4 * (initialWeightMap_it->second * boostFactor));
				//tempValue = (2 * (initialWeightMap_it->second * boostFactor));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("steep") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("steepness");
				tempValue = (3 * (initialWeightMap_it->second * boostFactor));
				//tempValue = (1.8 * initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else if (myGraph[i].myProposition.detail.compare("slightly steep") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find("steepness");
				tempValue = (2 * initialWeightMap_it->second * boostFactor);
				//tempValue = (1.5 * initialWeightMap_it->second * boostFactor);
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
			{
				initialWeightMap_it = myInitialWeightMap.find("steepness");
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
				myGraph[i].myProposition.conveyable = false;		//do not say anything about steepness
			}
		}
		else if ((myGraph[i].myProposition.type.compare("trend_initial_value") == 0) ||
				(myGraph[i].myProposition.type.compare("trend_end_value") == 0))
			//	(myGraph[i].myProposition.type.compare("maximum_point_value") == 0) ||
			//	(myGraph[i].myProposition.type.compare("minimum_point_value") == 0))
		{
			if (myGraph[i].myProposition.detail.compare("annotated") == 0)
			{
				initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
				tempValue = (4 * (initialWeightMap_it->second * boostFactor));
				myGraph[0].edge[i] = tempValue;
				myGraph[i].edge[0] = tempValue;
			}
			else
			{
				initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
				myGraph[0].edge[i] = initialWeightMap_it->second;
				myGraph[i].edge[0] = initialWeightMap_it->second;
			}
		}
		else
		{
			initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
			myGraph[0].edge[i] = initialWeightMap_it->second;
			myGraph[i].edge[0] = initialWeightMap_it->second;
			//printf("%f  ", myGraph[0].edge[j]);
		}
	}
}



/* This method is responsible for applying relationship weights to the edges on myGraph based on 
 * myRelationWeightMap. The method connectAncestorsAndDescendents will take care of cleaning up
 * relationships between composed_trends, trends and features that don't belong to the same membership
 */
void PageRank::applyEdgeWeight()
{
	string tempPropType1, tempPropType2;
	myRelationWeightMap_it = myRelationWeightMap.begin();

	// connecting all the nodes that have some relationship on myRelationWeightMap
	for (int i = 1; i < int(myGraph.size()); i++)
	{
		//myGraph[i].edge[1] = 0.00f;
		tempPropType1 = myGraph[i].myProposition.type;
		
		for (int j = 1; j < int(myGraph.size()); j++)
		{
			// this if statement is taking care of different entries on myRelationWeightMap for composed trends
			if ((myGraph[i].myProposition.type.compare("composed_trend") == 0) &&
				(myGraph[j].myProposition.type.compare("composed_trend") == 0))
			{			
				tempPropType1 = myGraph[i].myProposition.membership[1];
				tempPropType2 = myGraph[j].myProposition.membership[1];
			}
			else
				tempPropType2 = myGraph[j].myProposition.type;
			propositionPair aTempPropPair1 (tempPropType1,tempPropType2);
			propositionPair aTempPropPair2 (tempPropType2,tempPropType1);
			myRelationWeightMap_it = myRelationWeightMap.find(aTempPropPair1);
			if (myRelationWeightMap_it != myRelationWeightMap.end())
			{
				if (myRelationWeightMap_it->second.compare("complementRelation") == 0)
				{
					myGraph[i].edge[j] = complementRelation;
					myGraph[j].edge[i] = complementRelation;
				}
				else if (myRelationWeightMap_it->second.compare("contrastRelation") == 0)
				{
					myGraph[i].edge[j] = contrastRelation;
					myGraph[j].edge[i] = contrastRelation;
				}
				else if (myRelationWeightMap_it->second.compare("redundancyRelation") == 0)
				{
					myGraph[i].edge[j] = redundancyRelation;
					myGraph[j].edge[i] = redundancyRelation;
				}
				else if (myRelationWeightMap_it->second.compare("belongsToRelation") == 0)
				{
					myGraph[i].edge[j] = belongsToRelation;
					myGraph[j].edge[i] = belongsToRelation;
				}
				else if (myRelationWeightMap_it->second.compare("sameMembershipRelation") == 0)
				{
					myGraph[i].edge[j] = sameMembershipRelation;
					myGraph[j].edge[i] = sameMembershipRelation;
				}
			} 
			else
			{
				myRelationWeightMap_it = myRelationWeightMap.find(aTempPropPair2);
				if (myRelationWeightMap_it != myRelationWeightMap.end())
				{
					if (myRelationWeightMap_it->second.compare("complementRelation") == 0)
					{
						myGraph[i].edge[j] = complementRelation;
						myGraph[j].edge[i] = complementRelation;
					}
					else if (myRelationWeightMap_it->second.compare("contrastRelation") == 0)
					{
						myGraph[i].edge[j] = contrastRelation;
						myGraph[j].edge[i] = contrastRelation;
					}
					else if (myRelationWeightMap_it->second.compare("redundancyRelation") == 0)
					{
						myGraph[i].edge[j] = redundancyRelation;
						myGraph[j].edge[i] = redundancyRelation;
					}
					else if (myRelationWeightMap_it->second.compare("belongsToRelation") == 0)
					{
						myGraph[i].edge[j] = belongsToRelation;
						myGraph[j].edge[i] = belongsToRelation;
					}
					else if (myRelationWeightMap_it->second.compare("sameMembershipRelation") == 0)
					{
						myGraph[i].edge[j] = sameMembershipRelation;
						myGraph[j].edge[i] = sameMembershipRelation;
					}
				}
				else
				{
					myGraph[i].edge[j] = 0.00f;
				}
			}
		}
	}
	//delete all the weights from the edges that are connecting feature nodes that don't belong to the same membership
	for (int i = 1; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.membership[0].compare("line graph") == 0)
			continue;
		else
		{
			for (int j = 1; j < int(myGraph.size()); j++)
			{
				if (myGraph[j].myProposition.membership[0].compare("line graph") == 0)
					continue;
				//check if the nodes belong to the same membership then connect them
				else if(!(PageRank::checkIfSameMembership(myGraph[i], myGraph[j])))
				{
				//cout << "Not same membership detected for node: " << myGraph[i].myProposition.description << " and Node: " <<
				//		myGraph[j].myProposition.description << endl;
					myGraph[i].edge[j] = 0.00f;
				}
			}
		}
	}
}



/* This method receives the current graph and a composed trend and updates all the nodes in the
 * graph that have that composed trend as an ancestor. It is called whenever a trend was already inserted
 * so the membership field of all nodes are updated accordingly
 * @param:	composedTrend: composed trend that needs to be updated at the membership fields of its descendant
 *			aProposition: proposition that needs to have the composedTrend added to its membership vector (if not already there)
 */
void PageRank::updateMembership(string composedTrend, string trend)
{
	bool found = false;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.detail.compare(trend) == 0)
		{
			for (int j = 0; j < int(myGraph[i].myProposition.membership.size()); j++)
			{
				if (myGraph[i].myProposition.membership[j].compare(composedTrend) == 0)
				{
					found = true;
				}
			}
			if (!(found))
			{
				cout << "Adding:" << composedTrend << " to " << trend << endl;
				myGraph[i].myProposition.membership.push_back(composedTrend);
			}
		}
	}
}
 

/* This method is needed because BigJumps and BigFalls are going inside non-sustained segments
 * and they are not supposed to.
 */
void PageRank::adjustMembershipForBFandBJ()
{
	//cout << "Calling the function adjustMembershipForBFandBJ" << endl;
	string ancestor;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			//cout << "Getting composed" << endl;
			if (myGraph[i].myProposition.description.compare("BF") == 0)
			{
				//cout << "Getting BF" << endl;
				ancestor = myGraph[i].myProposition.membership[myGraph[i].myProposition.membership.size()-1];
				//cout << "Ancestor: " << ancestor << endl;
				for (int j = 0; j < int(myGraph.size()); j++)
				{
					if ((myGraph[j].myProposition.type.compare("trend_description") == 0) &&
						(myGraph[j].myProposition.description.compare("rising trend") == 0))
					{
					//	cout << "------------------Getting here 1!! ---------------" << endl;
						for (int m = 0; m < int(myGraph[j].myProposition.membership.size()); m++)
						{
							if (myGraph[j].myProposition.membership[m].compare(ancestor) == 0)
							{
								myGraph[j].myProposition.membership.erase(myGraph[j].myProposition.membership.begin()+m);
								break;
							}
							else
								continue;
						}
					}
				}
			}
			else if (myGraph[i].myProposition.description.compare("BJ") == 0)
			{
			//	cout << "Getting BJ" << endl;
				for (int k = 0; k < int(myGraph.size()); k++)
				{
					if ((myGraph[k].myProposition.type.compare("trend_description") == 0) &&
						(myGraph[k].myProposition.description.compare("falling trend") == 0))
					{
						for (int l = 0; l < int(myGraph[i].myProposition.membership.size()); l++)
						{
							if (myGraph[k].myProposition.membership[l].compare(ancestor) == 0)
							{
								myGraph[k].myProposition.membership.erase(myGraph[k].myProposition.membership.begin()+l);
								break;
							}
							else
								continue;
						}
					}
				}
			}
		}
		else
		{
			continue;
		}
	}
}
 


/* This method sums the weights of all the edges linked to a given node
 * @param:	aNode - a node from myGraph
 * returns the sum of all edges' weights
 */
float PageRank::sumWeightsOfEdges(node aNode)
{
	float tempSumWeights = 0.00f;

	for (int i = 0; i< int(myGraph.size()); i++)
	{
		if (aNode.edge[i] > 0.00)  // taking care of negative weights that might occur when the graph is created
		{
			tempSumWeights += aNode.edge[i];
		}
	}
	return tempSumWeights;
}



/* This method updates each node with its degree information
 * @param:	none
 */
void PageRank::getDegree()
{
	int degree = 0;
	for (int i = 0; i< int(myGraph.size()); i++)
	{
		for (int j = 0; j< int(myGraph.size()); j++)
		{
			if (myGraph[i].edge[j] > 0.000000)  // taking care of negative weights that might occur when the graph is created
				degree++;
		}
		myGraph[i].degree = degree;
		degree = 0;
	}
}



/** This method instantiates a vector with the initial PageRanks of each node
 *  the initial PageRank is currently defined by the initial importance of the node
 *  iterations will be performed to converge to the final PageRank values 
 */
void PageRank::setInitialPageRank()
{
	pageRankVector.resize(myGraph.size());
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		/* getting the weights from the graph since the initial weights from myInitialWeightMap may have changed
		   on specific cases such as volatility and steepness */
		pageRankVector[i] = myGraph[0].edge[i];
	}
	//cout << "Printing Initial PageRankVector---------------------" << endl;
	//PageRank::printPageRankVector();
	//cout << "Alright setInitialPageRank" << endl;
}


/* This method checks if the new scores differ from the old calculated scores by less than 0.000001. If so, it
 * stops running pageRank in the graph. It is used to converge the nodes scores.
 */
void PageRank::convergePageRank()
{
	bool stopConvergence		= false;
	bool continueConvergence	= false;
	float difference			= 0.00f;

	//PageRank::setInitialPageRank();
	//PageRank::printPageRankVector();
	//PageRank::printGraphContent();
	pageRankVectorCopy = pageRankVector;

	while (!(stopConvergence))
	{
		PageRank::runPageRank();
		for (int i = 0; i < int(pageRankVector.size()); i++)
		{
			//cout << "PageRank comparision: " << i << endl;
			difference = abs(pageRankVector[i] - pageRankVectorCopy[i]);
			//cout << "Difference: " << difference << endl;
			if (difference < 0.000001)
			{
				continueConvergence = false;
				break;
			}
			else
			{
				continueConvergence = true;
				continue;
			}
		}
		if (!(continueConvergence))
			stopConvergence = true;
		else
			pageRankVectorCopy = pageRankVector;
	}
	//PageRank::printPageRankVector();
}

/* This method just prints on the console all the nodes in the PageRank vector */
void PageRank::printPageRankVector()
{
	for (int i = 0; i < int(pageRankVector.size()); i++)
	{
		cout << "Pos: " << i << " Node type: " << myGraph[i].myProposition.type << " Content: " << 
			myGraph[i].myProposition.description << " PageRank: " << pageRankVector[i];
		if (myGraph[i].myProposition.conveyable)
			cout << " Convey" << endl;
		else
			cout << " NOT Convey" << endl;
	}
}


float PageRank::calculateImportanceScore(node aNode)
{
	float dampingFactor = 0.15f;
	float partialImportanceScore = 0.00f;
	float importanceScore;
	
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		//if ((aNode.edge[i] > 0) && (!(myGraph[i].selected)))
		if (aNode.edge[i] > 0)
		{
			partialImportanceScore += ((aNode.edge[i] / PageRank::sumWeightsOfEdges(myGraph[i])) * pageRankVector[i]);
		}
	}
	importanceScore = ((1.00f - dampingFactor) + (dampingFactor * partialImportanceScore));
	return importanceScore;
}



/* This method runs PageRank in the entire graph (it calls calculateImportanceScore on the nodes that weren't selected yet).
 */
void PageRank::runPageRank()
{
	float tempImpScore = 0.00f;
	
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (!(myGraph[i].selected))
		{
			tempImpScore = PageRank::calculateImportanceScore(myGraph[i]);
			pageRankVector[i] = tempImpScore;
		}
	}
}



void PageRank::selectPropositions()
{
	nodeScore aNodeScore;
	node highestRankedNode;
	
	//vector<nodeScore> rankedNodes;
	float highestImpScore = 0.00f;
	float highestImpScoreAll = 0.00f;
	int nodePosition = 0;

	mySelectionHistoryMap_it = mySelectionHistoryMap.begin();

	cout << "Printing selected vertices steps: " << endl;
	
	for (int i = 1; i < int(pageRankVector.size()); i++)
	{
		for (int j = 1; j < int(pageRankVector.size()); j++)
		{
			if (!(myGraph[j].selected))
			{
				if (pageRankVector[j] > highestImpScore)
				{
					highestImpScore		= pageRankVector[j];
					aNodeScore.aNode	= myGraph[j];
					aNodeScore.score	= highestImpScore;
					nodePosition		= j;
				}
				if (highestImpScore > highestImpScoreAll)
					highestImpScoreAll = highestImpScore;
			}
		}
		highestImpScore					= 0.00f;
		myGraph[nodePosition].selected	= true;
		//decreases the importance score of the selected node at the current response
		myGraph[0].edge[nodePosition] = myGraph[0].edge[nodePosition] / penaltyFactor;
		myGraph[nodePosition].edge[0] = myGraph[nodePosition].edge[0] / penaltyFactor;

		//set the importance scores of the priority vertex and the selected vertex to the highest one calculated so far
		pageRankVector[0] = highestImpScoreAll;
		pageRankVector[nodePosition] = highestImpScoreAll;

		//increments the number of times the node was selected in the current response
		mySelectionHistoryMap_it = mySelectionHistoryMap.find(nodePosition);
		mySelectionHistoryMap_it->second += 1;

		//boost and penalize all the nodes that are related to the selected node, excluding the priority vertex
		PageRank::updatePropositionsRelations(nodePosition);

		//runs PageRank again since it needs to be run after each proposition has been taken
		//PageRank::runPageRank();
		PageRank::convergePageRank();  

		//include selected node to the ranking vector (rankedNodes)
		rankedNodes.push_back(aNodeScore);

		//cout << "------------------" << endl;
		//PageRank::printRankedNodes(rankedNodes);
		//cout << "------------------" << endl;
		//PageRank::printGraphContent();
		//cout << "------------------" << endl;
		//PageRank::printPageRankVector();
		//cout << "------------------" << endl;

	}
}

//void PageRank::initialSummaryPropositions(vector<nodeScore> allPropositions, int index)
// This whole method was commented on 06/26/2013 becuase I'm going to try a different stopping criteria
/* This method selects the top propositions that will compose the initial summary. This function is
 * probably going to be called by the Generation Module.
 * @param: vector of nodes that return from method selectProposition.
 *		   index - number that will define the gap between propositions. Follow up responses will allow a
 *				   bigger gap than the initial summary (for initial summary index is zero)
 * returns: vector of nodes that will be realized in the initial summary. 
 */
//{
//	float normalizedScore1	= 0.00f;
//	float normalizedScore2	= 0.00f;
//	float averageScore		= 0.00f;
//	int j;
//	//int numberOfNodes = int(allPropositions.size());
//	int numberOfNodes = int(rankedNodes.size());
//	int numberOfCompAndTrends = 0;
//	int minimumNumberNodes;
//	cout << "Number of nodes: " << numberOfNodes << endl;
//	for (int i = 0; i < numberOfNodes; i++)
//	{
//		averageScore += rankedNodes[i].score / rankedNodes[i].aNode.degree;
//		if ((rankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0) ||
//			(rankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0))
//		//if (allPropositions[i].aNode.myProposition.type.compare("composed_trend") == 0)
//			numberOfCompAndTrends++;
//	}
//	averageScore = averageScore / numberOfNodes;
//	minimumNumberNodes = numberOfCompAndTrends + 3;
//	cout << "Minimum number of nodes: " << minimumNumberNodes << endl;
//
//	/* The possibilities for the stopping criteria are:
//	1 - just check if the absolute difference between normalized nodes is less than the calculated average
//	2 - if the next node has a higher normalized score, add it. If not, see if the difference
//	    between the next and current is less than the calculated average
//	3 - check if the difference between next and current nodes is less tahn the average but force a 
//		minimum of 5 nodes in the response 
//	They will be performed incrementally. So if the first option doesn't bring us more than 3 propositions,
//	try the second one and so on.
//	*/
//	finalRankedNodes.clear();
//	finalRankedNodes.push_back(rankedNodes[0]);
//	for (j = 0; j < (numberOfNodes - 1); j++)
//	{
//		cout << "Average: " << averageScore << endl;
//		normalizedScore1 = rankedNodes[j].score / rankedNodes[j].aNode.degree;
//		normalizedScore2 = rankedNodes[j+1].score / rankedNodes[j+1].aNode.degree;
//		// ------------------- option 1 (just gap):
//		if ((abs(normalizedScore1 - normalizedScore2) < ((index / 100) * averageScore) + averageScore) &&
//			(rankedNodes[j+1].aNode.myProposition.conveyable))
//			finalRankedNodes.push_back(rankedNodes[j+1]);
//		else if (!(rankedNodes[j+1].aNode.myProposition.conveyable))
//			continue;
//		else
//			break;
//		cout << "Current vector size: " << finalRankedNodes.size() << endl;
//	}
/*	if (int(finalRankedNodes.size()) < minimumNumberNodes)
//	{
//		cout << "Option 1 wasn't chosen" << endl;
//		finalRankedNodes.clear();
//		finalRankedNodes.push_back(allPropositions[0]);
//		for (j = 0; j < (numberOfNodes - 1); j++)
//		{
//			if (allPropositions[j+1].aNode.myProposition.conveyable)
//			{
//				cout << "Average: " << averageScore << endl;
//				normalizedScore1 = allPropositions[j].score / allPropositions[j].aNode.degree;
//				normalizedScore2 = allPropositions[j+1].score / allPropositions[j+1].aNode.degree;
//				// ------------------- option 2 (add current proposition if the next is bigger):
//				if (normalizedScore2 > normalizedScore1)
//					finalRankedNodes.push_back(allPropositions[j+1]);
//				else if ((normalizedScore1 - normalizedScore2) < ((index / 100) * averageScore) + averageScore)
//					finalRankedNodes.push_back(allPropositions[j+1]);
//				else
//					break;
//			}
//			else
//				continue;
//		}
//	}
*///	if (int(finalRankedNodes.size()) < minimumNumberNodes)
//	{
//		cout << "Option 2 wasn't chosen" << endl;
//		finalRankedNodes.clear();
//		finalRankedNodes.push_back(rankedNodes[0]);
//		for (j = 0; j < (numberOfNodes - 1); j++)
//		{	
//		//	if (allPropositions[j+1].aNode.myProposition.conveyable)
//		//	{
//			cout << "Average: " << averageScore << endl;
//			normalizedScore1 = rankedNodes[j].score / rankedNodes[j].aNode.degree;
//			normalizedScore2 = rankedNodes[j+1].score / rankedNodes[j+1].aNode.degree;
//			// ------------------- option 3 (at least 5 propositions):
//			//if (((normalizedScore1 - normalizedScore2) < ((index / 100) * averageScore) + averageScore) || (int(finalRankedNodes.size()) < 3))
//			// in this case that I am forcing the response to have at least 3 propositions, I am comparing the gap
//			// with half of the average score and it needs to be smaller than that to be added
//			cout << "Norm score 1: " << normalizedScore1 << endl;
//			cout << "Norm score 2: " << normalizedScore2 << endl;
//			cout << "Abs norm1 - norm2: " << abs(normalizedScore1 - normalizedScore2) << endl;
//			cout << "Calculation for comparison: " << ((((index / 100) * averageScore) + averageScore) / 2) << endl;
//			if ((((abs(normalizedScore1 - normalizedScore2)) < ((((index / 100) * averageScore) + averageScore) / 2)) || 
//				 (int(finalRankedNodes.size()) < minimumNumberNodes)) &&
//				(rankedNodes[j+1].aNode.myProposition.conveyable))
//				finalRankedNodes.push_back(rankedNodes[j+1]);
//			else if (!(rankedNodes[j+1].aNode.myProposition.conveyable))
//				continue;
//			else
//				break;
//			cout << "Current vector size: " << finalRankedNodes.size() << endl;
//		}
//	}
//}

void PageRank::initialSummaryPropositions(vector<nodeScore> allPropositions, int index)
// This method was written on 06/26/2013 to try a different stopping criteria (it is only looking at the importance
// score. The damping factor went from 0.85 to 0.15 and that fixed the problem and we don't need to normalize
// a node's importance score any more
/* This method selects the top propositions that will compose the initial summary. This function is
 * probably going to be called by the Generation Module.
 * @param: vector of nodes that return from method selectProposition.
 *		   index - number that will define the gap between propositions. Follow up responses will allow a
 *				   bigger gap than the initial summary (for initial summary index is zero)
 * returns: vector of nodes that will be realized in the initial summary. 
 */
{
	float normalizedScore1	= 0.00f;
	float normalizedScore2	= 0.00f;
	float averageScoreGap	= 0.00f;
	int j;
	int numberOfNodes = int(rankedNodes.size());
	int numberOfCompAndTrends = 0;
	int minimumNumberNodes;

	vector<string> tempRequiredNodes;
	tempRequiredNodes.push_back("graph_type");
	tempRequiredNodes.push_back("entity_description");
	tempRequiredNodes.push_back("composed_trend");

	// added on 09/18 to change the stopping criteria according to discussion regarding the proposal.
	// It now checks if the propositions: linegraph, entity_description and composed_trend were already selected
	// and it adds 1 to this variable when one of them is selected. When the gap is bigger than the average,
	// it checks if those propositions are already there

	// -------------------------------------------------------------------------------------------//

	for (int i = 0; i < numberOfNodes - 1; i++)
	{
		averageScoreGap += abs(rankedNodes[i].score - rankedNodes[i+1].score); // / rankedNodes[i].aNode.degree;
		if ((rankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0) ||
			(rankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0))
			numberOfCompAndTrends++;
	}
	averageScoreGap /= numberOfNodes;
	minimumNumberNodes = numberOfCompAndTrends + 3;
	cout << "Minimum number of nodes: " << minimumNumberNodes << endl;

	/* The possibilities for the stopping criteria are:
	1 - just check if the absolute difference between normalized nodes is less than the calculated average
	2 - if the next node has a higher normalized score, add it. If not, see if the difference
	    between the next and current is less than the calculated average
	3 - check if the difference between next and current nodes is less tahn the average but force a 
		minimum of 5 nodes in the response 
	They will be performed incrementally. So if the first option doesn't bring us more than 3 propositions,
	try the second one and so on.
	*/
	finalRankedNodes.clear();
	finalRankedNodes.push_back(rankedNodes[0]);
	for (j = 0; j < (numberOfNodes - 1); j++)
	{
		cout << "Average: " << averageScoreGap << endl;
		normalizedScore1 = rankedNodes[j].score; /// rankedNodes[j].aNode.degree;
		normalizedScore2 = rankedNodes[j+1].score; /// rankedNodes[j+1].aNode.degree;
		// ------------------- option 1 (just gap):
		if ((abs(normalizedScore1 - normalizedScore2) < ((index / 100) * averageScoreGap) + averageScoreGap) &&
			(rankedNodes[j+1].aNode.myProposition.conveyable))
			finalRankedNodes.push_back(rankedNodes[j+1]);
		else if (!(rankedNodes[j+1].aNode.myProposition.conveyable))
			continue;
		else
			break;
		cout << "Current vector size: " << finalRankedNodes.size() << endl;
	}
	if (int(finalRankedNodes.size()) < minimumNumberNodes)
	{
		cout << "Option 2 wasn't chosen" << endl;
		finalRankedNodes.clear();
		finalRankedNodes.push_back(rankedNodes[0]);
		for (j = 0; j < (numberOfNodes - 1); j++)
		{	
			cout << "Average: " << averageScoreGap << endl;
			normalizedScore1 = rankedNodes[j].score; /// rankedNodes[j].aNode.degree;
			normalizedScore2 = rankedNodes[j+1].score; /// rankedNodes[j+1].aNode.degree;
			// ------------------- option 3 (at least minimumNumberNodes):
			//if (((normalizedScore1 - normalizedScore2) < ((index / 100) * averageScore) + averageScore) || (int(finalRankedNodes.size()) < 3))
			// in this case that I am forcing the response to have a minimum number of propositions, I am comparing the gap
			// with half of the average score and it needs to be smaller than that to be added
			cout << "Norm score 1: " << normalizedScore1 << endl;
			cout << "Norm score 2: " << normalizedScore2 << endl;
			cout << "Abs norm1 - norm2: " << abs(normalizedScore1 - normalizedScore2) << endl;
			cout << "Calculation for comparison: " << ((((index / 100) * averageScoreGap) + averageScoreGap) / 2) << endl;
			if ((((abs(normalizedScore1 - normalizedScore2)) < ((((index / 100) * averageScoreGap) + averageScoreGap) / 2)) || 
			//if ((((abs(normalizedScore1 - normalizedScore2)) < (((index / 100) * averageScoreGap) + averageScoreGap)) || 
				 (int(finalRankedNodes.size()) < minimumNumberNodes)) &&
				(rankedNodes[j+1].aNode.myProposition.conveyable))
				finalRankedNodes.push_back(rankedNodes[j+1]);
			else if (!(rankedNodes[j+1].aNode.myProposition.conveyable))
				continue;
			else
				break;
			cout << "Current vector size: " << finalRankedNodes.size() << endl;
		}
	}
}

// This method is called by initialSummaryPropositions in order to check if there is 
//bool checkRequiredNodes(vectorNodeScore finalRankedNodes)
//{
//	vector<string> tempRequiredNodes;
//	tempRequiredNodes.push_back("graph_type");
//	tempRequiredNodes.push_back("entity_description");
//	tempRequiredNodes.push_back("composed_trend");

//}

//void PageRank::initialSummaryPropositions(vector<nodeScore> allPropositions, int index)
// This method was written on 09/18/2013 to try a different stopping criteria (it now checks if the 
// propositions: linegraph, entity_description and composed_trend were already selected
// and it adds 1 to the variable "requiredNodes" when one of them is selected. When the gap is bigger 
// than the average, it checks if those propositions are already there
/* This method selects the top propositions that will compose the initial summary. This function is
 * probably going to be called by the Generation Module.
 * @param: vector of nodes that return from method selectProposition.
 *		   index - number that will define the gap between propositions. Follow up responses will allow a
 *				   bigger gap than the initial summary (for initial summary index is zero)
 * returns: vector of nodes that will be realized in the initial summary. 
 */
//{
//	float normalizedScore1	= 0.00f;
//	float normalizedScore2	= 0.00f;
//	float averageScoreGap	= 0.00f;
//	int j;
//	int numberOfNodes = int(rankedNodes.size());
//	int numberOfCompAndTrends = 0;
//	int minimumNumberNodes;
//	int requiredNodes = 0;

//	for (int i = 0; i < numberOfNodes - 1; i++)
//	{
//		averageScoreGap += abs(rankedNodes[i].score - rankedNodes[i+1].score); // / rankedNodes[i].aNode.degree;
//		if ((rankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0) ||
//			(rankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0))
//			numberOfCompAndTrends++;
//	}
//	averageScoreGap /= numberOfNodes;
//	minimumNumberNodes = numberOfCompAndTrends + 3;
//	cout << "Minimum number of nodes: " << minimumNumberNodes << endl;

	/* The possibilities for the stopping criteria are:
	1 - just check if the absolute difference between normalized nodes is less than the calculated average
	2 - if the next node has a higher normalized score, add it. If not, see if the difference
	    between the next and current is less than the calculated average
	3 - check if the difference between next and current nodes is less tahn the average but force a 
		minimum of 5 nodes in the response 
	They will be performed incrementally. So if the first option doesn't bring us more than 3 propositions,
	try the second one and so on.
	*/
//	finalRankedNodes.clear();
//	finalRankedNodes.push_back(rankedNodes[0]);
	// here I'm checking if the node is one of the required types
//	if ((rankedNodes[0].aNode.myProposition.type.compare("graph_type") == 0) ||
//		(rankedNodes[0].aNode.myProposition.type.compare("entity_description") == 0) ||
//		(rankedNodes[0].aNode.myProposition.type.compare("composed_trend") == 0)  ||
//		(rankedNodes[0].aNode.myProposition.type.compare("trend_description") == 0))
//	{
//		requiredNodes++;
//	}
//	cout << "Number of required nodes (0): " << requiredNodes << endl;
//	for (j = 0; j < (numberOfNodes - 1); j++)
//	{
//		cout << "Average: " << averageScoreGap << endl;
//		normalizedScore1 = rankedNodes[j].score; /// rankedNodes[j].aNode.degree;
//		normalizedScore2 = rankedNodes[j+1].score; /// rankedNodes[j+1].aNode.degree;
		// ------------------- option 1 (just gap):
//		if (!(rankedNodes[j+1].aNode.myProposition.conveyable))
//			continue;
//		else
//		{
//			if (abs(normalizedScore1 - normalizedScore2) < ((index / 100) * averageScoreGap) + averageScoreGap)
//			{
//				finalRankedNodes.push_back(rankedNodes[j+1]);
//				if ((rankedNodes[j+1].aNode.myProposition.type.compare("graph_type") == 0) ||
//					(rankedNodes[j+1].aNode.myProposition.type.compare("entity_description") == 0) ||
//					(rankedNodes[j+1].aNode.myProposition.type.compare("composed_trend") == 0) ||
//					(rankedNodes[j+1].aNode.myProposition.type.compare("trend_description") == 0))
//				{
//					requiredNodes++;
//				}
//			}
			//else if (requiredNodes < minimumNumberNodes)
			//{
			//	finalRankedNodes.push_back(rankedNodes[j+1]);
			//	if ((rankedNodes[j+1].aNode.myProposition.type.compare("graph_type") == 0) ||
			//		(rankedNodes[j+1].aNode.myProposition.type.compare("entity_description") == 0) ||
			//		(rankedNodes[j+1].aNode.myProposition.type.compare("composed_trend") == 0))
			//	{
			//		requiredNodes++;
			//	}
			//}
//		}
//		cout << "Current vector size: " << finalRankedNodes.size() << endl;
//		cout << "Number of required nodes (1): " << requiredNodes << endl;
//	}
//	if (requiredNodes < minimumNumberNodes)
//	{
		//requiredNodes = 0;
//		cout << "Could not go with the normal way" << endl;
		//finalRankedNodes.clear();
		//finalRankedNodes.push_back(rankedNodes[0]);
//		for (j = j; j < (numberOfNodes - 1); j++)
		//j = 0;
//		while (requiredNodes < minimumNumberNodes)
//		{	
//			cout << "Average: " << averageScoreGap << endl;
			//normalizedScore1 = rankedNodes[j].score; /// rankedNodes[j].aNode.degree;
			//normalizedScore2 = rankedNodes[j+1].score; /// rankedNodes[j+1].aNode.degree;
			// ------------------- option 3 (at least minimumNumberNodes):
			//if (((normalizedScore1 - normalizedScore2) < ((index / 100) * averageScore) + averageScore) || (int(finalRankedNodes.size()) < 3))
			// in this case that I am forcing the response to have a minimum number of propositions, I am comparing the gap
			// with half of the average score and it needs to be smaller than that to be added
//			cout << "Norm score 1: " << normalizedScore1 << endl;
//			cout << "Norm score 2: " << normalizedScore2 << endl;
//			cout << "Abs norm1 - norm2: " << abs(normalizedScore1 - normalizedScore2) << endl;
//			cout << "Calculation for comparison: " << ((((index / 100) * averageScoreGap) + averageScoreGap) / 2) << endl;
//			if ((((abs(normalizedScore1 - normalizedScore2)) < ((((index / 100) * averageScoreGap) + averageScoreGap) / 2)) || 
//				 (int(finalRankedNodes.size()) < minimumNumberNodes)) &&
//				(rankedNodes[j+1].aNode.myProposition.conveyable))
//			{
//			finalRankedNodes.push_back(rankedNodes[j+1]);
//			if ((rankedNodes[j+1].aNode.myProposition.type.compare("graph_type") == 0) ||
//				(rankedNodes[j+1].aNode.myProposition.type.compare("entity_description") == 0) ||
//				(rankedNodes[j+1].aNode.myProposition.type.compare("composed_trend") == 0) ||
//				(rankedNodes[j+1].aNode.myProposition.type.compare("trend_description") == 0))
//			{
//				requiredNodes++;
//			}
//			j++;
//		}
//	}
//}

/* This method is responsible for checking the type of the relationship between the corresponding node 
 * and all its neighbors and update the weight of the relationships based on the type (attractor or repeller)
 * Attraction relations will be boosted while Repellent relations will be penalized
 * @params:	nodePosition - the corresponding position of the chosen node in the graph
 */
void PageRank::updatePropositionsRelations(int nodePosition)
{
	string tempNodeType1, tempNodeType2;
	myRelationWeightMap_it = myRelationWeightMap.begin();
	tempNodeType1 = myGraph[nodePosition].myProposition.type;

	for (int i = 1; i < int(myGraph.size()); i++)
	{
		if (myGraph[nodePosition].edge[i] > 0)
		{
			// this if statement is taking care of different entries on myRelationWeightMap for composed trends
			if ((myGraph[nodePosition].myProposition.type.compare("composed_trend") == 0) &&
				(myGraph[i].myProposition.type.compare("composed_trend") == 0))
			{
				tempNodeType1 = myGraph[nodePosition].myProposition.membership[1];
				tempNodeType2 = myGraph[i].myProposition.membership[1];
			}
			else
				tempNodeType2 = myGraph[i].myProposition.type;
			propositionPair aTempPropPair1 (tempNodeType1,tempNodeType2);  //since the map key can be in
			propositionPair aTempPropPair2 (tempNodeType2,tempNodeType1);  // one of the two possible orders

			myRelationWeightMap_it = myRelationWeightMap.find(aTempPropPair1);
		
			if (myRelationWeightMap_it != myRelationWeightMap.end())
			{
				if ((myRelationWeightMap_it->second.compare("complementRelation") == 0) ||
					(myRelationWeightMap_it->second.compare("contrastRelation") == 0) ||
					(myRelationWeightMap_it->second.compare("belongsToRelation") == 0))
				{				
					myGraph[nodePosition].edge[i] = myGraph[nodePosition].edge[i] * boostFactor;
					myGraph[i].edge[nodePosition] = myGraph[i].edge[nodePosition] * boostFactor;
				}
				else if (myRelationWeightMap_it->second.compare("redundancyRelation") == 0)
				{
					for (int j = 1; j < int(myGraph.size()); j++)
					{
					// uncomment that if you want to penalize trend_description further when you find a redundancy on its suggestion
					//	if ((myGraph[i].edge[j] > 0) && (myGraph[j].myProposition.type.compare("trend_description") == 0))
					//	{
					//	//	myGraph[i].edge[j] = myGraph[i].edge[j] / penaltyFactor;
					//	//	myGraph[j].edge[i] = myGraph[j].edge[i] / penaltyFactor;
					//		for (int l = 0; l < int(myGraph.size()); l++)	//going second level when finding a node of type trend_description
					//		{
					//			if (myGraph[j].edge[l] > 0)
					//			{
					//				myGraph[j].edge[l] = myGraph[j].edge[l] / penaltyFactor;
					//				myGraph[l].edge[j] = myGraph[l].edge[j] / penaltyFactor;
					//			}
					//		}
					//	}
					//	else if (myGraph[i].edge[j] > 0)
						if (myGraph[i].edge[j] > 0)
						{
							myGraph[i].edge[j] = myGraph[i].edge[j] / penaltyFactor;
							myGraph[j].edge[i] = myGraph[j].edge[i] / penaltyFactor;
						}
					}
				}
			} 
			else
			{
				myRelationWeightMap_it = myRelationWeightMap.find(aTempPropPair2);
				if (myRelationWeightMap_it != myRelationWeightMap.end())
				{
					if ((myRelationWeightMap_it->second.compare("complementRelation") == 0) ||
						(myRelationWeightMap_it->second.compare("contrastRelation") == 0) ||
						(myRelationWeightMap_it->second.compare("belongsToRelation") == 0))
					{
						myGraph[nodePosition].edge[i] = myGraph[nodePosition].edge[i] * boostFactor;
						myGraph[i].edge[nodePosition] = myGraph[i].edge[nodePosition] * boostFactor;
					}
					else if (myRelationWeightMap_it->second.compare("redundancyRelation") == 0)
					{
						for (int j = 1; j < int(myGraph.size()); j++)
						{
						// uncomment that if you want to penalize trend_description further when you find a redundancy on its suggestion
						//	if ((myGraph[i].edge[j] > 0) && (myGraph[j].myProposition.type.compare("trend_description") == 0))
						//	{
						//	//	myGraph[i].edge[j] = myGraph[i].edge[j] / penaltyFactor;
						//	//	myGraph[j].edge[i] = myGraph[j].edge[i] / penaltyFactor;
						//		for (int k = 0; k < int(myGraph.size()); k++)	//going second level when finding a node of type trend_description
						//		{
						//			if (myGraph[j].edge[k] > 0)
						//			{
						//				myGraph[j].edge[k] = myGraph[j].edge[k] / penaltyFactor;
						//				myGraph[k].edge[j] = myGraph[k].edge[j] / penaltyFactor;
						//			}
						//		}
						//	}
						//	else if (myGraph[i].edge[j] > 0)
							if (myGraph[i].edge[j] > 0)
							{
								myGraph[i].edge[j] = myGraph[i].edge[j] / penaltyFactor;
								myGraph[j].edge[i] = myGraph[j].edge[i] / penaltyFactor;
							}
						}
					}
				} 
				else
					continue;
			}
		}
	}
}



/* This method is responsible for boosting the edges that connect special nodes (nodes of propositions that were boosted based
 * on its volatility or steepness degree, annotation, etc). It applies the boostFactor to all the edges connecting
 * those nodes to other nodes in the graph. It is called before running PageRank for the first time and it will increase
 * the weights of the edges on relationships of any type.
 * @params:	none
 */
void PageRank::initialBoostingSpecialProps()
{
	initialWeightMap_it = myInitialWeightMap.begin();
	float tempValue;
	for (int i = 1; i < int(myGraph.size()); i++)
	{
		//if ((myGraph[i].myProposition.type.compare("graph_type") != 0) &&
		//	(myGraph[i].myProposition.type.compare("entity_description") != 0))
		//{
			initialWeightMap_it = myInitialWeightMap.find(myGraph[i].myProposition.type);
			if (myGraph[i].edge[0] > initialWeightMap_it->second)
				for (int j = 1; j < int(myGraph.size()); j++)
				{
					if (myGraph[i].edge[j] > 0)
					{				
						myGraph[i].edge[j] = myGraph[i].edge[j] * boostFactor;
					//	cout << "Updating initial weight of: " << myGraph[i].myProposition.description << " at position " << j << endl;
					}
				}
		//}
		//else
		//	continue;
	}
}


/* This method is responsible for creating entries on myRelationWeightMap between ComposedTrend nodes only. These
 * relationships are dynamic and can be either belongsToRelation or redundancyRelation (depending on whether one
 * suggestion contains or not the other one. If both suggestions are composed of more than one segment (CHT, CTR,
 * CSCT, etc) than they will repel each other; if one of them is a RT, FT, ST, BJ or BF, and it is part of the other
 * one, than they will attract each other. If this method is being used to instantiate this special relationships,
 * then the methods that check myRelationWeightMap need to treat composedTrend nodes differently since their keys
 * on the map are now going to be their actual names, not their types as in the other node types' relations.
 * @params:	none
 * It calls comTrendRelationSetupHelper that takes care of checking if a simple composedTrend is part of a complex
 * composed trend (if a RT is part of a CTR, for example).
 */
void PageRank::compTrendRelationSetup()
{
	vector<pair<string,string>> setOfCompTrends;
	
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			setOfCompTrends.push_back(pair<string, string> (myGraph[i].myProposition.description,myGraph[i].myProposition.membership[1]));	
		//	cout << "Going inside first for " << i << endl;
		}
	}
//	for (int l = 0; l < int(setOfCompTrends.size()); l++)
//	{
//		cout << "Position: " << l << " of setOfCompTrend: " << setOfCompTrends[l].first << ", " << setOfCompTrends[l].second << endl; 
//	}
	for (int j = 0; j < (int(setOfCompTrends.size()) - 1); j++)
		for (int k = 1; k < int(setOfCompTrends.size()); k++)
		{
			if (setOfCompTrends[j].second.compare(setOfCompTrends[k].second) == 0)
				continue;
			else if (((setOfCompTrends[j].first.compare("CHT") == 0) || (setOfCompTrends[j].first.compare("CSCT") == 0) ||
				 (setOfCompTrends[j].first.compare("CTR") == 0) || (setOfCompTrends[j].first.compare("CTLS") == 0) ||
				 (setOfCompTrends[j].first.compare("PC") == 0)) &&
				((setOfCompTrends[k].first.compare("CHT") == 0) || (setOfCompTrends[k].first.compare("CSCT") == 0) ||
				 (setOfCompTrends[k].first.compare("CTR") == 0) || (setOfCompTrends[k].first.compare("CTLS") == 0) ||
				 (setOfCompTrends[k].first.compare("PC") == 0)))
			{
				myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> 
				(setOfCompTrends[j].second, setOfCompTrends[k].second)), "redundancyRelation"));
			}
			else if ((((setOfCompTrends[j].first.compare("RT") == 0) || (setOfCompTrends[j].first.compare("FT") == 0) ||
					   (setOfCompTrends[j].first.compare("ST") == 0) || (setOfCompTrends[j].first.compare("BJ") == 0) ||
					   (setOfCompTrends[j].first.compare("BF") == 0)) &&
					  ((setOfCompTrends[k].first.compare("CHT") == 0) || (setOfCompTrends[k].first.compare("CSCT") == 0) ||
					   (setOfCompTrends[k].first.compare("CTR") == 0) || (setOfCompTrends[k].first.compare("CTLS") == 0) ||
					   (setOfCompTrends[k].first.compare("PC") == 0))) ||
					 (((setOfCompTrends[k].first.compare("RT") == 0) || (setOfCompTrends[k].first.compare("FT") == 0) ||
					   (setOfCompTrends[k].first.compare("ST") == 0) || (setOfCompTrends[k].first.compare("BJ") == 0) ||
					   (setOfCompTrends[k].first.compare("BF") == 0)) &&
					  ((setOfCompTrends[j].first.compare("CHT") == 0) || (setOfCompTrends[j].first.compare("CSCT") == 0) ||
					   (setOfCompTrends[j].first.compare("CTR") == 0) || (setOfCompTrends[j].first.compare("CTLS") == 0) ||
					   (setOfCompTrends[j].first.compare("PC") == 0))))
			{
				if (PageRank::compTrendRelationSetupHelper(setOfCompTrends[j].second, setOfCompTrends[k].second))
				{
					myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> 
					(setOfCompTrends[j].second, setOfCompTrends[k].second)), "belongsToRelation"));
				}
				else
				{
					myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> 
					(setOfCompTrends[j].second, setOfCompTrends[k].second)), "redundancyRelation"));
				}
			}
			else  //on cases where both of them are simple composed trends (RT and FT, for example)
			{
				myRelationWeightMap.insert(pair<propositionPair,string> ((pair<string,string> 
				(setOfCompTrends[j].second, setOfCompTrends[k].second)), "redundancyRelation"));
			}	
		}
}



/* This method is responsible for checking all the trend_description node types' membership to confirm
 * if one of the simple composed trend passed as a parameter to it belongs to the complex composed
 * trend also passed on the same call. It returns true if it finds a trend that has both composed trends
 * on its membership vector.
 * @params:	compTrend1 (string of type TL_TLS_RTS, TL_TLS_CHT1)
 *			compTrend2 same type of compTrend1
 */
bool PageRank::compTrendRelationSetupHelper(string compTrend1, string compTrend2)
{
	bool checkTrend1, checkTrend2;
	checkTrend1 = false;
	checkTrend2 = false;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("trend_description") == 0)
		{
			for (int j = 0; j < int(myGraph[i].myProposition.membership.size()); j++)
				if (myGraph[i].myProposition.membership[j].compare(compTrend1) == 0)
					checkTrend1 = true;
				else if (myGraph[i].myProposition.membership[j].compare(compTrend2) == 0)
					checkTrend2 = true;
				else
					continue;
			if ((checkTrend1) && (checkTrend2))
			{
		//		cout << "------- Making those belong: " << compTrend1 << ", " << compTrend2 << endl;
				return true;
			}
			else
			{
				checkTrend1 = false;
				checkTrend2 = false;
			}
		}
		else
			continue;
	}
	return false;
}



/* Since the relationship between nodes is dynamic, this method connects nodes that belong to a specific
 * composed trend and trend to their respective parents
 */
/*
void PageRank::connectAncestorsandDescendants()
{
	string tempComposedTrend, tempTrendDescription;

	for (int i = 1; i < int(myGraph.size()); i++)
	{
		cout << "Prop type: " << myGraph[i].myProposition.type << endl;
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			tempComposedTrend = myGraph[i].myProposition.membership[0];
			cout << "Description: " << tempComposedTrend << endl;
			for (int j = 1; j < int(myGraph.size()); j++)
			{
				if (i == j) //making the main diagonal 0, since the same elements have the same membership
							//but we don't want a weight associated to it
				{
					myGraph[i].edge[j] = 0.00f;
					myGraph[j].edge[i] = 0.00f;
				}
				else
				{
					for (int k = 0; k < int(myGraph[j].myProposition.membership.size()); k++)
					{
						if (myGraph[j].myProposition.membership[k].compare(tempComposedTrend) == 0)
						{					
							myGraph[i].edge[j] = belongsToRelation;
							myGraph[j].edge[i] = belongsToRelation;
							cout << "Connecting: " << myGraph[i].myProposition.description <<
								" to: " << myGraph[j].myProposition.description << endl;
						}
					}
				}
			} 
		}
		else if (myGraph[i].myProposition.type.compare("trend_description") == 0)
		{
			tempTrendDescription = myGraph[i].myProposition.detail;
			cout << "Description: " << tempTrendDescription << endl;
			for (int l = 1; l < int(myGraph.size()); l++)
			{
				if (i == l)
				{
					myGraph[i].edge[l] = 0.00f;
					myGraph[l].edge[i] = 0.00f;
				}
				else
				{
					for (int m = 0; m < int(myGraph[l].myProposition.membership.size()); m++)
					{
						if (myGraph[l].myProposition.membership[m].compare(tempTrendDescription) == 0)
						{					
							myGraph[i].edge[l] = belongsToRelation;
							myGraph[l].edge[i] = belongsToRelation;
							cout << "Connecting: " << myGraph[i].myProposition.description <<
								" to: " << myGraph[l].myProposition.description << endl;
						}
					}
				}
			}
		}
	}
}*/


/* Since the relationship between nodes is dynamic, this method connects nodes that belong to a specific
 * composed trend and trend to their respective parents
 * This method is needed in addition to applyEdgeWeight and checkIfSameMembership because of
 * the exceptions present when dealing with nodes that have a different number of ancestors in their
 * membership vectors.
 */
void PageRank::connectAncestorsandDescendants()
{
	string tempComposedTrend, tempTrendDescription, tempDescription;

	for (int i = 1; i < int(myGraph.size()); i++)
	{
		//cout << "Prop type: " << myGraph[i].myProposition.type << endl;
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			//tempDescription = myGraph[i].myProposition.detail;
			tempComposedTrend = myGraph[i].myProposition.membership[0];
		//	cout << "Description: " << tempComposedTrend << endl;
			for (int j = 1; j < int(myGraph.size()); j++)
			{
				if (i == j) //making the main diagonal 0, since the same elements have the same membership
							//but we don't want a weight associated to it
				{
					//cout << "Diagonal = 0" << endl;
					myGraph[i].edge[j] = 0.00f;
					myGraph[j].edge[i] = 0.00f;
				}
				else
				{
					for (int k = 0; k < int(myGraph[j].myProposition.membership.size()); k++)
					{
						//cout << "Member of: " << myGraph[j].myProposition.membership[k] << endl;
						if (myGraph[j].myProposition.membership[k].compare(tempComposedTrend) == 0)
						{					
							myGraph[i].edge[j] = belongsToRelation;
							myGraph[j].edge[i] = belongsToRelation;
						//	cout << "Connecting: " << myGraph[i].myProposition.description <<
						//		" to: " << myGraph[j].myProposition.description << endl;
						}
						else
							continue;
					}
				}
			} 
		}
		else if (myGraph[i].myProposition.type.compare("trend_description") == 0)
		{
			tempTrendDescription = myGraph[i].myProposition.detail;
		//	cout << "Description: " << tempTrendDescription << endl;
			for (int l = 1; l < int(myGraph.size()); l++)
			{
				if (i == l)
				{
					myGraph[i].edge[l] = 0.00f;
					myGraph[l].edge[i] = 0.00f;
				}
				else
				{
					for (int m = 0; m < int(myGraph[l].myProposition.membership.size()); m++)
					{
						if (myGraph[l].myProposition.membership[m].compare(tempTrendDescription) == 0)
						{					
							myGraph[i].edge[l] = belongsToRelation;
							myGraph[l].edge[i] = belongsToRelation;
						//	cout << "Connecting: " << myGraph[i].myProposition.description <<
						//		" to: " << myGraph[l].myProposition.description << endl;
						}
					}
				}
			}
		}
	}
}

/* This method initializes the map that keeps track of already selected propositions
 * and the corresponding number of times it was selected.
 * This map should be reset after each response is concluded
 */
void PageRank::instantiateSelectionHistoryMap()
{
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		mySelectionHistoryMap.insert(pair<int,int> (i, 0));
	}
}



/* This method checks if two nodes belong to the same trend. It is called by applyEdgeWeights method
 * @param:	aNode1 - first node
 *			aNode2 - second node
 * returns: true if the nodes belong to the same trend, false otherwise
 */
bool PageRank::checkIfSameMembership(node aNode1, node aNode2)
{
	bool sameMembership = false;
	int j = 0;
	string tempTrend;
	
	if (int(aNode1.myProposition.membership.size()) == int(aNode2.myProposition.membership.size()))
	{
		for (int i = 0; i < int(aNode1.myProposition.membership.size()); i++)
		{
			if (aNode2.myProposition.membership[i].compare(aNode1.myProposition.membership[i]) == 0)
			{
				sameMembership = true;
				//cout << "Same membership node1: " << aNode1.myProposition.detail << " and node2: " <<
				//	aNode2.myProposition.detail <<endl;
			}
			else
			{
				sameMembership = false;
			}
		}
	}
	return sameMembership;
}



/* This method checks if a node belongs to the ancestor.
 * @param:	aNode1 - a node
 *			ancestor - unique description of an entity
 * returns: true if the node belongs to the ancestor, false otherwise
 */
bool PageRank::checkIfBelongs(node aNode, string ancestor)
{
	bool belongs = false;
	
	for (int i = 0; i < int(aNode.myProposition.membership.size()); i++)
	{
		if (aNode.myProposition.membership[i].compare(ancestor) == 0)
		{
			belongs = true;
			break;
		}
		else
			continue;
	}
	return belongs;
}



/* This method receives a volatility weight and returns the volatility degree of a
 * proposition based on the volatility value. Examples are: "highly volatile", "slightly volatile"
 * @param:	aNode1 - node
 * returns: the volatility degree that will be attached to the proposition when realizing it
 */
string PageRank::getVolatilityDegree(string weight)
{
	float tempWeight;
	tempWeight = 0.00f;
	tempWeight = atof(weight.c_str());
	//if ((tempWeight >= -1) && (tempWeight < 0.10))
	//	return "smooth";
	if ((tempWeight >= 0.10) && (tempWeight < 0.30))
	//	return "relatively smooth";
	//else if ((tempWeight >= 0.20) && (tempWeight < 0.30))
		return "slightly volatile";
	else if ((tempWeight >= 0.31) && (tempWeight < 0.40))
		return "volatile";
	else if (tempWeight >= 0.41)
		return "highly volatile";
	else
		return "smooth";
} 



/* This method receives the trend angle and returns the steepness degree of a
 * proposition based on the angle value. Examples are: "very steep", "steep", "slight", "almost flat".
 * Since the experiment showed that for low degree angles the volatility played a role, we need to look at it
 * before we decide on how to classify the steepness
 * @param:	aNode1 - node
 * returns: the steepness degree that will be attached to the proposition when realizing it
 */
string PageRank::getSteepnessDegree(string angle, string volatility)
{
	float tempAngle, tempVolat;
	tempAngle = 0.00f;
	tempVolat = 0.00f;
	tempAngle = atof(angle.c_str());
	tempVolat = atof(volatility.c_str());
	if (((tempAngle > 0) && (tempAngle < 6) && (tempVolat >= 0.30)) ||
		((tempAngle >= 345) && (tempAngle < 360) && (tempVolat >= 0.30)))
		return "almost flat";
	else if (((tempAngle > 0) && (tempAngle < 6) && (tempVolat < 0.30)) ||
			((tempAngle >= 345) && (tempAngle < 360) && (tempVolat < 0.30)))
			return "slightly steep";
	else if ((tempAngle >= 7) && (tempAngle < 14))
		return "slightly steep";
	else if (((tempAngle >= 15) && (tempAngle < 54)) || ((tempAngle >= 295) && (tempAngle < 345)))
		return "default";
	else if (((tempAngle >= 55) && (tempAngle < 74)) || ((tempAngle >= 285) && (tempAngle < 294)))
		return "steep";
	else if (((tempAngle >= 75) && (tempAngle < 90)) || ((tempAngle >= 275) && (tempAngle < 284)))
		return "very steep";
	else
		return "default";
} 



/* This method return a node of type composed_trend given is unique description. composed_trend node
 * types have their unique descriptions stored on the first position of the membership vector
 */
node PageRank::getCompTrendNodeFromDescription(string aCompTrend)
{
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			for (int j = 0; j < int(myGraph[i].myProposition.membership.size()); j++)
			{
				if (myGraph[i].myProposition.membership[j].compare(aCompTrend) == 0)
				{
					return myGraph[i];
					break;
				}
				else
					continue;
			}
		}
		else
			continue;
	}
}

/* This method returns an integer that represents the node position of the unique type nodeType
 * in the graph (myGraph). It should only be called for unique types (only for the overall graph
 * related proposition, since it will return the first position found and it will not work for
 * proposition types that can appear more than once)
 */
int PageRank::getNodePositionFromUniqueType(string nodeType)
{
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare(nodeType) == 0)
		{
			return i;
			break;
		}
		else
			continue;
	}
}



/* This method receives the unique description of a composed trend and returns a vector
 * of the nodes of the trends that belong to the composed trend
 */
vector<node> PageRank::getVectorOfTrendNodes(string aCompTrend)
{
	vector<node> myNodeVector;
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("trend_description") == 0)
		{
			for (int j = 0; j < int(myGraph[i].myProposition.membership.size()); j++)
			{
				if (myGraph[i].myProposition.membership[j].compare(aCompTrend) == 0)
					myNodeVector.push_back(myGraph[i]);
				else
					continue;

			}
		}
		else
			continue;
	}
	return myNodeVector;
}

/* This method receives the unique description of the intention and of a suggestion that is a secondary
 * message and returns an integer that identifies the relation between the two. The options are:
 * 1 - Intention is single trend and it belongs to a secondary that is complex (more than one trend)
 * 2 - Intention is single trend and it does NOT belong to a secondary that is complex
 * 3 - Intention is complex and contains a secondary that is simple
 * 4 - Both are simple, so they are separate
 * 5 - Both are complex and the intention contains the secondary suggestion
 * 6 - Both are complex and they have some overlap
 * 7 - Both are complex and they are completely separate
 * 8 - Intention is complex and the secondary is simple and DOES NOT belong to the intention
 * 9 - Both are complex and the suggestion contains the intention
 */
int PageRank::getRelationBetweenSuggestions(string intention, string additionalSuggestion)
{
	int i, j;
	int notContained = 0;
	bool oneContained, allContained, someContained;
	node intentionNode, suggestionNode;
	vector<node> intentionTrendNodeVector, suggestionTrendNodeVector;
	intentionNode = PageRank::getCompTrendNodeFromDescription(intention);
	suggestionNode = PageRank::getCompTrendNodeFromDescription(additionalSuggestion);
	if ((intentionNode.myProposition.description.compare("RT") == 0) ||
		(intentionNode.myProposition.description.compare("FT") == 0) ||
		(intentionNode.myProposition.description.compare("ST") == 0) ||
		(intentionNode.myProposition.description.compare("BJ") == 0) ||
		(intentionNode.myProposition.description.compare("BF") == 0) ||
		(intentionNode.myProposition.description.compare("PC") == 0))
		if ((suggestionNode.myProposition.description.compare("CHT") == 0) ||
			(suggestionNode.myProposition.description.compare("CTR") == 0) ||
			(suggestionNode.myProposition.description.compare("CSCT") == 0) ||
			(suggestionNode.myProposition.description.compare("CTLS") == 0))
			if (PageRank::compTrendRelationSetupHelper(intention, additionalSuggestion))
				return 1;
			else
				return 2;
		else
			return 4;
	else if ((intentionNode.myProposition.description.compare("CHT") == 0) ||
			 (intentionNode.myProposition.description.compare("CTR") == 0) ||
			 (intentionNode.myProposition.description.compare("CSCT") == 0) ||
			 (intentionNode.myProposition.description.compare("CTLS") == 0))
			if ((suggestionNode.myProposition.description.compare("RT") == 0) ||
				(suggestionNode.myProposition.description.compare("FT") == 0) ||
				(suggestionNode.myProposition.description.compare("ST") == 0) ||
				(suggestionNode.myProposition.description.compare("BJ") == 0) ||
				(suggestionNode.myProposition.description.compare("BF") == 0) ||
				(suggestionNode.myProposition.description.compare("PC") == 0))
				if (PageRank::compTrendRelationSetupHelper(intention, additionalSuggestion))
					return 3;
				else
					return 8;
			else
			{
				/*when we have two complex suggestions we need to look at all their trends to see the options*/
				intentionTrendNodeVector = PageRank::getVectorOfTrendNodes(intention);
				suggestionTrendNodeVector = PageRank::getVectorOfTrendNodes(additionalSuggestion);
				oneContained = false;
				allContained = true;
				someContained = false;
				if (intentionTrendNodeVector.size() <= suggestionTrendNodeVector.size())
				{
					for (i = 0; i < intentionTrendNodeVector.size(); i++)
					{
						for (j = 0; j < suggestionTrendNodeVector.size(); j++)
							if (intentionTrendNodeVector[i].myProposition.detail.compare(suggestionTrendNodeVector[j].myProposition.detail) == 0)
							{
								oneContained = true;
								someContained = true;
							}
							else
								continue;
						if (!oneContained)
						{
							notContained++;
							allContained = false;
						}	
					}
					if (notContained == intentionTrendNodeVector.size())
						return 7;
					else if (allContained)
						return 9;
					else if (someContained)
						return 6;
				}
				else
					for (i = 0; i < suggestionTrendNodeVector.size(); i++)
					{
						for (j = 0; j < intentionTrendNodeVector.size(); j++)
							if (suggestionTrendNodeVector[i].myProposition.detail.compare(intentionTrendNodeVector[j].myProposition.detail) == 0)
							{
								oneContained = true;
								someContained = true;
							}
							else
								continue;
						if (!oneContained)
						{
							notContained++;
							allContained = false;
						}	
					}
					if (notContained == suggestionTrendNodeVector.size())
						return 7;
					else if (allContained)
						return 5;
					else if (someContained)
						return 6;			
			}
}



/* It prints the ranked nodes as the result from Page Rank */
void PageRank::printRankedNodes(vectorNodeScore thisRankedNodes)
{
	cout << "Printing ranked nodes, number of nodes:" << thisRankedNodes.size() << endl;
	for (int i = 0; i < int(thisRankedNodes.size()); i++)
	{
		cout << "Node: " << thisRankedNodes[i].aNode.myProposition.type << endl;
		if (thisRankedNodes[i].aNode.myProposition.conveyable)
			cout << " Conveyable " << endl;
		else
			cout << " NOT conveyable " << endl;
		cout << "Description: " << thisRankedNodes[i].aNode.myProposition.description << endl << " Detail: "
		 << thisRankedNodes[i].aNode.myProposition.detail << endl << " Score: " << thisRankedNodes[i].score << endl << 
		 " Degree: " << thisRankedNodes[i].aNode.degree << endl << " Normalized Score: " << 
		 (thisRankedNodes[i].score / thisRankedNodes[i].aNode.degree) << endl;
		for (int j = 0; j < int(thisRankedNodes[i].aNode.myProposition.membership.size()); j++)
			cout << "Membership: " << thisRankedNodes[i].aNode.myProposition.membership[j] << endl;
		for (int k = 0; k < int(thisRankedNodes[i].aNode.myProposition.additionalInfo.size()); k++)
		{
			cout << "Additional Info " << k << ", Position 0: " << thisRankedNodes[i].aNode.myProposition.additionalInfo[k].first << endl;
			cout << "Additional Info " << k << ", Position 1: " << thisRankedNodes[i].aNode.myProposition.additionalInfo[k].second << endl;
		}
	}
}


/**/
void PageRank::printGraphNodes()
{
	for (int i = 0; i < int(myGraph.size()); i++)
	{
		cout << " Node: " << myGraph[i].myProposition.type << " Content: "
			<< myGraph[i].myProposition.description << " Detail: " 
			<< myGraph[i].myProposition.detail << " Membership: " << endl;
		for (int j = 0; j < int(myGraph[i].myProposition.membership.size()); j++)
		{
			cout << " Position: " << j << " Member: " << myGraph[i].myProposition.membership[j] << endl;
		}
	}
}



void PageRank::printMap()
{
	myRelationWeightMap_it = myRelationWeightMap.begin();
	cout << "myRelationWeightMap contains:\n";
	for ( myRelationWeightMap_it=myRelationWeightMap.begin() ; myRelationWeightMap_it != myRelationWeightMap.end(); myRelationWeightMap_it++ )
		cout << "1: " << (*myRelationWeightMap_it).first.first << " 2: " << (*myRelationWeightMap_it).first.second << " Relation => " << 
		(*myRelationWeightMap_it).second << endl;
}


void PageRank::printTrendNameMap()
{
	trendNameMap_it = trendNameMap.begin();
	cout << "trendNameMap contains:\n";
	for ( trendNameMap_it=trendNameMap.begin() ; trendNameMap_it != trendNameMap.end(); trendNameMap_it++ )
		cout << "1: " << (*trendNameMap_it).first << " 2: " << (*trendNameMap_it).second << endl;
}


/*
 *
 */
void PageRank::boostBasedOnSuggestion()
{
	float tempProb, intentionProb;
	tempProb = 0.00f;
	intentionProb = 0.00f;
	string intentionDescription;
	string compTrendDescription;
	int i;

	for (i = 0; i < int(myGraph.size()); i++)
	{
		if (myGraph[i].myProposition.type.compare("composed_trend") == 0)
		{
			//cout << "Prob text: " << myGraph[i].myProposition.detail << endl;
			tempProb = atof(myGraph[i].myProposition.detail.c_str());
			compTrendDescription = myGraph[i].myProposition.membership[1];
			if (intentionProb < tempProb)
			{
				intentionProb = tempProb;
				intentionDescription = compTrendDescription;
			}
			// boosting all the direct relations in the edge of the current composed_trend
			for (int j = 0; j < int(myGraph.size()); j++)
			{
				if (myGraph[i].edge[j] > 0.00)
				{
				//	cout << "Multiplying: " << myGraph[i].edge[j] << endl;
					myGraph[i].edge[j] += myGraph[i].edge[j] * tempProb;
					myGraph[j].edge[i] += myGraph[j].edge[i] * tempProb;
				//	cout << "Result: " << myGraph[i].edge[j] << endl; 
				}
			}
			// for each element of the graph
			for (int k = 0; k < int(myGraph.size()); k++)
			{
				// if it belongs to the current composed_trend
				for (int l = 0; l < int(myGraph[k].myProposition.membership.size()); l++)
				{
					if (myGraph[k].myProposition.membership[l].compare(compTrendDescription) == 0)
					{
						// boost its initial weight
						myGraph[k].edge[0] += myGraph[k].edge[0] * tempProb;
						myGraph[0].edge[k] += myGraph[0].edge[k] * tempProb;
					}
				}
			}
		}
	}
	// look for all the trend_description type nodes. If they belong to the intention, boost it using
	// the boost factor in their initial weights
	for (i = 0; i < int(myGraph.size()); i++)
	{
		// if it is a trend_description type node, boost it using the boost factor,
		// not the suggestion probability
		if ((PageRank::checkIfBelongs(myGraph[i],intentionDescription)) &&
			(myGraph[i].myProposition.type.compare("trend_description") == 0))
		{
			myGraph[i].edge[0] += myGraph[i].edge[0] * boostFactor;
			myGraph[0].edge[i] += myGraph[0].edge[i] * boostFactor;
		}
	}
}
 


PageRank::~PageRank()
{
	for (int i = 0; i < int(myGraph.size()); i++)
		delete myGraph[i].edge;
}