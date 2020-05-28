#include "Generation.h"

using namespace std;

Generation::Generation(char* linegraphKey)
{

	// initializing it to none, it will change if we have a mostImportantSegment in the graph
	mostImportantSegment = "none";

	myPageRank.startPageRank(linegraphKey, "0.1", 0);
//	myPageRank.printRankedNodes(myPageRank.rankedNodes);
	myPageRank.printRankedNodes(myPageRank.finalRankedNodes);
//	myPageRank.printTrendNameMap();
	fillOrganizationStruct();
	responseOrganization();
	addFufSurgeTemplates(); // method that adds the FUF/SURGE templates to the selected nodes
	printOrganizationStruct();
	printOrganizedSet();
//	printOrganizedSetFufSurgeTemplates();
//	printComparableFeaturesMap();
//	printComparableSets();
//	printFufSurgeTemplates();
//	responseRealization();		//it realizes the sentences in myOrganizedSet, one at a time
//	myPageRank.printGraphContent();
	fillPropositionsSelected();  // it fills the maps that keep the propositions selected by pagerank. The maps are used when aggregating
	printGraphPropsSet();
//	printTrendPropsMap();
	callFunctionForTemplate(1);  // this method calls the right aggregateTemplate method depending on the Level. It will be called by the method that will handle the reading level assessment of the article's text
	responseRealizationAggregatedSentences(allSentences); //this method calls FUFSURGE to realize the aggregated sentences in allSentences since this vector contains the result of the aggregateTemplateN methods
	printAggregatedFufSurgeTemplates();

}


/* This method fills the structure response (and all of its sub-structs) with the information that is relevant
 * for the organization process. The nodes that are used to filled the structure are the ones that were 
 * selected by page rank and are stored at myPageRank.finalRankedNodes; the structure is stored at
 * myPageRank.response
 */
void Generation::fillOrganizationStruct()
{
	double intentionProb, totalWeight;
	int numberOfFeatures = 0;
	totalWeight = 0.00;
	intentionProb = 0.00;
	string ancestor;
	//cout << "Getting here 0" << endl;
	for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		if (myPageRank.finalRankedNodes[i].aNode.conceptRole == 1)		//node represents an entity
		{
			//cout << "Getting here 1" << endl;
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0)
			{
				//cout << "Getting here 2" << endl;
				if (intentionProb < atof(myPageRank.finalRankedNodes[i].aNode.myProposition.detail.c_str()))
				{
					//cout << "Getting here 3" << endl;
					intentionProb = atof(myPageRank.finalRankedNodes[i].aNode.myProposition.detail.c_str());
					myPageRank.aResponse.intention.first = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
					myPageRank.aResponse.intention.second = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1];
					cout << myPageRank.aResponse.intention.first << " " << myPageRank.aResponse.intention.second << endl;
				}
				else
				{
					pair<string, string> aSecMessage(myPageRank.finalRankedNodes[i].aNode.myProposition.description, myPageRank.finalRankedNodes[i].aNode.myProposition.membership[myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1]);
					pair<pair<string,string>,bool> aCompSecMessage(aSecMessage,true);  //after the secondary message is used in the organization, it changes to false
					myPageRank.aResponse.secondaryMessages.push_back(aCompSecMessage);
				}
			}
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0)
			{
				myPageRank.anEntity.special = false;
				for (int l = 0; l < int(myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size()); l++)
				{
					// if any of the trend segments is one of the single segment suggestions, make it special
					if ((myPageRank.finalRankedNodes[i].aNode.myProposition.membership[l].substr(3,2).compare("BJ") == 0)
						|| (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[l].substr(3,2).compare("BF") == 0)
						|| (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[l].substr(3,2).compare("RT") == 0)
						|| (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[l].substr(3,2).compare("FT") == 0)
						|| (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[l].substr(3,2).compare("ST") == 0))
					{
						myPageRank.anEntity.special = true;
					}
				}
				ancestor = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
				for (int j = 0; j < int(myPageRank.finalRankedNodes.size()); j++)
				{
					if (myPageRank.finalRankedNodes[j].aNode.conceptRole == 2)
						for (int k = 0; k < int(myPageRank.finalRankedNodes[j].aNode.myProposition.membership.size()); k++)
						{
							if (myPageRank.finalRankedNodes[j].aNode.myProposition.membership[k].compare(ancestor) == 0)
							{
							//	cout << "Found descendent of " << ancestor << endl;
								numberOfFeatures++;
								totalWeight += myPageRank.finalRankedNodes[j].score;
							}
						}
				}
				// instantiating an entity object
				myPageRank.anEntity.description = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
				myPageRank.anEntity.segmentPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition;
				myPageRank.anEntity.featureCount = numberOfFeatures;
				myPageRank.anEntity.key = ancestor;
				myPageRank.anEntity.totalWeight = totalWeight;
				//pushing the entity created above into the vector of entities that belongs to the response object
				myPageRank.aResponse.allEntities.push_back(myPageRank.anEntity);
				numberOfFeatures = 0;
				totalWeight = 0.00;
			}
			else
				continue;
		}
		else //if (myPageRank.finalRankedNodes[i].aNode.conceptRole == 2)		// node represents a feature of an entity
		{
			continue;
		}
	}
}


/* This is the main method used to organize the proposition in a response. It basically reads info from
 * both myPageRank.myGraph (which contains all the nodes from the graph) and myPageRank.finalRankedNodes
 * (which contains only the nodes selected for the current response). That is necessary because sometimes
 * we need to check membership in order to organize and the ancestor node might have not been selected in
 * the current response.
 * It also uses a vector of the trend orders which is sorted. All the trend's features have the trend order
 * in the segmentPosition field of the proposition struct. This is also used to organize the trend features.
 * It call the method segmentOrganization() to handle some types of organization in the segment level
 * The ordering of the organization is as follows:
 * 1 - If the reponse has a segment that is either the whole intention (BF, BJ, RT, FT, ST), than talk about
 *		this segment then talk about the other ones
 * 2 - If we have multiple segments intention, check if there is a segment that is the most important based
 *		in either: its number of features selected in the current response is greater than that of any other
 *		segment; or its totalFeatureWeight is 50% higher than the second largest totalFeatureWeight in the
 *		response
 * 3 - If none of the previous two apply, check if the segments can be organized in a way that we compare 
 *		their features (that might make the response go over the whole graph more than once, since it will
 *		be grouping the features in the response based on the comparison). This organization will call the
 *		method checkIfComparable() and then segmentComparisonOrganization()
 * 4 - The last option, if none of the previous apply, is to organize the proposition time-wise. In this
 *		case we will convey all the features of each segment in the order that the segments appear in
 *		the graph.
 * This method uses the organization structure myPageRank.aResponse (which contains summarized info about
 * the propositions selected in a response) and it fills up the structure called myOrganizedSet.
 */
void Generation::responseOrganization()
{
	int maxPosition = myPageRank.myGraph.size() + 100;
	int suggestionPos, numberOfSecondary, curPosition;
	bool importantSegment = false;
	string intention, importantSegmentPos;
	vector<string> trendOrder;
	numberOfSecondary = myPageRank.aResponse.secondaryMessages.size();
	suggestionPos = 0;
	
	// calling the method that will fill up myOrganizedSet only with the propositions that are related
	// to the overall graph (at the introduction and in the conclusion of the response)
	// (it returns the current suggestionPos to be used later)
	suggestionPos = Generation::overallGraphOrganization(numberOfSecondary, maxPosition, suggestionPos);
	
	// if it is not about the overall graph, look at the responseComponents struct (aResponse)
	// to get the intention, secondary messages, etc
	// here we are organizing the body of the response (the details of the segments)

	// first we need to get all the trend order numbers an sort them
	for (int j = 0; j < int(myPageRank.myGraph.size()); j++)
	{
		if (myPageRank.myGraph[j].myProposition.type.compare("trend_description") == 0)
		{
			//cout << "Getting trend_description number " << myPageRank.myGraph[j].myProposition.segmentPosition << endl;	
			// see what to use to have them already ordered
			trendOrder.push_back(myPageRank.myGraph[j].myProposition.segmentPosition);
		}
		else
			continue;
	}
	if (trendOrder.size() > 0)
		sort (trendOrder.begin(), trendOrder.end());

	//***** This little block is just printing the ordered trendOrder vector *****//
	//for (int a = 0; a < int(trendOrder.size()); a++)
	//	cout << "Trend Order position " << a << " has trend number " << trendOrder[a] << endl;
	//****************************************************************************//
	
	curPosition = numberOfSecondary + 12;
	//cout << "Current position: " << curPosition << endl;
	intention = myPageRank.aResponse.intention.second;

	if ((myPageRank.aResponse.intention.first.compare("RT") == 0) 
		|| (myPageRank.aResponse.intention.first.compare("FT") == 0)
		|| (myPageRank.aResponse.intention.first.compare("ST") == 0)
		|| (myPageRank.aResponse.intention.first.compare("BJ") == 0)
		|| (myPageRank.aResponse.intention.first.compare("BF") == 0))
	{
		// in this case we have a segment that is important because of its suggestion
		// inside segmentOrganization method we are storing its unique name in mostImportantTSegment
		importantSegmentPos = Generation::getImportantSegmentPosition(intention);
		Generation::segmentOrganization(curPosition, trendOrder, importantSegmentPos);
	}
	// here we will check if we have a segment that is important because of its features
	else
	{
		for (int n = 0; n < int(myPageRank.aResponse.allEntities.size()); n++)
		{
			if (myPageRank.aResponse.allEntities[n].special)
			{
				//cout << "Marking important segment for SPECIAL" << endl;
				importantSegment = true;
				importantSegmentPos = myPageRank.aResponse.allEntities[n].segmentPosition;
			}
		}
		if (!(importantSegment))
		{
			int highestFeatureCount = 0;
			int secondHighestFeatureCount = 0;
			for (int o = 0; o < int(myPageRank.aResponse.allEntities.size()); o++)
			{
				if ((myPageRank.aResponse.allEntities[o].featureCount > highestFeatureCount) &&
					(highestFeatureCount == 0))
				{
					highestFeatureCount = myPageRank.aResponse.allEntities[o].featureCount;
					secondHighestFeatureCount = highestFeatureCount;
				}
				else if (myPageRank.aResponse.allEntities[o].featureCount > highestFeatureCount)
				{
					secondHighestFeatureCount = highestFeatureCount;
					highestFeatureCount = myPageRank.aResponse.allEntities[o].featureCount;
				}
				else
					continue;
			}
			if (highestFeatureCount > secondHighestFeatureCount)
			{
				//cout << "Getting into HighestFeatureCount > SecondHighestFeatureCount" << endl;
				for (int r = 0; r < int(myPageRank.aResponse.allEntities.size()); r++)
				{
					if (myPageRank.aResponse.allEntities[r].featureCount = highestFeatureCount)
					{
						//cout << "Marking important segment for FEATURE COUNT" << endl;
						importantSegment = true;
						importantSegmentPos = myPageRank.aResponse.allEntities[r].segmentPosition;
					}
					else
						continue;
				}
			}
		}
		if (!(importantSegment))
		{
			double highestFeatureWeight = 0.00;
			double secondHighestFeatureWeight = 0.00; 
			for (int p = 0; p < int(myPageRank.aResponse.allEntities.size()); p++)
			{
				if ((myPageRank.aResponse.allEntities[p].totalWeight > highestFeatureWeight) &&
					(highestFeatureWeight == 0))
				{
					highestFeatureWeight = myPageRank.aResponse.allEntities[p].totalWeight;
					secondHighestFeatureWeight = highestFeatureWeight;
				}
				else if (myPageRank.aResponse.allEntities[p].totalWeight > highestFeatureWeight)
				{
					secondHighestFeatureWeight = highestFeatureWeight;
					highestFeatureWeight = myPageRank.aResponse.allEntities[p].totalWeight;
				}
				else
					continue;
			}
			if (highestFeatureWeight > (secondHighestFeatureWeight + (secondHighestFeatureWeight / 2)))
			{
				//cout << "Getting into HighestFeatureWeight > SecondHighestFeatureWeight + 50%" << endl;
				for (int q = 0; q < int(myPageRank.aResponse.allEntities.size()); q++)
				{
					if (myPageRank.aResponse.allEntities[q].totalWeight == highestFeatureWeight)
					{
						//cout << "Marking important segment for TOTAL WEIGHT" << endl;
						importantSegment = true;
						importantSegmentPos = myPageRank.aResponse.allEntities[q].segmentPosition;
					}
					else
						continue;
				}
			}
		}
		if (importantSegment)  // there is a importantSegment that is based on number of features or their total weight
		{
			Generation::segmentOrganization(curPosition, trendOrder, importantSegmentPos);
		}
		// if not, check if we can organize based on comparison of features (contrasting) - call function checkIfComparable
		else if (Generation::checkIfComparable(trendOrder))
		{
			Generation::segmentComparisonOrganization(curPosition, trendOrder);
		}
		else
		{
			Generation::segmentOrganization(curPosition, trendOrder, trendOrder[0]);
		}
	}
	cout << "Leaving responseOrganization" << endl;
}


