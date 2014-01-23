#include "DBConnection.h"

using namespace std;


DBConnection::DBConnection()
{
	/* Gets today's date */
	today = myHelper.getCurrentDate();
}



/** It establishes a connection to the DataBase
  * returns: conn  -- pointer to the connection to the database
  */
PGconn* DBConnection::InitiateConn()
{
	/* Makes a connection to the database */
	PGconn *conn = NULL;
	
	conn = PQconnectdb("hostaddr = '' port = '5432' dbname = 'postgres' user = 'postgres' password = 'password'");
	
	/* Check to see that the backend connection was successfully made  */
	if (PQstatus(conn) != CONNECTION_OK)
	{
		cout << "Connection to database failed - :(" << endl;
		DBConnection::CloseConn(conn);
	}

	//cout << "Connection to database OK - :)" << endl;
	return conn;
}



/** It closes the connection with the DataBase
  * @params: conn  -- connection to the database
  */
void DBConnection::CloseConn(PGconn *conn)
{
    PQfinish(conn);
	//getchar();
    exit(1);
}



/** This method takes care of calling all the individual insertion methods for each of the
  *	elements in the linegraph structure.
  *	@params: aLineGraph	-- object of class Input that contains the info retrieved from the text 
  *						   and xml files output from the Intention Recognition Module of Line Graphs.
  */
void DBConnection::Insertion(PGconn *conn, Input linegraph, string lineGraphNumber)
{
	linegraph.map_it		= linegraph.mySuggestionMap.begin();
	linegraph.trendMap_it	= linegraph.myTrendMap.begin();

	bool transaction = true;

	// Start a transaction block
	PGresult *res  = PQexec(conn, "BEGIN");

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("BEGIN command failed");
		PQclear(res);
		DBConnection::CloseConn(conn);
	}

	// Clear result
	PQclear(res);
	
	// Start record insertion
	if (DBConnection::InsertLineGraphRec(conn, linegraph.myLineGraph, lineGraphNumber))
	{
		for (int i = 0; i < int(linegraph.myTrendMap.size()); i++)
		{
			if (DBConnection::InsertTrendRec(conn, linegraph.trendMap_it->first, linegraph))
			{
				for (int j = 0; j < int(linegraph.trendMap_it->second.samplePoints.size()); j++)
				{
					if (DBConnection::InsertSamplePointRec(conn, linegraph.trendMap_it->second.samplePoints[j]))
					{
						cout << "Insertion of trends and sample points completed successfully!" << endl;
					}
					else
					{
						/* the insertion of a sample point record failed */
						transaction = false;
					}
				//	res  = PQexec(conn, "SAVEPOINT mySavePoint");
				}
			} 
			else
			{
				/* the insertion of a trend record failed */
				transaction = false;
			}
			linegraph.trendMap_it++;
		}
		for (int i = 0; i < int(linegraph.mySuggestionMap.size()); i++)
		{
			if (DBConnection::InsertComposedTrendRec(conn, linegraph.map_it->first, linegraph))
			{
				for (int j = 0; j < int(linegraph.map_it->second.trends.size()); j++)
				{
					pair <int,int> aPair (linegraph.map_it->second.trends[j]->initial_point, linegraph.map_it->second.trends[j]->end_point);
					if (DBConnection::InsertRelComTreRec(conn, linegraph.map_it->second.trends[j]->trend_order, aPair))
						cout << "Composed trends and relationship records inserted successfully" << endl;
					else
						transaction = false;
				}
			} 
			else 
			{
				/* the insertion of a composed trend record failed */
				transaction = false;
			}
			linegraph.map_it++;
		}
	}
	else
	{
		transaction = false;
	}
	
	if (transaction){
		// Commit the transaction block
		PGresult *res  = PQexec(conn, "COMMIT");
	} else
	{
		//Rollback the transaction block
		PGresult *res  = PQexec(conn, "ROLLBACK");
	}
	// Clear result
	PQclear(res);

	DBConnection::updateLGInformation(conn, linegraphKey);

//	DBConnection::CloseConn(conn);
}



/** Append SQL statement and insert record into tb_linegraph table
  * @params: conn		-- connection to the database.
  *			 aLineGraph	-- the object linegraph from class Input. It has all the graph info
  *						   instatiated into structs.
  */
