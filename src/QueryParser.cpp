/*
 * QueryParser.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Gangasagar and Harshad
 */

#include "QueryParser.h"
#define FILE_PATH "./tempFile"
#define FILE_PATH_O "./resFile"

void externalSort(int totalFiles);
string removeQuotes(string current);
std::vector <int> columnNoForOrderBy;
std::vector <string> columnTypeForOrderBy;
std::vector <int> numberOfRecordsInTempFile;

QueryParser::QueryParser() {
	// TODO Auto-generated constructor stub
	configFilePath="config.txt";
	//dbObject.readConfig(configFilePath);
	pathForData=".";
	resultSize=0;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

QueryParser::QueryParser(string configFilePath1){

	configFilePath=configFilePath1;
	resultSize=0;
	//dbObject.readConfig(configFilePath);
	//pathForData=dbObject.getPathForData();
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

QueryParser::QueryParser(DBSystem dbObject1){
	//dbObject=dbObject1;
	pathForData=dbObject1.getPathForData();
	resultSize=0;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

QueryParser::~QueryParser() {
	// TODO Auto-generated destructor stub

	tablesInSelect.clear();
	columnsInSelect.clear();
	distinctStrs.clear();
	conditionsInSelect.clear();
	orderByStrs.clear();
	groupByStrs.clear();
	havingStrs.clear();

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

bool QueryParser::isStringPresentInVector(string tableName,vector < string > tNames)
{
	for(int i=0;i<tNames.size();i++)
		if(tNames[i]==tableName)
			return true;

	return false;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::writeTable(string tableName,vector< pair<string, string > > columns)
{
	string pathForTable=pathForData+"/"+tableName+".data";
	string pathForTableCsv=pathForData+"/"+tableName+".csv";

	FILE *fd1=fopen(pathForTable.c_str(),"w+");
	FILE *fd2=fopen(pathForTableCsv.c_str(),"w+");

	if(fd2==NULL || fd1==NULL)
	{
		cout<<"Failed to create file"<<endl;
		return;
	}

	FILE *fd3=fopen(configFilePath.c_str(),"a+");
	if(fd3==NULL)
	{
		cout<<"File not found"<<endl;
		return;
	}

	// Write to config file
	fprintf(fd3,"BEGIN\n%s\n",tableName.c_str());

	for(int i=0;i<columns.size();i++)
	{
		fprintf(fd3,"%s,%s\n",columns[i].first.c_str(),columns[i].second.c_str());

		//write to tablename.data file
		fprintf(fd1,"%s:%s,",columns[i].first.c_str(),columns[i].second.c_str());
	}
	fprintf(fd3,"END\n");
	fprintf(fd1,"\n");

	fclose(fd1);
	fclose(fd2);
	fclose(fd3);
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

bool QueryParser::areColumnsFromTable(	DBSystem dbObject,string tableName, vector< string> columns)
{
	vector< pair < string, string > > colm=dbObject.getTableNameToColumnNames()[tableName];
	int colmSize=columns.size();
	int presentColmSize=colm.size();

	for(int i=0;i<colmSize;i++){
		bool find1=false;
		for(int j=0;j<presentColmSize;j++){
			if(columns[i]==colm[j].first){

				find1=true;
				break;
			}
		}
		if(!find1){
			return false;
		}
	}
	return true;

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printVector(vector < string > temp)
{
	int i=0;
	int l=temp.size();
	if(l==0)
		cout<<"NA"<<endl;
	else
	{
		for(i=0;i<l-1;i++)
		{
			cout<<temp[i]<<",";
		}
		cout<<temp[i]<<endl;
	}
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printOutput(string type)
{
	cout<<"Querytype:"<<type<<endl;

	cout<<"Tablename:";
	printVector(tablesInSelect);

	cout<<"Columns:";
	printVector(columnsInSelect);

	cout<<"Distinct:";
	printVector(distinctStrs);

	cout<<"Condition:";
	printVector(conditionsInSelect);

	cout<<"Orderby:";
	printVector(orderByStrs);

	cout<<"Groupby:";
	printVector(groupByStrs);

	cout<<"Having:";
	printVector(havingStrs);

	/*tablesInSelect.clear();
	columnsInSelect.clear();
	distinctStrs.clear();
	conditionsInSelect.clear();
	orderByStrs.clear();
	groupByStrs.clear();
	havingStrs.clear();
*/
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printCreate(string tableName,vector< pair < string, string> > temp)
{
	cout<<"Querytype:create"<<endl;

	cout<<"Tablename:"<<tableName<<endl;

	cout<<"Attributes:";
	int i=0;
	int l=temp.size();
	if(l==0)
		cout<<"NA"<<endl;
	else
	{
		for(i=0;i<l-1;i++)
		{
			cout<<temp[i].first<<" "<<temp[i].second<<",";
		}
		cout<<temp[i].first<<" "<<temp[i].second<<endl;
	}

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
bool distinctPresent(string query){
	int l=query.length();
	string temp;

	for(int k=0;k<l && k<20;k++){
		if(query.at(k)==' ' || query.at(k)=='(' ||query.at(k)==')' || query.at(k)==',')
			continue;
		temp+=query.at(k);

		if(temp=="select" || temp=="Select" || temp=="SELECT")
			temp="";
		else if(temp=="Distinct" || temp=="DISTINCT" || temp=="distinct")
			return true;

	}
	return false;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
bool havingPresent(string query){
	int l=query.length();
	string temp;

	for(int k=0;k<l ;k++){
		if(query.at(k)==' ' || query.at(k)=='(' ||query.at(k)==')' || query.at(k)==','){
			temp="";
			continue;
		}
		temp+=query.at(k);
		if(temp=="Having" || temp=="HAVING" || temp=="having" )
			return true;

	}
	return false;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

bool ifAllCaps(string current){
	for(int i=0;i<current.length();i++)
		if((current.at(i)!='\n' && current.at(i)!=' ') && (current.at(i)<'z' && current.at(i)>'a'))
			return false;

	return true;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::queryType(DBSystem dbObject,string query){

	pathForData=dbObject.getPathForData();
	stringstream  stream(query);

	string  word;
	stream >> word;

	transform(word.begin(), word.end(), word.begin(), ::tolower);

	if(word == "select")
		selectCommand(dbObject,query);
	else if(word == "create")
		createCommand(dbObject,query);
	else
		cout<<"Query Invalid\n";

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::queryFile(string queryFile){
	FILE *sqlFile=fopen(queryFile.c_str(),"r");
	gsp_sqlparser *parser;
	int rc;

	if(sqlFile == NULL){
				fprintf(stderr,"script file %s doesn't exist!\n", queryFile.c_str() );
	}

	if(sqlFile!=NULL){
			rc= gsp_check_syntax_file(parser, sqlFile);
		}
	if (rc != 0){
			fprintf(stderr,"parser error:%s",gsp_errmsg(parser));
			gsp_parser_free(parser);
	}

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printOutputOfQuery(DBSystem dbObject)
{
	string patho = FILE_PATH_O;
	ifstream infile;
	infile.open(patho.c_str());

	vector< string > columnsInTable;
	columnsInTable=dbObject.getAllColumnNames(tablesInSelect[0],columnsInTable);

	vector< int > colmnNum;
	int len=columnsInSelect.size();

	/*for(int i=0;i<len;i++)
	{
			if(i>0)
				cout<<",";
			cout<<"\""<<columnsInTable[i]<<"\"";
	}*/

	for(int i = 0;i<len;i++)
		colmnNum.push_back(dbObject.columnNumberInRow(tablesInSelect[0],columnsInSelect[i]));
/*
	for(int i=0;i<len;i++)
	{
		if(i>0)
			cout<<",";
		cout<<colmnNum[i]<<endl;
	}*/

	//cout<<colmnNum.size()<<endl;
//	sort(colmnNum.begin(),colmnNum.end());


	for(int j=0;j<len;j++){
			if(j>0)
				cout<<",";
			cout<<"\""<<columnsInTable[colmnNum[j]]<<"\"";
	}
	cout<<endl;

	while(!infile.eof())
	{
		resultSize--;
		//cout<<resultSize<<endl;
		string current("");
		getline(infile, current);

		//cout<<current<<endl;
		current=removeQuotes(current);

		vector< string > a;
		std::istringstream ssA(current);
		std::string token;

		while(std::getline(ssA, token, ',')) {
		   		a.push_back(token);
		}

		/*cout<<a.size()<<endl;
		cout<<colmnNum.size()<<" "<<len<<endl;
*/
		for(int i=0;i<len;i++){
			if(i>0)
				cout<<",";
			cout<<"\""<<a[colmnNum[i]]<<"\"";
		}

		cout<<endl;
		if(resultSize<=0)
			break;
	}

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printOutputOfJoin(DBSystem dbObject,vector<string> record)
{
	vector< string > columnsInTable1,columnsInTable2;
	columnsInTable1=dbObject.getAllColumnNames(orderOfTableInjoin[0],columnsInTable1);
	columnsInTable2=dbObject.getAllColumnNames(orderOfTableInjoin[1],columnsInTable2);
	//cout<<"----------------------------------------------"<<endl;
	vector< pair< int, int > > colmnNum;
	int len=columnsInSelect.size();

	for(int i = 0;i<len;i++){
		int pos = columnsInSelect[i].find(".");
		string tableN=columnsInSelect[i].substr(0,pos);
		string c=columnsInSelect[i].substr(pos+1);

		if(orderOfTableInjoin[0]==tableN){
			std::pair< int, int > p(1,dbObject.columnNumberInRow(orderOfTableInjoin[0],c));
			colmnNum.push_back(p);
		}
		else{
			std::pair< int, int > p(2,dbObject.columnNumberInRow(orderOfTableInjoin[1],c));
			colmnNum.push_back(p);
		}
	}

	for(int j=0;j<len;j++){
			if(j>0)
				cout<<",";
			if(colmnNum[j].first==1)
				cout<<"\""<<columnsInTable1[colmnNum[j].second]<<"\"";
			else
				cout<<"\""<<columnsInTable2[colmnNum[j].second]<<"\"";
	}
	cout<<endl;
	int recordL=record.size();
	for(int k=0;k<recordL;k++)
	{
		string current=record[k];
		current=removeQuotes(current);

		int pos=current.find("**");
		string current1=current.substr(0,pos);
		string current2=current.substr(pos+2);

		vector< string > a1,a2;
		std::istringstream ssA1(current1);
		std::string token;
		while(std::getline(ssA1, token, ',')) {
				a1.push_back(token);
		}

		std::istringstream ssA(current2);
		while(std::getline(ssA, token, ',')) {
				a2.push_back(token);
		}

		for(int i=0;i<len;i++){
			if(i>0)
				cout<<",";
			if(colmnNum[i].first==1)
				cout<<"\""<<a1[colmnNum[i].second]<<"\"";
			else
				cout<<"\""<<a2[colmnNum[1].second]<<"\"";
		}
		cout<<endl;
		//cout<<" -------------------        "<<endl;
	}
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::selectCommand(DBSystem dbObject,string query)
{
	char* sql=new char[query.length()+1];
	strcpy(sql,query.c_str());

	gsp_sqlparser *parser;
	List *nodeList;
	Iterator iter;
	orderByExistsBolean=false;

	int rc;

	rc = gsp_parser_create(dbvoracle,&parser);
	if (rc){
			fprintf(stderr,"create parser error");
			return ;
	}

	rc= gsp_check_syntax(parser,sql);
	if (rc != 0){
		fprintf(stderr,"parser error:%s\n",gsp_errmsg(parser));
		return;
	}

	SqlTraverser *traverser= createSqlTraverser();
	nodeList = traverser->traverseSQL(traverser, &parser->pStatement[0]);

	iter = nodeList->getIterator(nodeList);
	bool starFlag=false;
	bool distinctFlag=false;
	bool isJoinTrue=false;


	distinctFlag=distinctPresent(query);
	string joinCondition="";

	while(nodeList->hasNext(nodeList,&iter))
	{
			gsp_node *node = (gsp_node*)nodeList->next(&iter);
			/********************* column name******************************/
			if(node->nodeType == t_gsp_resultColumn){
				string str(gsp_node_text(node));
				string colStr="";
				int ln=str.length();
				for(int i=0;i<ln;i++){
					if(str.at(i)==')' && distinctFlag)
						distinctFlag=false;

					if(str.at(i)=='(' || str.at(i)==')' || str.at(i)==' ')
						continue;

					/*if(str.at(i)=='.'){
						colStr="";
						continue;}
					 */
					colStr+=str.at(i);
				}

				if(colStr=="*"){

					starFlag=true;
					continue;
				}

				if(isStringPresentInVector(colStr,columnsInSelect))
					continue;

				if(distinctFlag)
					distinctStrs.push_back(colStr);

				columnsInSelect.push_back(colStr);
			}
			/********************* table Name ******************************/
			else if(node->nodeType == t_gsp_fromTable ){
				string tableStr(gsp_node_text(node));
				//cout<<tableStr;
                //continue;

				if(!isStringPresentInVector(tableStr,dbObject.getTableNames())){

					if(tableStr.find("join")!=string::npos || tableStr.find("JOIN")!=string::npos )
						continue;
					cout<<"Query Invalid2\n";
					return;
				}
				else{

					if(isStringPresentInVector(tableStr,tablesInSelect))
							continue;

					if(starFlag){
						dbObject.getAllColumnNames(tableStr,columnsInSelect);
						if(distinctFlag)
							dbObject.getAllColumnNames(tableStr,distinctStrs);
					}

					tablesInSelect.push_back(tableStr);
				}
			}

			/********************* where BY******************************/
			else if(node->nodeType == t_gsp_whereClause){

				string whereStr(gsp_node_text(node));
				string temp=whereStr.substr(6,whereStr.length());
				if(isStringPresentInVector(temp,conditionsInSelect))
						continue;
				if(whereParser(dbObject,temp)){
					conditionsInSelect.push_back(temp);
					continue;
				}
				else{
				cout<<"Query Invalid 1\n";
					return;
				}
			}
			/********************* order BY******************************/
			else if(node->nodeType == t_gsp_orderBy){

					orderByExistsBolean=true;
					string orderByStr(gsp_node_text(node));
					int l=orderByStr.length();
					string temp="";

					for(int m=9;m<l;m++){
						if((orderByStr.at(m)==',' || orderByStr.at(m)==')')&& !temp.empty()){
							if(isStringPresentInVector(temp,orderByStrs)){
								temp="";
								continue;
							}
							/*if(!isStringPresentInVector(temp,columnsInSelect))
							{
								cout<<"Query Invalid \n";
								return;
							}*/
							orderByStrs.push_back(temp);
							temp="";
						}
						else if(orderByStr.at(m)!='(')
							temp+=orderByStr.at(m);

					}
					if(temp!="")
						orderByStrs.push_back(temp);

			}

			/********************* Group BY******************************/
			else if(node->nodeType == t_gsp_groupBy){
					string currentStr(gsp_node_text(node));

					string groupBy("");
					string havingStr("");
					int length=currentStr.length();

					for(int i=9;i<length;i++){
						if(currentStr.at(i)==' ' || currentStr.at(i)=='\n')
						{
							if(!groupBy.empty()){
								length=i;
							break;
							}
						}
						else if(currentStr.at(i)==',' || (currentStr.at(i)==')')) {
							if(!isStringPresentInVector(groupBy,columnsInSelect)){
								cout<<"Query Invalid \n";
								return;

							}
							if(!isStringPresentInVector(groupBy,groupByStrs)){
								groupByStrs.push_back(groupBy);
								groupBy="";
							}
						}
						else if(currentStr.at(i)!='(')
							groupBy+=currentStr.at(i);

					}

					if(!isStringPresentInVector(groupBy,columnsInSelect)){

						cout<<"Query Invalid 3\n";
						return;
					}

					if(!isStringPresentInVector(groupBy,groupByStrs)){
							groupByStrs.push_back(groupBy);
							groupBy="";
					}

					/************************* having **********************************/
					if(!havingPresent(currentStr))
						continue;

					havingStr=currentStr.substr(length+2+6,currentStr.length());
					if(whereParser(dbObject,havingStr)){
						havingStrs.push_back(havingStr);
						continue;
					}
					else{
						cout<<"Query Invalid 4\n";
						return;
					}

			}

			/**********************   Join Expr   *********************/
			else if(node->nodeType == t_gsp_joinExpr){

				string jStr(gsp_node_text(node));
		//		cout<<"Join expr -->  "<<jStr<<endl;
				int pos=jStr.rfind(" ON ")+4;

				joinCondition=jStr.substr(pos,jStr.size());
				isJoinTrue=true;
				joinConditionsDP.push_back(joinCondition);
			//	cout<<"join condition --< "<<joinCondition<<endl;
			}
	}

	if(!isLeftHandOperatorIsValid(dbObject,columnsInSelect) ){

		cout<<"Query Invalid 5 \n";
		return;
	}

	//printOutput("select");

	/**********************   Result   *********************/

	//
	if(joinConditionsDP.size()>1){
		getMultiJoinAttribs(dbObject);
		calculateRes();
	}
	else if(isJoinTrue)
		executeJoin(joinCondition,dbObject);
	else{
		executeWhereinSelect(dbObject);
		printOutputOfQuery(dbObject);
	}
	gsp_parser_free(parser);

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::createCommand(DBSystem dbObject,string query) {

	char* sql=new char[query.length()+1];
	strcpy(sql,query.c_str());

	gsp_sqlparser *parser;
	List *nodeList;
	Iterator iter;

	int rc = gsp_parser_create(dbvoracle,&parser);
	if (rc){
		fprintf(stderr,"create parser error");
		return ;
	}

	rc= gsp_check_syntax(parser,sql);
	if (rc != 0){
		fprintf(stderr,"parser error:%s\n",gsp_errmsg(parser));
		return;
	}

	SqlTraverser *traverser= createSqlTraverser();
	nodeList = traverser->traverseSQL(traverser, &parser->pStatement[0]);
	iter = nodeList->getIterator(nodeList);
	vector< pair<string, string > > columns;


	while(nodeList->hasNext(nodeList,&iter))
	{
			gsp_node *node = (gsp_node*)nodeList->next(&iter);

			/********************* column name ******************************/
			if(node->nodeType == t_gsp_columnDefinition)
			{
				string tempColumnLine(gsp_node_text(node));
				string col=tempColumnLine.substr(0,tempColumnLine.find(' '));
				string type=tempColumnLine.substr(tempColumnLine.find(' ')+1,tempColumnLine.length());
				std::pair < string,string > bar = std::make_pair (col,type);
				columns.push_back(bar);
			}
			/********************* table name ******************************/
			else if(node->nodeType == t_gsp_table)
			{
				string tableName(gsp_node_text(node));
				if(isStringPresentInVector(tableName,dbObject.getTableNames()))
				{
					cout<<"Query Invalid\n";
					return;
				}

				dbObject.addTableNameToColumnNames(tableName,columns);

				writeTable(tableName,columns);
				printCreate(tableName,columns);
				columns.clear();
			}
		}

		gsp_parser_free(parser);
}
/*****************************************************************************************************/
/* Deliverable 3*/
/*****************************************************************************************************/
string removeQuotes(string current)
{
	string temp="";
	int len=current.length();
	for(int i=0;i<len;i++)
	{
		if(current.at(i)!='\"' && current.at(i)!='\'' )
			temp+=current.at(i);
	}
	return temp;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
vector <string> QueryParser::executeWhereinSelect(DBSystem dbObject)
{
	string patho = FILE_PATH_O;
	columnNoForOrderBy.clear();
	columnTypeForOrderBy.clear();
	numberOfRecordsInTempFile.clear();

	vector<string>  result;
	ofstream of1(patho.c_str(),ios::trunc);

	if(orderByExistsBolean)
		of1.close();

	string primaryKey;
	int noOfTempFile = 0;
	int recordId = 0;
			string record;
			record = dbObject.getRecord(tablesInSelect[0],recordId);

			while(record != "")
			{

				if(conditionsInSelect.size() == 0){
					if(!orderByExistsBolean){
						of1<<record<<endl;
						resultSize++;
					}
					else{
						/*if(orderByExistsBolean){
							cout<<"it's orderby"<<endl;
						}*/
					result.push_back(record);
					if(result.size()>=1000){
						executeOrderBy(dbObject,result,noOfTempFile);
						numberOfRecordsInTempFile.push_back(result.size());
						result.resize(0);
						noOfTempFile++;
					}}
				}
				else
					if(checkConditionForRecord(dbObject,record))
					{
						if(!orderByExistsBolean){
							of1<<record<<endl;
							resultSize++;
						}
						else{
						result.push_back(record);
						if(result.size()>=1000){
							executeOrderBy(dbObject,result,noOfTempFile);
							numberOfRecordsInTempFile.push_back(result.size());
							result.resize(0);
							noOfTempFile++;
							}}
					}
				recordId++;
				record = dbObject.getRecord(tablesInSelect[0],recordId);
			}
			if(orderByExistsBolean && result.size()>0){
				executeOrderBy(dbObject,result,noOfTempFile);
				numberOfRecordsInTempFile.push_back(result.size());
				noOfTempFile++;
				result.resize(0);
			}

			if(orderByExistsBolean)
				externalSort(noOfTempFile);

			if(!orderByExistsBolean)
					of1.close();

	return result;
}

/*****************************************************************************************************/
/**/

/*****************************************************************************************************/
bool QueryParser::checkConditionForRecord(DBSystem dbObject,string record)
{
	std::vector <int> columnNo;
	record=removeQuotes(record);
	//cout<<record<<endl;
	/*for(int i = 0;i<leftOperand.size();i++)
		cout<<leftOperand[i]<<" ";
	cout<<"\n";
	for(int i = 0;i<leftHandOperatorType.size();i++)
			cout<<leftHandOperatorType[i]<<" ";
	cout<<"\n";
	for(int i = 0;i<rightOperand.size();i++)
			cout<<rightOperand[i]<<" ";
	cout<<"\n";
	for(int i = 0;i<columnNo.size();i++)
				cout<<columnNo[i]<<" ";
*/
	for(int i = 0;i<leftOperand.size();i++)
			columnNo.push_back(dbObject.columnNumberInRow(tablesInSelect[0],leftOperand[i]));

	/*for(int i = 0;i<columnNo.size();i++)
				cout<<columnNo[i]<<" ";
*/

	int count = 0, colIndex = 0;
	bool isAnd,result;
	if(conjunction.size()>0 && conjunction[0] == "OR")
		isAnd = false;
	else
		isAnd = true;

	result = isAnd;

	istringstream ss(record);
	string token;

	while(std::getline(ss, token, ',')) {
		//cout<<token<<endl;

		for(colIndex=0;colIndex<columnNo.size();colIndex++)
		{
		if(columnNo[colIndex] == count){
			bool individualRes = false;
	    	int valI,valIR;
	    	float valF,valFR;
	    	string valS,valSR;
	    	if(midOperator[colIndex] == "<"){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    			if(valI<valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF<valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}

	    	}
	    	if(midOperator[colIndex] == "<="){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    	//		cout<<valI<<"  "<<valIR<<endl;
	    			if(valI<=valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF<=valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}

	    	}
	    	if(midOperator[colIndex] == ">"){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    			if(valI>valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF>valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}

	    	}
	    	if(midOperator[colIndex] == ">="){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    			//cout<<valI<<"  "<<valIR<<endl;
	    			if(valI>=valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF>=valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}

	    	}
	    	if(midOperator[colIndex] == "="){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    			if(valI == valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF == valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "VARCHAR")
	    			if(token == removeQuotes(rightOperand[colIndex]))
	    				individualRes = true;
	    			else
	    				individualRes = false;

	    	}
	    	if(midOperator[colIndex] == "!="){
	    		if(leftHandOperatorType[colIndex] == "INT"){
	    			valI = boost::lexical_cast<int>(token);
	    			valIR = boost::lexical_cast<int>(rightOperand[colIndex]);
	    			if(valI != valIR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    		if(leftHandOperatorType[colIndex] == "FLOAT"){
	    			valF = boost::lexical_cast<float>(token);
	    			valFR = boost::lexical_cast<float>(rightOperand[colIndex]);
	    			if(valF != valFR)
	    				individualRes = true;
	    			else
	    				individualRes = false;
	    		}
	    	}
	    	if(midOperator[colIndex] == "LIKE"){

	    		transform(token.begin(),token.end(),token.begin(), ::tolower);
	    		valSR = removeQuotes(rightOperand[colIndex]);
	    		transform(valSR.begin(),valSR.end(),valSR.begin(), ::tolower);

	    		//cout<<valSR<<"  "<<token<<endl;
	    		if(valSR == token)
    				individualRes = true;
    			else
    				individualRes = false;
	    	}
	    	if(isAnd){
	    		result = result && individualRes;
	    	}
	    	else{
	    		result = result || individualRes;
	    	}
	    }
	}
		count++;
	}
//	cout<<result<<endl;
	return result;

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
int QueryParser::comparatorForOrderBy(string a, string b)
{
	a=removeQuotes(a);
	b=removeQuotes(b);

	vector< string > tempForA,tempForB;
	std::istringstream ssA(a),ssB(b);
	std::string token;

	while(std::getline(ssA, token, ',')) {
		tempForA.push_back(token);
	}

	while(std::getline(ssB, token, ',')) {
		tempForB.push_back(token);
	}

	int len=columnNoForOrderBy.size();

	for(int i=0;i<len;i++ )
	{
		if(tempForA[columnNoForOrderBy[i]]==tempForB[columnNoForOrderBy[i]])
			continue;

		if(columnTypeForOrderBy[i] == "INT")
		{
			  int valI = boost::lexical_cast<int>(tempForA[columnNoForOrderBy[i]]);
			  int valIR = boost::lexical_cast<int>(tempForB[columnNoForOrderBy[i]]);
			  if(valI > valIR)
			  		return 1;
			  else if (valI < valIR)
				  	return -1;
			  else
			  		continue;
		}

		if(columnTypeForOrderBy[i] == "FLOAT")
		{
				float valI = boost::lexical_cast<float>(tempForA[columnNoForOrderBy[i]]);
				float valIR = boost::lexical_cast<float>(tempForB[columnNoForOrderBy[i]]);
				if(valI > valIR)
					return 1;
				else if (valI < valIR)
				 	return -1;
				else
					continue;
		}

		if(tempForA[columnNoForOrderBy[i]].compare(tempForB[columnNoForOrderBy[i]])<0)
			return -1;

		if(tempForA[columnNoForOrderBy[i]].compare(tempForB[columnNoForOrderBy[i]])>0)
			return 1;

	}
	return 0;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::writeResultVectorToFile(vector<string> temp, int a)
{
	string path= FILE_PATH + boost::lexical_cast<string>(a);

	ofstream myfile;
	myfile.open(path.c_str(),ios::trunc);

	int len=temp.size();
	for(int i=0;i<len;i++){
		if(i>0)
			myfile<<"\n";
		myfile<<temp[i];

		resultSize++;
	}
	myfile<<"\n";
	myfile.close();
}


/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::executeOrderBy(DBSystem dbObject,vector <string> result,int nof)
{
	int len=orderByStrs.size();
	for(int i = 0;i<len;i++){
		columnNoForOrderBy.push_back(dbObject.columnNumberInRow(tablesInSelect[0],orderByStrs[i]));
		columnTypeForOrderBy.push_back(typeForColumnName(dbObject,tablesInSelect[0]+"."+orderByStrs[i]));
	}
	for(int i=0;i<result.size();i++){
		for(int j=0;j<result.size()-i-1;j++){
			if(comparatorForOrderBy(result[j],result[j+1]) == 1){
				string temp = result[j];
				result[j] = result[j+1];
				result[j+1] = temp;
			}
		}
	}
	writeResultVectorToFile(result,nof);


}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
string removeCarretAtTheEnd(string temp)
{
	unsigned found = temp.find_last_of("^");
	return temp.substr(0,found);

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
class CompareRecord{

public:
	int operator()(string a,string b) // t2 has highest prio than t1 if t2 is earlier than t1
    {

		a=removeQuotes(a);
    	b=removeQuotes(b);

    	a=removeCarretAtTheEnd(a);
    	b=removeCarretAtTheEnd(b);


    	vector< string > tempForA,tempForB;
    	std::istringstream ssA(a),ssB(b);
    	std::string token;

    	while(std::getline(ssA, token, ',')) {
    		tempForA.push_back(token);
    	}

    	while(std::getline(ssB, token, ',')) {
    		tempForB.push_back(token);
    	}

    	int len=columnNoForOrderBy.size();

    	for(int i=0;i<len;i++ )
    	{
    		if(tempForA[columnNoForOrderBy[i]]==tempForB[columnNoForOrderBy[i]])
    			continue;

    		if(columnTypeForOrderBy[i] == "INT")
    		{
    			int valI = boost::lexical_cast<int>(tempForA[columnNoForOrderBy[i]]);
    			  int valIR = boost::lexical_cast<int>(tempForB[columnNoForOrderBy[i]]);
    			  if(valI > valIR)
    			  		return true;
    			  else if (valI < valIR)
    				  	return false;
    			  else
    			  		continue;
    		}

    		if(columnTypeForOrderBy[i] == "FLOAT")
    		{
    				float valI = boost::lexical_cast<float>(tempForA[columnNoForOrderBy[i]]);
    				float valIR = boost::lexical_cast<float>(tempForB[columnNoForOrderBy[i]]);
    				if(valI > valIR)
    					return true;
    				else if (valI < valIR)
    				 	return false;
    				else
    					continue;
    		}

    		if(tempForA[columnNoForOrderBy[i]].compare(tempForB[columnNoForOrderBy[i]])<0)
    			return false;

    		if(tempForA[columnNoForOrderBy[i]].compare(tempForB[columnNoForOrderBy[i]])>0)
    			return true;

    	}
    	return true;
}
};


/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void externalSort(int totalFiles)
{
	string path = FILE_PATH;
	string patho = FILE_PATH_O;
	vector <std::ifstream *> ifs;
	priority_queue< string, vector<string>, CompareRecord > pq;
	int i;
	for(i=0;i<=totalFiles;i++){
		string tempPath = path + boost::lexical_cast<std::string>(i);
		std::ifstream* f = new std::ifstream(tempPath.c_str(),std::ios::in);
				ifs.push_back(f);
	}
	string line;
	for(i=0;i<=totalFiles;i++){
		if(numberOfRecordsInTempFile[i]>0 && getline ((*(ifs[i])),line)){
			numberOfRecordsInTempFile[i]--;

			line += "^" + boost::lexical_cast<std::string>(i);
			pq.push(line);
		}
	}

	ofstream of1(patho.c_str(),ios::trunc);

	while(!pq.empty()){
		string oline,line = pq.top();
		pq.pop();
		int pos = line.find_last_of("^");
		int fileNo = boost::lexical_cast<int>(line.substr(pos+1));
		oline = line.substr(0,pos);
		of1<<oline<<"\n";

		if(numberOfRecordsInTempFile[fileNo]>0 && getline (*ifs[fileNo],line)){
			numberOfRecordsInTempFile[fileNo]--;
			line += "^" + boost::lexical_cast<std::string>(fileNo);
			pq.push(line);

		}
	}
	of1.close();
	for(i=0;i<totalFiles;i++)
		(*(ifs[i])).close();

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
string compareFunc(string a,string b,int cn1,int cn2,string type1, string type2)
{
	a = removeQuotes(a);
	b = removeQuotes(b);
	vector< string > tempForA,tempForB;
    std::istringstream ssA(a),ssB(b);
    std::string token;
    string retVal;

    while(std::getline(ssA, token, ',')) {
    	tempForA.push_back(token);
    }

    while(std::getline(ssB, token, ',')) {
    	tempForB.push_back(token);
    }
   	string col1,col2;
   	col1 = tempForA[cn1];
   	col2 = tempForB[cn2];

    if(type1 == "INT")
    {
    	int valI = boost::lexical_cast<int>(col1);
    	int valIR = boost::lexical_cast<int>(col2);
    	if(valI > valIR){
    		//cout<<"Greater "<<valI<<" "<<valIR<<"\n";
    		retVal = "Greater";
    	}
    	else if (valI == valIR){
    		retVal = "Equal";
//    		cout<<valI<<"\n";
    	}
    	else{
    	//	cout<<"Lesseer "<<valI<<" "<<valIR<<"\n";
    		retVal = "Lesser";
    	}
    }

    else if(type1 == "FLOAT")
    		{
    				float valI = boost::lexical_cast<float>(col1);
    				float valIR = boost::lexical_cast<float>(col2);
    				if(valI > valIR)
    				    		retVal = "Greater";
    				    	else if (valI == valIR)
    				    		retVal = "Equal";
    				    	else
    				    		retVal = "Lesser";
    		}

    else{
    	if(col1.compare(col2)<0)
    			retVal = "Lesser";
    	else if(col1.compare(col2)>0)
    			retVal = "Greater";
    	else
    		retVal = "Equal";

    	}
    	return retVal;
}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::executeJoin(string joinCondition,DBSystem dbObject)
{
	//cout<<"enetered"<<"\n";
	int index=joinCondition.find("=");
	string prev=joinCondition.substr(0,index);
	string last=joinCondition.substr(index+1);
	priority_queue< string, vector<string>, CompareRecord > pq1;
	priority_queue< string, vector<string>, CompareRecord > pq2;
	int columnNumber1,columnNumber2;
	vector <string> result;
	string tableName1,tableName2,column1,column2,colType1,colType2;

	int pLen=prev.find(".");
	int lLen=last.find(".");

	tableName1 = prev.substr(0,pLen);
	tableName2 = last.substr(0,lLen);
	column1 = prev.substr(pLen+1);
	column2 = last.substr(lLen+1);

	orderOfTableInjoin.push_back(tableName1);
	orderOfTableInjoin.push_back(tableName2);
	/*************************************************************88888*/
	columnNoForOrderBy.clear();
	columnTypeForOrderBy.clear();
	columnNoForOrderBy.push_back(dbObject.columnNumberInRow(tableName1,column1));
	columnNumber1 = dbObject.columnNumberInRow(tableName1,column1);
	columnTypeForOrderBy.push_back(typeForColumnName(dbObject,prev));
	colType1 = typeForColumnName(dbObject,prev);
	string filePath(dbObject.getPathForData());
	string path1 = filePath + "/" + tableName1 + ".csv";
	string path2 = filePath + "/" + tableName2 + ".csv";
	//cout<<tableName1<<" "<<column1<<" "<<path1<<"\n";
	//cout<<tableName2<<" "<<column2<<" "<<path2<<"\n";
	ifstream infile;
	infile.open(path1.c_str());
	infile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	string sLine;

	try{
		while(!infile.eof()){

			getline(infile, sLine);
			//cout<<sLine<<endl;
			pq1.push(sLine);
			}
	}
	catch(std::ifstream::failure e){
		;
	}
	cout<<"******"<<endl;
	infile.close();

	/*************************************************************88888*/
	columnNoForOrderBy.clear();
	columnTypeForOrderBy.clear();
	columnNoForOrderBy.push_back(dbObject.columnNumberInRow(tableName2,column2));
	columnTypeForOrderBy.push_back(typeForColumnName(dbObject,last));
	columnNumber2 = dbObject.columnNumberInRow(tableName2,column2);
	colType2 = typeForColumnName(dbObject,last);
	ifstream infile1;
	infile1.open(path2.c_str());
	infile1.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	try{
	while(!infile1.eof()){
		getline(infile1, sLine);
		//cout<<sLine<<endl;
		pq2.push(sLine);
	}
	}
	catch(std::ifstream::failure e){
			;
	}
	infile1.close();
	vector<string> res1,res2;
	while(!pq1.empty() && !pq2.empty()){

		string rec1 = pq1.top();
		string rec2 = pq2.top();
		if(compareFunc(rec1,rec2,columnNumber1,columnNumber2,colType1,colType2) == "Equal"){

			res1.push_back(rec1);
			res2.push_back(rec2);
			pq1.pop();
			pq2.pop();
			if(pq1.empty() || pq2.empty())
				break;
			rec1 = pq1.top();
			rec2 = pq2.top();
			while(compareFunc(res1[0],rec2,columnNumber1,columnNumber2,colType1,colType2) == "Equal" && !pq1.empty() && !pq2.empty()){
				res2.push_back(rec2);
				pq2.pop();
				if(!pq2.empty())
					rec2 = pq2.top();
			}
			while(compareFunc(rec1,res2[0],columnNumber1,columnNumber2,colType1,colType2) == "Equal" && !pq1.empty() && !pq2.empty()){
				res1.push_back(rec1);
				pq1.pop();
				if(!pq1.empty())
					rec1 = pq1.top();

			}
			int l1,l2;
			l1 = res1.size();
			l2 = res2.size();
			for(int i= 0;i<l1;i++)
				for(int j=0;j<l2;j++){
					result.push_back(res1[i]+"**"+res2[j]);
					//string re=res1[i]+"**"+res2[j];

				//	cout<<res1[i]<<"**"<<res2[j]<<"\n";
				}
			res1.clear();
			res2.clear();
		}
		else if(compareFunc(rec1,rec2,columnNumber1,columnNumber2,colType1,colType2) == "Greater")
			pq2.pop();
		else
			pq1.pop();

	}
	printOutputOfJoin(dbObject,result);
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
std::pair<string, string> getTablePairInCondition(string condition)
{
	int pos=condition.find("=");
	string left=condition.substr(0,pos);
	string right=condition.substr(pos+1);

	string lTable=left.substr(0,left.find("."));
	string rTable=right.substr(0,right.find("."));

	pair< string, string> p(lTable,rTable);

	return p;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
std::pair<string, string> getColumnPairInCondition(string condition)
{
	int pos=condition.find("=");
	string left=condition.substr(0,pos);
	string right=condition.substr(pos+1);

	//cout<<"in get colmun pair"<<endl;
	//cout<<condition<<endl;
	//cout<<left<<endl;
	//cout<<right<<endl;

	string lTable=left.substr(left.find(".")+1);
	string rTable=right.substr(right.find(".")+1);

	pair< string, string> p(lTable,rTable);

	return p;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::getMultiJoinAttribs(DBSystem dbObject)
{
	//cout<<tablesInSelect[0]<<endl;
	//cout<<tablesInSelect[1]<<endl;
	//cout<<tablesInSelect[2]<<endl;

	//cout<<"multi join table order +++++++++"<<endl;
	int len=joinConditionsDP.size();
	vector< pair< string,string> > tablePairs;
	vector< pair< string,string> > columnPairs;
	vector< bool > areTablesSwapped;

	for(int i=0;i<len;i++)
	{
		areTablesSwapped.push_back(false);
		//cout<<joinConditionsDP[i]<<endl;
		tablePairs.push_back(getTablePairInCondition(joinConditionsDP[i]));
		columnPairs.push_back(getColumnPairInCondition(joinConditionsDP[i]));
	}

	for(int j=1;j<len;j++){
		if(tablePairs[j].first == tablePairs[j-1].first && !areTablesSwapped[j-1]){
			string temp=tablePairs[j-1].first;
			tablePairs[j-1].first=tablePairs[j-1].second;
			tablePairs[j-1].second=temp;
			areTablesSwapped[j-1]=true;
			temp=tablePairs[j-1].first;
						tablePairs[j-1].first=tablePairs[j-1].second;
						tablePairs[j-1].second=temp;
		}

		else if(tablePairs[j].second == tablePairs[j-1].first && !areTablesSwapped[j-1]){
			string temp=tablePairs[j-1].first;
			tablePairs[j-1].first=tablePairs[j-1].second;
			tablePairs[j-1].second=temp;
			areTablesSwapped[j-1]=true;
			areTablesSwapped[j]=true;

			temp=tablePairs[j].first;
			tablePairs[j].first=tablePairs[j].second;
			tablePairs[j].second=temp;

			temp=columnPairs[j-1].first;
			columnPairs[j-1].first=columnPairs[j-1].second;
			columnPairs[j-1].second=temp;

			temp=columnPairs[j].first;
			columnPairs[j].first=columnPairs[j].second;
			columnPairs[j].second=temp;
		}
		else if(tablePairs[j].second == tablePairs[j-1].second){
				areTablesSwapped[j]=true;
				string temp=tablePairs[j].first;
				tablePairs[j].first=tablePairs[j].second;
				tablePairs[j].second=temp;

				temp=columnPairs[j].first;
				columnPairs[j].first=columnPairs[j].second;
				columnPairs[j].second=temp;
		}
		else if(tablePairs[j].first == tablePairs[j-1].second){
					areTablesSwapped[j-1]=true;
					areTablesSwapped[j]=true;
		}
	}

	int cnt=0;
	for(int j=0;j<len;j++){
		if(j!=0){
			string t1=tablePairs[j].first;
			string t2=tablePairs[j].second;

			if(tableSequenceDP[cnt-1]==t1){
				tableSequenceDP.push_back(t2);

				pair< int, int > p2(dbObject.V(t2,columnPairs[j].second),0);
				distinctValuesDP.push_back(p2);
				distinctValuesDP[cnt-1].second=dbObject.V(t1,columnPairs[j].first);

				cnt++;
			//	cout<<tableSequenceDP[cnt-1]<<endl;
			}
			else{
				tableSequenceDP.push_back(t1);
				tableSequenceDP.push_back(t2);
				pair< int, int > p1(0,dbObject.V(t1,columnPairs[j].first));
				pair< int, int > p2(dbObject.V(t2,columnPairs[j].second),0);

				distinctValuesDP.push_back(p1);
				distinctValuesDP.push_back(p2);

				cnt+=2;
		//		cout<<tableSequenceDP[cnt-2]<<endl;
		//		cout<<tableSequenceDP[cnt-1]<<endl;
			}
		}
		else if(j==0){
			tableSequenceDP.push_back(tablePairs[j].first);
			tableSequenceDP.push_back(tablePairs[j].second);

			pair< int, int > p1(0,dbObject.V(tablePairs[j].first,columnPairs[j].first));
			pair< int, int > p2(dbObject.V(tablePairs[j].second,columnPairs[j].second),0);

			distinctValuesDP.push_back(p1);
			distinctValuesDP.push_back(p2);

			cnt+=2;
		//	cout<<tableSequenceDP[0]<<endl;
		//	cout<<tableSequenceDP[1]<<endl;
		}


	}

	for(int i=0;i<cnt;i++){
		tableSizeDP.push_back(dbObject.tableNameToNumberOfRecords[tableSequenceDP[i]]);
		//cout<<tableSequenceDP[i]<<endl;
		//cout<<distinctValuesDP[i].first<<"-"<<distinctValuesDP[i].second<<endl;
	}


}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
int QueryParser::calcCostOfJoin(int i,int j,int k, vector<vector<int> > &m)
{
	int n1,n2;
	n1 = m[i][k]>0 ? m[i][k] : (int)tableSizeDP[i];
	n2 =  m[k+1][j]>0 ? m[k+1][j]: (int)tableSizeDP[j];
	return (n1*n2)/(int)(std::max(distinctValuesDP[k].second, distinctValuesDP[k+1].first));
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::calculateRes()
{
	int n = tableSequenceDP.size();
	vector<vector<int> > m(n+1, vector<int> (n+1,0.0));
	vector<vector<int> > s(n+1, vector<int> (n+1,0));
	int i,j,l,k,q,minK=0;
	for(l=2;l<=n;l++){
		for(i=0;i<=n-l;i++){
			j = i+l-1;
			m[i][j] = INT_MAX;
			for(k=i; k<j; k++){
				q = m[i][k] + m[k+1][j];
				if(q<m[i][j]){
					m[i][j] = q;
					s[i][j] = k;
					minK = k;
				}
			}
			m[i][j] += calcCostOfJoin(i,j,minK,m);
		}
	}
	printSoln(s,0,n-1);
	cout<<"\n";
	cout<<"Cost "<<m[0][n-1] - calcCostOfJoin(0,n-1,s[0][n-1],m)<<endl;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::printSoln(vector<vector<int> > &s,int i,int j)
{
	if(i==j)
		cout<<tableSequenceDP[i]<<" ";
	else{
		cout<<"(";
		printSoln(s,i,s[i][j]);
		printSoln(s,s[i][j]+1,j);
		cout<<")";
	}
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