/* This method is reponsible for filling up myOrganizedSet with the propositions that belong to the
 * overall graph (it doesn't handle propositions from particular segments).
 * @param: numberOfSecondary - number of secondary messages in a response
 *		   maxPosition - maximum number of propositions in the overall graph (we get this from myGraph.size())
 *		   suggestionPos - the position of a suggestion
 * returns: suggestionPos incremented or not
 */
int Generation::overallGraphOrganization(int numberOfSecondary, int maxPosition, int suggestionPos)
{
	for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		if (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0].compare("line graph") == 0)
		{
			//cout << "Linegraph prop: " << i << endl;
			//cout << "Prop: " << myPageRank.finalRankedNodes[i].aNode.myProposition.description << endl;
			// introduction block
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_type") == 0)
				myOrganizedSet.insert(pair<int,int> (1,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("entity_description") == 0)
				myOrganizedSet.insert(pair<int,int> (2,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0)
			{
				myOrganizedSet.insert(pair<int,int> ((suggestionPos + 3),i));
				suggestionPos++;
			}
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_volatility") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 4),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_value") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 5),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_end_value") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 6),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_date") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 7),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_years") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 8),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_months") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 9),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_days") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 10),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_end_date") == 0)
				myOrganizedSet.insert(pair<int,int> ((numberOfSecondary + 11),i));
			// conclusion block
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_behaviour") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_absolute_change") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+1,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_rate_change") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+2,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_value") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+3,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_date") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+4,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("minimum_point_value") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+5,i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("minimum_point_date") == 0)
				myOrganizedSet.insert(pair<int,int> (maxPosition+6,i));
		}
		else
			continue;
	}
	return suggestionPos;
}



/* This method stores the segments information in the OrganizedSet map. It orders the segments' related
 * propositions but it is only called when we have a main segment that needs to be mentioned first and then
 * th other segments, if they exist.
 * @param: curPosition - current position to be inserted on the map
 *		   intention - the unique description of the intention, so the membeership fields can be searched
 *		   trendOrder - vector that contains the segments positions
 */
void Generation::segmentOrganization(int curPosition, vector<string> trendOrder, string importantSegmentPos)
{
	for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare(importantSegmentPos) == 0)
		{
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0)
			{
				myOrganizedSet.insert(pair<int,int> (curPosition,i));
				// stoting its name to use when generating surge templates			
				mostImportantSegment = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
			}
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("volatility") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 1),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("steepness") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 2),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_value") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 3),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_value") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 4),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_absolute_change") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 5),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_rate_change") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 6),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_date") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 7),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_years") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 8),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_months") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 9),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_days") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 10),i));
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_date") == 0)
				myOrganizedSet.insert(pair<int,int> ((curPosition + 11),i));
		}
	}
	if (trendOrder.size() > 0)	// if there are more segments other than the ones on the intention
	{
		for (int k = 0; k < int(trendOrder.size()); k++)
		{
			if (trendOrder[k].compare(importantSegmentPos) != 0) // only ordering now propositions that are NOT part of the main segment
			{
				for (int l = 0; l < int(myPageRank.finalRankedNodes.size()); l++)
				{
					//check if order of proposition is the one on the vector of segment positions
					if (myPageRank.finalRankedNodes[l].aNode.myProposition.segmentPosition.compare(trendOrder[k]) == 0)
					{
						if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_description") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 0),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("volatility") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 1),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("steepness") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 2),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_initial_value") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 3),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_end_value") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 4),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_absolute_change") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 5),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_rate_change") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 6),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_initial_date") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 7),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_overall_period_years") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 8),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_overall_period_months") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 9),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_overall_period_days") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 10),l));
						else if (myPageRank.finalRankedNodes[l].aNode.myProposition.type.compare("trend_end_date") == 0)
							myOrganizedSet.insert(pair<int,int> ((curPosition + (12 * (k + 1)) + 11),l));
					}
					else
						continue;
				}
			}
			else
				continue;
		}
	}
}


/* This method will organize some or all of the features selected in the response based on comparison.
 * It is called only if the method checkIfComparable returns true.
 * The comparison based organization is organized as follows:
 * 1 - trend_description, trend_initial_value, trend_initial_date, trend_end_value, trend_end_date will
 *		form the first type of referring expression of a segment (and that will be time-wise) so that
 *		the following features can be compared.
 * 2 - volatility and steepness are compared having the referring expression as First, Second, etc (those
 *		will be used for all the following comparisons)
 * 3 - overall period (that comprises any type of trend_overall_period - year, month, day). They can be
 *		combined in any way
 * 4 - trend value change (which comprises both absolute and rate change)
 * if only part of those features are comparable, the remaining ones will be conveyed in the order
 * of their segments, after all the comparable ones were conveyed.
 */
void Generation::segmentComparisonOrganization(int curPosition, vector<string> trendOrder)
{
	comparableSets_it = comparableSets.begin();
	int counter = curPosition;
	cout << "------- Calling comparison based organization --------" << endl;
	cout << "Trend order size: " << trendOrder.size() << endl;
	for (int j = 0; j < int(trendOrder.size()); j++)
	{
		cout << "Getting features of trend order number: " << trendOrder[j] << endl;
		for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
		{
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare(trendOrder[j]) == 0)
			{
				cout << "Feature: " << myPageRank.finalRankedNodes[i].aNode.myProposition.type << 
					"Detail: " << myPageRank.finalRankedNodes[i].aNode.myProposition.description << endl;
				if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0)
				{
					myOrganizedSet.insert(pair<int,int> (curPosition,i));
					//counter++;
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("volatility") == 0)
				{
					comparableSets_it = comparableSets.find("volatility");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 1),i));
						//counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("steepness") == 0)
				{
					comparableSets_it = comparableSets.find("steepness");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 2),i));
						//counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_value") == 0)
				{
					myOrganizedSet.insert(pair<int,int> ((curPosition + 3),i));
					//counter++;
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_value") == 0)
				{
					myOrganizedSet.insert(pair<int,int> ((curPosition + 4),i));
					//counter++;
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_absolute_change") == 0)
				{
					comparableSets_it = comparableSets.find("trend_absolute_change");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 5),i));
					//	counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_rate_change") == 0)
				{
					comparableSets_it = comparableSets.find("trend_rate_change");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 6),i));
						//counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_date") == 0)
				{
					myOrganizedSet.insert(pair<int,int> ((curPosition + 7),i));
					//counter++;
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_years") == 0)
				{
					comparableSets_it = comparableSets.find("trend_overall_period_years");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 8),i));
					//	counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_months") == 0)
				{
					comparableSets_it = comparableSets.find("trend_overall_period_months");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 9),i));
					//	counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_days") == 0)
				{
					comparableSets_it = comparableSets.find("trend_overall_period_days");
					if (comparableSets_it == comparableSets.end())
					{
						myOrganizedSet.insert(pair<int,int> ((curPosition + 10),i));
					//	counter++;
					}
				}
				else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_date") == 0)
				{
					myOrganizedSet.insert(pair<int,int> ((curPosition + 11),i));
					//counter++;
				}
				else
					continue;
			}
		}
		curPosition = curPosition + 12;
	}
	//cout << "Finished with trends introduction with not comparable features" << endl;
	for (int t = 0; t < int(trendOrder.size()); t++)
	{
		// now we need to order the comparable volatility and steepness if they exist on the comparableSets
		comparableSets_it = comparableSets.find("volatility");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("volatility");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int k = 0; k < int(candidateComparableFeatures_it->second.size()); k++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[k].first.compare(trendOrder[t]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[k].second));
					}
					else
						continue;
				}
			}
		}
		comparableSets_it = comparableSets.find("steepness");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("steepness");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int l = 0; l < int(candidateComparableFeatures_it->second.size()); l++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[l].first.compare(trendOrder[t]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[l].second));
					}
					else
						continue;
				}
			}
		}
		//curPosition = counter;
	}
	for (int m = 0; m < int(trendOrder.size()); m++)
	{
		// now we need to put the comparable absolute_change and rate_change if they exist on the comparableSets
		comparableSets_it = comparableSets.find("trend_absolute_change");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("trend_absolute_change");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int n = 0; n < int(candidateComparableFeatures_it->second.size()); n++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[n].first.compare(trendOrder[m]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[n].second));
					}
					else
						continue;
				}
			}
		}
		comparableSets_it = comparableSets.find("trend_rate_change");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("trend_rate_change");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int o = 0; o < int(candidateComparableFeatures_it->second.size()); o++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[o].first.compare(trendOrder[m]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[o].second));
					}
					else
						continue;
				}
			}
		}
	}
	//curPosition = counter;
	for (int p = 0; p < int(trendOrder.size()); p++)
	{
		// now we need to put the comparable absolute_change and rate_change if they exist on the comparableSets
		comparableSets_it = comparableSets.find("trend_overall_period_years");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_years");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int q = 0; q < int(candidateComparableFeatures_it->second.size()); q++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[q].first.compare(trendOrder[p]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[q].second));
					}
					else
						continue;
				}
			}
		}
		comparableSets_it = comparableSets.find("trend_overall_period_months");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_months");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int r = 0; r < int(candidateComparableFeatures_it->second.size()); r++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[r].first.compare(trendOrder[p]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[r].second));
					}
					else
						continue;
				}
			}
		}
		comparableSets_it = comparableSets.find("trend_overall_period_days");
		if (comparableSets_it != comparableSets.end())
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_days");
			if (candidateComparableFeatures_it != candidateComparableFeatures.end())
			{
				//getting inside the vector of segment positions
				for (int s = 0; s < int(candidateComparableFeatures_it->second.size()); s++)
				{
					//comparing the segment position to get the proposition position at finalRankedNodes (2nd element)
					if (candidateComparableFeatures_it->second[s].first.compare(trendOrder[p]) == 0)
					{
						curPosition++;
						myOrganizedSet.insert(pair<int,int> (curPosition,candidateComparableFeatures_it->second[s].second));
					}
					else
						continue;
				}
			}
		}
	}
}