bool DBConnection::InsertLineGraphRec(PGconn *conn, lineGraph aLineGraph, string lineGraphNumber)
{
	stringstream sVolat, sMaxVal, sMinVal, sStart, sEnd, syInitialTick, syEndTick, sScale (stringstream::in | stringstream::out);

	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_linegraph VALUES (DEFAULT, '");
	sSQL.append(aLineGraph.entity);
	sSQL.append("', ");
	sSQL.append("CAST('");
	sVolat << aLineGraph.volatility;
	string volatility = sVolat.str();
	sSQL.append(volatility);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	if (aLineGraph.maxPointValue == 0)
		sMaxVal << -1;
	else
		sMaxVal << aLineGraph.maxPointValue;
	string maxPoint = sMaxVal.str();
	sSQL.append(maxPoint);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	if (aLineGraph.minPointValue == 0)
		sMinVal << -1;
	else
		sMinVal << aLineGraph.minPointValue;
	string minPoint = sMinVal.str();
	sSQL.append(minPoint);
	sSQL.append("' AS real), '");
	sSQL.append(aLineGraph.maxPointDate);
	sSQL.append("', '");
	sSQL.append(aLineGraph.minPointDate);
	sSQL.append("', ");
	sSQL.append("CAST('");
	if (aLineGraph.startValue == 0)
		sStart << -1;
	else
		sStart << aLineGraph.startValue;
	string startValue = sStart.str();
	sSQL.append(startValue);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	if (aLineGraph.endValue == 0)
		sEnd << -1;
	else
		sEnd << aLineGraph.endValue;
	string endValue = sEnd.str();
	sSQL.append(endValue);
	sSQL.append("' AS real), '");
	sSQL.append(aLineGraph.startDate);
	sSQL.append("', '");
	sSQL.append(aLineGraph.endDate);
	sSQL.append("', '");
	sSQL.append(aLineGraph.caption);
	sSQL.append("', '");
	sSQL.append(aLineGraph.publicationDate);
	sSQL.append("', '");
	sSQL.append(aLineGraph.source);
	sSQL.append("', '");
	sSQL.append(aLineGraph.yAxisUnit);
	sSQL.append("', '");
	sSQL.append(aLineGraph.url);
	sSQL.append("', '");
	sSQL.append(aLineGraph.description);
	sSQL.append("', '");
	sSQL.append(aLineGraph.xAxisLabel);
	sSQL.append("', '");
	sSQL.append(aLineGraph.yAxisLabel);
	sSQL.append("', '");
	sSQL.append(aLineGraph.textInGraphic);
	sSQL.append("', '");
	sSQL.append(aLineGraph.textUnderGraphic);
	sSQL.append("', '");
	sSQL.append(today);
	sSQL.append("', DEFAULT, ");
	sSQL.append("CAST('");
	syInitialTick << aLineGraph.yFirstTickmark;
	string yInitialTick = syInitialTick.str();
	sSQL.append(yInitialTick);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	syEndTick << aLineGraph.yLastTickmark;
	string yEndTick = syEndTick.str();
	sSQL.append(yEndTick);
	sSQL.append("' AS real), '");
	sSQL.append(aLineGraph.yScale);
	sSQL.append("', '");
	sSQL.append(lineGraphNumber);
	sSQL.append("', DEFAULT");
	sSQL.append(") RETURNING lin_linegraph_id;");

	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Insert line graph record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	}

	/* linegraphKey holds the primary key from the just inserted linegraph record */
	linegraphKey = PQgetvalue(res, 0, 0);

	//cout << linegraphKey << endl;
	//printf("Insert line graph record - OK\n");

	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}


/** Append SQL statement and insert record into tb_composedtrend table
  * @params: conn			-- connection to the database.
  *			 description	-- the key field from the suggestionMap. It uniquely identifies
  *							   a suggestion.
  *			 aLineGraph		-- the object linegraph from class Input. It has all the graph info
  *							   instatiated into structs.
  */
bool DBConnection::InsertComposedTrendRec(PGconn *conn, string description, Input aLineGraph)
{
	stringstream sProb (stringstream::in | stringstream::out);
	aLineGraph.map_it = aLineGraph.mySuggestionMap.find(description);
	string lineGraphCaption = aLineGraph.myLineGraph.caption;

	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_composedtrend VALUES (DEFAULT, '");
	sSQL.append(aLineGraph.map_it->second.description);
	sSQL.append("', '");
	sSQL.append(aLineGraph.map_it->second.category);
	sSQL.append("', '");
	sSQL.append(aLineGraph.map_it->second.sustained);
	sSQL.append("', ");
	sSQL.append("CAST('");
	sProb << aLineGraph.map_it->second.probability;
	string probability = sProb.str();
	sSQL.append(probability);
	sSQL.append("' AS real), '");
	sSQL.append(aLineGraph.map_it->second.intention);
	sSQL.append("', '");
	sSQL.append(linegraphKey);
	sSQL.append("', DEFAULT, '");
	sSQL.append(today);
	sSQL.append("') RETURNING com_composedtrend_id;");


	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Insert composed trend record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	}

	/* composedtrendKey holds the primary key from the just inserted composed trend record */
	composedtrendKey = PQgetvalue(res, 0, 0);
	
	//printf("Insert composed trend record - OK\n");

	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}


/** Append SQL statement and insert record into tb_trend table
  * @params: conn		-- connection to the database.
  *			 aPair		-- the key field from myTrendMap. It uniquely identifies
  *						   a simpleTrend in the map.
  *			 aLineGraph	-- the object linegraph from class Input. It has all the graph info
  *						   instatiated into structs.
  */
