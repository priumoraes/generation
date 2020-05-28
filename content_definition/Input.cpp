#include "Input.h"

using namespace std;

/* The constructor instantiates the structures which represent a linegraph, composed of suggestions,
 * dates and trends with some of the information needed to populate the DB
 */

Input::Input()
{
}

Input::Input(string axmlfile, string atxtfile)
{
}

/* It maps the txt document to a matrix where each field is a text token
 * @param: txt file (LXBNResultPar.txt) which contains the result from the Intention Recognition
 *		   module for LineGraphs
 */
int Input::makeTokenMatrix(string txtfilename)
{
	ifstream txtfile;
	string temp;
	txtfile.open(txtfilename.c_str());
	if (!txtfile.good()) 
	    return NULL; // exit if file not found
	for (int i = 0; i < MAX_LINES_PER_FILE; i++)
	{
		char buf[MAX_CHARS_PER_LINE];
		txtfile.getline(buf, MAX_CHARS_PER_LINE);
		if (!(strlen(buf) == 0))
		{
			char* token[MAX_TOKENS_PER_LINE] = {0};
			char* str;
			str = strtok(buf, DELIMITER);
			while (str != NULL)
			{
				for (int n = 0; n < MAX_TOKENS_PER_LINE; n++)
				{
					token[n] = str; 
					str = strtok(0, DELIMITER); // subsequent tokens
					if (token[n] == NULL) temp = ""; else
						temp = token[n];
						temp = temp.c_str();
					tokens[i][n] = temp;
					if (!token[n]) break; // no more tokens
				}
			}
		} 
	}
	txtfile.close();
	return 0;
} 


/* Methods instantiateStructs and all the helpers it calls are used for getting information from the txt
 * file given as parameter when the system is first called. It uses this info to populate the structures 
 * and maps declared on Input.h
 */
int Input::instantiateStructs()
{
	vector<string> tempRow;
	int j = 0;
	for (int i = 0; i < MAX_LINES_PER_FILE; i++)
	{
		for (int n = 0; n < MAX_TOKENS_PER_LINE; n++)
		{
			tempRow.push_back(tokens[i][n]);
		}
		if (tempRow[0] == ""){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "State"){
			instantiateState(tempRow);
			tempRow.clear();
		} else if (tempRow[0] == "Intention"){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "Suggestion"){
			isIntention = "true";
			instantiateIntention(tempRow);
			isIntention = "false";
			tempRow.clear();
		} else if (tempRow[0] == "Probability"){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "Start"){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "Change"){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "End"){
			tempRow.clear();
			continue;
		} else if (tempRow[0] == "Category"){
			tempRow.clear();
			continue;
		} else if (tempRow[j] == "Description"){
			j = (int(tempRow.size()) - 1);
			continue;
		} else if (tempRow[j+1] == "SubIntention"){
			j = (int(tempRow.size()) - 1);
			continue;
		} else if (tempRow[j+1] == "Points"){
			j = int(tempRow.size());
			continue;
		} else if (tempRow[j] == "TrendDetail"){
			j = int(tempRow.size());
			continue;
		} else if (tempRow[j] == "StartDetail"){
			j = int(tempRow.size());
			continue;
		} else if (tempRow[j] == "ChangeDetail"){
			j = int(tempRow.size());
			continue;
		} else if (tempRow[j] == "EndDetail"){
		//	cout << tempRow.size() << endl;
			instantiateDescription(tempRow);
			tempRow.clear();
			j = 0;
		}
	}
	return 0;
}

/* It gets information details from each STATE line
 * @param: aTempRow: 
 */
int Input::instantiateState(vector<string> aTempRow)
{
	suggestion aSuggestion;
	aSuggestion.category = aTempRow[1];
	aSuggestion.description = aTempRow[2];
	aSuggestion.probability = atof((aTempRow[3]).c_str());
	pair <string, suggestion> tempSuggestion (aTempRow[2],aSuggestion);
	mySuggestionMap.insert(tempSuggestion);
	suggestionMap::iterator map_it = mySuggestionMap.find(aTempRow[2]);
	//cout << (map_it->second).category << endl;
	return 0;
}

/* It gets Intention information details 
 * @param: aTempRow: 
 */
int Input::instantiateIntention(vector<string> aTempRow)
{
	suggestionMap::iterator map_it = mySuggestionMap.find(aTempRow[1]);
	(map_it->second).intention = isIntention;
	//cout << (map_it->second).description << endl;
	return 0;
}

/* It gets information on the block that details each suggestion
 * @param: aTempRow: 
 */
