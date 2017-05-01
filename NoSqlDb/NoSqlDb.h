#pragma once
/////////////////////////////////////////////////////////////////////
//  NoSqlDb.h - Key-value pair in Database                         //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Lenovo , Windows 10      			           //
//  Application:   NoSql Database - CIS 687 Project 1              //
//  Author:        Sushanth Suresh SUID:987471535			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
It defines two classes No SQL database class and an Element class
*No SQL Database works as a container class implementing an unordereded
map on Heap memory with an instance of Element class as the value with
a unique Key.
* No SQL class defines operations such as editiing, deleting, 
persisting of values into an XML file and extracting the values from 
XML.
These two classes use Template functionalities

Public Interface:
=================

getChildren()             // Member function that gives the the dependent keys
editData()                // Overloaded member function that edit values
toXml()                   //Member function that persists the values into an XML
Keys()                    // Member function that gives the keys in the Database
save()                    //Member function saves elememt in the database
value()                   //Member function Gets the value

Element Class contains

name							// Member variables of type Property
childrenKeys					//Member variable Vector of strings holding dependent Keys



Build Process:
==============
Required files
- Cppproperties.h, Convert.h,
- XmlDocument.h, XmlElement.h,



Build commands (either one)

- devenv Project1_Sushanth.sln


Maintenance History:
====================
ver 2.0 : used only creation and edition
ver 1.0 : 07 Feb 17
- first release
*/

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include "../CppProperties/CppProperties.h"
#include "../XMLnew/XmlDocument/XmlDocument.h"
#include "../XMLnew/XmlElement/XmlElement.h"
#include <fstream>
#include<string>
/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
// - in our NoSql database that is just the value in a key/value pair
// - it needs to store child data, something for you to implement
//
template<typename Data>
class Element
{
public:
	
  using Name = std::string;
  using Childrens = std::vector<std::string>;  
  Property<Name> name;            // metadata
  Property<Childrens> children;    //metadata  
  std::string show();
};

template<typename Data>
std::string Element<Data>::show()
{
	std::ostringstream out;
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	Childrens children_ = static_cast<Childrens>(children);
	if (children_.size() > 0)
	{
		out << "\n    " << std::setw(8) << "children" << " : ";
		for (size_t i = 0; i < children_.size(); ++i)
		{
			out << children_[i];
			if (i < children_.size())
				out << ", ";
		}
	}
	out << "\n";
	return out.str();
}



/////////////////////////////////////////////////////////////////////
// NoSqlDb class is a key/value pair in-memory database
// - stores and retrieves elements
// - you will need to add query 
//   That should probably be another class that uses NoSqlDb
//
using namespace XmlProcessing;
using SPtr = std::shared_ptr<AbstractXmlElement>;
template<typename Data>
class NoSqlDb
{
public:
  using Key = std::string;
  using ToBeModified = std::string;
  using Keys = std::vector<Key>;
  using xml = std::string;
  using listOf = std::vector<xml>;

  std::vector<std::string> getChildren(Key key);
  void editData(Key, std::string, std::string);
  std::string toXml();

  Keys keys();
  bool save(Key key, Element<Data> elem);
  Element<Data> value(Key key);
  void show();
  size_t count();
private:
  using Item = std::pair<Key, Element<Data>>;
  std::unordered_map<Key,Element<Data>> store; 
};
template<typename Data>
inline void NoSqlDb<Data>::show()
{

	Keys temp = keys();
	Element<Data> elemTem;
	for (auto each : temp) {
		elemTem = value(each);
		std::cout << elemTem.show();
	}
}

//---------------Puts the values of the database into the XML-----------------------------
template<typename Data>
std::string NoSqlDb<Data>::toXml()
{
	xml theDocument;
	XmlDocument doc;
	try	{
	listOf chil;
	SPtr pRoot = makeTaggedElement("Database");
	doc.docElement() = pRoot;
	for (xml each : keys()) {
		Element<Data> x = value(each);
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
		chil = getChildren(x.name);//children
		int count = chil.size();
		int i = 0;
		SPtr pChildElement = makeTaggedElement("children");
		dElement->addChild(pChildElement);
		while (i != count) {
			xml child = chil[i];
			SPtr pchildtag = makeTaggedElement("child");
			pChildElement->addChild(pchildtag);
			SPtr pchildText = makeTextElement(child);
			pchildtag->addChild(pchildText);
			i++;
		}
	}
		theDocument = doc.toString();
		std::ofstream input_file("../database.xml");
		input_file << theDocument;
		input_file.close();
	}
	catch (const std::exception& e){std::cout << "exception:" << e.what();
	}	
	return theDocument;
}



//----------------------------- Editing name and childrens of the element----------
template<typename Data>
void NoSqlDb<Data>::editData(Key key,std::string e, std::string dat) {
	bool flag = false;

	Element<Data> elem = value(key);
	if (e == "name") {
		elem.name = dat;
		flag = true;
	}
	else if (e == "children") {
		Element<Data> elem = value(key);
		std::vector<std::string> childList = getChildren(key);
		std::vector<std::string>::iterator it;
		if ((it = std::find(childList.begin(), childList.end(), dat)) != childList.end()) {

		}
		else
		{
				childList.push_back(dat);
				elem.children = childList;
				store[key] = elem;		
		}

	}
	else {
		std::cout << "provide parameter";
	}
	
}


//-----------Returns all the Keys in the database-----------------
template<typename Data>
typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
{
  Keys keys;
  for (Item item : store)
  {
     keys.push_back(item.first);
  }
  return keys;
}
//-----------Adds an Element into the database---------------------
template<typename Data>
bool NoSqlDb<Data>::save(Key key, Element<Data> elem)
{
	//time_t createdtime = time(&createdtime);
	//elem.timeDate = createdtime;
  if(store.find(key) != store.end())
    return false;
  store[key] = elem;
  return true;
}
//--------------------Gets the Element using its associated Key-----------
template<typename Data>
Element<Data> NoSqlDb<Data>::value(Key key)
{
  if (store.find(key) != store.end())
    return store[key];
  return Element<Data>();
}

//------------------Counts the number of keys-----------------------
template<typename Data>
size_t NoSqlDb<Data>::count()
{
  return store.size();
}


// --------------------------get the children list------------------------------------
template<typename Data>
std::vector<std::string> NoSqlDb<Data>::getChildren(Key key)
{
	Element<Data> temp = value(key);
	std::vector<std::string> childList = temp.children;
	return childList;
}