bool DBConnection::InsertTrendRec(PGconn *conn, trendPairPoints aPair, Input aLineGraph)
{
	stringstream sVolat, sSteep, sStart, sEnd, sChange (stringstream::in | stringstream::out);
	aLineGraph.trendMap_it = aLineGraph.myTrendMap.find(aPair);

	/* calls helper to concatenate day, month and year depending on the parameter type of struct trendDate */
	string startDate	= myHelper.getTrendDate(aLineGraph.trendMap_it->second, 1);
	string endDate		= myHelper.getTrendDate(aLineGraph.trendMap_it->second, 2);
	
	/* getting small pieces of date to process periods in years, months and days */
	string initialYear	= aLineGraph.trendMap_it->second.trend_dates[0].year;
	string endYear		= aLineGraph.trendMap_it->second.trend_dates[1].year; 
	string initialMonth	= aLineGraph.trendMap_it->second.trend_dates[0].month; 
	string endMonth		= aLineGraph.trendMap_it->second.trend_dates[1].month; 
	string initialDay	= aLineGraph.trendMap_it->second.trend_dates[0].day; 
	string endDay		= aLineGraph.trendMap_it->second.trend_dates[1].day; 

	/* getting the absolute change value of the trend */
	sChange << abs(aLineGraph.trendMap_it->second.initial_value - aLineGraph.trendMap_it->second.end_value);
	string absoluteChange = sChange.str();
	
	/* since the trend name might be in different formats, call getTrendName to map it to a unique format */
	string trendName = myHelper.getTrendName(aLineGraph.trendMap_it->second.name);

	/* converting integer to string */
	char charInitialPoint [33];
	_itoa(aLineGraph.trendMap_it->second.initial_point, charInitialPoint, 10);

	char charEndPoint [33];
	_itoa(aLineGraph.trendMap_it->second.end_point, charEndPoint, 10);

	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_trend VALUES (");
	sSQL.append("CAST('");
	if (aLineGraph.trendMap_it->second.volatility == 0)
		sVolat << -1;
	else
		sVolat << aLineGraph.trendMap_it->second.volatility;
	string volatility = sVolat.str();
	sSQL.append(volatility);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	sSteep << aLineGraph.trendMap_it->second.steepness;
	string steepness = sSteep.str();
	sSQL.append(steepness);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	sStart << aLineGraph.trendMap_it->second.initial_value;
	string startValue = sStart.str();
	sSQL.append(startValue);
	sSQL.append("' AS real), ");
	sSQL.append("CAST('");
	sEnd << aLineGraph.trendMap_it->second.end_value;
	string endValue = sEnd.str();
	sSQL.append(endValue);
	sSQL.append("' AS real), '");
	sSQL.append(trendName);
	sSQL.append("', '");		
	sSQL.append(startDate);
	sSQL.append("', '");
	sSQL.append(endDate);
	sSQL.append("', ");
	sSQL.append("CAST('");
	sSQL.append(charInitialPoint);
	sSQL.append("' AS int), ");
	sSQL.append("CAST('");
	sSQL.append(charEndPoint);
	sSQL.append("' AS int), DEFAULT, DEFAULT, '");
	sSQL.append(linegraphKey);
	sSQL.append("', '");		
	sSQL.append(initialYear);
	sSQL.append("', '");
	sSQL.append(endYear);
	sSQL.append("', '");
	sSQL.append(initialMonth);
	sSQL.append("', '");
	sSQL.append(endMonth);
	sSQL.append("', '");
	sSQL.append(initialDay);
	sSQL.append("', '");
	sSQL.append(endDay);
	sSQL.append("', '");
	sSQL.append(absoluteChange);
	sSQL.append("') RETURNING tre_trend_id;");
	
	cout << "----------------------------LG key: " << linegraphKey << endl;
	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Insert trend record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	}

	/* trendKey holds the primary key from the just inserted trend record */
	trendKey = PQgetvalue(res, 0, 0);
//	cout << "Current trend key: " << trendKey << endl;
	
	//DBConnection::InsertRelComTreRec(conn, i)
//	printf("Insert trend record - OK\n");

	//DBConnection::CloseConn(conn);
	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}

/** Append SQL statement and insert record into tb_rel_composedtrend_trend table
  * @params:	conn			--  connection to the database.
  *				trendOrder		--	the number corresponding to the order that the trend appears
  *									in the composed trend.
  * returns: the SQL string for inserting a record into the (tb_composedtrend | tb_trend) 
  *			 relationship table.
  */