/* This method receives the intention unique identifier and founds the main segment position.
 * It is only called when we have an intention that is a single segment (RT, FT, ST, BF, BJ) 
 * @param: intention - the unique identifier of the intention
 */
string Generation::getImportantSegmentPosition(string intention)
{
	string position = "";
	for (int i = 0; i < int(myPageRank.myGraph.size()); i++)
	{
		if ((myPageRank.myGraph[i].myProposition.type.compare("trend_description") == 0) &&
			(myPageRank.checkIfBelongs(myPageRank.myGraph[i], intention)))
		{
			// storing its name to use when generating surge templates
			mostImportantSegment = myPageRank.myGraph[i].myProposition.detail;
			position = myPageRank.myGraph[i].myProposition.segmentPosition;
			break;
		}
		else
			continue;
	}
	return position;
}


/* This method is responsible for checking into myPageRank.aResponse and myPageRank.finalRankedNodes
 * and defining if the propositions selected in the current response are eligible for the organization
 * that is based on the comparison of segments' features.
 * All the features that can be compared will have their types inserted into the vector candidateComparableFeatures
 * along with the segment order numbers that can be compared
 * @param: trendOrder - vector that contains the segment order numbers
 * It returns true if they are comparable and false otherwise
 */
bool Generation::checkIfComparable(vector<string> trendOrder)
{
	int numberOfTrends = trendOrder.size();
	vector<pair<string,int>> tempSegmentVector;
	candidateComparableFeatures_it = candidateComparableFeatures.begin();	
	comparableSets_it = comparableSets.begin();
	for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		if (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0].compare("line graph") != 0)
		{
			candidateComparableFeatures_it = candidateComparableFeatures.find(myPageRank.finalRankedNodes[i].aNode.myProposition.type);
			if (candidateComparableFeatures_it == candidateComparableFeatures.end())
			{
				tempSegmentVector.push_back(pair<string,int>(myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition,i));
				candidateComparableFeatures.insert(pair<string,vector<pair<string,int>>>(myPageRank.finalRankedNodes[i].aNode.myProposition.type,tempSegmentVector));
			}
			else
			{
				candidateComparableFeatures_it->second.push_back(pair<string,int>(myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition,i));
			}
			tempSegmentVector.clear();
		}
		else
			continue;
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("volatility");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		comparableSets.insert("volatility");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("steepness");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("steepness");
		comparableSets.insert("steepness");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("trend_absolute_change");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("trend_absolute_change");
		comparableSets.insert("trend_absolute_change");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("trend_rate_change");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("trend_rate_change");
		comparableSets.insert("trend_rate_change");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_years");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("trend_overall_period_years");
		comparableSets.insert("trend_overall_period_years");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_months");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("trend_overall_period_months");
		comparableSets.insert("trend_overall_period_months");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	candidateComparableFeatures_it = candidateComparableFeatures.find("trend_overall_period_days");
	if ((candidateComparableFeatures_it != candidateComparableFeatures.end()) &&
		(candidateComparableFeatures_it->second.size() == numberOfTrends))
	{
		//comparableSets.push_back("trend_overall_period_days");
		comparableSets.insert("trend_overall_period_days");
		sort(candidateComparableFeatures_it->second.begin(), candidateComparableFeatures_it->second.end());
	}
	if (comparableSets.size() > 1)
		return true;
	else
		return false;
}



/* This method adds the FUFSURGE templates to the proposition nodes in the field myRealization. myRealization
 * is a field of the proposition struct and it is a vector, meaning that we might have more than one template
 * in order to realize a proposition sentence.
 */
