#pragma once
/////////////////////////////////////////////////////////////////////
//  HtnlDepDiplay.h - Displays the dependencies in internet explorer/
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Lenovo , Windows 10       			           //
//  Application:   CodeAnalysis - CIS 687 Project 3                //
//  Author:        Sushanth Suresh SUID:987471535			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
It defines MakingDirectory class
*MakingDirectory copies all the directories in a repository 
and opens the index page which contains all the files inside the given 
folder




Public Interface:
=================
copyTheFile(std::string destination)					//copies all the files inside a specified directory
computeStartLine(std::string forFile)					//gets the start Lines for a specific file
computeEndLine(std::string forFile)						//gets the end tLines for a specific file
displayIndexPage(std::string Repository)				//displays the index file in a repository


Build Process:
==============
Required files
-TypeTable.h
-NosqlDb.h
-FileSystem.h
Build commands (either one)

- devenv HtmlDepDisplay.sln


Maintenance History:
====================
ver 1.0 : 07 March 17
- first release
*/

#include<vector>
#include<stack>
#include "../NoSqlDb/NoSqlDb.h"
#include "../TypeTable/TypeTable.h"
using directorySet = std::vector<std::string>;


class MakingDirectory
{
public:
	MakingDirectory(NoSqlDb<std::string>& database, std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>>,std::string);
	~MakingDirectory();
	void copyTheFile(std::string destination);		//copies all the files inside a specified directory
	void computeStartLine(std::string forFile);		//gets the start Lines for a specific file
	void computeEndLine(std::string forFile);		//gets the end tLines for a specific file
	void displayIndexPage(std::string Repository, LPCWSTR browser);	//displays the index file in a repository

private:
	directorySet filesToCopy;
	NoSqlDb<std::string> db;
	directorySet newFiles;
	std::string replaceEscape(std::string s);
	std::string getFileCon(std::string fName);
	void createHTMLFile(std::string filename, std::string& contentsOfFile,std::string destination);
	void addheaderDepen(NoSqlDb<std::string>& db);
	std::string appendFileName(std::string oldFileName);
	void createRepHtml();
	std::string extractContent(std::string originalString);
	std::string prologueHtml(std::string fName);
	std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>> localLineNO;
	std::unordered_map<std::string, std::string> allFiles;
	int codeLineNo;
	std::vector<int> startLine;
	std::vector<int> endLine;
	std::string repositoryLocation;
};