bool DBConnection::InsertRelComTreRec(PGconn *conn, int trendOrder, pair<int,int> aPair)
{
	int startPoint = aPair.first;
	int endPoint = aPair.second;

	/* converting integer to string */
	char charTrendOrder [5];
	_itoa(trendOrder, charTrendOrder, 10);

	char charStartPoint [5];
	_itoa(startPoint, charStartPoint, 10);

	char charEndPoint [5];
	_itoa(endPoint, charEndPoint, 10);

	// Append the SQL statement for retrieving the right trend primary key
	string auxsSQL;
	auxsSQL.append("SELECT tre_trend_id FROM tb_trend WHERE (tre_start_point = ");
	auxsSQL.append(charStartPoint);
	auxsSQL.append(" and tre_end_point = ");
	auxsSQL.append(charEndPoint);
	auxsSQL.append(" and tre_linegraph_id = ");
	auxsSQL.append(linegraphKey);
	auxsSQL.append(")");
	
//	cout << "Current Select SQL script: " << auxsSQL.c_str() << endl;

	PGresult *auxres = PQexec(conn, auxsSQL.c_str());

	if (PQresultStatus(auxres) != PGRES_TUPLES_OK)
	{
		 printf("Select trend primary key failed");
		 PQclear(auxres);
		 DBConnection::CloseConn(conn);
		 return false;
	}
	
	char * auxTrendKey;
	auxTrendKey = PQgetvalue(auxres, 0, 0);
	cout << auxTrendKey << endl;

	// Append the SQL statement for insertion into tb_rel_composedtrend_trend
	string sSQL;
	sSQL.append("INSERT INTO tb_rel_composedtrend_trend VALUES ('");
	sSQL.append(composedtrendKey);
	sSQL.append("', '");
	sSQL.append(auxTrendKey);
	sSQL.append("', ");
	sSQL.append("CAST('");
	sSQL.append(charTrendOrder);
	sSQL.append("' AS int), DEFAULT)");
	
	
//	cout << "Current Insert SQL script: " << sSQL.c_str() << endl;
	
	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		 printf("Insert rel_composed_trend record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	}

//	printf("Insert rel_composed_trend record - OK\n");

	// Clear results
	PQclear(auxres);
	PQclear(res);
	return true;
}


/** Append SQL statement and insert record into tb_samplepoint table
  * @params: conn			-- connection to the database.
  *			 aSamplePoint	-- the sample point element that needs to be inserted.
  */  
bool DBConnection::InsertSamplePointRec(PGconn *conn, samplePoint aSamplePoint)
{
	stringstream sValue (stringstream::in | stringstream::out);
	char xCoordinate [10];
	char yCoordinate [10];

	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_samplepoint VALUES (DEFAULT, ");
	sSQL.append("CAST('");
	sValue << aSamplePoint.yAxisValue;
	string pointValue = sValue.str();
	sSQL.append(pointValue);
	sSQL.append("' AS real), '");
	sSQL.append(trendKey);
	sSQL.append("', DEFAULT, '");
	sSQL.append(aSamplePoint.xAxisValue);
	sSQL.append("', '");
	sSQL.append(aSamplePoint.annotation);
	sSQL.append("', '");
	_itoa(aSamplePoint.x_coordinate, xCoordinate, 10);
	sSQL.append(xCoordinate);
	sSQL.append("', '");
	_itoa(aSamplePoint.y_coordinate, yCoordinate, 10);
	sSQL.append(yCoordinate);
	sSQL.append("') RETURNING sam_samplepoint_id;");
		
	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Insert samplePoint record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	} else
	{
		samplepointKey = PQgetvalue(res, 0, 0);
	/*	if (aSamplePoint.maxPoint == "true")
		{
			cout << "Entered MaxPoint" << endl;
			if (!(DBConnection::InsertRelSamImpRec(conn, aSamplePoint, "Maximum Point", samplepointKey)))
				return false;
		}
		if (aSamplePoint.minPoint == "true")
		{
			cout << "Entered MinPoint" << endl;
			if (!(DBConnection::InsertRelSamImpRec(conn, aSamplePoint, "Minimum Point", samplepointKey)))
				return false;
		}
		if (aSamplePoint.annotation != "")
		{
			cout << "Entered Annotated" << endl;
			if (!(DBConnection::InsertRelSamImpRec(conn, aSamplePoint, "Annotated", samplepointKey)))
				return false;
		}*/
//		printf("Insert samplePoint record - OK\n");
	}
	
	//DBConnection::CloseConn(conn);
	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}


/** Append SQL statement and insert record into tb_rel_samplepoint_impreason table
  * @params: conn			-- connection to the database.
  *			 aSamplePoint	-- the samplePoint inserted into tb_samplepoint in the 
  *							   function InsertSamplePointRec.
  *			 importance		-- string that defines the reason why a sample point is considered important
  *							   examples are: Annotated, Maximum Point, Minimum Point.
  *			 samplePointKey	-- global variable that holds the sample point pkey to be inserted here as a fkey.
    
bool DBConnection::InsertRelSamImpRec(PGconn *conn, samplePoint aSamplePoint, string importance, string samplePointKey)
{
	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_rel_samplepoint_impreason VALUES ('");
	sSQL.append(samplePointKey);
	sSQL.append("', ");
	sSQL.append("(SELECT imp_importancereason_id FROM tb_importancereason WHERE imp_description = '");
	sSQL.append(importance);
	sSQL.append("'), DEFAULT)");
		
	//cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		 printf("Insert rel_samplepoint_impreason record failed");
		 PQclear(res);
		 return false;
		 //CloseConn(conn);
	}

//	printf("Insert rel_samplepoint_impreason record - OK\n");

	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}
*/


/** Append SQL statement and insert record into tb_importancereason table
  * @params: description -- description of the reason (examples are: Annotated, Maximum Point, Minimum Point).
  *			 realization -- "true" or "false". When true, the reason will compose a proposition 
  *							for realization.
   
bool DBConnection::InsertImportanceReasonRec(PGconn *conn, char * description, char * realization)
{
	// Append the SQL statement
	string sSQL;
	sSQL.append("INSERT INTO tb_impotancereason VALUES (DEFAULT, '");
	sSQL.append(description);
	sSQL.append("', '");
	sSQL.append(realization);
	sSQL.append("')");
	
	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		 printf("Insert importance_reason record failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return false;
	}

	printf("Insert importance_reason record - OK\n");

	// Clear result
	PQclear(res);

	// If it got here, the insertion was succesfull
	return true;
}
*/


/* This function counts the number of Composed Trends (suggestions) within a linegraph 
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: integer representing the number of suggestions of a linegraph
 */
int DBConnection::getNumberOfComposedTrends(PGconn *conn, string linegraphKey, string minimumProbability)
{

	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT COUNT(com_composedtrend_id) FROM tb_composedtrend WHERE (com_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' and com_probability > ");
	sSQL.append(minimumProbability);
	sSQL.append(")");
	
//	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select number of composed trends failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return -1;
	}

	/* the result value is in row 0 col 0 */
	return atoi(PQgetvalue(res, 0, 0));
	//return atoi(tempRes);

//	printf("Select number of composed trends - OK\n");

	// Clear result
	PQclear(res);
}



/* This function receives a graph number and returns its key in the database 
 * @param:	conn - database connection
 *			lineGraphNumber - string that represents the linegraph number
 * returns: integer representing the key of the entry of this lineGraphNumber in the database
 */
char* DBConnection::returnGraphKey(PGconn *conn, string lineGraphNumber)
{

	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT lin_linegraph_id FROM tb_linegraph WHERE lin_linegraphnumber = '");
	sSQL.append(lineGraphNumber);
	sSQL.append("'");
	
	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select linegraph ID failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return "-1";
	}

	if (PQntuples(res) == 0)
	{
		return "-1";
	}
	else
	{
		/* the key is in row 0 col 0 */
		//return atoi(PQgetvalue(res, 0, 0));
		return PQgetvalue(res, 0, 0);
	}

	// Clear result
	PQclear(res);
}