void Generation::addFufSurgeTemplates()
{
	string surgeTemplate, surgeTemplateAux, variable1, variable2, variable3, entity1, entity2, entity3;
	string tempLastTrend, tempLastTrend2, position, segment, tempUniqueTrend, tempInitialDate, tempEndDate;
	int compRelationType, i, j;
	int numberOfFT = 0;
	int numberOfRT = 0;
	int numberOfST = 0;
	int numberOfBF = 0;
	int numberOfBJ = 0;
	exceptionOnGraphDates = false;
	// trendClassifier is going to have either (definite yes/no) or (determiner none)(classifier === another)
	// trendPosition is responsible for appending "second", "third", etc to the trends that are not the first on 
	// their behavior
	string trendClassifier, trendPosition;
	realization aRealization;
	variable1 = "show";
	bool hasScaleUnit = false;
	// the following two boolean variables are responsible for defining the determiner of the trends
	bool introducedTrends = false;
	//bool introducedTrendsHelper = false;
	string scaleUnit;
	myPageRank.trendNameMap_it = myPageRank.trendNameMap.begin();
	cout << "Passing initiation of Map" << endl;
	for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
	//for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		i = myOrganizedSet_it->second;
		if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_type") == 0)
		{
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.detail.size() == 0)
				hasScaleUnit = false;
			else
			{
				hasScaleUnit = true;
				scaleUnit = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
			}
			variable1 = "show";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes)(lex image)))(affected((cat common)(number singular) (definite no) (lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("entity_description") == 0)
		{
			variable1 = "present";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			if (!(myPageRank.finalRankedNodes[i].aNode.myProposition.detail.compare("") == 0))
			{
				entity2.append(" in ");
				entity2.append(myPageRank.finalRankedNodes[i].aNode.myProposition.detail);
			}
			surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes) (number singular) (lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat proper)(determiner none)(lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_volatility") == 0)
		{
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)(mode attributive)))");
			surgeTemplate.append("(partic((carrier((cat common)(definite yes)(lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(attribute((cat common)(determiner none)(lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_behaviour") == 0)
		{
			variable1 = "show";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type material)(voice passive)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes) (number singular) (lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat proper)(definite no) (lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\")(describer((cat adj)(lex overall))))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0)
		{
			variable1 = "show";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.myHelper.getSuggestionName(myPageRank.finalRankedNodes[i].aNode.myProposition.description);
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.membership[1].compare(myPageRank.aResponse.intention.second) == 0)
			{
				if (myPageRank.totalNumberOfTrends != (myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() / 2))
				{
					for (j = 0; j < int(myPageRank.myGraph.size()); j++)
					{
						if (myPageRank.myGraph[j].myProposition.type.compare("graph_initial_date") == 0)
							tempInitialDate = myPageRank.myGraph[j].myProposition.description;
						else if (myPageRank.myGraph[j].myProposition.type.compare("graph_end_date") == 0)
							tempEndDate = myPageRank.myGraph[j].myProposition.description;
						else
							continue;
					}
					exceptionOnGraphDates = true;
				}
				// this template is being stored on the variable surgeTemplateAux and it will only be called
				// if there is the exception of the intention not be covering the whole graph trends
				// it will be appended at the end of the intention template when it is the case
				surgeTemplateAux.append("(circum ((concession((cat pp)(position front)(prep === although)(mood concession-mood)(np ((cat clause) "
									 "(proc((type material)(lex span)))(partic((agent((cat common)(lex \\\"");
				surgeTemplateAux.append(entity1);
				surgeTemplateAux.append("\\\")))(affected((cat pp)(prep === from)(np((cat common)(determiner none)(lex \\\"");
				surgeTemplateAux.append(tempInitialDate);
				surgeTemplateAux.append("\\\")(qualifier((cat pp)(prep === to)(np((cat common)(determiner none)(lex \\\"");
				surgeTemplateAux.append(tempEndDate);
				surgeTemplateAux.append("\\\"))))))))))))))))))");

				//it is the intention and it is a simple trend, the dates need to be added here
				if	((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("RT") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("FT") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("ST") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("BF") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("BJ") == 0))
				{
					surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
					surgeTemplate.append(variable1);
					surgeTemplate.append("\\\")))(partic((agent((cat common)(number singular)");
					if (exceptionOnGraphDates)
					{
						surgeTemplate.append("(determiner none)(lex \\\"");
						surgeTemplate.append("it");
					}
					else
					{
						surgeTemplate.append("(definite yes) (lex \\\"");
						surgeTemplate.append(entity1);
					}
					surgeTemplate.append("\\\")))(affected((cat common)(definite no) (lex \\\"");
					surgeTemplate.append(entity2);
					surgeTemplate.append("\\\")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np((cat common)(lex \\\"");
					surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].first);
					surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === to)(np((cat common)(lex \\\"");
					surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].second);
					surgeTemplate.append("\\\")(determiner none))))))))))))");
					if (exceptionOnGraphDates)
						surgeTemplate.append(surgeTemplateAux);
					else
						surgeTemplate.append(")");
					aRealization.fufSurgeFrame = surgeTemplate;
					aRealization.partOfSentence = "object";
					aRealization.partOfSpeech = "noun";
					myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
					surgeTemplate.clear();
				}
				// it is the intention but it is a complex one, the dates will be added to the proposition
				// that details the trends of the intention
				// !!!TODO: still need to append date exceptions on the graph, to the intention 
				else
				{
					if (entity2.compare("changing trend") == 0)
						entity3 = "trend that changes";
					else if (entity2.compare("changing trend that returns") == 0)
						entity3 = "trend that changes and then reverses back";
					else 
						entity3 = entity2;
					surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
					surgeTemplate.append(variable1);
					surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes) (number singular) (lex \\\"");
					surgeTemplate.append(entity1);
					surgeTemplate.append("\\\")))(affected((cat common)(definite no) (lex \\\"");
					surgeTemplate.append(entity3);
					surgeTemplate.append("\\\"))))))");
					aRealization.fufSurgeFrame = surgeTemplate;
					aRealization.partOfSentence = "object";
					aRealization.partOfSpeech = "noun";
					myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
					surgeTemplate.clear();
					variable1 = "consist";
					// this block is responsible for listing all the trends of a complex composed trend and the start and end dates
					// the structure of the proposition changes if we have CHT, CTR, etc
					if ((myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() > 0) &&
						(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("CTR") == 0))
					{
						introducedTrends = true;
						surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
						surgeTemplate.append(variable1);
						surgeTemplate.append("\\\")))(partic((agent((cat common)(number singular)(definite yes)(lex \\\"");
						surgeTemplate.append(entity2);
						surgeTemplate.append("\\\")))(affected((complex conjunction)(distinct ~(((cat pp)(prep === of)(np((cat common)(definite no)(lex \\\"");
						// append description of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first);
						surgeTemplate.append("\\\")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np((cat common)(lex \\\"");
						// append start date of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].first);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === to)(np((cat common)(lex \\\"");
						// append end date of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))((cat common)(definite no)(lex \\\"");
						// append description of second trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].first);
						surgeTemplate.append("\\\")(qualifier((cat pp)(prep === until)(np((cat common)(lex \\\"");
						// append end date of second trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[3].second);
						surgeTemplate.append("\\\")(determiner none))))))((cat common)(definite no)(lex \\\"");
						// append description of third trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[4].first);
						surgeTemplate.append("\\\")(qualifier((cat pp)(prep === through)(np((cat common)(lex \\\"");
						// append end date of third trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[5].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))))");
						aRealization.fufSurgeFrame = surgeTemplate;
						aRealization.partOfSentence = "object";
						aRealization.partOfSpeech = "noun";
						myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
						surgeTemplate.clear();
					}
					else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() > 0) &&
						(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("CSCT") == 0))
					{
						tempLastTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[4].first;
						tempLastTrend2 = tempLastTrend;
						tempLastTrend2.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[4].second);
						//cout << "tempLastTrend" << tempLastTrend << endl;
						//cout << "tempLastTrend2" << tempLastTrend2 << endl;
						if (tempLastTrend.compare("rising trend") == 0)
						{
							myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend2);
							myPageRank.trendNameMap_it->second = "short rise";
						}
						else if (tempLastTrend.compare("falling trend") == 0)
						{
							myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend2);
							myPageRank.trendNameMap_it->second = "short fall";
						}										
						introducedTrends = true;
						surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
						surgeTemplate.append(variable1);
						surgeTemplate.append("\\\")))(partic((agent((cat common)(number singular)(definite yes)(lex \\\"");
						surgeTemplate.append(entity2);
						surgeTemplate.append("\\\")))(affected((complex conjunction)(distinct ~(((cat pp)(prep === of)(np((cat common)(definite no)(lex \\\"");
						// append description of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first);
						surgeTemplate.append("\\\")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np((cat common)(lex \\\"");
						// append start date of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].first);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === to)(np((cat common)(lex \\\"");
						// append end date of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))((cat common)(definite no)(lex \\\"");
						// append description of second trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].first);
						surgeTemplate.append("\\\")(qualifier((cat pp)(prep === until)(np((cat common)(lex \\\"");
						// append end date of second trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[3].second);
						surgeTemplate.append("\\\")(determiner none))))))((cat common)(definite no)(lex \\\"");
						// append description of third trend
						//getting description from trendNameMap
						tempLastTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[4].first;
						tempLastTrend.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[4].second);
						myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend);
						surgeTemplate.append(myPageRank.trendNameMap_it->second);
						surgeTemplate.append("\\\")(qualifier((cat pp)(prep === through)(np((cat common)(lex \\\"");
						// append end date of third trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[5].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))))");
						aRealization.fufSurgeFrame = surgeTemplate;
						aRealization.partOfSentence = "object";
						aRealization.partOfSpeech = "noun";
						myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
						surgeTemplate.clear();
					}
					else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() > 0) &&
						(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("CHT") == 0))
					{
						introducedTrends = true;
						surgeTemplate.clear();
						surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
						surgeTemplate.append(variable1);
						surgeTemplate.append("\\\")))(partic((agent((cat common)(lex \\\"");
						surgeTemplate.append(entity2);
						surgeTemplate.append("\\\")(number singular)(definite yes)))(affected((cat pp)(prep === of)(np((cat common)(definite no)(lex \\\"");
						// append description of first trend
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first);
						surgeTemplate.append("\\\")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].first);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === to)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].second);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === \\\"followed by\\\")(np((cat common)(lex \\\"");
						// append description of last trend
						//getting description from trendNameMap
						tempLastTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].first;
						tempLastTrend.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].second);
						myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend);
						surgeTemplate.append(myPageRank.trendNameMap_it->second);
						surgeTemplate.append("\\\")(definite no))))((cat pp)(prep === through)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[3].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))))))");
						aRealization.fufSurgeFrame = surgeTemplate;
						aRealization.partOfSentence = "object";
						aRealization.partOfSpeech = "noun";
						myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
						surgeTemplate.clear();
					}
					else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() > 0) &&
						(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("CTLS") == 0))
					{
						tempLastTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].first;
						tempLastTrend2 = tempLastTrend;
						tempLastTrend2.append( myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].second);
						if (tempLastTrend.compare("rising trend") == 0)
						{
							myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend2);
							myPageRank.trendNameMap_it->second = "short rise";
						}
						else if (tempLastTrend.compare("falling trend") == 0)
						{
							myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend2);
							myPageRank.trendNameMap_it->second = "short fall";
						}	
						introducedTrends = true;
						surgeTemplate.clear();
						surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
						surgeTemplate.append(variable1);
						surgeTemplate.append("\\\")))(partic((agent((cat common)(lex \\\"");
						surgeTemplate.append(entity2);
						surgeTemplate.append("\\\")(number singular)(definite yes)))(affected((cat pp)(prep === of)(np((cat common)(definite no)(lex \\\"");
						// first trend description
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first);
						surgeTemplate.append("\\\")(qualifier((cat list)(distinct ~(((cat pp)(prep === from)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].first);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === to)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[1].second);
						surgeTemplate.append("\\\")(determiner none))))((cat pp)(prep === \\\"followed by\\\")(np((cat common)(lex \\\"");
						// second trend description
						tempLastTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].first;
						tempLastTrend.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[2].second);
						myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempLastTrend);
						surgeTemplate.append(myPageRank.trendNameMap_it->second);
						surgeTemplate.append("\\\")(definite no))))((cat pp)(prep === through)(np((cat common)(lex \\\"");
						surgeTemplate.append(myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[3].second);
						surgeTemplate.append("\\\")(determiner none)))))))))))))))");
						aRealization.fufSurgeFrame = surgeTemplate;
						aRealization.partOfSentence = "object";
						aRealization.partOfSpeech = "noun";
						myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
						surgeTemplate.clear();
					}
				}
				surgeTemplateAux.clear();
				// still need to check if there is only the intention but there are more trends on the graph
			}
			// here is the description of secondary messages, no dates are added
			else
			{
				string intentionName;
				intentionName = myPageRank.myHelper.getSuggestionName(myPageRank.aResponse.intention.first);
				compRelationType = myPageRank.getRelationBetweenSuggestions(myPageRank.aResponse.intention.second, myPageRank.finalRankedNodes[i].aNode.myProposition.membership[1]);
				// * 1 - Intention is single trend and it belongs to a secondary that is complex (more than one trend)
				if (compRelationType == 1)
				{
					surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
					surgeTemplate.append("belong");
					surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes) (number singular) (lex \\\"");
					surgeTemplate.append(intentionName);
					surgeTemplate.append("\\\")))(affected((cat pp)(prep === to)(np((definite no)(lex \\\"");
					surgeTemplate.append(entity2);
					surgeTemplate.append("\\\"))))))))");
					aRealization.fufSurgeFrame = surgeTemplate;
					aRealization.partOfSentence = "object";
					aRealization.partOfSpeech = "noun";
					myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
					surgeTemplate.clear();
					//introducedTrendsHelper = true;
				}
				/* 2 - Intention is single trend and it does NOT belong to a secondary that is complex
				 * 4 - Both are simple, so they are separate
				 * 7 - Both are complex and they are completely separate
				 * 8 - Intention is complex and the secondary is simple and DOES NOT belong to the intention
				 */
				else if ((compRelationType == 2) || (compRelationType == 4) || (compRelationType == 7) || (compRelationType == 8))
				{
					surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
					surgeTemplate.append("also show");
					surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes) (number singular) (lex \\\"");
					surgeTemplate.append(entity1);
					surgeTemplate.append("\\\")))(affected((cat common)(definite no) (lex \\\"");
					surgeTemplate.append(entity2);
					surgeTemplate.append("\\\"))))))");
					aRealization.fufSurgeFrame = surgeTemplate;
					aRealization.partOfSentence = "object";
					aRealization.partOfSpeech = "noun";
					myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
					surgeTemplate.clear();
				}
				// * 3 - Intention is complex and contains a secondary that is simple
				else if (compRelationType == 3)
				{
					surgeTemplate.append("((cat clause)(proc((type ascriptive)(mode attributive)))");
					surgeTemplate.append("(partic((carrier((cat common)(definite yes)(lex \\\"");
					surgeTemplate.append(entity2);
					surgeTemplate.append("\\\")))(attribute((cat common)(determiner none)(lex significant))))))");
					aRealization.fufSurgeFrame = surgeTemplate;
					aRealization.partOfSentence = "object";
					aRealization.partOfSpeech = "noun";
					myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
					surgeTemplate.clear();
				}
				else if (compRelationType == 5)
				{
					cout << "* 5 - Both are complex and the intention contains the secondary suggestion" << endl;
					// existencial
				}
				else if (compRelationType == 6)
				{
					cout << "* 6 - Both are complex and they have some overlap" << endl;
				}
				else if (compRelationType == 9)
				{
					cout << "* 9 - Both are complex and the suggestion contains the intention" << endl;
				}
			}
		}
		else if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_description") == 0)
		{
			string definite;
			myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(myPageRank.finalRankedNodes[i].aNode.myProposition.detail);
			//entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			entity1 = myPageRank.trendNameMap_it->second;
			// TODO: redo this block adding appropriate surge templates (ordinal numbers and relative clauses)
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("1") == 0)
				position = "first";
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("2") == 0)
				position = "second";
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("3") == 0)
				position = "third";
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("4") == 0)
				position = "fourth";
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("5") == 0)
				position = "fifth";
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.segmentPosition.compare("6") == 0)
				position = "sixth";
			if ((mostImportantSegment.compare(myPageRank.finalRankedNodes[i].aNode.myProposition.detail) == 0) &&
				(position.compare("first") != 0))
			{
				position.append(" segment, which is");
				segment = "the most important one,";
			}
			else
				segment = "segment";
			// this is defining the determiner of a trend (the trends can have been introduced or not)
			if ((introducedTrends) || (segment.compare("the most important one,") == 0))
				definite = "yes";
			else
				definite = "no";
			/* this block is taking care of the classifier of a trend if the trend is not the 
			 * first rising, falling, etc to appear */
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("rising trend") == 0)
				numberOfRT++;
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("falling trend") == 0)
				numberOfFT++;
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("stable trend") == 0)
				numberOfST++;
			else if (myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("big jump") == 0)
				numberOfBJ++;
			else
				numberOfBF++;
			if (((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("rising trend") == 0) &&
				(numberOfRT > 1)) ||
				((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("falling trend") == 0) &&
				(numberOfFT > 1)) ||
				((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("stable trend") == 0) &&
				(numberOfST > 1)) ||
				((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("big jump") == 0) &&
				(numberOfBJ > 1)) ||
				((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("big fall") == 0) &&
				(numberOfBF > 1)))
				trendClassifier = "(determiner none)(classifier === another)";
			else
			{
				trendClassifier = "(definite ";
				trendClassifier.append(definite);
				trendClassifier.append(")");
			}
			//-------------------------------end of trend classifier definition------------------------------------
			cout << "Size of additional Info: " << myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() << endl;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)))(partic((identified((cat common)(definite yes)(number singular)(lex \\\"");
			surgeTemplate.append(segment);
			surgeTemplate.append("\\\")(classifier === \\\"");
			surgeTemplate.append(position);
			surgeTemplate.append("\\\")))(identifier((cat common)");
			surgeTemplate.append(trendClassifier);
			surgeTemplate.append("(lex \\\"");
			surgeTemplate.append(entity1);
			//if (myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo.size() > 0)
			//	surgeTemplate.append("\\\")(qualifier ((cat pp)(prep === with)(np ((cat common)(determiner none)(lex exceptions)(classifier === some))))))))))");
			//else
			//	surgeTemplate.append("\\\"))))))");
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
			trendClassifier.clear();
			/* now I need to change the description of trends that are not the first one on their behavior
			 * the second rising trend needs to be named as "second rising trend" when describing all of its
			 * features from now on. A method to make their names go back to the original is needed*/
			tempUniqueTrend = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("rising trend") == 0) &&
				(numberOfRT > 1))
			{
				if (numberOfRT == 2)
					trendPosition = "second rising trend";
				else if (numberOfRT == 3)
					trendPosition = "third rising trend";
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempUniqueTrend);
				myPageRank.trendNameMap_it->second = trendPosition;
			}
			else if	((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("falling trend") == 0) &&
				(numberOfFT > 1))
			{
				if (numberOfFT == 2)
					trendPosition = "second falling trend";
				else if (numberOfFT == 3)
					trendPosition = "third falling trend";
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempUniqueTrend);
				myPageRank.trendNameMap_it->second = trendPosition;
			}
			else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("stable trend") == 0) &&
				(numberOfST > 1))
			{
				if (numberOfST == 2)
					trendPosition = "second stable trend";
				else if (numberOfST == 3)
					trendPosition = "third stable trend";
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempUniqueTrend);
				myPageRank.trendNameMap_it->second = trendPosition;			
			}
			else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("big jump") == 0) &&
				(numberOfBJ > 1))
			{
				if (numberOfBJ == 2)
					trendPosition = "second big jump";
				else if (numberOfBJ == 3)
					trendPosition = "third big jump";
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempUniqueTrend);
				myPageRank.trendNameMap_it->second = trendPosition;			
			}
			else if	((myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare("big fall") == 0) &&
				(numberOfBF > 1))
			{
				if (numberOfBF == 2)
					trendPosition = "second big fall";
				else if (numberOfRT == 3)
					trendPosition = "third big fall";
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(tempUniqueTrend);
				myPageRank.trendNameMap_it->second = trendPosition;
			}
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("volatility") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("steepness") == 0))
		{
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
			entity1 = myPageRank.trendNameMap_it->second;
			//entity1 = entity1.substr(0, entity1.size() - 1);
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.detail;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)))(partic((identified((cat common)(definite yes)(number singular)(lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(identifier((cat common)(determiner none)(lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_rate_change") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_rate_change") == 0))
		{
			variable1 = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first;
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			// taking care of trend position identification that comes at the end of its description in the membership field
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_rate_change") == 0)
			{
			//	entity1 = entity1.substr(0, entity1.size() - 1);
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
				entity1 = myPageRank.trendNameMap_it->second;
			}
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type material)(mode creative)(tense past)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes)(lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat measure)(quantity((value ");
			surgeTemplate.append(entity2);
			surgeTemplate.append(")))(unit((lex time))))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_absolute_change") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_absolute_change") == 0))
		{
			variable1 = myPageRank.finalRankedNodes[i].aNode.myProposition.additionalInfo[0].first;
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			// taking care of trend position identification that comes at the end of its description in the membership field
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_absolute_change") == 0)
			{
			//	entity1 = entity1.substr(0, entity1.size() - 1);
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
				entity1 = myPageRank.trendNameMap_it->second;
			}
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)(mode equative)))(partic((identified((cat common)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")(definite yes)(number singular)(classifier === overall)(qualifier((cat pp)(prep((lex in)))(np((lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")(cat common)(definite yes)))))))(identifier");
			if (hasScaleUnit)
			{
				surgeTemplate.append("((cat measure)(quantity((value ");
				surgeTemplate.append(entity2);
				surgeTemplate.append(")))(unit((lex \\\"");
				surgeTemplate.append(scaleUnit);
				surgeTemplate.append("\\\"))))))))");
			}
			else
			{
				surgeTemplate.append("((cat common)(determiner none)(lex \\\"");
				surgeTemplate.append(entity2);
				surgeTemplate.append("\\\"))))))");
			}
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_years") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_months") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_days") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_years") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_months") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_days") == 0))
		{
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_years") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_years") == 0))
				entity3 = "year";
			else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_months") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_months") == 0))
				entity3 = "month";
			else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_period_days") == 0) ||
					(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_days") == 0))
				entity3 = "day";
			variable1 = "span";
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			// taking care of trend position identification that comes at the end of its description in the membership field
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_years") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_months") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_overall_period_days") == 0))
			{
				//entity1 = entity1.substr(0, entity1.size() - 1);
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
				entity1 = myPageRank.trendNameMap_it->second;
			}
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat pp)(prep ((lex over)))(np ((cat measure)(quantity((value ");
			surgeTemplate.append(entity2);
			surgeTemplate.append(")))(unit((lex \\\"");
			surgeTemplate.append(entity3);
			surgeTemplate.append("\\\"))))))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_date") == 0) ||
			(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_end_date") == 0) ||
			(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_date") == 0) ||
			(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_date") == 0))
		{
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			// taking care of trend position identification that comes at the end of its description in the membership field
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_date") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_date") == 0))
			{
				//entity1 = entity1.substr(0, entity1.size() - 1);
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
				entity1 = myPageRank.trendNameMap_it->second;
			}
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_date") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_date") == 0))
				variable1 = "initial date";
			else
				variable1 = "end date";
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)))(partic((identified((cat common)(definite yes)(number singular)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")(classifier === \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(identifier((cat common)(determiner none)(lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\"))))))");
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_value") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_value") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_end_value") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_value") == 0))
		{
			variable1 = "have";
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_initial_value") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_value") == 0))
				variable2 = "starting";
			else
				variable2 = "ending";
		/*	The Ent1 has a starting/ending value of Ent2. */
			int ownerPosition = myPageRank.finalRankedNodes[i].aNode.myProposition.membership.size() - 1;
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[ownerPosition];
			// getting rid of trend position identification that comes at the end of its description in the membership field
			if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_initial_value") == 0) ||
				(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("trend_end_value") == 0))
			{
				//entity1 = entity1.substr(0, entity1.size() - 1);
				myPageRank.trendNameMap_it = myPageRank.trendNameMap.find(entity1);
				entity1 = myPageRank.trendNameMap_it->second;
			}
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes)(lex \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat common)(lex value)(definite no)(classifier === \\\"");
			surgeTemplate.append(variable2);
			surgeTemplate.append("\\\")(qualifier((cat pp)(prep ((lex of)))");
			if (hasScaleUnit)
			{
				surgeTemplate.append("(np((cat measure)(quantity((value ");
				surgeTemplate.append(entity2);
				surgeTemplate.append(")))(unit((lex \\\"");
				surgeTemplate.append(scaleUnit);
				surgeTemplate.append("\\\"))))))))))))");
			}
			else
			{
				surgeTemplate.append("(np((cat common)(determiner none)(lex \\\"");
				surgeTemplate.append(entity2);
				surgeTemplate.append("\\\"))))))))))");
			}
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_value") == 0) ||
			(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("minimum_point_value") == 0))

		{
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_value") == 0)
				variable1 = "maximum point";
			else
				variable1 = "minimum point";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
			surgeTemplate.append("((cat clause)(proc((type ascriptive)))(partic((identified((cat common)(definite yes)(number singular)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")(classifier === \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))");
			if (hasScaleUnit)
			{
				surgeTemplate.append("(identifier((cat measure)(quantity((value ");
				surgeTemplate.append(entity2);
				surgeTemplate.append(")))(unit((lex \\\"");
				surgeTemplate.append(scaleUnit);
				surgeTemplate.append("\\\"))))))))");
			}
			else
			{
				surgeTemplate.append("(identifier((cat common)(determiner none)(lex \\\"");
				surgeTemplate.append(entity2);
				surgeTemplate.append("\\\"))))))");
			}
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_date") == 0) ||
			(myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("minimum_point_date") == 0))

		{
			// TODO: if the maximum or minimum date occurs on a DAY, the preposition should change from IN to ON
			variable1 = "occur";
			if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("maximum_point_date") == 0)
				variable2 = "maximum point";
			else
				variable2 = "minimum point";
			entity1 = myPageRank.finalRankedNodes[i].aNode.myProposition.membership[0];
			entity2 = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
		/*	surgeTemplate.append("((cat clause)(proc((type material)(lex \\\"");
			surgeTemplate.append(variable1);
			surgeTemplate.append("\\\")))(partic((agent((cat common)(definite yes)(lex \\\"");
			surgeTemplate.append(variable2);
			surgeTemplate.append("\\\")(number singular)(classifier === \\\"");
			surgeTemplate.append(entity1);
			surgeTemplate.append("\\\")))(affected((cat pp)(prep((lex in)))(np((cat common)(determiner none)(lex \\\"");
			surgeTemplate.append(entity2);
			surgeTemplate.append("\\\"))))))))");
		*/	
			
			aRealization.fufSurgeFrame = surgeTemplate;
			aRealization.partOfSentence = "object";
			aRealization.partOfSpeech = "noun";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		else
		{
			aRealization.fufSurgeFrame = "none";
			aRealization.partOfSentence = "none";
			aRealization.partOfSpeech = "none";
			myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.push_back(aRealization);
		}
		surgeTemplate.clear();
	}
	cout << "Finished adding fuf surge templates" << endl;
}

