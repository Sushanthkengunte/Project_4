/////////////////////////////////////////////////////////////////////
//  DependencyAnalysis.h - Initialises the typeAnalysis and        //
//  and does dependency analysis ver 1.0                           //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Lenovo , Windows 10       			           //
//  Application:   CodeAnalysis - CIS 687 Project 2                //
//  Author:        Sushanth Suresh SUID:987471535			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
It performs the dependency analysis by creating the TypeTable and checking 
type dependencies.


Public Interface:
=================
DependencyAnalysis class contains

doDependAnal()				// does the dependency analysis for the File Set
getDatabase()				//return the dependency database
getTheTable()				//returns the type table


Build Process:
==============
Required files
-Tokenizer.h
-TypeTable.h
-NoSqlDb.h
-FileSystem.h



Build commands

- devenv DependencyAnalysis.sln


Maintenance History:
====================
ver 1.0 : 07 March 17
- first release
*/
#pragma once
#include<iostream>
#include "../Tokenizer/Tokenizer.h"
#include "../TypeTable/TypeTable.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileSystem/FileSystem.h"

class DependencyAnalysis
{
public:
	DependencyAnalysis(std::unordered_map<std::string, std::vector<std::string>>& file) : file_paths(file) {
		std::cout << "\n Creating Type table";
		row.doTableAnal();				//creates Type table for all the file set
		Type_Final = row.getTable();	//gets the created TypeTable
		std::cout << "\n Type table created\n Startind Dependency analysis";
		toker.returnComments(false);	//discards the comments in the file
	};
	~DependencyAnalysis();
	void doDependAnal();				// does the dependency analysis for the File Set
	NoSqlDb<std::string>& getDatabase();
	std::unordered_map<std::string, std::vector<CodeAnalysis::Tuple>>& getTheTable();
	void persistIntoXml(std::string filePa);
	std::unordered_map<std::string, std::vector<CodeAnalysis::LineNO>> getTypeLine();

private:
	void validate(std::vector<std::string>&, std::string);
	std::vector<std::string> allFiles();			//returns all the file names in the unordered map
	std::vector<std::string> pushFileTokens();		// check if any of the tokens are present in the type table
	CodeAnalysis::TypeTable row;
	std::unordered_map<std::string, std::vector<CodeAnalysis::Tuple>> Type_Final;
	Scanner::Toker toker;
	std::unordered_map<std::string, std::vector<std::string>>& file_paths;
	NoSqlDb<std::string> db;
};