int Input::instantiateDescription(vector<string> aTempRow)
{
	string sustained = "false";
	if (aTempRow.size() < 1){
		return 1;
	}
	//for (int i = 0; i < aTempRow.size(); i++){
	//	cout << "Position: " << i << " Content: " << aTempRow[i] << endl;
	//}
	if (aTempRow[6] == "SubIntention")
	{
		string subintention = aTempRow[7].substr(2);
	//	cout << subintention << endl;
		aTempRow[20] = subintention;
		if (subintention == "S") 
			sustained = "true";
		instantiateDescriptionDetail(aTempRow, 19, 1, 13, 24, sustained);
	} else {
		instantiateDescriptionDetail(aTempRow, 13, 1, 7, 18, sustained);
	}
	return 0;
}


/* Helper method  for instantiateDescription()
 * @params: aTempRow: 
			trend_detail:
			date_order:
			points_counter:
			dates_counter:
			sustained:
 */
int Input::instantiateDescriptionDetail(vector<string> aTempRow, int trend_detail, int date_order, int points_counter, int dates_counter, string sustained)
{
	//suggestionMap::iterator map_it = mySuggestionMap.find(aTempRow[1]);
	map_it = mySuggestionMap.find(aTempRow[1]);
	trendMap_it = myTrendMap.begin();
	int trend_order = 1;
	for (int i = trend_detail; i <= (trend_detail + 4); i++){
		if ((aTempRow[i].compare("NS") != 0) && (aTempRow[i].compare("S") != 0))
		{
			//cout << "Loop number: " << i << endl;
			if (aTempRow[i] == ""){
			//	cout << "End of trends" << endl;
				break;
			} else {
				//trend atrend;
				trendPairPoints aPair;
				simpleTrend aSimpleTrend;
				trendDate atrend_initialdate, atrend_enddate;
				map_it->second.sustained = sustained;
				//atrend_initialdate instantiation
				atrend_initialdate.name = aTempRow[dates_counter];
				atrend_initialdate.type = aTempRow[dates_counter + 1];
				atrend_initialdate.order = date_order;
				atrend_initialdate.year = (aTempRow[dates_counter + 2]);
				// Since the IR module sometimes adds 1 to say the date label has a month or a day without them being
				// there, I am only considering the initial/end month and day to be different than 0 if they are > 1
				if (((aTempRow[dates_counter + 3]).compare("1") == 0) && ((aTempRow[dates_counter + 4]).compare("1") == 0))
					atrend_initialdate.month = "0";
				// treating cases where we actually have the month and it is January (only when day is > 1)
				else if (((aTempRow[dates_counter + 3]).compare("1") == 0) && 
					(((aTempRow[dates_counter + 4]).compare("1") != 0) && 
					((aTempRow[dates_counter + 4]).compare("0") != 0)))
					atrend_initialdate.month = (aTempRow[dates_counter + 3]);
				else
					atrend_initialdate.month = (aTempRow[dates_counter + 3]);
				if ((aTempRow[dates_counter + 4]).compare("1") == 0)
					atrend_initialdate.day = "0";
				else
					atrend_initialdate.day = (aTempRow[dates_counter + 4]);
				//atrend_enddate instantiation
				atrend_enddate.name = aTempRow[dates_counter + 6];
				atrend_enddate.type = aTempRow[dates_counter + 7];
				atrend_enddate.order = (date_order + 1);
				atrend_enddate.year = (aTempRow[dates_counter + 8]);
				// same observation as above
				if (((aTempRow[dates_counter + 9]).compare("1") == 0) && ((aTempRow[dates_counter + 10]).compare("1") == 0))
					atrend_enddate.month = "0";
				else if (((aTempRow[dates_counter + 9]).compare("1") == 0) && 
					(((aTempRow[dates_counter + 10]).compare("1") != 0) && 
					((aTempRow[dates_counter + 10]).compare("0") != 0)))
					atrend_enddate.month = (aTempRow[dates_counter + 9]);
				else
					atrend_enddate.month = (aTempRow[dates_counter + 9]);
				if ((aTempRow[dates_counter + 10]).compare("1") == 0)
					atrend_enddate.day = "0";
				else
					atrend_enddate.day = (aTempRow[dates_counter + 10]);
				//atrend instantiation
				aSimpleTrend.name = aTempRow[i];
				aSimpleTrend.initial_point = atoi((aTempRow[points_counter]).c_str());
				aSimpleTrend.end_point = atoi((aTempRow[points_counter + 1]).c_str());
				// the following line is getting the initial point as the index to search on the samplePoints
				// vector, get the Y coordinate and interpolate to find the Y axis value
				aSimpleTrend.initial_value = Input::interpolateYaxisValues(allSamplePoints[aSimpleTrend.initial_point - 1].y_coordinate);
				aSimpleTrend.end_value = Input::interpolateYaxisValues(allSamplePoints[aSimpleTrend.end_point - 1].y_coordinate);
				// this following line call a function which will add volatility and steepness details
				// to each simpleTrend
				//Input::addSimpleTrendInfo();
				/** Instantiating Map of unique simple trends and their set of SamplePoints */
				aPair.first = aSimpleTrend.initial_point;
				aPair.second = aSimpleTrend.end_point;
				//aSimpleTrend.volatility = m_dVolatility;
				aSimpleTrend.trend_dates.push_back(atrend_initialdate);
				aSimpleTrend.trend_dates.push_back(atrend_enddate);
				for (int k = (aSimpleTrend.initial_point - 1); k <= (aSimpleTrend.end_point - 1); k++){
					aSimpleTrend.samplePoints.push_back(allSamplePoints[k]);
				}
				pair <trendPairPoints, simpleTrend> tempTrend (aPair, aSimpleTrend);
				/* The following block is taking care of the trends that are both named falling trend and big fall
				 * or rising trend and big jump. It renames the trend name if it is already inserted and it is found
				 * again with the name of bigjump or bigfall
				 */
				trendMap_it = myTrendMap.find(aPair);
				if ((trendMap_it != myTrendMap.end()) && 
					(trendMap_it->second.name.compare(aSimpleTrend.name) != 0) &&
					((aSimpleTrend.name.compare("bigjump") == 0) || (aSimpleTrend.name.compare("bigfall") == 0)))
					trendMap_it->second.name = aSimpleTrend.name;
				else
					myTrendMap.insert (trendMap_it, tempTrend);
				(map_it->second).trends.push_back(&(myTrendMap.find(aPair))->second);
				/* End */
				//date_order+=2;
				points_counter++;
				dates_counter+=6;
				trend_order++;
			}
		}
		else
			continue;
	}
	return 0;
}