/*********************************** AGGREGATION METHODS **************************************************/

/* This method is responsible for checking what are the propositions that were selected for the response.
 * It will fill the graphProps and trendProps set and map. For graph propositions I am storing the types of
 * the propositions that are present within a set. For trend, for each new trend, I am storing the segment
 * position of  the trend, which is unique, as the key of the map myTrendProps and the value is a set on
 * which all the propositions present are included.
 * When using it, I only need to check if I can find a specific proposition type within the right set
 */
void Generation::fillPropositionsSelected()
{
	myTrendProps_it = myTrendProps.begin();
	//myTypeValuePropMap_it = myTypeValuePropMap.begin(); //this map will contain proposition type and position of it in the finalRankedNodes vector
	set<string> tempTrendProps;
	string currentPropType;
	string segmentPos;	// number that identifies the trend
 	for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
	{
		currentPropType = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type;
		if ((currentPropType.compare("graph_type") == 0) || (currentPropType.compare("entity_description") == 0) ||
			(currentPropType.compare("graph_volatility") == 0) || (currentPropType.compare("graph_overall_behaviour") == 0) ||
			(currentPropType.compare("graph_absolute_change") == 0) || (currentPropType.compare("graph_rate_change") == 0) ||
			(currentPropType.compare("graph_overall_period_years") == 0) || (currentPropType.compare("graph_overall_period_months") == 0) ||
			(currentPropType.compare("graph_overall_period_days") == 0) || (currentPropType.compare("graph_initial_date") == 0) ||
			(currentPropType.compare("graph_end_date") == 0) || (currentPropType.compare("graph_initial_value") == 0) ||
			(currentPropType.compare("graph_end_value") == 0) || (currentPropType.compare("maximum_point_value") == 0) ||
			(currentPropType.compare("minimum_point_value") == 0) || (currentPropType.compare("maximum_point_date") == 0) ||
			(currentPropType.compare("minimum_point_date") == 0) || (currentPropType.compare("composed_trend") == 0))
			myGraphProps.insert(currentPropType);
		else
		{
			segmentPos = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition;
			myTrendProps_it = myTrendProps.find(segmentPos);
			if (myTrendProps_it == myTrendProps.end())
			{
				myTrendProps.insert(pair<string,set<string>>(segmentPos,tempTrendProps));
				myTrendProps_it = myTrendProps.find(segmentPos);
				myTrendProps_it->second.insert(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type);
			}
			else
			{
				myTrendProps_it = myTrendProps.find(segmentPos);
				myTrendProps_it->second.insert(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type);
			}
		}
	}
	cout << "Finished filling propositions selected" << endl;
}

