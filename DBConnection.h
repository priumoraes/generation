#pragma once
#include <stdio.h>
#include <sstream>
#include "libpq-fe.h"
#include "Input.h"
#include "Helper.h"

class DBConnection
{

public:
	Helper myHelper;
	//Input myInput;
	string today;
	string linegraphKey;
	string composedtrendKey; 
	string trendKey;
	string samplepointKey;
	
	DBConnection::DBConnection();
	
	PGconn* DBConnection::InitiateConn();
	
	PGresult* DBConnection::getLinegraphInformation(PGconn *conn, string linegraphKey, string minimumProbability);
	PGresult* DBConnection::getLGInfoOrderedTrend(PGconn *conn, string linegraphKey, string minimumProbability);
	PGresult* DBConnection::getAnnotationOnTrend(PGconn *conn, string linegraphKey);
	PGresult* DBConnection::getAnnotationOnGraphic(PGconn *conn, string linegraphKey);
	
	// these two methods are both to get the list of unique ordered trend, either for a specific composed trend
	// or for the whole graph (needed in the organization module)
	PGresult* DBConnection::getTrends(PGconn *conn, string linegraphKey);
	PGresult* DBConnection::getTrends(PGconn *conn, string linegraphKey, string composedTrend);

	void DBConnection::CloseConn(PGconn *conn);
	void DBConnection::Insertion(PGconn *conn, Input linegraph, string lineGraphNumber);
	void DBConnection::updateLGInformation(PGconn *conn, string linegraphKey);
	void DBConnection::FetchReasonRec(PGconn *conn);

	bool DBConnection::InsertLineGraphRec(PGconn *conn, lineGraph aLineGraph, string lineGraphNumber);
	bool DBConnection::InsertComposedTrendRec(PGconn *conn, string description, Input aLineGraph);
	bool DBConnection::InsertTrendRec(PGconn *conn, trendPairPoints aPair, Input aLineGraph);
	bool DBConnection::InsertRelComTreRec(PGconn *conn, int trendOrder, pair<int,int> aPair);
	bool DBConnection::InsertSamplePointRec(PGconn *conn, samplePoint aSamplePoint);
	
	//bool DBConnection::InsertRelSamImpRec(PGconn *conn, samplePoint aSamplePoint, string importance, string samplePointKey);
	//bool DBConnection::InsertImportanceReasonRec(PGconn *conn, char * description, char * realization);
	
	int DBConnection::getNumberOfComposedTrends(PGconn *conn, string linegraphKey, string minimumProbability);
	int DBConnection::getNumberOfTrends(PGconn *conn, string linegraphKey, string minimumProbability);
	//int DBConnection::getExceptionInTrend(PGconn *conn, string linegraphKey, string trendName, string trendOrder);

	char* DBConnection::returnGraphKey(PGconn *conn, string lineGraphNumber);
	
	DBConnection::~DBConnection(void);
};