/* This method receives a simple trend and sets true at the sample point elements when they are
 * either the maximum or minimum point within a trend
 * It uses a Helper (compareYCoordinate) to sort the vector of samplePoints by the Y coordinate
 * !! This method can only be called at the very end. The original order of the vector of sample
 * points is important for finding the values of initial and end points of a trend
 *
 */ 
bool compareYCoordinate(samplePoint &a, samplePoint &b) 
{ 
	return a.y_coordinate < b.y_coordinate; 
} 
void Input::addMaxMinPointsInfo(simpleTrend aTrend)
{
	sort(aTrend.samplePoints.begin(), aTrend.samplePoints.end(), compareYCoordinate);
	aTrend.samplePoints[0].minPoint = "true";
	aTrend.samplePoints[(aTrend.samplePoints.size() - 1)].maxPoint = "true";
}


/* This method reads the Suggestion File and extracts information about Xaxis and Yaxis, Label, Unit,
 * Scale, Lenght of the axis, TickMarks, Volatility and Steepness
 */
int Input::getXMLinfo(string anxmlfile)
{
	static bool                     gDoNamespaces          = false;
	static bool                     gDoSchema              = false;
	static bool                     gSchemaFullChecking    = false;
	static bool                     gDoCreate              = false;
	static XMLCh*                   gOutputEncoding        = 0;

	static xercesc::XercesDOMParser::ValSchemes    gValScheme       = xercesc::XercesDOMParser::Val_Auto;

	try
		{
			xercesc::XMLPlatformUtils::Initialize();

			m_pParser = new xercesc::XercesDOMParser;
			m_pParser->setValidationScheme(gValScheme);
			m_pParser->setDoNamespaces(gDoNamespaces);
			m_pParser->setDoSchema(gDoSchema);
			m_pParser->setValidationSchemaFullChecking(gSchemaFullChecking);
			m_pParser->setCreateEntityReferenceNodes(gDoCreate);

		/**  Parses the XML file, catching any XML exceptions that might propagate from it.  */

			m_pParser->parse(anxmlfile.c_str());
			m_pDoc = m_pParser->getDocument();

			if(m_pDoc==NULL)
			{
				cout<<"XML file was not read successfully!"<<endl;
				exit(1);
			}

		}
		catch (...)
		{
			m_pParser = NULL;
		}

		m_pInfoGraphic = (xercesc::DOMElement*)(m_pDoc->getDocumentElement());


		initializeMemberVariables();
		XMLCh chtemp[100];
		
		//DatePublished
		xercesc::XMLString::transcode("DatePublished", chtemp, 99);
		m_pDatePublished = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));
		if (m_pDatePublished->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pDatePublished->getFirstChild()->getNodeValue()));
			m_sDatePublished = stemp;
			//cout << "Date" << stemp << endl;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sDatePublished = "";
		}

		//Source
		xercesc::XMLString::transcode("Source", chtemp, 99);
		m_pSource = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));
		if (m_pSource->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pSource->getFirstChild()->getNodeValue()));
			m_sSource = stemp;
			//cout << "Source" << stemp << endl;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sSource = "";
		}

		//LineGraph
		xercesc::XMLString::transcode("LineGraph", chtemp, 99);
		m_pLineGraph = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));

		//Caption content
		xercesc::XMLString::transcode("Caption", chtemp, 99);
		m_pCaption = (xercesc::DOMElement*)((m_pLineGraph->getElementsByTagName(chtemp))->item(0));

		xercesc::XMLString::transcode("Content", chtemp, 99);
		m_pCaptionContent = (xercesc::DOMElement*)((m_pCaption->getElementsByTagName(chtemp))->item(0));
		
		if (m_pCaptionContent->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pCaptionContent->getFirstChild()->getNodeValue()));
			m_sCaptionContent = stemp;
			//cout << "Caption" << stemp << endl;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sCaptionContent = "";
		}

		//Description content
		xercesc::XMLString::transcode("Descriptions", chtemp, 99);
		m_pDescriptions = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));

		xercesc::XMLString::transcode("Description", chtemp, 99);
		m_pDescription = (xercesc::DOMElement*)((m_pDescriptions->getElementsByTagName(chtemp))->item(0));
		
		xercesc::XMLString::transcode("Content", chtemp, 99);
		m_pDescriptionContent = (xercesc::DOMElement*)((m_pDescription->getElementsByTagName(chtemp))->item(0));

		if (m_pDescriptionContent->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pDescriptionContent->getFirstChild()->getNodeValue()));
			m_sDescriptionContent = stemp;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sDescriptionContent = "";
		}

		// TextInGraphic node instantiation
		xercesc::XMLString::transcode("TextInGraphic", chtemp, 99);
		m_pTextInGraphic = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));

		if (m_pTextInGraphic->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pTextInGraphic->getFirstChild()->getNodeValue()));
			m_sTextInGraphic = stemp;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sTextInGraphic = "";
		}

		// TextUnderGraphic node instantiation
		xercesc::XMLString::transcode("TextUnderGraphic", chtemp, 99);
		m_pTextUnderGraphic = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));

		if (m_pTextUnderGraphic->getFirstChild())
		{
			char* stemp = xercesc::XMLString::transcode((m_pTextUnderGraphic->getFirstChild()->getNodeValue()));
			m_sTextUnderGraphic = stemp;
			xercesc::XMLString::release(&stemp);
		}
		else
		{
			m_sTextUnderGraphic = "";
		}

		//Sample Points
		XMLCh subtemp[100];
		xercesc::XMLString::transcode("SamplePoint", subtemp, 99);
		m_pSamplePoints = (xercesc::DOMNodeList*)((m_pInfoGraphic->getElementsByTagName(subtemp)));
		m_iSamplePointsNum = m_pSamplePoints->getLength();
		//cout << "Number of Sample Points: " << m_iSamplePointsNum << endl;
		for (int i = 0; i < int(m_iSamplePointsNum); i++)
		{
			samplePoint aSamplePoint;
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pSamplePoints->item(i)));
			m_pSamplePointChildren = (xercesc::DOMNodeList*)((pNode->getChildNodes()));
			//cout << "Number of Children of Sample Points: " << m_pSamplePointChildren->getLength() << endl;
			m_pCoordinates = (xercesc::DOMNodeList*)((m_pSamplePointChildren->item(1)->getChildNodes()));
			//cout << "Number of Children of Coord: " << m_pCoordinates->getLength() << endl;
			if (m_pCoordinates->item(1)->getFirstChild())
			{
				// X Coord
				char* stemp = xercesc::XMLString::transcode((m_pCoordinates->item(1)->getFirstChild()->getNodeValue()));
				m_iXSPCoordinate = atoi(stemp);
				xercesc::XMLString::release(&stemp);
				// Y Coord
				char* stemp2 = xercesc::XMLString::transcode((m_pCoordinates->item(3)->getFirstChild()->getNodeValue()));
				m_iYSPCoordinate = atoi(stemp2);
				xercesc::XMLString::release(&stemp2);
			}
			else
			{
				m_iXSPCoordinate = -1;
				m_iYSPCoordinate = -1;
			}
	
			//getting AnnotationOnSample
			xercesc::DOMNode* pNode2 = (xercesc::DOMNode*)((m_pSamplePointChildren->item(7)));
			m_pAnnotationChildren = (xercesc::DOMNodeList*)((pNode2->getChildNodes()));
			//cout << "Annotation Children: " << m_pAnnotationChildren->getLength() << endl;
			if (m_pAnnotationChildren->item(1)->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((m_pAnnotationChildren->item(1)->getFirstChild()->getNodeValue()));
				m_sAnnotation = stemp;
				//cout << "Annotation" << stemp << endl;
				xercesc::XMLString::release(&stemp);
				//cout << "Annotation: " << m_sAnnotation << endl;
			}
			else
			{
				m_sAnnotation = "";
			}

			aSamplePoint.x_coordinate = m_iXSPCoordinate;
			aSamplePoint.y_coordinate = m_iYSPCoordinate;
			aSamplePoint.annotation   = m_sAnnotation;
			allSamplePoints.push_back(aSamplePoint);
		}

		//XAxis
		xercesc::XMLString::transcode("XAxis", chtemp, 99);
		m_pXAxis = static_cast<xercesc::DOMElement*>(m_pLineGraph->getElementsByTagName(chtemp)->item(0));
		if (m_pXAxis!=NULL)
		{//get the Label of XAxis and store it into m_sXLabel
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Label", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pXAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sXLabel = stemp;
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_sXLabel = "";
			}
		}
		if (m_pXAxis!=NULL)
		{//get the Unit of XAxis and store it into m_sXUnit
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Unit", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pXAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sXUnit = stemp;
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_sXUnit="";
			}
			//cout << "Unit: " << m_sXUnit << endl;
		}
		/*  -- I currently don't need those features (Scale and Lenght of the axis)
		if (m_pXAxis!=NULL)
		{//get the Adjective of caption and store it into m_sAdjective
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Scale", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pXAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sXScale = stemp;
				xercesc::XMLString::release(&stemp);
				cout << m_sXScale << endl;
			}
			else
			{
				m_sXScale="";
			}
			//cout << "Scale: " << m_sXScale << endl;
		}
		if (m_pXAxis!=NULL)
		{//get the Length of XAxis and store it into m_fXLength
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Length", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pXAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_fXLength = atof(stemp);
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_fXLength=-1;
			}
		}
		*/
		if (m_pXAxis!=NULL)
		{
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("TickMark", subtemp, 99);
			m_pXTickmarks = (xercesc::DOMNodeList*)((m_pXAxis->getElementsByTagName(subtemp)));
			m_iXTickmarksNum = m_pXTickmarks->getLength();
			//cout << "Number of XTickMarks: " << m_iXTickmarksNum << endl;
			for (int i = 0; i < int(m_iXTickmarksNum); i++)
			{
				tickmark aTickMark;
				xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pXTickmarks->item(i)));
				m_pXTickmarksChildren = (xercesc::DOMNodeList*)((pNode->getChildNodes()));
				
				if (m_pXTickmarksChildren->item(3)->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((m_pXTickmarksChildren->item(3)->getFirstChild()->getNodeValue()));
					m_sXTickValue = stemp;
					//cout << "Tickmark" << stemp << endl;
					xercesc::XMLString::release(&stemp);
				}
				else
				{
					m_sXTickValue="";
				}
				m_pXTickCoordinate = (xercesc::DOMNodeList*)((m_pXTickmarksChildren->item(7)->getChildNodes()));

				if (m_pXTickCoordinate->item(1)->getFirstChild())
				{
					char* stemp2 = xercesc::XMLString::transcode((m_pXTickCoordinate->item(1)->getFirstChild()->getNodeValue()));
					m_iXTickXCoord = atoi(stemp2);
					//cout << "Tickmark" << stemp2 << endl;
					xercesc::XMLString::release(&stemp2);
				}
				else
				{
					m_iXTickXCoord = -1;
				}
			
				if (m_pXTickCoordinate->item(3)->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((m_pXTickCoordinate->item(3)->getFirstChild()->getNodeValue()));
					m_iXTickYCoord = atoi(stemp);
					//cout << "Tickmark" << stemp << endl;
					xercesc::XMLString::release(&stemp);
				}
				else
				{
					m_iXTickYCoord = -1;
				}
				aTickMark.value = m_sXTickValue;
				aTickMark.x_coordinate = m_iXTickXCoord;
				aTickMark.y_coordinate = m_iXTickYCoord;
				x_tickmarks.push_back(aTickMark);
			}
		}
		//cout << "Size of vector of X Tickmarks: " << x_tickmarks.size() << endl;

		//YAxis
		xercesc::XMLString::transcode("YAxis", chtemp, 99);
		m_pYAxis = static_cast<xercesc::DOMElement*>(m_pLineGraph->getElementsByTagName(chtemp)->item(0));
		if (m_pYAxis!=NULL)
		{//get the Label of YAxis and store it into m_sYLabel
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Label", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pYAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sYLabel = stemp;
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_sYLabel="";
			}
		}
		if (m_pYAxis!=NULL)
		{//get the Unit of YAxis and store it into m_sYUnit
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Unit", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pYAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sYUnit = stemp;
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_sYUnit="";
			}
		}
		
		if (m_pYAxis!=NULL)
		{//get the Scale of YAxis and store it into m_sYScale
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Scale", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pYAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_sYScale = stemp;
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_sYScale="";
			}
		} /*  -- I currently don't need the feature "Lenght of the axis"
		if (m_pYAxis!=NULL)
		{//get the Length of YAxis and store it into m_fYLength
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("Length", subtemp, 99);
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pYAxis->getElementsByTagName(subtemp))->item(0));
			if (pNode && pNode->getFirstChild())
			{
				char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
				m_fYLength = atof(stemp);
				xercesc::XMLString::release(&stemp);
			}
			else
			{
				m_fYLength=-1;
			}
		}
		*/
		if (m_pYAxis!=NULL)
		{
			XMLCh subtemp[100];
			xercesc::XMLString::transcode("TickMark", subtemp, 99);
			m_pYTickmarks = (xercesc::DOMNodeList*)((m_pYAxis->getElementsByTagName(subtemp)));
			m_iYTickmarksNum = m_pYTickmarks->getLength();

			for (int i = 0; i < int(m_iYTickmarksNum); i++)
			{
				tickmark aTickMark;
				xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pYTickmarks->item(i)));
				m_pYTickmarksChildren = (xercesc::DOMNodeList*)((pNode->getChildNodes()));
				if (m_pYTickmarksChildren->item(3)->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((m_pYTickmarksChildren->item(3)->getFirstChild()->getNodeValue()));
					m_sYTickValue = stemp;
					//cout << "Tickmark" << stemp << endl;
					xercesc::XMLString::release(&stemp);
				}
				else
				{
					m_sYTickValue = "";
				}
			
				m_pYTickCoordinate = (xercesc::DOMNodeList*)((m_pYTickmarksChildren->item(7)->getChildNodes()));
				
				if (m_pYTickCoordinate->item(1)->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((m_pYTickCoordinate->item(1)->getFirstChild()->getNodeValue()));
					m_iYTickXCoord = atoi(stemp);
					//cout << "Tickmark" << stemp << endl;
					xercesc::XMLString::release(&stemp);
				}
				else
				{
					m_iYTickXCoord = -1;
				}
				if (m_pYTickmarksChildren->item(7)->getChildNodes())
				{
					char* stemp = xercesc::XMLString::transcode((m_pYTickCoordinate->item(3)->getFirstChild()->getNodeValue()));
					m_iYTickYCoord = atoi(stemp);
					//cout << "Tickmark" << stemp << endl;
					xercesc::XMLString::release(&stemp);
				}
				else
				{
					m_iYTickYCoord = -1;
				}
				aTickMark.value = m_sYTickValue;
				aTickMark.numberValue = atof(Input::extractDigits(m_sYTickValue.c_str()).c_str());
				aTickMark.x_coordinate = m_iYTickXCoord;
				aTickMark.y_coordinate = m_iYTickYCoord;
				y_tickmarks.push_back(aTickMark);
				cout << aTickMark.numberValue << endl;
				cout << m_sYTickValue << endl;
				//cout << m_iYTickXCoord << endl;
				//cout << m_iYTickYCoord << endl;
			}
		}
		//cout << "Size of vector of Y Tickmarks: " << y_tickmarks.size() << endl;

		//get the content of the graph volatility and store it into m_fVolatility
		xercesc::XMLString::transcode("GraphVolatility", chtemp, 99);
		m_pGraphVolat = (xercesc::DOMElement*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));
		m_pListSegmentVolat = (xercesc::DOMNodeList*)((m_pGraphVolat->getChildNodes()));
		m_iSegmentVolatNum = (m_pListSegmentVolat->getLength() - 1);
		if (m_pGraphVolat!=NULL)
		{
			xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));
			if (pNode)
			{
				if (pNode->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
					m_dVolatility = atof(stemp);
					xercesc::XMLString::release(&stemp);
					//cout << m_fVolatility << endl;
				}
				else
				{
					m_dVolatility =-1;
				}
			}
			for (int i = 1; i <= int(m_iSegmentVolatNum); i++)
			{
				xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pListSegmentVolat->item(i)));
				//cout << "Number of Volatility Segments: " << m_iSegmentVolatNum << endl;
				if (pNode->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
					m_dSegmentVolatility.push_back(atof(stemp));
					xercesc::XMLString::release(&stemp);
					//cout << m_fVolatility << endl;
				}
				else
				{
					m_dSegmentVolatility.push_back(-1);
				}
			}
		}

		//get the content of the graph steepness and store it into m_fVolatility
		xercesc::XMLString::transcode("GraphSteepness", chtemp, 99);
		m_pSegmentSteep = (xercesc::DOMNode*)((m_pInfoGraphic->getElementsByTagName(chtemp))->item(0));
		m_pListSegmentSteep = (xercesc::DOMNodeList*)((m_pSegmentSteep->getChildNodes()));
		m_iSegmentSteepNum = m_pListSegmentSteep->getLength();
		if (m_pListSegmentSteep!=NULL)
			for (int i = 0; i < int(m_iSegmentSteepNum); i++)
			{
				xercesc::DOMNode* pNode = (xercesc::DOMNode*)((m_pListSegmentSteep->item(i)));

				if (pNode->getFirstChild())
				{
					char* stemp = xercesc::XMLString::transcode((pNode->getFirstChild())->getNodeValue());
					m_dSegmentSteepness.push_back(atof(stemp));
					xercesc::XMLString::release(&stemp);
					//cout << m_fSteepness << endl;
				}
				else
				{
					m_dSegmentSteepness.push_back(-1);
				}
			}
	
	delete m_pParser;
	return 0;
}