/* This function just calls the right function for aggregation of selected propositions. It receives the
 * templateNumber which is defined by applying the SMOG readability measure to the text in the article.
 * If there is no available text to measure readability, templateNumber will contain -1 and in this
 * case templateNumber 1 is generated.
 */
void Generation::callFunctionForTemplate(int templateNumber)
{
	switch (templateNumber)
	{
	case 0:
		allSentences = aggregateTemplate0();
		break;
	case 1:
		allSentences = aggregateTemplate1();
		break;
	case 2:
		allSentences = aggregateTemplate2();
		break;
	case 3:
		allSentences = aggregateTemplate3();
		break;
	case 4:
		allSentences = aggregateTemplate4();
		break;
	default:
		allSentences = aggregateTemplate1();
	}	
}

vector<string> Generation::aggregateTemplate0()
{
	vector<string> tempAllSentences;
	return tempAllSentences;
}

vector<string> Generation::aggregateTemplate1()
{
	vector<string> tempAllSentences;
	string tempSentence;
	string tempString = "";
	string tempString1 = "";
	string tempString2 = "";
	string tempVolatility = "";
	string tempGraphVolatility = "";
	string tempEntityDescription = "";
	string tempTrendPosition;
	string tempTrendDetail, tempSteepness, tempRateChange, tempValueChange, tempPeriod;
	string tempInitialValue, tempEndValue;
	string tempScaleUnit, tempPeriodUnit;
	string tempClassifier;
	string tempBehavior;
	bool bInitialValue = false;
	bool bEndValue = false;
	bool bVolatility = false;
	bool bSteepness = false;
	bool bRateChange = false;
	bool bValueChange = false;
	bool bPeriod = false;
	int tempInt, i;
	myGraphProps_it = myGraphProps.begin();
	
	/* This block is getting some information from the overall graph. It doesn't matter which type of intention
	   the graph has, it will always have those features present or not in myOrganizedMap - which contains selected
	   propositions from page rank */
	// Getting the overall behavior of the trend
	for (i = 0; i < int(myPageRank.finalRankedNodes.size());i++)
	{
		if (myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("graph_overall_behaviour") == 0)
			tempBehavior = myPageRank.finalRankedNodes[i].aNode.myProposition.description;
		else
			continue;
	}
	if (tempBehavior.compare("no change") == 0)
		tempBehavior = "small variation";
	
	//Getting the position of the intention in myGraph
	for (i = 0; i < int(myPageRank.finalRankedNodes.size());i++)
	{
		if ((myPageRank.finalRankedNodes[i].aNode.myProposition.type.compare("composed_trend") == 0) &&
			(myPageRank.finalRankedNodes[i].aNode.myProposition.description.compare(myPageRank.aResponse.intention.second) == 0))
			tempInt = i;
		else
			continue;
	}
	
	// Getting the graph_volatility
	cout << "Found volatility in the graph" << endl;
	myGraphProps_it = myGraphProps.find("graph_volatility");
	if (myGraphProps_it != myGraphProps.end())
	{
		for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
		{
			//1: check if the current organized set item is of type graph_volatility
			//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
			cout << "Print node type from myOrganizedMap: " << myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type << endl;
			if (myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("graph_volatility") == 0)
			{	
				cout << "Volat degree: " << myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.detail << endl;
				tempGraphVolatility = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.detail;
				tempVolatility = tempGraphVolatility;	// for simple intentions it is the same
				bVolatility = true;
			}
			else
				continue;
		}
	}
	if (tempGraphVolatility.compare("highly volatile") == 0) 
		tempString1 = "many";
	else if ((tempGraphVolatility.compare("volatile") == 0) || (tempGraphVolatility.compare("slightly volatile") == 0))
		tempString1 = "some";
	
	// Getting the entity description
	myGraphProps_it = myGraphProps.find("entity_description");
	if (myGraphProps_it != myGraphProps.end())
	{
		tempInt = myPageRank.getNodePositionFromUniqueType("entity_description");
		tempEntityDescription = myPageRank.myGraph[tempInt].myProposition.description;
	}
	//(RT, FT, ST) for volatility, if either graph_volatility or volatility is there, put it on the trend info
	// simple and complex intentions have different aggregation templates. 
	// HERE IT BEGINS FOR SIMPLE INTENTIONS
	//tempSentence.append("(uni '");

	if ((myPageRank.aResponse.intention.first.compare("RT") == 0) ||
		(myPageRank.aResponse.intention.first.compare("FT") == 0) ||
		(myPageRank.aResponse.intention.first.compare("ST") == 0))
	{
		// --- The image shows a GRAPH_TYPE, which presents DESCRIPTOR. ---//
		tempSentence.append(myFufSurge.realizeClause("material", "", "show","",""));
		tempSentence.append("(agent");
		tempSentence.append(myFufSurge.realizeSubjectCommon("image", 1, "", ""));
		tempSentence.append("))");
		tempSentence.append("(affected");
		tempInt = myPageRank.getNodePositionFromUniqueType("graph_type");
		tempString = myPageRank.myGraph[tempInt].myProposition.description;
		tempSentence.append(myFufSurge.realizeSubjectCommon(tempString, 2, "", ""));
		cout << "Passing point 1.0" << endl;
		if (tempEntityDescription.compare("") != 0)
		{
			tempSentence.append("(qualifier((cat clause)(mood relative)(restrictive no)(proc((type material)"
								"(lex present)))(scope {^ partic agent})(partic((affected");
			tempSentence.append(myFufSurge.realizeSubjectCommon(tempEntityDescription, 3, "", ""));
			tempSentence.append(")))))))))))");
		}
		else
			tempSentence.append(")))))");
		//tempSentence.append(")"); //closing parenthesis of (uni '
		tempAllSentences.push_back(tempSentence);
		tempSentence.clear();
		// --------------------------------------------------------------------------------------------//
		// --- It conveys a TREND_DESCRIPTION from INITIAL_DATE to END_DATE, which has a starting value of 
		// --- INITIAL_VALUE and an ending value of END_VALUE. ---//

		tempInt = myPageRank.getNodePositionFromUniqueType("composed_trend");

		tempSentence.append(myFufSurge.realizeClause("material", "", "convey","",""));
		tempSentence.append("(agent");
		tempSentence.append(myFufSurge.realizeSubjectPronoun());
		tempSentence.append(")");
		
		tempSentence.append("(affected");
		tempSentence.append(myFufSurge.realizeSubjectCommon(myPageRank.myGraph[tempInt].myProposition.additionalInfo[0].first, 2, "", ""));
		
		tempSentence.append("(qualifier");
		tempSentence.append(myFufSurge.realizePP("from"));
		
		tempSentence.append(myFufSurge.realizeSubjectCommon(myPageRank.myGraph[tempInt].myProposition.additionalInfo[1].first, 3, "", ""));
		tempSentence.append("(qualifier");
		tempSentence.append(myFufSurge.realizePP("to"));
		tempSentence.append(myFufSurge.realizeSubjectCommon(myPageRank.myGraph[tempInt].myProposition.additionalInfo[1].second, 3, "", ""));

		// now I need to get the segment position of the trend to check if its features are in the feature set
		// look at the set of proposition in order and find the first trend_description type
		//for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
		//{
		//	if (myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_description") == 0)
		//	{	// tempTrendPosition will store the segment position now (so we can get the right set of trend features)
		//		tempTrendPosition = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition;
		//		tempTrendDetail = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.detail;
		//		break;
		//	}
		//	else
		//		continue;
		//}
		// look at the set of proposition in order and find the first trend_description type
		
		for (int j = 0; j < int(myPageRank.myGraph.size()); j++)
		{
			if (myPageRank.myGraph[j].myProposition.type.compare("trend_description") == 0)
			{	// tempTrendPosition will store the segment position now (so we can get the right set of trend features)
				tempTrendPosition = myPageRank.myGraph[j].myProposition.segmentPosition;
				tempTrendDetail = myPageRank.myGraph[j].myProposition.detail;  //unique trend identifier
				break;
			}
			else
				continue;
		}
		cout << "TempTrendPosition: " << tempTrendPosition << endl;
		// checking if there is initial value of the current trend
		myTrendProps_it = myTrendProps.begin();
		myTrendProps_it = myTrendProps.find(tempTrendPosition);
		myTrendProps_aux.insert(myTrendProps_it->second.begin(), myTrendProps_it->second.end());
		myTrendProps_aux_it = myTrendProps_aux.find("trend_initial_value");
		//myTrendProps_aux_it = myTrendProps.find(tempTrendPosition)->second.find("trend_initial_value");

		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type initial value
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_initial_value") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempInitialValue = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
					bInitialValue = true;
				}
				else
					continue;
			}
		}
		// checking if there is end value of the current trend
		myTrendProps_aux_it = myTrendProps_aux.begin();
		myTrendProps_aux_it = myTrendProps_aux.find("trend_end_value");
		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type end value
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_end_value") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempEndValue = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
					bEndValue = true;
				}
				else
					continue;
			}
		}
		tempScaleUnit = myPageRank.myGraph[1].myProposition.detail;
		if ((bInitialValue) && (bEndValue)) // if I have both, add a complex conjunction
		{
			tempSentence.append("(punctuation ((after \\\"\\\,\\\")))))))))))))))(circum((manner((cat clause)(mood relative)"
								"(restrictive no)(proc((type possessive)(mode attributive)))(partic((carrier((gap yes)))");
			tempSentence.append(myFufSurge.aggregatedValueTrendSentence(false,false,"",tempInitialValue,tempEndValue,tempScaleUnit,"","starting","ending"));
			tempSentence.append("))))))"); //closing rest of pharenthesis of the FD	
		}
		else if ((bInitialValue) || (bEndValue))
		{
			if (bInitialValue)
			{
				tempString = tempInitialValue;
				tempClassifier = "starting";
			}
			else
			{
				tempString = tempEndValue;
				tempClassifier = "ending";
			}
			tempSentence.append("(punctuation ((after \\\"\\\,\\\"))))))))))))))(circum((manner((cat clause)(mood relative)"
								"(restrictive no)(proc((type possessive)(mode attributive)))(partic((carrier((gap yes)))"
								"(affected((cat np)(definite no)(classifier === ");
			tempSentence.append(tempClassifier);
			tempSentence.append("\\\")(head ((lex value)))(qualifier((cat pp)(prep === of)(np ((cat measure)(quantity((value ");
			tempSentence.append(tempString);
			if (tempScaleUnit.compare("") != 0)
			{
				tempSentence.append("(unit((lex \\\"");
				tempSentence.append(tempScaleUnit);
				tempSentence.append("))))))))))))))))");
			}
			else
				tempSentence.append("(unit((gap yes))))))))))))))");
		}
		else
		{
			// close parenthesis before without adding circumstance (have neither initial nor end value
			tempSentence.append(")))))))))))))");
		}
		//tempSentence.append(")"); //closing parenthesis of (uni '
		tempAllSentences.push_back(tempSentence);
		tempSentence.clear();
		// --------------------------------------------------------------------------------------------//
		// --- This trend is sharp and has many ups and downs. ---//
		if (!(bVolatility))	//I didn't find the volatility in the graph, then search for the trend volatility
		{
			myTrendProps_aux_it = myTrendProps_aux.begin();
			myTrendProps_aux_it = myTrendProps_aux.find("volatility");
			if (myTrendProps_aux_it != myTrendProps_aux.end())
			{
				cout << "Found volatility in the trend" << endl;
				for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
				{
					//1: check if the current organized set item is of type end value
					//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
					if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("volatility") == 0) &&
						(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
					{	
						tempVolatility = myPageRank.getVolatilityDegree(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.detail);
						bVolatility = true;
					}
					else
						continue;
				}
			}
		}
		myTrendProps_aux_it = myTrendProps_aux.begin();
		myTrendProps_aux_it = myTrendProps_aux.find("steepness");
		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type end value
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("steepness") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempSteepness = myPageRank.getSteepnessDegree(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.detail, tempVolatility);
					bSteepness = true;
				}
				else
					continue;
			}
		}
		if (tempVolatility.compare("highly volatile") == 0) 
			tempString1 = "many";
		else if ((tempVolatility.compare("volatile") == 0) || (tempVolatility.compare("slightly volatile") == 0))
			tempString1 = "some";
		if (tempSteepness.compare("very steep") == 0)
			tempString2 = "very";
		else if (tempSteepness.compare("steep") == 0)
			tempString2 = "";
		else if (tempSteepness.compare("slightly steep") == 0)
			tempString2 = "a little bit";
		if ((bVolatility) && (bSteepness))
		{
			tempSentence.append(myFufSurge.realizeClause("ascriptive","","","",""));
			tempSentence.append("(identified");
			tempSentence.append(myFufSurge.realizeSubjectDistance("trend"));
			tempSentence.append(")(identifier((complex conjunction)(distinct ~(");
			tempSentence.append(myFufSurge.realizeSubjectCommon("sharp",3,tempString2,""));
			tempSentence.append(")");
			tempSentence.append(myFufSurge.volatilitySentence1Level0Obj(true, true, false,"","","up","down",tempString1));
			tempSentence.append(")))))))");
			tempAllSentences.push_back(tempSentence);
			tempSentence.clear();
		}
		else if (bVolatility)
		{
			tempSentence.append(myFufSurge.volatilitySentence1Level0Obj(true, true, true,"trend","distance","peak","valley",tempString1));
			tempAllSentences.push_back(tempSentence);
			tempSentence.clear();
		}
		else if (bSteepness)
		{
			tempSentence.append(myFufSurge.steepnessSentenceObj("","distance","sharp",tempString2));
			tempAllSentences.push_back(tempSentence);
			tempSentence.clear();
		}
		// --------------------------------------------------------------------------------------------//
		// It shows an increase of 300 percent, which represents a total of 6.07 inches, over the period of 28 years.
		myTrendProps_aux_it = myTrendProps_aux.begin();
		myTrendProps_aux_it = myTrendProps_aux.find("trend_rate_change");
		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type trend_rate_change
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_rate_change") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempRateChange = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
					tempBehavior = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.additionalInfo[0].first;
					bRateChange = true;
				}
				else
					continue;
			}
		}
		myTrendProps_aux_it = myTrendProps_aux.begin();
		myTrendProps_aux_it = myTrendProps_aux.find("trend_absolute_change");
		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type trend_absolute_change
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_absolute_change") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempValueChange = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
					tempBehavior = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.additionalInfo[0].first;
					bValueChange = true;
				}
				else
					continue;
			}
		}
		myTrendProps_aux_it = myTrendProps_aux.begin();
		myTrendProps_aux_it = myTrendProps_aux.find("trend_overall_period_years");
		if (myTrendProps_aux_it != myTrendProps_aux.end())
		{
			for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
			{
				//1: check if the current organized set item is of type end value
				//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
				if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_overall_period_years") == 0) &&
					(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
				{	
					tempPeriod = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
					bPeriod = true;
					tempPeriodUnit = "year";
				}
				else
					continue;
			}
		}
		else 
		{
			myTrendProps_aux_it = myTrendProps_aux.begin();
			myTrendProps_aux_it = myTrendProps_aux.find("trend_overall_period_months");
			if (myTrendProps_aux_it != myTrendProps_aux.end())
			{
				for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
				{
					//1: check if the current organized set item is of type end value
					//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
					if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_overall_period_months") == 0) &&
						(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
					{	
						tempPeriod = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
						bPeriod = true;
						tempPeriodUnit = "month";
					}
					else
						continue;
				}
			}
			else
			{
				myTrendProps_aux_it = myTrendProps_aux.begin();
				myTrendProps_aux_it = myTrendProps_aux.find("trend_overall_period_days");
				if (myTrendProps_aux_it != myTrendProps_aux.end())
				{
					for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
					{
						//1: check if the current organized set item is of type end value
						//2: check if the current organized set item belongs to the current trend (the trend and its features have the same segment position
						if ((myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.type.compare("trend_overall_period_days") == 0) &&
							(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.segmentPosition.compare(tempTrendPosition) == 0))	
						{	
							tempPeriod = myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.description;
							bPeriod = true;
							tempPeriodUnit = "day";
						}
						else
							continue;
					}
				}
			}
		}
		if ((bRateChange) && (bValueChange) && (bPeriod))
			tempSentence.append(myFufSurge.rateAbsolutePeriodSentence("",tempBehavior,"pronoun",tempRateChange,"percent",tempValueChange,tempScaleUnit,tempPeriod,tempPeriodUnit));
		else if (((bRateChange) || (bValueChange)) && (bPeriod))
		{
			if (bRateChange)
				tempSentence.append(myFufSurge.overallChangePeriodSentence("",tempBehavior,"pronoun",tempRateChange,"percent",tempPeriod,tempPeriodUnit));
			else
				tempSentence.append(myFufSurge.overallChangePeriodSentence("",tempBehavior,"pronoun",tempValueChange,tempScaleUnit,tempPeriod,tempPeriodUnit));
		}
		else if ((bRateChange) || (bValueChange))
		{
			if (bRateChange)
				tempSentence.append(myFufSurge.overallChangeSentence("",tempBehavior,"pronoun",tempRateChange,"percent"));
			else
				tempSentence.append(myFufSurge.overallChangeSentence("",tempBehavior,"pronoun",tempValueChange,tempScaleUnit));
		}
		tempAllSentences.push_back(tempSentence);
		tempSentence.clear();
	}
	// **********************************************************************************************//
	// HERE IT STARTS FOR COMPLEX INTENTIONS(CHT, CTR, CTLS, CSCT, BF, BJ)
	else
	{
		// --- The image shows a GRAPH_TYPE, which has many ups and downs, presenting DESCRIPTOR. ---//
		tempSentence.append(myFufSurge.realizeClause("material", "", "show","",""));
		tempSentence.append("(agent");
		tempSentence.append(myFufSurge.realizeSubjectCommon("image", 1, "", ""));
		tempSentence.append("))");
		tempSentence.append("(affected");
		tempInt = myPageRank.getNodePositionFromUniqueType("graph_type");
		tempString = myPageRank.myGraph[tempInt].myProposition.description;
		tempSentence.append(myFufSurge.realizeSubjectCommon(tempString, 2, "", ""));
		if (bVolatility)
		{
			tempSentence.append("(qualifier");
			tempSentence.append(myFufSurge.realizeClause("material","","have","(mood relative)(restrictive no)",""));
			tempSentence.append("(agent((gap yes)))");
			tempSentence.append(myFufSurge.volatilitySentence1Level0Obj(false,false,false,"","","up","down",tempString1));
			//tempSentence.append("(punctuation ((after \\\"\\\,\\\"))))))))))))))))");
			tempSentence.append("))))))))");
		}
		else
			tempSentence.append("))))");
		tempSentence.append("(circum((manner");
		tempSentence.append(myFufSurge.realizeClause("material","","present","","(ending pesent-participle)"));
		tempSentence.append("(agent((gap yes)))(affected");
		tempSentence.append(myFufSurge.realizeSubjectCommon(tempEntityDescription, 3, "", ""));
		tempSentence.append(")))))))))");
		tempAllSentences.push_back(tempSentence);
		tempSentence.clear();

		// TODO: Template for intention

		// Here I need to iterate in the total number of trends in the graph while making aggregation decisions
		
		for (i = 0; i < myPageRank.totalNumberOfTrends; i++)
		{
			// --- The <first> segment is the <trend_description>, which <repetition> shows many ups and downs. ---//
			cout << "temp" << endl;
		}
	// different types of intentions (BF, BJ - get the current intention proposition)
	// different number of trends (call loop. either complex conjunction
	}
	return tempAllSentences;
}

