#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <unordered_map>
#include <xercesc\util\PlatformUtils.hpp>
#include <xercesc\dom\DOM.hpp>
#include <xercesc\parsers\XercesDOMParser.hpp>
#include <xercesc\util\XMLString.hpp>
#include <regex>

using namespace std;
using namespace xercesc;

XERCES_CPP_NAMESPACE_USE

const int MAX_CHARS_PER_LINE = 128;
const int MAX_LINES_PER_FILE = 128;
const int MAX_TOKENS_PER_LINE = 6;
const char* const DELIMITER = " ";

struct samplePoint{
	int x_coordinate;
	int y_coordinate;
	double yAxisValue;
	string xAxisValue;  // currently we are not guessing the xAxis values that are not annotated, so this might be empty in most of the cases
	string annotation;
	string maxPoint;					// true or false
	string minPoint;					// true or false
};

struct tickmark{
	string value;
	double numberValue; // the actual value, without any measurement sign
	bool gridline;      // not using this field for now
	int x_coordinate;
	int y_coordinate;
};

struct trendDate{
	string name;		// one of: Start, Change, End
	string type;		// it can be 100, 110, 011 or 111 (it tells the date format)
	int order;
	string year;
	string month;
	string day;
};

/* This structure will hold the name of the simple trend (rising, falling, stable, etc) and
 * a vector of the sample points. It will be stored as a value in the trendMap Map
 */
struct simpleTrend{
	string name;
	int trend_order;
	//int db_key;				   // check if you will need this!!!
	int initial_point;
	int end_point;
	double initial_value;
	double end_value;
	double volatility;
	double steepness;
	vector<trendDate> trend_dates;
	vector<samplePoint> samplePoints;
};

struct suggestion{
	string category;
	string description;
	double probability;
	string intention;
	string sustained;
	vector<simpleTrend*> trends;
};

struct lineGraph{
	string entity;
	string publicationDate;
	string source;
	string caption;
	string description;
	string textInGraphic;
	string textUnderGraphic;
	string xAxisLabel;
	string yAxisLabel;
	string yAxisUnit;
	double yFirstTickmark;
	double yLastTickmark;
	string url;
	float maxPointValue;		// maybe I won't need this
	float minPointValue;		// idem here
	string maxPointDate;
	string minPointDate;
	float startValue;
	float endValue;
	string startDate;
	string endDate;
	double volatility;
	string yScale;
};


typedef tr1::unordered_map<string,suggestion> suggestionMap;
typedef std::tr1::regex rx;

/* Initial and end points of a trend are the key for the trendMap this map contains a unique instance 
 * of each simple trend (rising, stable, falling, bigfall, bigjump, and sustained or not sustained segments)
 */
typedef pair<int,int> trendPairPoints;			
typedef map<trendPairPoints,simpleTrend> trendMap;	
											


class Input
{
string tokens[MAX_LINES_PER_FILE][MAX_TOKENS_PER_LINE];

public:
	suggestionMap mySuggestionMap;
	suggestionMap::iterator map_it;
	trendMap myTrendMap;
	trendMap::iterator trendMap_it;
	lineGraph myLineGraph;
	string isIntention;
	Input();
	Input(string xmlfilename, string txtfilename);
	int Input::makeTokenMatrix(string atxtfile);
	int Input::instantiateStructs();
	int Input::instantiateState(vector<string> aTempRow);
	int Input::instantiateIntention(vector<string> aTempRow);
	int Input::instantiateDescription(vector<string> aTempRow);
	int Input::instantiateDescriptionDetail(vector<string> aTempRow, int trend_detail, int date_order, int points_counter, int dates_counter, string sustained);
	int Input::getXMLinfo(string anxmlfile);
	void Input::instantiateLineGraph();
	void Input::addMaxMinPointsInfo(simpleTrend aTrend);
	void Input::addSimpleTrendInfo();
	void Input::addSamplePointValue();
	void Input::printSuggestionAndTrends();
	string Input::extractDigits(string aValue);
	string Input::extractOnlyDigits(string aString);
	double Input::interpolateYaxisValues(int valuePoint);
	vector<tickmark> x_tickmarks;
	vector<tickmark> y_tickmarks;
	vector<samplePoint> allSamplePoints;
	~Input(void);

protected:
	xercesc::XercesDOMParser	*m_pParser;
	xercesc::DOMDocument		*m_pDoc;
	xercesc::DOMElement			*m_pDatePublished;
	xercesc::DOMElement			*m_pSource;
	xercesc::DOMElement			*m_pInfoGraphic;
	xercesc::DOMElement			*m_pLineGraph;
	xercesc::DOMElement			*m_pCaption;
	xercesc::DOMElement			*m_pCaptionContent;
	xercesc::DOMElement			*m_pDescriptions;
	xercesc::DOMElement			*m_pDescription;
	xercesc::DOMElement			*m_pDescriptionContent;
	xercesc::DOMElement			*m_pTextInGraphic;
	xercesc::DOMElement			*m_pTextUnderGraphic;