void Input::instantiateLineGraph()
{
	myLineGraph.publicationDate		= m_sDatePublished;
	myLineGraph.source				= m_sSource;
	myLineGraph.caption				= m_sCaptionContent;
	myLineGraph.description			= m_sDescriptionContent;
	myLineGraph.textInGraphic		= m_sTextInGraphic;
	myLineGraph.textUnderGraphic	= m_sTextUnderGraphic;
	myLineGraph.xAxisLabel			= m_sXLabel;
	myLineGraph.yAxisLabel			= m_sYLabel;
	myLineGraph.yAxisUnit			= m_sYUnit;
	myLineGraph.yScale				= m_sYScale;
	myLineGraph.yFirstTickmark		= y_tickmarks[0].numberValue;
	myLineGraph.yLastTickmark		= y_tickmarks[y_tickmarks.size() - 1].numberValue;
	myLineGraph.url					= "";	
	myLineGraph.volatility			= m_dVolatility;
	myLineGraph.maxPointValue		= 0.0;					// Those values are already being updated on the DB
	myLineGraph.minPointValue		= 0.0;					// through another method
	myLineGraph.maxPointDate		= "01/01/2000";			// 
	myLineGraph.minPointDate		= "01/01/2000";			//
	myLineGraph.startValue			= 0.0;					//
	myLineGraph.endValue			= 0.0;					//	
	myLineGraph.startDate			= "01/01/2000";			//
	myLineGraph.endDate				= "01/01/2000";			//
}