vector<string> Generation::aggregateTemplate2()
{
	vector<string> tempAllSentences;
	return tempAllSentences;
}

vector<string> Generation::aggregateTemplate3()
{
	vector<string> tempAllSentences;
	return tempAllSentences;
}

vector<string> Generation::aggregateTemplate4()
{
	vector<string> tempAllSentences;
	return tempAllSentences;
}


/* This method is responsible for calling a.out for realizing a FUF/SURGE sentence at a time. a.out is available
 * on the path c:\cygwin\home\SIGHT\irnet\FUFSURGE. To run it, just call a.out "(here you should write your
 * fufsurge sentence template)". Currently the result is being written to the file outputnewgen.txt in the
 * same directory.
 * The fufsurge templates are obtained from the selected nodes that are present in finalRankedNodes vector.
 */
void Generation::responseRealizationIndividualSentences()
{
	char arguments[20000];
	//the next four lines are cleaning the content of the file before writing to it the new output
	ifstream File;
 	string filepath = "c:\\cygwin\\home\\SIGHT\\irnet\\FUFSURGE\\outputnewgen.txt";
	File.open(filepath.c_str(), ifstream::out | ifstream::trunc);
	File.close();
	// --------------------- finish cleaning the file -----------------------------//

	for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++)
	{
		// propositions such as composed_trend have more than one sentence to be realized
		for (int i = 0; i < int(myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.myRealization.size()); i++)
		{
			strcpy(arguments, "chdir c:\\cygwin\\home\\SIGHT\\irnet\\FUFSURGE && a.out \"");	
			strcat(arguments,myPageRank.finalRankedNodes[myOrganizedSet_it->second].aNode.myProposition.myRealization[i].fufSurgeFrame.c_str());  //insert the fuf description of the sentence 
			// enable this line (...\\output\\NLG.txt) when integrating the new generation module with the SIGHT system
			//strcat(arguments, "\" >> C:\\Users\\SIGHT\\Documents\\LineGraph\\output\\NLG.txt");
			//strcat(arguments, "\" >> C:\\Users\\SIGHT\\Documents\\LineGraph\\output\\response.txt");
			strcat(arguments, "\" >> outputnewgen.txt");
			system(arguments);
		}
		//cout << arguments << endl;
	}
}

