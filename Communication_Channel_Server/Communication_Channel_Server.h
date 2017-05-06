#pragma once

/////////////////////////////////////////////////////////////////////////
// Communication_Channel_Server.h - provides Server functionality      //
//                 and file transfer                                   //
//                                                                     //
// Sushanth Suresh, CSE687 - Object Oriented Design, Spring 2017       //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Lenovo, Windows 10			       //
//(SUID:987471535)													   //
/////////////////////////////////////////////////////////////////////////
/*
*Module Operations:
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
*This package uses two clases Clienthandler and MsgClient, where clientHandler
*handles the client requests and msgClient sends the reply back
*
.
*/
/*
* Required Files:
*
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

*/
/*Public Interdfaces:
*-----------------------
* ClientHandler:
*Its a functor that reads a message and process it on a different thread using threads
*
*MsgClient:
*initialiseListener(int)       // initialises the reciever to run forever on a different thread
*processMessage()			   // calls the appropiate function
*deletingFilesInCategory()	   // deletes files in specific category
*publishTheCategory()		   //Publisehs all the *.cpp,*.h and *.css files
*dispplayFilesInCategory()		//displays all the files in the category after publishing it, in a list box
*downloadCategory()				//Sends back all the files in a specific category
*lazyDownload()					//populates all the file names which are required to be sent back
*downloadSpecifiedFiles()		//Sends back all the files in a specific category
*
*/
#include "../HttpMessage/HttpMessage.h"
#include "Sockets.h"
#include "FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <sstream>
#include<stack>
#include <iostream>
#include "../Analyzer/Executive.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;

/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You no longer need to be careful using data members of this class
//   because each client handler thread gets its own copy of this 
//   instance so you won't get unwanted sharing.
// - I changed the SocketListener semantics to pass
//   instances of this class by value for version 5.2.
// - that means that all ClientHandlers need copy semantics.
//
class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	Async::BlockingQueue<HttpMessage>& msgQ_;

};
//----< this defines processing to frame messages >------------------
/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages, std::string type, std::string category);
	void initialiseListener(int);
	void processMessage(HttpMessage msg);
	void deletingFilesInCategory(std::string category);
	void publishTheCategory(std::string category);
	std::string dispplayFilesInCategory(std::string category);
	void downloadCategory(std::string category, std::string type, std::vector<std::string> files);
	void lazyDownload(std::string files, std::string category, std::string type);
	void findHtmlDependencies(std::string forEach, std::unordered_map<std::string, std::vector<std::string>> correctTable);
	void downloadSpecifiedFiles(std::string type, std::vector<std::string> files, std::string category);

private:
	void connectToTheClient(HttpMessage msg);
	void doDownloadProcessing(HttpMessage msg);
	void douploadProcessing(HttpMessage msg);
	void dodeleteProcessing(HttpMessage msg);
	std::unordered_map<std::string, std::vector<std::string>> category1HtmlDependencytable;
	std::unordered_map<std::string, std::vector<std::string>> category2HtmlDependencytable;
	std::unordered_map<std::string, std::vector<std::string>> category3HtmlDependencytable;
	std::vector<std::string> filesForLazyDownload;
	std::unordered_map<std::string, std::vector<std::string>> getCorrectTable(std::string category);
	void putInCorrectTable(std::string category, std::unordered_map<std::string, std::vector<std::string>>);
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string category, std::string type);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket, std::string category, std::string type);
};