/* This method extracts only the digits from the string that corresponds to the value in a Y-axis Tickmark
 * Ex: It receives $60 and returns 60
 */
string Input::extractDigits(string anYaxisValue)
{
	// For values represented as strings we need to be careful. Some will be represented as 150,000 and they
	// can be replaced by 150000 but if we have 15.00 we CANNOT replace it by 1500. Those two regular expression
	// pieces of code are dealing with these two cases and taking care of symbols and letters that might come
	// along with the values in the tickmarks
	std::tr1::match_results<std::string::const_iterator> result;
	string pattern = "(\\D*)(\\d+)(,)?(\\d*)((\\.)?(\\d+))?(\\w)*";
	rx aRegex(pattern);
	string replacement = "$2$4$5";
	string newYaxisValue = std::tr1::regex_replace(anYaxisValue, aRegex, replacement);
	return newYaxisValue;
}

/* This method extracts only the digits from a string (it doesn't get dots or commas)
 */
string Input::extractOnlyDigits(string aString)
{
	// This method is to get ONLY numbers (it was written to get the lineGraphNumber from the command line)
	std::tr1::match_results<std::string::const_iterator> result;
	string pattern = "(\\D*)(\\d+)(\\D*)";
	rx aRegex(pattern);
	string replacement = "$2";
	string newString = std::tr1::regex_replace(aString, aRegex, replacement);
	return newString;
}

