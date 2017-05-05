#pragma once

/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates simple one-way HTTP style messaging    //
//                 and file transfer                                   //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the receiving code into a Receiver class
* - Receiver should own a BlockingQueue, exposed through a
*   public method:
*     HttpMessage msg = Receiver.GetMessage()
* - You will start the Receiver instance like this:
*     Receiver rcvr("localhost:8080");
*     ClientHandler ch;
*     rcvr.start(ch);
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


