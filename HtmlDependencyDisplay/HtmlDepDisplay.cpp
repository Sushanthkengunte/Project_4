////////////////////////////////////////////////////////////////////////
// HtmlDepDisplay.cpp - Displays the dependencies in internet explorer//
// ver 1.0                                                            //
//--------------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                               //
// Platform:    Lenovo, Windows 10                                    //
// Application: Project #3, CSE687 - Object Oriented Design, S2015    //
// Author:		sushanth suresh SUID:987471535					      //
//		       					                                      //
////////////////////////////////////////////////////////////////////////

#include "HtmlDepDisplay.h"
#include "../FileSystem/FileSystem.h"
#include<iostream>
#include<fstream>
#include<istream>
#include<ostream>
#include<sstream>
#include<direct.h>
#include<vector>
#include<regex>

//-------takes the dependecy table line no map and the repository path-----------//
MakingDirectory::MakingDirectory(NoSqlDb<std::string>& database, std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>> lTable, std::string indexrepository)
{
	filesToCopy = database.keys();
	db = database;
	addheaderDepen(db);
	localLineNO = lTable;
	repositoryLocation = indexrepository;

}
MakingDirectory::~MakingDirectory()
{
}
//------------copies all the files inside a specified directory-----------------------//
void MakingDirectory::copyTheFile(std::string destination)
{
	directorySet newDirectory;
	std::string replacedString;
	for (auto each : filesToCopy) {
		std::string newFileName = appendFileName(each);
		newDirectory.push_back(newFileName);
		computeStartLine(each);
		computeEndLine(each);
		replacedString = getFileCon(each);
		createHTMLFile(newFileName,replacedString,destination);
		allFiles[newFileName] = repositoryLocation + "/"+newFileName;
	}
	createRepHtml();
}

//------------gets the start Lines for a specific file-------------------------------//
void MakingDirectory::computeStartLine(std::string forFile)
{
	startLine.clear();
	auto each = localLineNO[forFile];
		for (auto startingLine : each)
		startLine.push_back(startingLine.startLine);
	
}
//--------------------gets the end tLines for a specific file----------------------//
void MakingDirectory::computeEndLine(std::string forFile)
{
	endLine.clear();
	auto each = localLineNO[forFile];
	for (auto endingLine : each)
			endLine.push_back(endingLine.endLine);
}
//-------------------replaces < with &lt; and > with &gt;-------------------------//
std::string MakingDirectory::replaceEscape(std::string originalString)
{
	std::string correctString;
	for (auto eachChar : originalString) {
		if (eachChar == '<')
			correctString += "&lt;";
		else if (eachChar == '>')
			correctString += "&gt;";
		else 
			correctString += eachChar;
	}
	return correctString;
}
//---------------------returns the correct content of a specific file----------------//
std::string MakingDirectory::getFileCon(std::string fName)
{
	codeLineNo = 0;
	std::string originalString;
	std::string correctContent;
	std::vector<std::string> dependentFiles;
	dependentFiles = db.getChildren(fName);
	std::ifstream file(fName);
		//Sets the prologue for a specific file
		correctContent += prologueHtml(FileSystem::Path::getName(fName));
		//puts the head body tags
		correctContent +="<html><head><link rel=\"stylesheet\" href=\"../style.css\"></head><body><h3>" + fName + "</h3><hr/><div class = \"indent\"><h4>dependencies:</h4>";
		correctContent += "<ul>";
		for (auto item : dependentFiles) {
			correctContent += "<li>";
			correctContent+="<a href = \"" + FileSystem::Path::getName(item) + ".html\">" + item + "</a>";
			correctContent += "</li>";
		}
		correctContent += "</ul>";
		correctContent+="</div><hr/><pre>";
		//for each line get escape character or add div tags
		while (getline(file, originalString))
		{ 
			codeLineNo++;
			correctContent += extractContent(originalString);
			//if (std::find(startLine.begin(), startLine.end(), codeLineNo) != startLine.end())
			//{
			//	correctContent += "<div class = \"externalPart\"><span> + </span></div><div class = \"internalPart\">";
			//	correctContent += originalString;
			//}
			//else if (std::find(endLine.begin(), endLine.end(), codeLineNo) != endLine.end()) {
			//	correctContent += originalString;
			//	correctContent += "</div>";
			//}
			//else {
			//	correctContent += replaceEscape(originalString);
			//	correctContent += "\n";
			//}
		}
		std::string jQuery = " https://code.jquery.com/jquery-1.11.3.min.js";//----jQuery library link---------------------//
		correctContent += "</pre><script src=\"" + jQuery + "\"></script>";
		correctContent += "<script src=\"../project3.js\"></script></body></html>";//----Java sricpt path---------------//
	file.close();
	return correctContent;
}
//------------------Creates the Html File using the content and puts it in the desired repository------------------//
void MakingDirectory::createHTMLFile(std::string filename, std::string & contentsOfFile,std::string destination)
{
	try
	{
		std::ofstream dirFile;
		std::string storeInsideDestination;
		storeInsideDestination += destination + "/";
		storeInsideDestination += FileSystem::Path::getName(filename);
		dirFile.open(storeInsideDestination);
		if (!dirFile.is_open()) {
			std::cout << "couldnt open";
		}

		dirFile << contentsOfFile;
		dirFile.close();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}

}