/* This method receives a Y coordinate that has no tickmark value and interpolates to get the 
 * correspondent Y axis value for that coordinate
 */
double Input::interpolateYaxisValues(int aPoint)
{
	int i = 0;
	double value3;
	while ((y_tickmarks[i].y_coordinate < aPoint) && (i < int(y_tickmarks.size() - 1)))
	{
		i++;
	}
	if (i == 0){
		i = 1;	
	}
	int point1 = y_tickmarks[i - 1].y_coordinate;
	double value1 = y_tickmarks[i - 1].numberValue;
	//cout << "Value 1: " << value1 << endl;
	int point2 = y_tickmarks[i].y_coordinate;
	double value2 = y_tickmarks[i].numberValue;
	//cout << "Value 2: " << value2 << endl;
	value3 = (value1 + (double(aPoint - point1)/double(point2 - point1)) * (value2 - value1));
	//cout << "Value 3: " << value3 << endl;
	//cout << "Value 3 rounded: " << ceil(value3) << endl;
	//return ceil(value3);
	return value3;
}

/* Augments a simpleTrend struct with details regarding volatility and steepness
 */
void Input::addSimpleTrendInfo()
{
	trendMap_it = myTrendMap.begin();
	for (int i = 0; i < int(myTrendMap.size()); i++)
	{
		if (i < int(m_dSegmentVolatility.size()))
		{
			trendMap_it->second.volatility = m_dSegmentVolatility[i];
			trendMap_it->second.steepness  = m_dSegmentSteepness[i];
			trendMap_it->second.trend_order = (i + 1);
			trendMap_it++;
		} else
		{
			trendMap_it->second.volatility = -1;
			trendMap_it->second.steepness  = -1;
			trendMap_it->second.trend_order = (i + 1);
			trendMap_it++;
		}
	}
}


