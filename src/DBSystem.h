/*
 * dbsystem.h
 *
 *  Created on: Jan 25, 2014
 *      Author: sagar
 */

#ifndef DBSYSTEM_H_
#define DBSYSTEM_H_

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <utility>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <set>

using namespace std;

struct Page
{
	int pageId;
	int currentSize;
	vector<string> *pageRecords;
	struct Page *next;
	struct Page *prev;
	int frameNumber;
};
struct Memory{
	struct Page *front;
	struct Page *rear;
	int count;
};
class DBSystem
{
	private:
		int pageSize,numberOfPages;
		char* pathForData;
		std::vector<string> tableNames;
	public:
		map<int, pair<int,int> > pageToRecordMap;
		map< string, vector<int> > tableNameToPageNumbersMap;
		map<int, struct Page*> pageMemoryHash;
		map <string,int> ifAddNewlineToTable;// 0 for NO and 1 for YES 
		struct Memory * currentMemory;
		map< int,int > pageIdToSizeHash;
		int currentPageCount;
		map< string, vector< pair < string, string > > > tableNameToColumnNames;
		map< string, string  > tableNameToPrimaryKeys;
		int currentFrameCount;
		std::map< string, std::vector< std::set<string> > > uniqueAttribs;
		std::map< string, int > tableNameToNumberOfRecords;
//---------------------------------------------------------------------------------//
		
		DBSystem(){
			pageSize=0;
			numberOfPages=0;
			pathForData=new char[256];
			strcpy(pathForData,".");
			currentMemory =  (struct Memory *)malloc(sizeof(struct Memory));
			currentMemory->front = NULL;
			currentMemory->rear = NULL;
			currentMemory->count = 0;
			currentFrameCount = 0;
		};

		int getPageSize(){	return pageSize;};
		int getNumberOfPages(){	return numberOfPages;}
		char* getPathForData() { return pathForData;}
		std::vector<string> getTableNames(){	return tableNames;};
		map<int,pair<int , int > > getPageToRecordMap(){	return pageToRecordMap;};
		map< string, vector<int> > getTableNameToPageNumbersMap(){ 	return tableNameToPageNumbersMap;};
		map< string, vector< pair < string, string > > > getTableNameToColumnNames(){	return tableNameToColumnNames;};


		void readConfig(string configFilePath);
		void populateDBInfo();
		string getRecord(string tableName, int recordId);
		void insertRecord(string tableName, string record);

		struct Page* createNewPage(string tableName, int pageNumber);
		void lruPageReplace(int pageNumber,struct Page* newPage);
		void placePageInFront(struct Page * newPage);
		int getPageNumber(string tableName, int recordId);
		int getLastPageForTable(string tableName);
		int getNewPageIdForTable(string tableName,string record);
		void printCurrentMemory();
		void printAllPages(string tableName);
		void addTableName(string tableN){	tableNames.push_back(tableN);};
		void addTableNameToColumnNames(string tableN,vector< pair<string, string > > col){ tableNameToColumnNames[tableN]=col;};
		vector< string > getAllColumnNames(string tableName,vector<string> &temp){

			//cout<<"::"<<tableName<<endl;
			int len=tableNameToColumnNames[tableName].size();
			for(int i=0;i<len;i++){
			//	cout<<tableNameToColumnNames[tableName][i].first<<"\n";
				temp.push_back(tableNameToColumnNames[tableName][i].first);
			}
			return temp;
		}

		void createPrimaryIndex();
		int columnNumberInRow(string tableName,string primKey);
		void readUniqueAttribs();
		int V(string tableName,string columnName);
		~DBSystem(){
	//		delete[] pathForData;
		//	pageToRecordMap.clear();
			//tableNameToPageNumbersMap.clear();
		};
};

#endif /* DBSYSTEM_H_ */
