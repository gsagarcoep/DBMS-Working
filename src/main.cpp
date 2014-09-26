/*
 * main.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Gangasagar and Harshad
 */

#include "QueryParser.h"
#include "DBSystem.h"

int main(int argc,char* argv[])
{
//	if(argc >3 || argc<1)
	//	cout<<"Invalid arguments!!\n ./run.sh <config.txt> <input.txt>\n";

	char *sql = "SELECT DISTINCT(ID) "
							"FROM   countries "
							"WHERE job_id=5 "
							"ORDER BY ID"
							;
	char *sqltext = "SELECT ProductID, ListPrice \nFROM Product, Product1 \nleft join Product1 t2 on t1.f1 = t2.f2 \nWHERE ListPrice!=1 AND ListPrice!=3 \nGROUP BY ListPrice \nHAVING SUM(ListPrice)>1000 \nORDER BY ListPrice ASC";

	char* sql1="CREATE TABLE Persons\n"
				"(\n"
				"PersonID INT,\n"
				"LastName VARCHAR(255),\n"
				"FirstName VARCHAR(255),\n"
				"Address VARCHAR(255),\n"
				"City VARCHAR(255)\n"
				")";

	char *sql2 = "SELECT * \nFROM Persons,countries\nWHERE ListPrice!=1 AND PersonID!=3";

	string tempLine;

	/*
	SELECT  ID,PersonID FROM countries,Persons WHERE ID=5 AND LastName LIKE "sagar" AND PersonID=ID GROUP BY ID HAVING ID=1 ORDER BY ID,NAME
	SELECT last_name,job_id,salary FROM employees WHERE job_id=5*/

	//ifstream infile;
	//infile.open(argv[2]);

	//DBSystem* dbObject2;
	//dbObject2=new DBSystem();


	cout<<"start"<<endl;
	int t;
	string ab;
	getline(std::cin,ab);
	t=boost::lexical_cast<int>(ab);

	while(t>0)
	{
		t--;
		DBSystem dbObject1;//=*dbObject2;
		dbObject1.readConfig(argv[1]);
		dbObject1.populateDBInfo();
		dbObject1.createPrimaryIndex();

		string table,attrib;
		string query;
		//cout<<"Enter table Name and attribute Name"<<endl;
		//cin>>table>>attrib;

		QueryParser q;

		//dbObject1.V(table,attrib);

		getline(std::cin,query);

		if(query.length()>0)
			q.queryType(dbObject1,query);

	}

/*	while(!infile.eof())
	{
		getline(infile, tempLine);

		QueryParser q;
		if(!tempLine.empty())
			q.queryType(dbObject1,tempLine);
	}
*/
	return 0;
}