/* Augments all the sample points elements with the interpolated Yaxis value
 */
void Input::addSamplePointValue()
{
	trendMap_it = myTrendMap.begin();
	for (int i = 0; i < int(myTrendMap.size()); i++)
	{
		for (int j = 0; j < int(trendMap_it->second.samplePoints.size()); j++)
		{
			trendMap_it->second.samplePoints[j].yAxisValue = Input::interpolateYaxisValues(trendMap_it->second.samplePoints[j].y_coordinate);
		}
		trendMap_it++;
	}
}


void Input::printSuggestionAndTrends()
{
	map_it = mySuggestionMap.begin();
	trendMap_it = myTrendMap.begin();
	//cout << "myTrendMap contains:\n";
	for ( trendMap_it=myTrendMap.begin() ; trendMap_it != myTrendMap.end(); trendMap_it++ )
		cout << (*trendMap_it).first.first << "," << (*trendMap_it).first.second << " => " << (*trendMap_it).second.name << "  1 SP: " << (*trendMap_it).second.samplePoints[0].x_coordinate << "  2 SP: " << (*trendMap_it).second.samplePoints[1].x_coordinate << endl;
	
	cout << (map_it->second).category << endl;
	cout << (map_it->second).description << endl;
	cout << (map_it->second).intention << endl;
	cout << (map_it->second).probability << endl;
	cout << (map_it->second).sustained << endl;

	for (int i = 0; i < int((map_it->second).trends.size()); i++){
		cout << "Trend Name: " << (map_it->second).trends[i]->name << endl;
		cout << "Initial point:" << (map_it->second).trends[i]->initial_point << endl;
		cout << "End point:" << (map_it->second).trends[i]->end_point << endl;
		cout << "Trend Order:" << (map_it->second).trends[i]->trend_order << endl;
		cout << "Initial value:" << (map_it->second).trends[i]->initial_value << endl;
		cout << "End value:" << (map_it->second).trends[i]->end_value << endl;
		cout << "Volatility:" << (map_it->second).trends[i]->volatility << endl;
		for (int j = 0; j < int((map_it->second).trends[i]->trend_dates.size()); j++){
			cout << (map_it->second).trends[i]->trend_dates[j].name << endl;
			cout << (map_it->second).trends[i]->trend_dates[j].type << endl;
			cout << (map_it->second).trends[i]->trend_dates[j].year << endl;
		}
	}
	cout << "-----------------------------------------" << endl;
}


Input::~Input(void)
{
	xercesc::XMLPlatformUtils::Terminate();
}

