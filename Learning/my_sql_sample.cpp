#include <mysql.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

MYSQL *conn, mysql;
MYSQL_RES *res;
MYSQL_ROW row;

int query_state;

int main()
{
   const char *server = "localhost";  // Change to your MySQL server host
   const char *user = "root";     // Change to your MySQL username
   const char *password = "your_password"; // Change to your MySQL password
   const char *database = "InstrumentManagement"; // Database name

   // Initialize MySQL connection
   mysql_init(&mysql);
   conn = mysql_real_connect(&mysql, server, user, password, database, 0, 0, 0);

   // Check if connection is successful
   if (conn == NULL)
   {
       cout << "MySQL Connection failed: " << mysql_error(&mysql) << endl;
       return 1;
   }

   // Query to fetch and display all instruments
   query_state = mysql_query(conn, "SELECT * FROM Player");
   if (query_state != 0)
   {
      cout << "Query failed: " << mysql_error(conn) << endl;
      return 1;
   }

   // Store and display results
   res = mysql_store_result(conn);
   cout << "Instruments in database:" << endl;
   while ((row = mysql_fetch_row(res)) != NULL)
   {
      cout << left;
      cout << setw(18) << row[0]
           << setw(18) << row[1]
           << setw(18) << row[2]
           << setw(18) << row[3] << endl;
   }

   // Close the result set
   mysql_free_result(res);

   // Open the Instrument.txt file to insert data into the database
   ifstream infile("Instrument.txt");
   if (infile.fail())
   {
      cout << "ERROR. Could not open file!" << endl;
      return 1;
   }

   // Variables to hold data for insertion
   string instnum, insttype, maker, year;
   string plID, name, salary, startdate, rating;

   while (infile >> instnum >> insttype >> maker >> year >> plID >> name >> salary >> startdate >> rating)
   {
      // Construct SQL insert statement for the Instrument table
      string instrumentQuery = "INSERT INTO Instrument (InstrumentID, InstrumentType, MakerName, YearMade) VALUES ('" + instnum + "', '" + insttype + "', '" + maker + "', " + year + ")";

      // Execute the insert for the Instrument table
      query_state = mysql_query(conn, instrumentQuery.c_str());
      if (query_state != 0)
      {
         cout << "Insert failed for Instrument: " << mysql_error(conn) << endl;
         continue;  // Skip to the next record if insertion fails
      }

      // Construct SQL insert statement for the Player table
      string playerQuery = "INSERT INTO Player (PlayerID, Name, Salary, StartDate) VALUES ('" + plID + "', '" + name + "', " + salary + ", '" + startdate + "')";

      // Execute the insert for the Player table
      query_state = mysql_query(conn, playerQuery.c_str());
      if (query_state != 0)
      {
         cout << "Insert failed for Player: " << mysql_error(conn) << endl;
         continue;  // Skip to the next record if insertion fails
      }

      // Construct SQL insert statement for the Plays table
      string playsQuery = "INSERT INTO Plays (InstrumentID, PlayerID, Rating) VALUES ('" + instnum + "', '" + plID + "', " + rating + ")";

      // Execute the insert for the Plays table
      query_state = mysql_query(conn, playsQuery.c_str());
      if (query_state != 0)
      {
         cout << "Insert failed for Plays: " << mysql_error(conn) << endl;
         continue;  // Skip to the next record if insertion fails
      }

      // Output the inserted data for verification
      cout << "Inserted: " << instnum << ", " << insttype << ", " << maker << ", " << year 
           << " | Player: " << plID << ", " << name << ", " << salary << ", " << startdate 
           << " | Rating: " << rating << endl;
   }

   // Close the file
   infile.close();

   // Clean up and close the MySQL connection
   mysql_close(conn);

   return 0;
}
