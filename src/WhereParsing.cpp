#include <iostream>
#include <string>
#include <vector>
#include "QueryParser.h"
using namespace std;

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
int isIntOrFloat(string term){
	int i,l,flag = 1;
	l = term.length();
	for(i=0;i<l;i++)
		if(!( (term[i]>='0'&& term[i]<='9') || term[i] == '.' ) )
			return 0;
	if(term.find(".") == std::string::npos)
		return 1;
	else
		return 2;

}

/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
bool QueryParser::whereParser(DBSystem dbObject, string str){

	//cout<<"i ma where "<<str<< "\n";
	//cout<<str<<endl;
	int i,l;
	l=str.length();
	string token = "";

	for(i=0;i<l;){
		while(i<l && ((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z') || str[i] == '.' || str[i] == '_' || str[i] == '-' || str[i] == '\'' || str[i] == '"')){
			token += str[i];
			i++;
		}
		if(token != "")
			leftOperand.push_back(token);
		while(i<l && str[i] == ' ')
			i++;
		token = "";
		if(str[i] == '!' && str[i+1] == '='){				//<>
			i+=2;
			midOperator.push_back("!=");
		}
		else if(str[i] == '<' && str[i+1] == '='){			//<=
			i+=2;
			midOperator.push_back("<=");
		}
		else if(str[i] == '>' && str[i+1] == '='){			//>=
			i+=2;
			midOperator.push_back(">=");
		}
		else if(str[i] == '='){						//=
			i++;
			midOperator.push_back("=");
		}
		else if(str[i] == '<'){						//<
			i++;
			midOperator.push_back("<");
		}
		else if(str[i] == '>'){						//>
			i++;
			midOperator.push_back(">");
		}
		else if((i+3)<l && (str.substr(i,4) == "LIKE" || str.substr(i,4) == "like")){			//like
		//	cout<<"enetred here";
			i+=4;
			midOperator.push_back("LIKE");
		}
		while(i<l && str[i] == ' ')
			i++;
		token = "";
		while(i<l && ((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z') || (str[i]>='0' && str[i]<='9') || str[i] == '.'|| str[i] == '_' || str[i] == '-'||str[i] == '\''|| str[i] == '"')){
			token += str[i];
			i++;
		}
		if(token != "")
			rightOperand.push_back(token);
		token = "";
		while(i<l && str[i] == ' ')
			i++;
		if(i+2<l && (str.substr(i,3) == "AND" || str.substr(i,3) == "and")){
			conjunction.push_back("AND");
			i+=3;
		}
		else if(i+1<l && (str.substr(i,2) == "OR" || str.substr(i,2) == "or")){
			conjunction.push_back("OR");
			i+=2;
		}

		while(i<l && str[i] == ' ')
					i++;

	}

	/*for(i=0;i<leftOperand.size();i++)
		cout<<leftOperand[i]<< " ";
	cout<<"\n";
	for(i=0;i<midOperator.size();i++)
		cout<<midOperator[i]<< " ";
	cout<<"\n";
	for(i=0;i<rightOperand.size();i++)
		cout<<rightOperand[i]<< " ";
	cout<<"\n";
	for(i=0;i<conjunction.size();i++)
		cout<<conjunction[i]<< " ";
	cout<<"\n";*/

	int l1,l2,l3;
	l1 = leftOperand.size();
	l2 = midOperator.size();
	l3 = rightOperand.size();
	typeforLeftHandOperator(dbObject,leftOperand);
	typeforRightHandOperator(dbObject,rightOperand);
	if(l1 != l2 || l2 != l3 || l1 != l3)
		return false;
	l1 = leftHandOperatorType.size();
	l3 = rightHandOperatorType.size();
	if(l1 != l2 || l2 != l3 || l1 != l3)
			return false;
	for(i=0;i<l1;i++){
		if(leftHandOperatorType[i] != rightHandOperatorType[i])
			return false;
		if(leftHandOperatorType[i] == "VARCHAR" && !(midOperator[i] == "LIKE" || midOperator[i] =="="))
			return false;
		if(leftHandOperatorType[i] != "VARCHAR" && midOperator[i] == "LIKE")
			return false;
	}
	return true;

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
bool QueryParser::isLeftHandOperatorIsValid(DBSystem dbObject,vector<string> lhOperator){
	//yet to handle for tablename.attribute
	bool answer = true;
	bool tempAns;
	int i,j,l1,l2;
	string currentColumn="";
	l1 = lhOperator.size();
	l2 = tablesInSelect.size();

	vector<string> tv;
	for(i=0;i<l1;i++){
		currentColumn = lhOperator[i];
		tv.resize(0);
		tv.push_back(currentColumn);
		tempAns = false;
		string tableN="";

		if(currentColumn.find(".")!= std::string::npos){
			tv.resize(0);
			int pos=currentColumn.find(".");
			string c = currentColumn.substr(pos+1);
			tableN = currentColumn.substr(0,pos);
			tv.push_back(c);

			tempAns = tempAns || areColumnsFromTable(dbObject,tableN,tv);
		}
		else{
			for(j=0;j<l2;j++)
				tempAns = tempAns || areColumnsFromTable(dbObject,tablesInSelect[j],tv);
		}
		answer = answer && tempAns;
	}
	return answer;
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/

void QueryParser::typeforLeftHandOperator(DBSystem dbObject,vector<string>& lhOperator){//
	int i,l;
	leftHandOperatorType.resize(0);
	l = lhOperator.size();
	for(i=0;i<l;i++)
		leftHandOperatorType.push_back(typeForColumnName(dbObject,lhOperator[i]));
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
void QueryParser::typeforRightHandOperator(DBSystem dbObject,vector<string>& rhOperator){//
	int i,l,k;
	string currentAttr;
	rightHandOperatorType.resize(0);
	l = rhOperator.size();
	for(i=0;i<l;i++){
		currentAttr = rhOperator[i];
		//cout<<currentAttr<<" in rhs\n";
		if(currentAttr.find("'") != std::string::npos || currentAttr.find("\"") != std::string::npos)
			rightHandOperatorType.push_back("VARCHAR");
		else{
			k = isIntOrFloat(currentAttr);
			//cout<<k<<" is k\n";
			if(k==0)
				rightHandOperatorType.push_back(typeForColumnName(dbObject,currentAttr));
			else if(k==1)
				rightHandOperatorType.push_back("INT");
			else
				rightHandOperatorType.push_back("FLOAT");
		}
	}
}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
string QueryParser::typeForColumnName(DBSystem dbObject,string columnName){
	int j,l2;
	size_t found;
	string operatorType = "";
	string tableName;

	map< string, vector< pair < string, string > > > tct;
	tct = dbObject.getTableNameToColumnNames();
	l2 = tablesInSelect.size();

	vector<string> tempV;
	found = columnName.find(".");

	if(found != std::string::npos){
		tableName = columnName.substr(0,found);
		columnName = columnName.substr(found+1);
		tempV.resize(0);
		tempV.push_back(columnName);
		if(!areColumnsFromTable(dbObject,tableName,tempV))
			return "FAIL";
		else{
			for(j=0;j<tct[tableName].size();j++){
				if(tct[tableName][j].first == columnName)
					return tct[tableName][j].second;
			}
			return "FAIL";
		}
	}
	else{
		tempV.resize(0);
		tempV.push_back(columnName);
		int count = 0;
		string tableName;
		for(j=0;j<l2;j++){
			if(areColumnsFromTable(dbObject,tablesInSelect[j],tempV)){
				tableName = tablesInSelect[j];
				count++;
			}
		}
		if(count!=1)
			return "FAIL";
		else{
			for(j=0;j<tct[tableName].size();j++)
				if(tct[tableName][j].first == columnName)
					return tct[tableName][j].second;

			return "FAIL";
		}
	}

}
/*****************************************************************************************************/
/**/
/*****************************************************************************************************/