/* This function gets all the sample points of a trend. It will be used to capture exceptions in rising and
 * falling trends. It will only control if it changes in behavior once (more than that we will let
 * to the volatility measure)
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 *			trendName - "falling" or "rising"
 *			trendOrder - the order that the trend appears in the graphic
 * returns: integer representing the number of exception in the trend (if it is a falling, how many types it rose)
 
int DBConnection::getExceptionInTrend(PGconn *conn, string linegraphKey, string trendName, string trendOrder)
{
	int numberOfTuples = 0;
	int numberOfExceptions = 0;
	int numberOfReturns = 0;
	bool rising = false; 
	bool falling = false;

	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT sam_yaxis FROM tb_samplepoint, tb_trend, tb_rel_composedtrend_trend WHERE "
				"rct_trend_id = tre_trend_id and sam_trend_id = tre_trend_id and tre_linegraph_id = '"); 
	sSQL.append(linegraphKey);
	sSQL.append("' 	and tre_trendname = '");
	sSQL.append(trendName);
	sSQL.append("' and rct_trendorder = '");
	sSQL.append(trendOrder);
	sSQL.append("'");
	
//	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Check exception in trend failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return -1;
	}

	numberOfTuples = PQntuples(res);

	if (trendName.compare("rising trend") == 0)
	{
		rising = true;
		for (int i = 0; i < numberOfTuples - 1; i++)
		{
			if (((atoi(PQgetvalue(res, i+1, 0))) < (atoi(PQgetvalue(res, i, 0)))) && rising)
			{
				while (((atoi(PQgetvalue(res, i+1, 0))) < (atoi(PQgetvalue(res, i, 0)))) && (i < numberOfTuples - 1))
				{
					numberOfExceptions++;
					i++;
				}
				cout << "First rising... then" << endl;
				cout << "Found an exception: " << numberOfExceptions << endl;
				cout << "First: " << PQgetvalue(res, i, 0) << " Second: " << PQgetvalue(res, i+1, 0) << endl;
				rising = false;
			}
			if (((atoi(PQgetvalue(res, i+1, 0))) > (atoi(PQgetvalue(res, i, 0)))) && (!(rising)) && numberOfExceptions > 3)
			{
				while (((atoi(PQgetvalue(res, i+1, 0))) > (atoi(PQgetvalue(res, i, 0)))) && (i < numberOfTuples - 1))
				{
					numberOfReturns++;
					i++;
				}
				if ((i == numberOfTuples - 1) && (numberOfReturns > 3))
					return 1;
				else
					return 0;
			}
			else 
				continue;
		}
	}
	else if (trendName.compare("falling trend") == 0)
	{
		falling = true;
		for (int i = 0; i < numberOfTuples - 1; i++)
		{
			if (((atoi(PQgetvalue(res, i+1, 0))) > (atoi(PQgetvalue(res, i, 0)) + 3)) && falling)
			{
				numberOfExceptions++;
				cout << "First falling... then" << endl;
				cout << "Found an exception: " << numberOfExceptions << endl;
				cout << "First: " << PQgetvalue(res, i, 0) << " Second: " << PQgetvalue(res, i+1, 0) << endl;
				falling = false;
			}
			else if (((atoi(PQgetvalue(res, i+1, 0))) < (atoi(PQgetvalue(res, i, 0)) + 3)) && (!(falling)))
			{
				numberOfExceptions++;
				cout << "Found an exception: " << numberOfExceptions << endl;
				cout << "Now rising and found another falling..." << endl;
				cout << "First: " << PQgetvalue(res, i, 0) << " Second: " << PQgetvalue(res, i+1, 0) << endl;
				falling = true;
			}
			else 
				continue;
		}
		if (numberOfExceptions = 1)
			return 1;
		else
			return 0;
	}
	else
		return 0;

	// Clear result
	PQclear(res);
}
*/


