/*
 * QueryParser.h
 *
 *  Created on: Feb 11, 2014
 *      Author: Gangasagar and Harshad
 */

#ifndef QUERYPARSER_H_
#define QUERYPARSER_H_

#include "gsp_base.h"
#include "gsp_node.h"
#include "gsp_list.h"
#include "gsp_sourcetoken.h"
#include "gsp_sqlparser.h"
#include "node_visitor.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <utility>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <queue>
#include "DBSystem.h"

using namespace std;

class QueryParser {
public:
	string configFilePath;
	string pathForData;

	vector< string > columnsInSelect;
	vector< string > tablesInSelect;
	vector< string > conditionsInSelect;
	vector< string > orderByStrs;
	vector< string > groupByStrs;
	vector< string > havingStrs;
	vector< string > distinctStrs;
	vector< string > leftHandOperatorType;
	vector< string > rightHandOperatorType;
	vector <string> leftOperand;
	vector <string> midOperator;
	vector <string> rightOperand;
	vector <string> conjunction;
	map< string, int > primaryIndex;
	vector< string > orderOfTableInjoin;
	vector< string > joinConditionsDP;
	vector< string > tableSequenceDP;
	vector< int > tableSizeDP;
	vector< pair< int,int > > distinctValuesDP;

	int resultSize;
	bool orderByExistsBolean;

	QueryParser();
	QueryParser(string configFilePath1);
	QueryParser(DBSystem dbObject1);
	virtual ~QueryParser();
	void writeTable(string tableName,vector< pair< string, string > > columns);
	void createCommand(	DBSystem dbObject,string query);
	void queryType(	DBSystem dbObject,string query);
	void selectCommand(	DBSystem dbObject,string query);
	void queryFile(string queryFile);
	bool isStringPresentInVector(string tableName,vector< string > tNames);
	bool areColumnsFromTable(DBSystem dbObject,string tableName, vector< string> columns);
	void printOutput(string type);
	void printVector(vector < string > temp);
	void printCreate(string tableName,vector< pair < string, string> > temp);
	bool whereParser(DBSystem dbObject,string str);
	bool isLeftHandOperatorIsValid(DBSystem dbObject,vector<string> lhOperator);
	void typeforLeftHandOperator(DBSystem dbObject,vector<string>& lhOperator);
	void typeforRightHandOperator(DBSystem dbObject,vector<string>& rhOperator);
	string typeForColumnName(DBSystem dbObject,string columnName);
	vector <string>  executeWhereinSelect(DBSystem dbObject);
	bool checkConditionForRecord(DBSystem dbObject,string record);
	void executeOrderBy(DBSystem dbObject,vector <string> result,int nof);
	int comparatorForOrderBy(string a, string b);
	void printOutputOfQuery(DBSystem dbObject);
	void writeResultVectorToFile(vector<string> temp, int a);
	void executeJoin(string joinCondition,DBSystem dbObject);
	void printOutputOfJoin(DBSystem dbObject,vector<string> record);
	void getMultiJoinAttribs(DBSystem dbObject);
	int calcCostOfJoin(int i,int j,int k, vector<vector<int> > &m);
	void calculateRes();
	void printSoln(vector<vector<int> > &s,int i,int j);

};

#endif /* QUERYPARSER_H_ */
