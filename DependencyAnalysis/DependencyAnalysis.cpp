/////////////////////////////////////////////////////////////////////
// DependencyAnalysis.cpp - Performs dependency analysis           //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Lenovo, Windows 10                                 //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:		sushanth suresh SUID:987471535					   //
//		       					                                   //
/////////////////////////////////////////////////////////////////////


#include "DependencyAnalysis.h"
#include<iostream>
//---------returns a reference to the database-------//
NoSqlDb<std::string>& DependencyAnalysis::getDatabase()
{
	return db;
}
//------------returns a reference to the type table-----------------//
std::unordered_map<std::string, std::vector<CodeAnalysis::Tuple>>& DependencyAnalysis::getTheTable()
{
	return Type_Final;
}

void DependencyAnalysis::persistIntoXml(std::string filePa)
{
	std::string theDocument;
	XmlDocument doc;
	try {
		std::vector<std::string> chil;
		SPtr pRoot = makeTaggedElement("Database");
		doc.docElement() = pRoot;
		for (std::string each : db.keys()) {
			Element<std::string> x = db.value(each);
			SPtr dElement = makeTaggedElement("record");
			pRoot->addChild(dElement);
			SPtr pKeyElement = makeTaggedElement("keyofValues");//Keys
			dElement->addChild(pKeyElement);
			SPtr pKeyText = makeTextElement(x.name);
			pKeyElement->addChild(pKeyText);
			SPtr pNameElement = makeTaggedElement("name");	//Name
			dElement->addChild(pNameElement);
			SPtr pNameText = makeTextElement(x.name);
			pNameElement->addChild(pNameText);
			chil = db.getChildren(x.name);//children
			int count = chil.size();
			int i = 0;
			SPtr pChildElement = makeTaggedElement("children");
			dElement->addChild(pChildElement);
			while (i != count) {
				std::string child = chil[i];
				SPtr pchildtag = makeTaggedElement("child");
				pChildElement->addChild(pchildtag);
				SPtr pchildText = makeTextElement(child);
				pchildtag->addChild(pchildText);
				i++;
			}
		}
		theDocument = doc.toString();
		std::ofstream input_file(filePa);
		input_file << theDocument;
		input_file.close();
	}
	catch (const std::exception& e) {
		std::cout << "exception:" << e.what();
	}
}

std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>> DependencyAnalysis::getTypeLine()
{

	return row.getLineNoTable();
}



DependencyAnalysis::~DependencyAnalysis()
{

}
//--------performs the dependency analysis-------------------//
 void DependencyAnalysis::doDependAnal()
{
	 try
	 {
		 std::vector<std::string> all_filepaths = allFiles();	//gets all the file names
		 std::vector<std::string> depFile;
		 for (auto eachFile : all_filepaths) { //for each files in the directory
			 std::ifstream in(eachFile);
			 toker.attach(&in);
			 do {
				 depFile = pushFileTokens();	//check if any of the token is in the file is also present in the Type Table 
												//and return the files which corresponds to the tokens 
				 validate(depFile, eachFile);	//adds the dependent files as children to the current file

			 } while (in.good());
			 //Testing
			 in.close();

		 }
	 }
	 catch (const std::exception& e)
	 {
		 std::cout<<e.what();
	 }


}
 //--------Adds the dependent files to as children to the current file--------------------//
void DependencyAnalysis::validate(std::vector<std::string>& depFile, std::string curFile)
{
	for (auto eachFile : depFile) {
		Element<std::string> temp;
		temp.name = curFile;
		db.save(curFile, temp);

		if (eachFile != curFile)
		{
			db.editData(curFile, "children", eachFile);//add to the db as a child
		}
	}


}
//---------gets all the file names-------------//
std::vector<std::string> DependencyAnalysis::allFiles()
{
	
		std::vector<std::string> tempKeys;
		for (auto item : file_paths)
			for (auto x : item.second)
				tempKeys.push_back(x);
		return tempKeys;
	

}
//------------checks if the token is present in the type table------------------//
std::vector<std::string> DependencyAnalysis::pushFileTokens()
{
	std::vector<std::string> depFiles;
	try
	{
		std::string tok = toker.getTok();
		//stores all the dependent files
		if (tok == "\n") { return depFiles; }
		std::vector<CodeAnalysis::Tuple> depTuple;
		if (Type_Final.find(tok) != Type_Final.end())
			depTuple = Type_Final[tok];// if you find tok in typeTable key return the value
		for (auto eachItem : depTuple)
			depFiles.push_back(eachItem.getFileName());
		
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	return depFiles;
}
#ifdef TEST_DEPENDENCY_ANALYSIS
int main() {
	std::unordered_map<std::string, std::vector<std::string>> file;// assign the fies needed to compute the analysis
	DependencyAnalysis depen(file);
	depen.doDependAnal();
	Display d;
	d.showDependencyTable(depen.getDatabase());

}

#endif // TEST_DEPENDENCY_ANALYSIS