/* This function counts the number of Trends within a linegraph
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: integer representing the number of trends of a linegraph
 */
int DBConnection::getNumberOfTrends(PGconn *conn, string linegraphKey, string minimumProbability)
{

	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT COUNT(DISTINCT(tre_trend_id)) "
				"FROM tb_linegraph INNER JOIN tb_composedtrend ON lin_linegraph_id = com_linegraph_id INNER JOIN "
				"tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN tb_rel_composedtrend_trend ON "
				"(com_composedtrend_id = rct_composedtrend_id and tre_trend_id = rct_trend_id) "
				"WHERE (tre_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' and com_probability > ");
	sSQL.append(minimumProbability);
	sSQL.append(")");
	
//	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select number of trends failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
		 return -1;
	}

	/* the result value is in row 0 col 0 */
	return atoi(PQgetvalue(res, 0, 0));
	//return atoi(tempRes);

//	printf("Select number of trends - OK\n");

	// Clear result
	PQclear(res);
}


/* This method groups the trend descriptions of THE WHOLE GRAPH and brings them in the order
 * they appear on the graph. This method was created just to be able to quickly have the trend names
 * in order. It is necessary for describing the intended message in the organization module.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: list of trend descriptions and their order on the graph
 */
PGresult* DBConnection::getTrends(PGconn *conn, string linegraphKey)
{
	//vector<pair<string,string>> tempTrendVector;
	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT tre_trendname, rct_trendorder, tre_start_date, tre_end_date "
				"FROM tb_linegraph INNER JOIN tb_composedtrend ON lin_linegraph_id = com_linegraph_id INNER JOIN "
				"tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN tb_rel_composedtrend_trend ON "
				"(com_composedtrend_id = rct_composedtrend_id and tre_trend_id = rct_trend_id) "
				"WHERE tre_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' GROUP BY tre_trendname, rct_trendorder, tre_start_date, tre_end_date ORDER BY rct_trendorder");
	
	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select trends failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

	return res;
	// Clear result
	PQclear(res);
}



/* This method groups the trend descriptions of A COMPOSED TREND and brings them in the order
 * they appear on the graph. This method was created just to be able to quickly have the trend names
 * in order. It is necessary for describing the intended message in the organization module.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: list of trend descriptions and their order on the graph
 */
PGresult* DBConnection::getTrends(PGconn *conn, string linegraphKey, string composedTrend)
{
	//vector<pair<string,string>> tempTrendVector;
	// Append the SQL statement
	string sSQL;
	sSQL.append("SELECT tre_trendname, rct_trendorder, tre_start_date, tre_end_date "
				"FROM tb_linegraph INNER JOIN tb_composedtrend ON lin_linegraph_id = com_linegraph_id INNER JOIN "
				"tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN tb_rel_composedtrend_trend ON "
				"(com_composedtrend_id = rct_composedtrend_id and tre_trend_id = rct_trend_id) "
				"WHERE tre_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' and com_description = '");
	sSQL.append(composedTrend);
	sSQL.append("' GROUP BY tre_trendname, rct_trendorder, tre_start_date, tre_end_date ORDER BY rct_trendorder");
	
	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select trends failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

	return res;
	// Clear result
	PQclear(res);
}



/* This function selects all the information need from a linegraph to instantiate the proposition
 * structures.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: list of fields and values that will compose the propositions
 */
PGresult* DBConnection::getLinegraphInformation(PGconn *conn, string linegraphKey, string minimumProbability)
{

	// Append the SQL statement
	string sSQL;
	sSQL.append(
		"SELECT lin_entity, lin_volatility, lin_maxpoint_value, lin_minpoint_value, lin_maxpoint_date," 
			"lin_minpoint_date, com_category, com_description, com_probability, tre_trendname, rct_trendorder, "
			"tre_volatility, tre_steepness, tre_start_value, tre_end_value, tre_start_date, tre_end_date, "
			"tre_start_point, tre_end_point, tre_initial_year, tre_end_year, tre_initial_month, tre_end_month, "
			"tre_initial_day, tre_end_day, tre_absolute_change, tre_trend_id, lin_start_value, lin_end_value, "
			"lin_start_date, lin_end_date, lin_yaxisunit, lin_scale, com_intention "
		"FROM tb_linegraph INNER JOIN tb_composedtrend ON lin_linegraph_id = com_linegraph_id INNER JOIN "
			"tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN tb_rel_composedtrend_trend ON "
			"(com_composedtrend_id = rct_composedtrend_id and tre_trend_id = rct_trend_id) "
		"WHERE lin_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' and com_probability > ");
	sSQL.append(minimumProbability);
	sSQL.append(" ORDER BY com_description, rct_trendorder");
	
//	cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select linegraph information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

//	printf("Select linegraph information - OK\n");

	return res;
	// Clear result
	PQclear(res);
}



