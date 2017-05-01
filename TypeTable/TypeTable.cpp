/////////////////////////////////////////////////////////////////////
// TypeTable.cpp - creating the TypeTable                          //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Lenovo, Windows 10                                 //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:		sushanth suresh SUID:987471535					   //
//		       					                                   //
/////////////////////////////////////////////////////////////////////
#include "TypeTable.h"
#include "../Parser/ActionsAndRules.h"
namespace CodeAnalysis {
	//--------------sets the type of the tuple----------------//
   bool Tuple::setType(Type_name type_arg)
	{
		type_of = type_arg;
		return true;
	}
   //---------sets the namespace of the tuple------------//
	 bool Tuple::setNamespace(Namespace_name namespace_arg)
	{
		namespace_of = namespace_arg;
		return true;
	}
//------------sets the file name of the tuple-----------//
	 bool Tuple::setFile(File_name file_arg)
	{
		file_of = file_arg;
		return true;
	}
	//-----returns the type of the tuple----------------//
	 std::string Tuple::getType()
	{
		return type_of;
	}
	 //--------returns the namespace of the tuple-------//
	 std::string Tuple::getNamespace()
	{
		return namespace_of;
	}
	//--------returns the file name of the tuple---------//
	 std::string Tuple::getFileName()
	{
		return file_of;
	}
	 //-------constructor of TypeTable getting the AST from repository----------//
	  TypeTable::TypeTable() :
		 ASTref_(Repository::getInstance()->AST()){
		 std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	 }

	 TypeTable::~TypeTable()
	 {
	 }
	 //-------------returns all the keys of the type table------------------//
	  Keys TypeTable::getKeys()
	 {
		 std::vector<std::string> keys_;
		 for (auto item : T_Table)
		 {
			 keys_.push_back(item.first);
		 }
		 return keys_;
	 }
	  //---------inserts into the typeTable---------------------------//
	  bool TypeTable::insert(Key k, std::vector< Tuple> t)
	 {
		 if (T_Table.find(k) != T_Table.end())
			 return false;
		 T_Table[k] = t;
		 return true;
	 }
	  //-----------returns a value of a particular key in the TypeTable---------//
	  std::vector<Tuple> TypeTable::value(Key key)
	 {
		 if (T_Table.find(key) != T_Table.end())
			 return T_Table[key];
		 return std::vector<Tuple>{};
	 }
	  //------------returns count of the entries inside the TypeTable------------//
	  int TypeTable::count()
	 {
		 return T_Table.size();
	 }
	  //------------returns a referens to the TypeTable------------------------//
	  std::unordered_map<Key, std::vector<Tuple>>& TypeTable::getTable()
	 {
		 return T_Table;
	 }
	  //------------populates the type table---------------------------//
	  void TypeTable::doTableAnal()
	 {
		 ASTNode* pRoot = ASTref_.root();
		 DFS(pRoot);
	 }
	  std::unordered_map<std::string, std::vector<LineNO>> TypeTable::getLineNoTable()
	  {
		  return tyLine;
	  }
	  //--------checks for the required types------------------------//
	  bool TypeTable::doDisplay1(ASTNode* pNode)
	 {
		 static std::string toDisplay[] = {
			 "class", "struct", "enum","function"
		 };
		 for (std::string type : toDisplay)
		 {
			 if (pNode->type_ == "namespace" && pNode->name_ == correct_namespace)
				 continue;
			 if (pNode->type_ == type)
				 return true;
		 }
	
		 return false;
	 }
	  //-----------get line Number-----//
	  void TypeTable::append(LineNO lined,std::string pathName)
	  {
		  std::vector<LineNO> temp;
		  temp = tyLine[pathName];
		  temp.push_back(lined);
		  tyLine[pathName] = temp;
	  }
	  //----------pushes into the type table if the type is a required one------------//
	  void TypeTable::pushIntoMap(ASTNode * pNode)
	 {
		 if (doDisplay1(pNode))
		 {
			 if (pNode->type_ == "class" || pNode->type_ == "function") {
				 LineNO a;
				 a.startLine = pNode->startLineCount_;
				 a.endLine = pNode->endLineCount_;
				 if(pNode->startLineCount_ != pNode->endLineCount_ && pNode->startLineCount_+1 != pNode->endLineCount_)
					 append(a, pNode->path_);
			 }
			 Tuple save;
			 if (pNode->type_ == "function" && correct_namespace == "Global Namespace" && pNode->name_ != "main" && pType == "namespace"&& pNode->name_!= "void") {//pushing a global function
				 save.setFile(pNode->path_);
				 save.setType(pNode->type_);
				 save.setNamespace(correct_namespace);
				 std::vector<Tuple> entry = value(pNode->name_);
				 entry.push_back(save);
				 insert(pNode->name_, entry);
			 }
			 else if (pNode->type_ != "function")// pushing classes, structs and enum
			 {
				 save.setFile(pNode->path_);
				 save.setType(pNode->type_);
				 save.setNamespace(correct_namespace);
				 std::vector<Tuple> entry = value(pNode->name_);
				 entry.push_back(save);
				 insert(pNode->name_, entry);
			 }

		 }
	 }
	  //------------walks the AST in a DFS manner--------------//
	 void TypeTable::DFS(ASTNode * pNode)
	 {
		/* currentName = pNode->path_;
		 if (previousName != currentName) {
			 previousName = currentName;
			 tyLine[currentName] = a_Line;
			 a_Line.clear();
		 }*/
		 
		 if (pNode->type_ == "namespace")
			 nameSpaceStack.push(pNode->name_); //stack to save the namespace
		 else
		 {
			 pushIntoMap(pNode);
		 }
		 for (auto child : pNode->children_) {
			 pType = pNode->type_;
			 correct_namespace = nameSpaceStack.top();
			 DFS(child);						// calling DFS to all the children of the node
			 pType = pNode->type_;
		 }
		 if (pNode->type_ == "namespace")
			 nameSpaceStack.pop();
	 }
}
#ifdef TYPE_TEST



#include "../Display/Display.h"
using namespace CodeAnalysis;
int main() {
	TypeTable ta;
	ta.doTableAnal();
	Display d;
	d.showTypeTable(ta.getTable());

}
#endif // TYPE_TEST