/* This method is responsible for calling a.out for realizing aggregated sentences. It receives a vector of strings as argument.
 * The vector passed as argument is the one returned from the methods (aggregateTemplateN).
 * @param: vector FUFSURGE templates that return from method aggregateTemplatN.
 * returns: nothing. It prints realizes the sentences by calling a.out from FUFSURGE and saves it to a file
 */
void Generation::responseRealizationAggregatedSentences(vector<string> allSentences)
{
	char arguments[20000];
	//the next four lines are cleaning the content of the file before writing to it the new output
	ifstream File;
 	string filepath = "c:\\cygwin\\home\\SIGHT\\irnet\\FUFSURGE\\outputnewgen.txt";
	File.open(filepath.c_str(), ifstream::out | ifstream::trunc);
	File.close();
	// --------------------- finish cleaning the file -----------------------------//

	for (int i = 0; i < int(allSentences.size()); i++)
	{
		strcpy(arguments, "chdir c:\\cygwin\\home\\SIGHT\\irnet\\FUFSURGE && a.out \"");	
		strcat(arguments, allSentences[i].c_str());  //insert the fuf description of the sentence 
		// enable this line (...\\output\\NLG.txt) when integrating the new generation module with the SIGHT system
		//strcat(arguments, "\" >> C:\\Users\\SIGHT\\Documents\\LineGraph\\output\\NLG.txt");
		//strcat(arguments, "\" >> C:\\Users\\SIGHT\\Documents\\LineGraph\\output\\response.txt");
		strcat(arguments, "\" >> outputnewgen.txt");
		system(arguments);
	}
}

/************************** PRINTING METHODS ***********************************************/
/* This method prints the response object that is filled by the method fillOrganizationStruct
 */
void Generation::printOrganizationStruct()
{
	cout << "***** Printing Organization Struct *****" << endl;
	cout << "Intention: " << myPageRank.aResponse.intention.first << " " << myPageRank.aResponse.intention.second << endl;
	for (int i = 0; i < int(myPageRank.aResponse.secondaryMessages.size()); i++)
	{
		cout << "Secondary Message: " << myPageRank.aResponse.secondaryMessages[i].first.second << endl;
	}
	for (int j = 0; j < int(myPageRank.aResponse.allEntities.size()); j++)
	{
		cout << "Entity " << j <<": " << myPageRank.aResponse.allEntities[j].description << endl;
		cout << "Features: " << myPageRank.aResponse.allEntities[j].featureCount << endl;
		cout << "Weight: " << myPageRank.aResponse.allEntities[j].totalWeight << endl;
		cout << "Key: " << myPageRank.aResponse.allEntities[j].key << endl;
		if (myPageRank.aResponse.allEntities[j].special)
			cout << "Special" << endl;
		else
			cout << "NOT Special" << endl;
	}
}


void Generation::printFufSurgeTemplates()
{
	cout << "Print finalRankedNodes FUF/Surge templates" << endl;
	for (int i = 0; i < int(myPageRank.finalRankedNodes.size()); i++)
	{
		for (int j = 0; j < int(myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization.size()); j++)
		{
			cout << myPageRank.finalRankedNodes[i].aNode.myProposition.myRealization[j].fufSurgeFrame << endl;
		}
	}
}


void Generation::printAggregatedFufSurgeTemplates()
{
	ofstream myFile;
	myFile.open("aggregatedSentences.txt");
	cout << "Print Aggregated FUF/Surge templates" << endl;
	cout << "There is/are " << allSentences.size() << " aggregated sentences." << endl;
	for (int i = 0; i < int(allSentences.size()); i++)
	{
		cout << allSentences[i] << endl;
		myFile << allSentences[i];
	}
	myFile.close();
}


void Generation::printOrganizedSet()
{
	cout << "***** Printing Organizanized Map *****" << endl;
	myOrganizedSet_it = myOrganizedSet.begin();
	cout << "myOrganizedSet contains " << myOrganizedSet.size() << " items." << endl;
	for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++ )
		cout << "Position: " << (*myOrganizedSet_it).first << 
		" Type " << myPageRank.finalRankedNodes[(*myOrganizedSet_it).second].aNode.myProposition.type <<
		" Detail " << myPageRank.finalRankedNodes[(*myOrganizedSet_it).second].aNode.myProposition.description << endl;
}


void Generation::printOrganizedSetFufSurgeTemplates()
{
	cout << "***** Printing Organizanized Fuf Surge Templates*****" << endl;
	myOrganizedSet_it = myOrganizedSet.begin();
	cout << "myOrganizedSet contains " << myOrganizedSet.size() << " items." << endl;
	for (myOrganizedSet_it = myOrganizedSet.begin(); myOrganizedSet_it != myOrganizedSet.end(); myOrganizedSet_it++ )
		for (int i = 0; i < int(myPageRank.finalRankedNodes[(*myOrganizedSet_it).second].aNode.myProposition.myRealization.size()); i++)
		{
			cout << "Position: " << (*myOrganizedSet_it).first << " Fuf/Surge Template " << 
				myPageRank.finalRankedNodes[(*myOrganizedSet_it).second].aNode.myProposition.myRealization[i].fufSurgeFrame << endl;
		}
}


void Generation::printComparableFeaturesMap()
{
	cout << "***** Printing Comparable features Map (all features that belong to all segments) *****" << endl;
	candidateComparableFeatures_it = candidateComparableFeatures.begin();
	cout << "candidateComparableFeatures map contains " << candidateComparableFeatures.size() << " items." << endl;
	for (candidateComparableFeatures_it = candidateComparableFeatures.begin(); candidateComparableFeatures_it != candidateComparableFeatures.end(); candidateComparableFeatures_it++)
	{
		cout << "Feature: " << (*candidateComparableFeatures_it).first << endl;
		for (int i = 0; i < int((*candidateComparableFeatures_it).second.size()); i++)
			cout << " Trend order which contains feature: " << (*candidateComparableFeatures_it).second[i].first << endl;
	}
}


void Generation::printComparableSets()
{
	cout << "***** Printing Comparable Sets (only the features that will be compared) *****" << endl;
	comparableSets_it = comparableSets.begin();
	cout << "The set of features that will be compared contain " << comparableSets.size() << " items." << endl;
	for (comparableSets_it = comparableSets.begin(); comparableSets_it != comparableSets.end(); comparableSets_it++)
	{
		cout << "Feature: " << (*comparableSets_it) << endl;
	}
}

void Generation::printGraphPropsSet()
{
	cout << "***** Printing Graph Propositions Set *****" << endl;
	myGraphProps_it = myGraphProps.begin();
	cout << "The set of GRAPH features that were selected " << myGraphProps.size() << " items." << endl;
	for (myGraphProps_it = myGraphProps.begin(); myGraphProps_it != myGraphProps.end(); myGraphProps_it++)
	{
		cout << "Element: " << (*myGraphProps_it) << endl;
	}
}

void Generation::printTrendPropsMap()
{
	cout << "***** Printing Trend Map Elements *****" << endl;
	myTrendProps_it = myTrendProps.begin();
	cout << "The set of TREND features maps " << myTrendProps.size() << " items." << endl;
	for (myTrendProps_it = myTrendProps.begin(); myTrendProps_it != myTrendProps.end(); myTrendProps_it++)
	{
		cout << "Element: " << (*myTrendProps_it).first << endl;
	}
}

Generation::~Generation(void)
{

}