/* This method selects all the information need from a linegraph to instantiate the proposition
 * structures.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: list of fields and values that will compose the propositions ordered by trend name
 */
PGresult* DBConnection::getLGInfoOrderedTrend(PGconn *conn, string linegraphKey, string minimumProbability)
{

	// Append the SQL statement
	string sSQL;
	sSQL.append(
		"SELECT lin_entity, lin_volatility, lin_maxpoint_value, lin_minpoint_value, lin_maxpoint_date," 
			"lin_minpoint_date, com_category, com_description, com_probability, tre_trendname, rct_trendorder, "
			"tre_volatility, tre_steepness, tre_start_value, tre_end_value, tre_start_date, tre_end_date, "
			"tre_start_point, tre_end_point, tre_initial_year, tre_end_year, tre_initial_month, tre_end_month, "
			"tre_initial_day, tre_end_day, tre_absolute_change, tre_trend_id, lin_start_value, lin_end_value, "
			"lin_start_date, lin_end_date, lin_yaxisunit, lin_scale, com_intention "
		"FROM tb_linegraph INNER JOIN tb_composedtrend ON lin_linegraph_id = com_linegraph_id INNER JOIN "
			"tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN tb_rel_composedtrend_trend ON "
			"(com_composedtrend_id = rct_composedtrend_id and tre_trend_id = rct_trend_id) "
		"WHERE lin_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' and com_probability > ");
	sSQL.append(minimumProbability);
	sSQL.append(" ORDER BY tre_trendname, rct_trendorder");
	
	//cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select linegraph information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

//	printf("Select linegraph information - OK\n");

	return res;
	// Clear result
	PQclear(res);
}



/* This method selects all the information about annotated sample points of TREND
 * structures.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 * returns: list of annotated sample points present on the graph
 */
PGresult* DBConnection::getAnnotationOnTrend(PGconn *conn, string linegraphKey)
{
	// Append the SQL statement
	string sSQL;
	sSQL.append(
		"SELECT tre_trend_id, sam_samplepoint_id, sam_annotationtext, sam_pointvalue, sam_pointdate "
		"FROM tb_linegraph INNER JOIN tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN "
			"tb_samplepoint ON tre_trend_id = sam_trend_id "
			"WHERE lin_linegraph_id =  '");
	sSQL.append(linegraphKey);
	sSQL.append("'");
	
	//cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select samplepoint annotation information of a TREND failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

	printf("Select TREND samplepoint annotation information - OK\n");

	return res;
	// Clear result
	PQclear(res);
}



/* This method selects all the information about annotated sample points of a GRAPHIC
 * structures.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 *			samplePointValue - way of finding if the point is annotated
 * returns: list of annotated sample points present on the graph
 */
PGresult* DBConnection::getAnnotationOnGraphic(PGconn *conn, string linegraphKey)
{
	// Append the SQL statement
	string sSQL;
	sSQL.append(
		"SELECT tre_trend_id "
		"FROM (select tre_trend_id, sam_samplepoint_id, sam_annotationtext, sam_pointvalue, sam_pointdate "
			"from tb_linegraph INNER JOIN tb_trend ON lin_linegraph_id = tre_linegraph_id INNER JOIN "
			"tb_samplepoint ON tre_trend_id = sam_trend_id WHERE lin_linegraph_id = '");
	sSQL.append(linegraphKey);
	sSQL.append("' ) tb_trend group by tre_trend_id order by tre_trend_id");
	
	//cout << sSQL.c_str() << endl;

	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select samplepoint annotation information of a GRAPHIC failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

//	printf("Select GRAPHIC samplepoint annotation information - OK\n");

	return res;
	// Clear result
	PQclear(res);
}



/* This method updates the tb_linegraph table with the start, end, maximum and minimum points values and dates of the line graph.
 * It checks the tb_trend to acquire the information needed.
 * @param:	conn - database connection
 *			linegraphKey - the key of the linegraph searched in the database
 */
