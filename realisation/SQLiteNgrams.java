/**
 * @author: Priscilla Moraes
 * Project: This class is part of the thesis Generation of Textual Summaries at Different Target Reading Levels (http://udspace.udel.edu/handle/19716/19984).
 * Class to connect to the SQLite database to load ngrams efficiently.
 */
package edu.udel.eecis.teahouse.sight;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.sql.*;

public class SQLiteNgrams
{
  public static void main( String args[] )
  {
    Connection c = null;
    try {
      Class.forName("org.sqlite.JDBC");
      c = DriverManager.getConnection("jdbc:sqlite:ngrams.db");
    } catch ( Exception e ) {
      System.err.println( e.getClass().getName() + ": " + e.getMessage() );
      System.exit(0);
    }
    System.out.println("Opened database successfully");
    //createTable();
    insert();
  }
  
  public static void createTable(){
	    Connection c = null;
	    Statement stmt = null;
	    try {
	      Class.forName("org.sqlite.JDBC");
	      c = DriverManager.getConnection("jdbc:sqlite:ngrams.db");
	      System.out.println("Opened database successfully");

	      stmt = c.createStatement();
	      String sql = "CREATE TABLE GOOGLE5GRAMS " +
	                   "(ID INT PRIMARY KEY     NOT NULL," +
	                   " GRAM           TEXT    NOT NULL, " + 
	                   " COUNT          INT     NOT NULL)"; 
	      stmt.executeUpdate(sql);
	      stmt.close();
	      c.close();
	    } catch ( Exception e ) {
	      System.err.println( e.getClass().getName() + ": " + e.getMessage() );
	      System.exit(0);
	    }
	    System.out.println("Table created successfully");
  }
  
  public static void insert(){

	    Connection c = null;
	    Statement stmt = null;
	    try {
		      Class.forName("org.sqlite.JDBC");
		      c = DriverManager.getConnection("jdbc:sqlite:ngrams.db");
		      c.setAutoCommit(false);
		      System.out.println("Opened database successfully");

		      File dir = new File("L:\\ngrams\\all\\group1");
		      File[] directoryListing = dir.listFiles();
		  	  String[] tempNgram = null;
		  	  int counter = 1001;
		  	  
		      if (directoryListing != null) {
					for (File child : directoryListing) {			
						System.out.println("Reading file "+child);
						try(BufferedReader br = new BufferedReader(new FileReader(child))) {
				            String line = br.readLine();
				            while (line != null) {
				            	tempNgram = line.split("\\|count:");
				            	if (tempNgram.length > 0){
				            		stmt = c.createStatement();
				            		String sql = "INSERT INTO GOOGLE5GRAMS (ID,GRAM,COUNT) " +
							                   "VALUES ("+counter+", '"+tempNgram[0].replaceAll("\\'", "")+"', '"+Integer.valueOf(tempNgram[1].trim())+"')"; 
				            		stmt.executeUpdate(sql);
				            		line = br.readLine();
				            		counter++;
				            	}
					        }
				            
						}
					}
				    stmt.close();
				    c.commit();	    
		      }

			  c.close();
		    } catch ( Exception e ) {
		      System.err.println( e.getClass().getName() + ": " + e.getMessage() );
	      System.exit(0);
	    }
	    System.out.println("Records created successfully");
  }

}