	string m_sDatePublished;
	string m_sSource;
	string m_sCaptionContent;
	string m_sDescriptionContent;
	string m_sTextInGraphic;
	string m_sTextUnderGraphic;
		
	//Sample Points
	xercesc::DOMNodeList* m_pSamplePoints;
	xercesc::DOMNode *m_pAsamplePoint;
	unsigned int m_iSamplePointsNum;
	xercesc::DOMNodeList* m_pSamplePointChildren;
	xercesc::DOMNodeList* m_pCoordinates;
	xercesc::DOMNodeList* m_pAnnotationChildren;
	xercesc::DOMNode *m_pSPCoordinate;
	xercesc::DOMNode *m_pSPAnnotation;
	int m_iXSPCoordinate;
	int m_iYSPCoordinate;
	
	//XAxis
	xercesc::DOMElement* m_pXAxis;
	string m_sXLabel;
	string m_sXUnit;
	//string m_sXScale;
	//double m_fXLength;
	xercesc::DOMNodeList* m_pXTickmarks;
	xercesc::DOMNodeList* m_pXTickmarksChildren;
	xercesc::DOMNode *m_pXTickMark;
	xercesc::DOMNode *m_pXTickValue;
	xercesc::DOMNodeList *m_pXTickCoordinate;
	xercesc::DOMNode *m_pXTickXCoord;
	xercesc::DOMNode *m_pXTickYCoord;
	string m_sXTickValue;
	int m_iXTickXCoord;
	int m_iXTickYCoord;
	unsigned int m_iXTickmarksNum;

	//YAxis
	xercesc::DOMElement* m_pYAxis;
	string m_sYLabel;
	string m_sYUnit;
	string m_sYScale;
	//double m_fYLength;
	xercesc::DOMNodeList* m_pYTickmarks;
	xercesc::DOMNodeList* m_pYTickmarksChildren;
	xercesc::DOMNode *m_pYTickMark;
	xercesc::DOMNode *m_pYTickValue;
	xercesc::DOMNodeList *m_pYTickCoordinate;
	xercesc::DOMNode *m_pYTickXCoord;
	xercesc::DOMNode *m_pYTickYCoord;
	string m_sYTickValue;
	int m_iYTickXCoord;
	int m_iYTickYCoord;
	unsigned int m_iYTickmarksNum;
	string m_sAnnotation;

	//Volatility
	xercesc::DOMElement *m_pGraphVolat;
	xercesc::DOMNodeList* m_pListSegmentVolat;
	xercesc::DOMNode *m_pSegmentVolat;
	unsigned int m_iSegmentVolatNum;
	double m_dVolatility;
	vector<double> m_dSegmentVolatility;
	
	//Steepness
	xercesc::DOMNodeList* m_pListSegmentSteep;
	xercesc::DOMNode *m_pSegmentSteep;
	unsigned int m_iSegmentSteepNum;
	vector<double> m_dSegmentSteepness;

	void initializeMemberVariables()
	{
		m_pDatePublished=NULL;
		m_pSource=NULL;
		m_pLineGraph=NULL;
		m_pCaption=NULL;
		m_pCaptionContent=NULL;
		m_pDescriptions=NULL;
		m_pDescription=NULL;
		m_pDescriptionContent=NULL;
		m_pTextInGraphic=NULL;
		m_pTextUnderGraphic=NULL;
		m_pXAxis=NULL;
		m_pXTickmarks=NULL;
		m_pXTickValue=NULL;
		m_pXTickCoordinate=NULL;
		m_pXTickXCoord=NULL;
		m_pXTickYCoord=NULL;
		m_pYAxis=NULL;
		m_pYTickmarks=NULL;
		m_pYTickValue=NULL;
		m_pYTickCoordinate=NULL;
		m_pYTickXCoord=NULL;
		m_pYTickYCoord=NULL;
		/** Overall Graph Volatility */
		m_pGraphVolat=NULL;
		/** List of Segments' Volatilities */
		m_pListSegmentVolat=NULL;
		m_pSegmentVolat=NULL;
		/** List of Segments' Steepnesses */
		m_pListSegmentSteep=NULL;
		m_pSegmentSteep=NULL;
	}
};
