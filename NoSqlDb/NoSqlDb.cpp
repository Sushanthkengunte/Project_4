/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database                 //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#ifdef NO_SQL
#include "NoSqlDb.h"
#include <iostream>
using StrData = std::string;
using intData = int;
using Key = NoSqlDb<StrData>::Key;
using Keys = NoSqlDb<StrData>::Keys;
void create(NoSqlDb<StrData>& db) {
	Element<StrData> elem1;
	elem1.name = "elem1";
	std::vector<std::string> keysForElem1;
	keysForElem1.push_back("elem2");
	keysForElem1.push_back("elem3");
	elem1.children = keysForElem1;
	db.save(elem1.name, elem1);
	Element<StrData> elem2;
	std::vector<std::string> keysForElem2;
	keysForElem2.push_back("elem1");
	elem2.children = keysForElem2;
	db.save(elem2.name, elem2);
	Element<StrData> elem3;
	elem3.name = "elem3";
	std::vector<std::string> keysForElem3;
	keysForElem3.push_back("elem2");
	elem3.children = keysForElem3;
	db.save(elem3.name, elem3);
}
void edit(NoSqlDb<StrData>& db) {
	std::cout << "\n  Editing name \n ------------------------------------------------------------------------------------------------------------------\n";
	db.editData("elem1", "name", "sushanth");
	std::cout << db.value("elem1").show();
	std::cout << "\n  Editing category\n ------------------------------------------------------------------------------------------------------------------\n";
	db.editData("elem1", "category", "sushanth");
	std::cout << db.value("elem1").show();
	std::cout << "\n  Editing children\n ------------------------------------------------------------------------------------------------------------------\n";
	std::cout << db.value("elem1").show();
	std::cout << "\n  Editing data\n ------------------------------------------------------------------------------------------------------------------\n";
	std::cout << db.value("elem1").show();
	std::cout << "\n  Editing timeStamp\n ------------------------------------------------------------------------------------------------------------------\n";
	std::cout << db.value("elem1").show();
}
void displayAllElements(NoSqlDb<StrData>& db) {
	Keys storeContents = db.keys();
	for (Key eachElement : storeContents)
	{
		std::cout << "\n  " << eachElement << ":";
		std::cout << db.value(eachElement).show();
	}
}
void icreate(NoSqlDb<intData>& idb) {
	Element<intData> ielem1;
	ielem1.name = "elem1";
	std::vector<std::string> ikeysForElem1;
	ikeysForElem1.push_back("ielem2");
	ikeysForElem1.push_back("ielem3");
	ielem1.children = ikeysForElem1;
	idb.save(ielem1.name, ielem1);
	Element<intData> ielem2;
	ielem2.name = "elem2";
	std::vector<std::string> ikeysForElem2;
	ikeysForElem2.push_back("ielem1");
	ielem2.children = ikeysForElem2;
	idb.save(ielem2.name, ielem2);
	Element<intData> ielem3;
	ielem3.name = "elem3";
	idb.save(ielem3.name, ielem3);
	Element<intData> ielem4;
	ielem3.name = "elem4";
	std::vector<std::string> ikeysForElem3;
	ikeysForElem3.push_back("ielem2");
	ielem3.children = ikeysForElem3;
	idb.save(ielem3.name, ielem3);
}
void idisplayAllElements(NoSqlDb<intData>& idb) {
	Keys storeContents = idb.keys();
	for (Key eachElement : storeContents)
	{
		std::cout << "\n  " << eachElement << ":";
		std::cout << idb.value(eachElement).show();
	}
}
int main()
{
  std::cout << "\n  Demonstrating NoSql Helper Code \n =================================\n\n  Creating and saving NoSqlDb elements with string data\n -------------------------------------------------------\n";
  NoSqlDb<StrData> db;
  create(db);
  std::cout << "\n  Converting to Xml\n------------------------------------------\n\n \n ";
  std::cout << "\n  Displaying all the records in DB \n ------------------------------------------\n";
	displayAllElements(db);
  std::cout << "\n\n\n  Querying elements for getting the children where key=elem3\n ------------------------------------------\n";
  Keys togetchildren = db.getChildren("elem3");
  for (auto child : togetchildren)
	  std::cout << "\n" << child << "\n";
  edit(db);
  std::cout << "\n-----------time demonstraion-----\n  deleting keys\n ------------------------------------------------------------------------------------------------------------------\n";
  std::cout << "\n  Retrieving elements from NoSqlDb<string> \n ------------------------------------------\n";
  std::cout << "\n  size of db = " << db.count() << "\n";
  displayAllElements(db);
  std::cout << "\n\n\n  Creating and saving NoSqlDb elements with int data\n ----------------------------------------------------\n";
   NoSqlDb<intData> idb;
  icreate(idb);
  std::cout << "\n  Editing data\n ------------------------------------------------------------------------------------------------------------------\n";
  std::cout << "\n  Retrieving elements from NoSqlDb<int>\n ---------------------------------------\n";
  std::cout << "\n  size of idb = " << idb.count() << "\n";
  idisplayAllElements(idb);
  std::cout << "\n\n";
  getchar();
}
#endif // NO_SQL