void MakingDirectory::addheaderDepen(NoSqlDb<std::string>& db)
{
	std::regex regularexp(".*.h");
	auto something = db.keys();
	for (std::string each : something) {
		if (std::regex_match(each, regularexp)) {
			std::string temp = each;
			temp.pop_back();
			temp.pop_back();
			temp.append(".cpp");
			if(std::find(something.begin(), something.end(),temp) != something.end())
				db.editData(each, "children", temp);
		}
	}
}
//--------------appends .html to the filenames--------------------//
std::string MakingDirectory::appendFileName(std::string oldFileName)
{

	oldFileName += ".html";
	return oldFileName;
}
//---------------creates the index file with all the files in the repository------------//
void MakingDirectory::createRepHtml()
{
	std::string correctContent;
	std::ofstream dirFile;
	std::string storeInsideDestination;
	storeInsideDestination = repositoryLocation + "/index.html";
	dirFile.open(storeInsideDestination);
	if (!dirFile.is_open()) {
		std::cout << "could not open";
	}
	correctContent += prologueHtml("index.html");
	correctContent += "<html><head><link rel=\"stylesheet\" href=\"../style.css\"></head><body><h3>Index Page</h3><hr/><ul>";
	std::vector<std::string> Keys = db.keys();
	for (auto each : Keys)
		correctContent += "<li><a href=\" " + FileSystem::Path::getName(each) + ".html\">" + each + "</a></li>";
	correctContent += "</ul></body></html>";
	dirFile << correctContent;
	dirFile.close();
	
}
std::string MakingDirectory::extractContent(std::string originalString)
{
	std::string temp;
	if (std::find(startLine.begin(), startLine.end(), codeLineNo) != startLine.end())
	{
		temp += "<div class = \"externalPart\"><span> + </span></div><div class = \"internalPart\">";
		temp += originalString;
	}
	else if (std::find(endLine.begin(), endLine.end(), codeLineNo) != endLine.end()) {
		temp += originalString;
		temp += "</div>";
	}
	else {
		temp += replaceEscape(originalString);
		temp += "\n";
	}
	return temp;
}
//--------------displays the index file in a repository---------------------------------//
void MakingDirectory::displayIndexPage(std::string Repository, LPCWSTR browser)
{
	try
	{
		std::string fullPathToRepository = FileSystem::Path::getFullFileSpec(Repository);
		std::string indexFile = fullPathToRepository + "\\index.html";
		std::string path = "\"file:///" + indexFile+"\"";
		std::wstring spath = std::wstring(path.begin(), path.end());
		LPCWSTR swpath = spath.c_str();
		LPCWSTR a = L"open";
		LPCWSTR ch = browser;
		ShellExecute(NULL, a, ch, swpath, NULL, SW_SHOWDEFAULT);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
 
}
//-----------provides prologue for a specific file-------------------------------//
std::string MakingDirectory::prologueHtml(std::string fName)
{
	std::string prologueString;
	prologueString += "<!-----------------------------------\n"+fName+"\n Help file for Project #3 Published by Sushanth Suresh, CSE687 - Object Oriented Design, Spring 2017";
	prologueString += "\n\nNote:\n - Markup characters in the text part, enclosed in <pre>...</pre> have to be\nreplaced with escape sequences, e.g., < becomes &lt; and > becomes &gt;\n";
	prologueString += "- Be careful that you don't replace genuine markup characters with escape \nsequences, e.g., everything outside of the <pre>...</pre> section. ";
	prologueString += "\n---------------------------------------->\n";
	return prologueString;
}

#ifdef HTML_DISPLAY
int main() {
	NoSqlDb<std::string> dummyDirectory;
	std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>> lineTable;
	std::string filesIn = "../Repository";
	LPCWSTR ch = browser;
	if (_mkdir("../Repository") == 0)
		std::cout << "Repository Created";
	else
		std::cout << "coudnt create repository";
	MakingDirectory a(dummyDirectory,lineTable, filesIn);
	a.copyTheFile(filesIn);
	htmlDisplay.displayIndexPage(filesIn, browser);
}
#endif // HTML_DISPLAY

