#pragma once
/////////////////////////////////////////////////////////////////////
//  TypeTable.h - Key-value pair storing type information          //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Lenovo , Windows 10       			           //
//  Application:   CodeAnalysis - CIS 687 Project 2                //
//  Author:        Sushanth Suresh SUID:987471535			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
It defines three classes Tuple class,TypeTable class and LineNO class
*TypeTable works as a container class implementing an unordereded
map on Heap memory with an instance of Tuple class as the value with
a unique Key.
* Tuple class is a user defined data type having type,namespace and file
 information
*LineNO : saves the starting and ending values


Public Interface:
=================
TypeTable class contains
getKeys()										//Returns all the keys in the TypeTable
insert(Key, std::vector< Tuple>)				//inserts a tuple into the TypeTable
value(Key)									    //Returns a tuple for a key in the TypeTable
count()										//Count of number of entries in the TypeTable
getTable()										//Returns a TypeTable Reference
doTableAnal()									// Creates the typeTable
getLineNoTable()								//return Line number table
Tuple Class contains

setType(Type_name)			//sets the type of the tuple
setNamespace(Namespace_name)  //sets the namespace of the tuple
setFile(File_name)			//sets the file name to which the type belongs to
getType()				//returns the type of the tuple
getNamespace()			//returns the namespace of the tuple
getFileName()			//returns the file name to which the type belongs to


Build Process:
==============
Required files
-ActionsAndRules.h



Build commands (either one)

- devenv TypeTable.sln


Maintenance History:
====================
ver 1.0 : 07 March 17
- first release
ver 2.0 : 04 April 17
-added code to create a unordered map which stores the starting
and ending lines of all the classes and functions
*/

#include "../Parser/ActionsAndRules.h"
#include<iostream>
#include<stack>
#pragma warning (disable : 4101)

namespace CodeAnalysis {
	class Tuple
	{
	public:
		using Type_name = std::string;
		using Namespace_name = std::string;
		using File_name = std::string;

		Tuple() {};
		Tuple(std::string typ, std::string nmespce, std::string fle) : type_of(typ), namespace_of(nmespce), file_of(fle) {};// Tuple is created using a initializer sequence
		bool setType(Type_name);			//sets the type of the tuple
		bool setNamespace(Namespace_name);  //sets the namespace of the tuple
		bool setFile(File_name);			//sets the file name to which the type belongs to
		std::string getType();				//returns the type of the tuple
		std::string getNamespace();			//returns the namespace of the tuple
		std::string getFileName();			//returns the file name to which the type belongs to
	private:

		Type_name type_of;
		Namespace_name namespace_of;
		File_name file_of;



	};
	class LineNO {
	public:
		int startLine;
		int endLine;
	};

	
	using Key = std::string;
	using Keys = std::vector<Key>;
	class  TypeTable
	{
	public:

		using SPtr = std::shared_ptr<ASTNode>;

		TypeTable();
		~TypeTable();

		Keys getKeys();														//Returns all the keys in the TypeTable
		bool insert(Key, std::vector< Tuple>);								//inserts a tuple into the TypeTable
		std::vector<Tuple> value(Key);									    //Returns a tuple for a key in the TypeTable
		int count();														//Count of number of entries in the TypeTable
		std::unordered_map<std::string, std::vector<Tuple>>& getTable();	//Returns a TypeTable Reference
		void doTableAnal();													// Creates the typeTable
		std::unordered_map<std::string, std::vector<LineNO>> getLineNoTable();//return Line number table
	private:
		
		std::string correct_namespace;										
		std::stack<std::string> nameSpaceStack;								//stack to record the name spaces
		void pushIntoMap(ASTNode* pNode);									// pushes required AST nodes into the TypeTable
		void DFS(ASTNode* pNode);											//Walks the Abstract Syntax tree in a Depth First Serach way
		bool doDisplay1(ASTNode* pNode);									//checks for the required types
		AbstrSynTree& ASTref_;
		std::unordered_map<std::string, std::vector<Tuple>> T_Table;
		std::unordered_map<std::string, std::vector<LineNO>> tyLine;
		void append(LineNO x,std::string pathName);
		std::string pType;
	};


	
}
