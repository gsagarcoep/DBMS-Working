#include "DBSystem.h"

/*****************************************************************************************************/
/*/*Initializes each node*/
/*****************************************************************************************************/

struct Page* initializeNode(int pageSize)
{
	struct Page* temp=new struct Page;
	temp->currentSize=0;
	temp->prev=NULL;
	temp->next=NULL;
	return temp;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

string removeSpaces(string current)
{
	string temp="";
	int len=current.length();
	for(int i=0;i<len;i++)
	{
		if(current.at(i)!=' ')
			temp+=current.at(i);
	}
//	cout<<temp<<endl;
	return temp;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

struct Page* createDoublyLinkedList(DBSystem* object)
{
	int numPages=object->getNumberOfPages();
	int pageSize=object->getPageSize();
	struct Page* currentNode;
	struct Page* head;
	struct Page* tempNode;
	for(int i=0;i<10;i++)
	{
		tempNode=initializeNode(pageSize);
		if(i==0){
			head=tempNode;
			currentNode=head;
			continue;
		}
		currentNode->next=tempNode;
		tempNode->prev=currentNode;
		currentNode=tempNode;
	}

	return head;

}

/**************************************************************************************/
/*	You need to read the configuration file and extract the page size and number of pages
 * (these two parameter together define the maximum main memory you can use).
 * Values are in number of bytes.
 * You should read the names of the tables from the configuration file.
 * You can assume that the table data exists in a file named <table_name>.csv at the path
 * pointed by the config parameter PATH_FOR_DATA.
***************************************************************************************/
void DBSystem::readConfig(string configFilePath)
{
	FILE *fd=fopen(configFilePath.c_str(),"rw+");
	int count=0;
	char* temp=new char[100];
	bool isBegin=false;
	bool isInTable=false;

	if(fd==NULL)
		cout<<"Config file not found"<<endl;

	fscanf (fd, "%s %d", temp, &pageSize);
	fscanf (fd, "%s %d", temp, &(numberOfPages));
	fscanf (fd, "%s %s", temp, (pathForData));

	fclose(fd);

	string sLine="";

	ifstream infile;
	infile.open(configFilePath.c_str());

	getline(infile, sLine);
	getline(infile, sLine);
	getline(infile, sLine);

	//cout<<sLine;
	//cout<<"Page size is : "<< pageSize << endl;
	//cout<<"Number of pages are : "<< numberOfPages <<endl;
	//cout<<"Path for table files is : "<< pathForData <<endl;
	string currentTableName;

	while(!infile.eof())
	{
		string current("");

		getline(infile, current);
		//cout << current<<endl;
		if( current.find("BEGIN") != -1 && !isBegin)
		{
			count=0;
			isBegin=true;
			continue;
		}
		else if( current.find("END") != -1 )
		{
			if(count++==1)
				break;

			isBegin=false;
			isInTable=false;

			continue;
		}

		if(isBegin)
		{
			isInTable=true;
			isBegin=false;

			tableNames.push_back(current.substr(0,current.length()));
			currentTableName=current;
			tableNameToPrimaryKeys[currentTableName]="";
		}

		else if(isInTable)
		{
			int index=current.find(',');
			string currentColumnName=current.substr(0,index);
			int len=current.length();
			string currentDataType=current.substr(index+1,len);

			//	cout<<currentDataType<<endl;
			currentColumnName=removeSpaces(currentColumnName);
			currentDataType=removeSpaces(currentDataType);

			if(currentDataType.find("varchar")!=string::npos || currentDataType.find("VARCHAR")!=string::npos)
				currentDataType="VARCHAR";

			if(currentDataType=="integer")
				currentDataType="INT";

			if(currentDataType=="float")
				currentDataType="FLOAT";


			if(currentColumnName.compare("PRIMARY_KEY")==0){
			//	cout<<currentTableName<<"  "<<currentDataType<<endl;
				tableNameToPrimaryKeys[currentTableName]=currentDataType;
			}
			else{
				std::pair < string,string > bar = std::make_pair (currentColumnName,currentDataType);
				tableNameToColumnNames[currentTableName].push_back(bar);
			}
		}
	}

}
/*****************************************************************************************************/
/* 	The data present in each table needs to be represented in pages.
 * 	Read the file corresponding to each table line by line (for now	assume 1 line = 1 record)
 * 	Maintain a mapping from PageNumber to (StartingRecordId,EndingRecordId) in memory.
 * 	You can assume unspanned file organisation and record length will not be greater than page size. */
/*****************************************************************************************************/
void DBSystem::populateDBInfo()
{
	//struct Page* head=createDoublyLinkedList(this);
	std::vector<string> tableNames = getTableNames();
	int pageSize = getPageSize();
	string folderPath= getPathForData();
	bool forFolder=false;
	int pageCount=0;

	if(folderPath.at(folderPath.length()-1)=='/')
		forFolder=true;

	for(int i=0;i<tableNames.size();i++)
	{
		string filePath;
		if(forFolder)
			filePath=folderPath+tableNames[i]+".csv";
		else
			filePath=folderPath+"/"+tableNames[i]+".csv";

		FILE *fd=fopen(filePath.c_str(),"r+");
		if(fd==NULL){ cout<<"Table file not found "<<endl;continue;}

		int recordCounter=-1;
		char* record=new char[pageSize];
		int currentSizeOfPage=0;
		int StartingRecordId=-1,EndingRecordId=-1;
		bool needToAddPage=false;
		bool ifNewlineChar=false;

		while(fgets(record,pageSize,fd)!=NULL)
		{
			//cout<<record;
			recordCounter++;
			
			string recordStr(record);
			if(ifNewlineChar)			
			{
				recordStr="\n"+recordStr;
				ifNewlineChar=false;
			}

			if(pageSize == currentSizeOfPage+recordStr.size()-1)
			{
				//cout<<recordCounter<<endl;				
				recordStr=recordStr.substr(0,recordStr.length()-1);
				ifNewlineChar=true;
				needToAddPage=true;
				currentSizeOfPage+=recordStr.size();
				if(StartingRecordId==-1)
				{
					StartingRecordId=recordCounter;
					EndingRecordId=recordCounter;
				}
				else
					EndingRecordId=recordCounter;				
			}
			
			else if(pageSize>currentSizeOfPage+recordStr.size())
			{
				currentSizeOfPage+=recordStr.size();
				needToAddPage=true;
				if(StartingRecordId==-1)
				{
					StartingRecordId=recordCounter;
					EndingRecordId=recordCounter;
				}
				else
					EndingRecordId=recordCounter;
			}
			else
			{
				//Whenever a new page for table is added
				needToAddPage=false;
				std::pair <int,int> bar = std::make_pair (StartingRecordId,EndingRecordId);
				pageToRecordMap.insert(std::pair< int, std::pair<int,int> >(pageCount,bar));
				tableNameToPageNumbersMap[tableNames[i]].push_back(pageCount);
				pageIdToSizeHash[pageCount]=currentSizeOfPage;
				pageCount++;
				currentSizeOfPage=recordStr.size();
				StartingRecordId=recordCounter;
				EndingRecordId=recordCounter;
			}

		}
		if(needToAddPage)
		{
			std::pair <int,int> bar = std::make_pair (StartingRecordId,EndingRecordId);
			pageToRecordMap.insert(std::pair< int, std::pair<int,int> >(pageCount,bar));
			tableNameToPageNumbersMap[tableNames[i]].push_back(pageCount);
			pageIdToSizeHash[pageCount]=currentSizeOfPage;
			pageCount++;
			currentSizeOfPage=0;
			StartingRecordId=-1;
			EndingRecordId=-1;
		}
		currentPageCount=pageCount+1;
	
		fclose(fd);
	}

	/*********************************************************************************/
	//printAllPages(tableNames[0]);
	/*Print the content in the maps*/
	/*std::map<int,pair<int,int> >::iterator it = pageToRecordMap.begin();
	for(it=pageToRecordMap.begin();it!=pageToRecordMap.end();it++)
		cout<<it->first<<" "<<it->second.first<<"-"<<it->second.second<<endl;
	cout<<"Table Name and page id"<<endl;

	std::map<string,vector<int> >::iterator itr = tableNameToPageNumbersMap.begin();
	for(itr=tableNameToPageNumbersMap.begin(); itr!=tableNameToPageNumbersMap.end();itr++)
	{
		cout<<itr->first<<" -->"<<endl;
		for(int j=0;j<itr->second.size();j++)	cout<<itr->second[j]<<endl;
	}
*/
}

/*****************************************************************************************************/
/* Return the page number for the given record id and table name*/
/*****************************************************************************************************/
int DBSystem::getPageNumber(string tableName, int recordId)
{
	/*Print the content in the maps*/
	//cout<<"getPageNUmnber "<<recordId<<endl;

	if(tableNameToPageNumbersMap.find(tableName)==tableNameToPageNumbersMap.end())
		return -1;

	vector<int>	pageForTable=tableNameToPageNumbersMap.at(tableName);
	///cout<<recordId<<endl;

	for(int it=0;it!=pageForTable.size();it++)
	{
		if(pageToRecordMap.find(pageForTable[it])==pageToRecordMap.end())
				return -1;

		pair<int,int > pairOfRecordIds=pageToRecordMap.at(pageForTable[it]);
	//	cout<<pairOfRecordIds.first<<"<________>"<<pairOfRecordIds.second<<endl;
		if(recordId< pairOfRecordIds.first || recordId>pairOfRecordIds.second)
			continue;
		else
			return pageForTable[it];
	}
	return -1;
}
/*****************************************************************************************************/

/*****************************************************************************************************/
int DBSystem::getLastPageForTable(string tableName)
{
	vector<int> pageForTable=tableNameToPageNumbersMap.at(tableName);
	int lastId=pageForTable.size();
	if (lastId<=0)
		return -1;
	return pageForTable[lastId-1];
}
/*****************************************************************************************************/

/*****************************************************************************************************/
int DBSystem::getNewPageIdForTable(string tableName,string record)
{
	int pageId=getLastPageForTable(tableName);
	if(pageId==-1)
		return -1;
	
	pair<int,int > pairOfRecordIds=pageToRecordMap.at(pageId);
	int currentPageSize=pageIdToSizeHash[pageId];	
	
	if(currentPageSize+record.size() < pageSize || (currentPageSize+record.size()-1 == pageSize))
	{
		pageToRecordMap[pageId].second+=1;
		pageIdToSizeHash[pageId]+=record.size();
		
		if(currentPageSize+record.size()-1 == pageSize)
			pageIdToSizeHash[pageId]--;
			
		return pageId;
	}
		
	pageToRecordMap.insert(std::make_pair(currentPageCount,std::make_pair (pairOfRecordIds.second+1,pairOfRecordIds.second+1)));
	pageIdToSizeHash[currentPageCount]+=record.size();

	return currentPageCount++;

}
/**********************************************************************************************************************************/
/* 	Get the corresponding record of the specified table.Each time a request is received, if
*	the page containing the record is already in memory, return that record else bring corresponding page in memory.
*	You are supposed to implement LRU page replacement algorithm for the same. Print HIT if the page is in memory, else print
*	MISS <pageNumber> where <pageNumber> is the page number of memory page which is to be replaced. (You can assume page
*	numbers starting from 0. So, you have total 0 to <NUM_PAGES ­- 1> pages.)
*/
/*********************************************************************************************************************************/
string DBSystem::getRecord(string tableName, int recordId)
{
	int pgNo,startRecord;
	struct Page* newPage;
	
	pgNo = getPageNumber(tableName,recordId);
	string error("");
	if( pgNo==-1)
		return (error);

	if(pageMemoryHash[pgNo] == NULL){
		//cout<<"MISS ";
		newPage = createNewPage(tableName, pgNo);
		/*if(currentMemory->count<numberOfPages)
			//cout<<newPage->frameNumber<<"\n";
		else
			if(currentMemory->rear)
				//cout<<currentMemory->rear->frameNumber<<"\n";
			else
				//cout<<"\n";
		*/
		lruPageReplace(pgNo,newPage);
		
		pageMemoryHash[pgNo] = newPage;
	}
	else{
		//cout<<"HIT\n";
		newPage = pageMemoryHash[pgNo];
		placePageInFront(newPage);
	}
	
	startRecord = pageToRecordMap[pgNo].first;
	string temp= newPage->pageRecords->at(recordId-startRecord);
	
	if(temp.at(temp.length()-1)=='\n')
		temp=temp.substr(0,temp.length()-1);
	
	return temp;
}
/*********************************************************************************************************************************/
//	Helper function to create new page if page is not in memory
/*********************************************************************************************************************************/
struct Page* DBSystem::createNewPage(string tableName, int pageNumber)
{
	int startRecord = pageToRecordMap[pageNumber].first;
	int endRecord = pageToRecordMap[pageNumber].second;
	int i;
	string path(pathForData);
	path +="/"+tableName +".csv";

	FILE *fd=fopen(path.c_str(),"r");
	if(fd==NULL)
	{ 
		cout<<"table file not found "<<endl;
		return NULL;
	}
	
	fseek ( fd, 0, SEEK_SET );
	char *temp = new char[pageSize];
	struct Page * newPage = (struct Page *)malloc(sizeof(struct Page));
	newPage->pageRecords =  new std::vector<string>();
//---------------------------------newcode-------------------------------------------------
	//cout<<"Page's frame count "<<currentFrameCount<<endl;
	newPage->frameNumber = currentFrameCount;
	currentFrameCount = (currentFrameCount+1)%numberOfPages;
//---------------------------------newcode-------------------------------------------------	
	for(i=0; i<startRecord; i++)
		fgets(temp,pageSize,fd);
	
	int sizeOfCurrentPage=0;
	
	for(i=startRecord; i<=endRecord; i++)
	{
		fgets(temp,pageSize,fd);
		string record(temp);
		if(i==endRecord && pageSize==sizeOfCurrentPage+record.size()-1)
		{
			record=record.substr(0,record.length()-1);
			sizeOfCurrentPage=sizeOfCurrentPage+record.size()-1;
		}	
		else sizeOfCurrentPage+=record.size();
		//cout<<record;
		newPage->pageRecords->push_back(record);
	}
	newPage->pageId = pageNumber;
	newPage->next = NULL;
	newPage->prev = NULL;
	fclose(fd);
	return newPage;
}
/*********************************************************************************************************************************/
//	Replace LRU page if memory is full. Add page otherwise
/*********************************************************************************************************************************/
void DBSystem::lruPageReplace(int pageNumber,struct Page * newPage){
	
	if((currentMemory->count)<numberOfPages){
	//No need to replace. Simply add the page to the front.
		
		newPage->next = currentMemory->front;
		if(currentMemory->count==0)
			currentMemory->rear = newPage;
		else
			currentMemory->front->prev = newPage;
		currentMemory->front = newPage;
		currentMemory->count++;
	}
	else{
	//Replace the last page
		int frameNum= currentMemory->rear->frameNumber;
		struct Page * lastPage = currentMemory->rear;

		currentMemory->rear = currentMemory->rear->prev;
		currentMemory->rear->next = NULL;
		pageMemoryHash[lastPage->pageId] = NULL;
		free(lastPage);
		
		newPage->frameNumber=frameNum;
		newPage->next = currentMemory->front;
		currentMemory->front->prev = newPage;
		currentMemory->front = newPage;
	}

}
/*****************************************************************************************************/
//After accessing a page place it in front so that the LRU page is always at the rear
/*****************************************************************************************************/
void DBSystem::placePageInFront(struct Page * newPage){
	if(newPage != currentMemory->front){
		struct Page * prevPage = newPage->prev;
		struct Page * nextPage = newPage->next;
		if(prevPage)
			prevPage->next = nextPage;
		if(nextPage)
			nextPage->prev = prevPage;
		newPage->next = currentMemory->front;
		currentMemory->front->prev = newPage;
		currentMemory->front = newPage;
	}
}

/*****************************************************************************************************/

/*****************************************************************************************************/
void DBSystem::insertRecord(string tableName, string record)
{
	//printCurrentMemory();
	int pageId = getLastPageForTable(tableName);
	struct Page * newPage;
	//cout<<record;
	if(pageId<0){
		cout<<"Such a page doesn't exist\n";
		return;
	}
	string path(pathForData);
	path +="/"+tableName +".csv";
	FILE *fd=fopen(path.c_str(),"ra+");
	
	if(fd==NULL){ cout<<"table file not found "<<endl;return;}
	
	fseek ( fd, 0, SEEK_END );
	fprintf (fd, "%s\n",record.c_str());
	fclose(fd);
	int sizeofPage = pageIdToSizeHash[pageId];
	
	if((sizeofPage + record.size()) <= pageSize)
	{
		if((sizeofPage + record.size()) == pageSize)
			ifAddNewlineToTable[tableName] = 1;
		else
			ifAddNewlineToTable[tableName] = 0;
	//We can append the record to this page
		if(pageMemoryHash[pageId] == NULL){
			//cout<<"MISS ";
			pageToRecordMap[pageId].second +=1;
			newPage = createNewPage(tableName, pageId);
			lruPageReplace(pageId,newPage);
			pageMemoryHash[pageId] = newPage;

		}
		else{
			//cout<<"HIT\n";
			pageToRecordMap[pageId].second +=1;
			newPage = pageMemoryHash[pageId];
			newPage->pageRecords->push_back(record);
			placePageInFront(newPage);
		}

		pageIdToSizeHash[pageId] += record.size();
		//Increment the end record id of the page
	}
	else{
		int newPageId = getNewPageIdForTable(tableName,record);//this function def needs to change
		newPage = createNewPage(tableName, newPageId);
		lruPageReplace(newPageId,newPage);
		pageMemoryHash[newPageId] = newPage;
		if(ifAddNewlineToTable[tableName] == 1){
			pageIdToSizeHash[newPageId]++;
			ifAddNewlineToTable[tableName] = 0;
		}
		//cout<<"MISS "<<newPageId<<endl;
	}

	/******************** Adding to unique attrib's list********************************/
	vector< string > temp;
	std::istringstream ss(record);
	std::string token;

	while(std::getline(ss, token, ',')) {
	    temp.push_back(token);
	}

	int si=temp.size();
	for(int m=0;m<si;m++)
	{
		if(uniqueAttribs[tableName].size()==0)
		{
			std::set<string> t;
			t.insert(temp[m]);
			uniqueAttribs[tableName].push_back(t);
		}
		else
		{
			uniqueAttribs[tableName][m].insert(temp[m]);
			//cout<<attribs[tableNames[i]][m].size()<<endl;l
		}
	}
	//printCurrentMemory();
	//printAllPages(tableName);
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void DBSystem::printCurrentMemory(){
	struct Page * start = currentMemory->front;
	while(start){
		cout<<start->frameNumber<<" ";
		start = start->next;
	}
	cout<<"\n";
}


/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void  DBSystem::printAllPages(string tableName){
	vector<int> pageForTable=tableNameToPageNumbersMap.at(tableName);
	///cout<<recordId<<endl;
	for(int it=0;it!=pageForTable.size();it++)
	{
		pair<int,int > pairOfRecordIds=pageToRecordMap.at(pageForTable[it]);
		cout<<pageForTable[it]<<" ------  "<<pairOfRecordIds.first<<"<________>"<<pairOfRecordIds.second<<endl;
	}
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
int DBSystem::columnNumberInRow(string tableName,string primKey)
{
		int primaryColumn=-1;
		vector< pair < string, string > > columns=tableNameToColumnNames[tableName];
		int noColumns= columns.size();
		//cout<<primKey<<endl;
		if( primKey=="")
			return -1;

		//cout<<noColumns<<endl;
		for(int j=0; j<noColumns;j++){
			//cout<<"--"<<columns[j].first<<"--"<<endl;
			if(primKey.compare(columns[j].first)==0){

				return j;
			}
		}
		return primaryColumn;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void  DBSystem::createPrimaryIndex()
{
	int noTables=tableNames.size();

	for(int i=0;i<noTables;i++){

		string fileName=tableNames[i]+"_"+"primary";

		FILE *fd=fopen(fileName.c_str(),"w+");

		if(fd==NULL){ cout<<"table file not found "<<endl;return;}

		//cout<<noTables<<endl;
		string primKey=tableNameToPrimaryKeys[tableNames[i]];
		int primaryColumn=columnNumberInRow(tableNames[i],primKey);
		//cout<<primKey<<endl;
		//cout<<primaryColumn<<endl;

		if(primaryColumn==-1)
			continue;

		int index=0;
		while(1){

			string record=getRecord(tableNames[i],index);
			if(record=="")
				break;

			vector< string > temp;
			std::istringstream ss(record);
			std::string token;

			while(std::getline(ss, token, ',')) {
			    temp.push_back(token);
			   //cout<<token<<"\n";
			}
	//		cout<<temp[primaryColumn].c_str()<<endl;
			fprintf(fd,"%s\n",temp[primaryColumn].c_str());
			index++;
		}

		fclose(fd);

	}
	//cout<<"ok"<<endl;
	readUniqueAttribs();
	//cout<<"I am out of here"<<endl;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void  DBSystem::readUniqueAttribs()
{
	int noTables=tableNames.size();

	for(int i=0;i<noTables;i++){

		int index=0;
		while(1){

			string record=getRecord(tableNames[i],index);
			if(record=="")
				break;

			vector< string > temp;
			std::istringstream ss(record);
			std::string token;

			while(std::getline(ss, token, ',')) {
			    temp.push_back(token);

			}

			int si=temp.size();
			for(int m=0;m<si;m++)
			{
				if(index==0)
				{
					std::set<string> t;
					t.insert(temp[m]);
					uniqueAttribs[tableNames[i]].push_back(t);
				}
				else
				{
					uniqueAttribs[tableNames[i]][m].insert(temp[m]);
					//cout<<attribs[tableNames[i]][m].size()<<endl;
				}
			}
			index++;
		}
		tableNameToNumberOfRecords[tableNames[i]]=index;
		//cout<<attribs[tableNames[i]][5].size()<<endl;
	}
	//cout<<"I am out of here"<<endl;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
int DBSystem::V(string tableName,string columnName)
{
	int colNum = columnNumberInRow(tableName,columnName);
	///cout<<tableName<<endl;
	//cout<<columnName<<"  "<<colNum<<endl;


	int num= uniqueAttribs[tableName][colNum].size();
	//cout<<num<<endl;
	return num;
}
/*****************************************************************************************************/