void DBConnection::updateLGInformation(PGconn *conn, string linegraphKey)
{
	string start_date, end_date, max_date, min_date, start_value, end_value, max_value, min_value;
	int numberOfTuples;
	
	// Append the SQL statement
	string sSQL1;
	sSQL1.append("SELECT * FROM tb_trend WHERE tre_linegraph_id = '");
	sSQL1.append(linegraphKey);
	sSQL1.append("' ORDER BY tre_start_point");
	
	string sSQL2;
	sSQL2.append("SELECT * FROM tb_trend WHERE tre_linegraph_id = '");
	sSQL2.append(linegraphKey);
	sSQL2.append("' ORDER BY tre_start_value");
	
	string sSQL3;
	sSQL3.append("SELECT * FROM tb_trend WHERE tre_linegraph_id = '");
	sSQL3.append(linegraphKey);
	sSQL3.append("' ORDER BY tre_end_value");

	string sSQL5;
	sSQL5.append("SELECT sam_pointvalue FROM tb_trend, tb_samplepoint WHERE tre_linegraph_id = '");
	sSQL5.append(linegraphKey);
	sSQL5.append("' and tre_trend_id = sam_trend_id ORDER BY sam_pointvalue");


	
	// Execute with sql statements and instantiate variables
	PGresult *res = PQexec(conn, sSQL1.c_str());
	numberOfTuples = PQntuples(res);

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select points from sql1 information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}
	else
	{
		/* the start value is in row 0 col 2 */
		start_value	= PQgetvalue(res, 0, 2);

		/* the start date is in row 0 col 5 */
		start_date	= PQgetvalue(res, 0, 5);

		/* the end value is in the last row col 3 */
		end_value	= PQgetvalue(res, (numberOfTuples - 1), 3);

		/* the end date is in the last row col 6 */
		end_date	= PQgetvalue(res, (numberOfTuples - 1), 6);
	}

	// Clear result
	PQclear(res);

	// Query to get temporary mimimum and maximum points values is SQL4 (dates of maximum and minimum
	// might not be available since we still do not have an interpolation method for the dates

	/*res = PQexec(conn, sSQL2.c_str());
	numberOfTuples = PQntuples(res);

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select points from sql2 information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}
	else
	{
		// the minimum value is in row 0 col 2
		min_value	= PQgetvalue(res, 0, 2);

		// the minimum date is in row 0 col 5
		min_date	= PQgetvalue(res, 0, 5);

		// the maximum value is in the last row col 3 
		max_value	= PQgetvalue(res, (numberOfTuples - 1), 2);

		// the maximum date is in the last row col 6 
		max_date	= PQgetvalue(res, (numberOfTuples - 1), 5);
	}

	// Clear result
	PQclear(res);
	*/

	//getting the maximum and minimum values based on the result of a query to tb_samplepoint
	res = PQexec(conn, sSQL5.c_str());
	numberOfTuples = PQntuples(res);

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select points from sql4 information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}
	else
	{
		// the minimum value is in row 0 col 0
		min_value	= PQgetvalue(res, 0, 0);

		// the maximum value is in the last row col 3 
		max_value	= PQgetvalue(res, (numberOfTuples - 1), 0);
	}

	// Clear result
	PQclear(res);

	// we need to check the maximum and minimum values that have dates to either: get the corresponding date,
	// or get "Undefined" -- Change that when we have a date interpolation method

	res = PQexec(conn, sSQL3.c_str());
	numberOfTuples = PQntuples(res);

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		 printf("Select points from sql3 information failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}
	else
	{
		for (int i = 0; i < numberOfTuples; i++)
		{
			if (min_value.compare(PQgetvalue(res, i, 2)) == 0)
				min_date = PQgetvalue(res, i, 5);
			else if (min_value.compare(PQgetvalue(res, i, 3)) == 0)
				min_date = PQgetvalue(res, i, 6);
		}
		if (min_date.size() == 0) // none of the previous cases applied (not equal, not around)
			min_date = "Undefined";
		
		for (int j = 0; j < numberOfTuples; j++)
		{
			if (max_value.compare(PQgetvalue(res, j, 2)) == 0)
				max_date = PQgetvalue(res, j, 5);
			else if (max_value.compare(PQgetvalue(res, j, 3)) == 0)
				max_date = PQgetvalue(res, j, 6);
		}
		if (max_date.size() == 0) // none of the previous cases applied (not equal, not around)
			max_date = "Undefined";

	}
	
	// Clear result
	PQclear(res);

	// Now we need to update tb_linegraph with all the info collected above
	// Update query for tb_linegraph
	string sSQL4;
	sSQL4.append("UPDATE tb_linegraph SET lin_start_value = '");
	sSQL4.append(start_value);
	sSQL4.append("', lin_end_value = '");
	sSQL4.append(end_value);
	sSQL4.append("', lin_start_date = '");
	sSQL4.append(start_date);
	sSQL4.append("', lin_end_date = '");
	sSQL4.append(end_date);
	sSQL4.append("', lin_maxpoint_value = '");
	sSQL4.append(max_value);
	sSQL4.append("', lin_maxpoint_date = '");
	sSQL4.append(max_date);
	sSQL4.append("', lin_minpoint_value = '");
	sSQL4.append(min_value);
	sSQL4.append("', lin_minpoint_date = '");
	sSQL4.append(min_date);
	sSQL4.append("' WHERE lin_linegraph_id = '");
	sSQL4.append(linegraphKey);
	sSQL4.append("'");

	// Execute with sql statement
	res = PQexec(conn, sSQL4.c_str());

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		 printf("Update tb_linegraph failed");
		 PQclear(res);
		 DBConnection::CloseConn(conn);
	}

//	printf("Update tb_linegraph - OK\n");

	// Clear result
	PQclear(res);
}


DBConnection::~DBConnection(void)
{
}
