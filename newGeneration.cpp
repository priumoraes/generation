#include <vector>
#include <string>
#include "Input.h"
#include "DBConnection.h"
#include "PageRank.h"
#include "Generation.h"

using namespace std;
 
/* This method compares two TickMarks' values and returns true if the first one passed is smaller
 * than the second
 */
bool compareYCoordinate(tickmark &a, tickmark &b) 
{ 
	return a.y_coordinate < b.y_coordinate; 
} 


int main( int argc, char *argv[] ) { 
    
	// the number that comes with the input files
	string lineGraphNumber;
	// the key of the graphic into the database
	char *lineGraphKey;
	vector<string> args;

	for ( int i = 0; i < argc; i++ ) { 
        args.push_back( argv[i] ); 
    } 
	
	// args[1] has the xml file and args[2] has the BNResultPar.txt file
	Input linegraph(args[1], args[2]);

	/* Methods that organize all the information from xml and txt files into structures */
	linegraph.makeTokenMatrix(args[2]);
	linegraph.getXMLinfo(args[1]);
	linegraph.instantiateStructs();
	sort(linegraph.y_tickmarks.begin(), linegraph.y_tickmarks.end(), compareYCoordinate);
	//for (int j = 0; j < int(linegraph.y_tickmarks.size()); j++)
	//	cout << "X coord: " << linegraph.y_tickmarks[j].x_coordinate << " Y coord: " <<
	//	linegraph.y_tickmarks[j].y_coordinate << " Value: " << linegraph.y_tickmarks[j].value << 
	//	" Number value: " << linegraph.y_tickmarks[j].numberValue << endl;
	linegraph.instantiateLineGraph();
	linegraph.addSimpleTrendInfo();
	linegraph.addSamplePointValue();


	/* Gets the lineGraphNumber from command file name*/
	lineGraphNumber = linegraph.extractOnlyDigits(args[1]);
	//cout << "Graph number: " << lineGraphNumber << endl;

	/* Instantiates a DBConnection object */
	DBConnection dbconnection;

	/* Initiates connection to the database */
	PGconn *conn = dbconnection.InitiateConn();
	

	/*  It checks if the graph is already in the database. If it is, it just skips the insertion step
		Calls the main method that will insert all the graph information into the database tables */
	if (dbconnection.returnGraphKey(conn, lineGraphNumber) == "-1")
	{
		dbconnection.Insertion(conn, linegraph, lineGraphNumber);
	}
	
	// getting the key of the graphic within the database
	lineGraphKey = dbconnection.returnGraphKey(conn, lineGraphNumber);
	
	//cout << "Line Graph Key: " << lineGraphKey << endl;

	//PageRank aPagerank;
	//aPageRank = PageRank.startPageRank(lineGraphKey, "0.1", 0);
	
	Generation generation(lineGraphKey);


	dbconnection.CloseConn(conn);
} 

