/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
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
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/

#include "Communication_Channel_Server.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

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
	void findHtmlDependencies(std::string forEach,std::unordered_map<std::string, std::vector<std::string>> correctTable);
	void downloadSpecifiedFiles(std::string type, std::vector<std::string> files, std::string category);
	
private:
	std::unordered_map<std::string, std::vector<std::string>> category1HtmlDependencytable;
	std::unordered_map<std::string, std::vector<std::string>> category2HtmlDependencytable;
	std::unordered_map<std::string, std::vector<std::string>> category3HtmlDependencytable;
	std::vector<std::string> filesForLazyDownload;
	std::unordered_map<std::string, std::vector<std::string>> getCorrectTable(std::string category);
	void putInCorrectTable(std::string category, std::unordered_map<std::string, std::vector<std::string>>);
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string category, std::string type);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket,std::string category, std::string type);
};
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string category, std::string type)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::attribute("type", type));

		msg.addAttribute(HttpMessage::attribute("Category", category));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgClient::sendFile(const std::string& filename, Socket& socket, std::string category, std::string type)
{
	// assumes that socket is connected
	std::string pathTill = FileSystem::Path::getFullFileSpec("../");
	std::string pathToTheCategory = pathTill + "Server_Files\\HtmlFiles\\Category" + category + "\\"+filename;
	
	
	std::string fqname = pathToTheCategory;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;


	HttpMessage msg = makeMessage(1, "", "localhost::8080",category,type);
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
void MsgClient::initialiseListener(int port)
{
	/*std::thread t1(
		[&]() {*/
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	Show::attach(&std::cout);
	Show::start();
	std::cout << "\n  HttpMessage Server started";

	Async::BlockingQueue<HttpMessage> msgQ;

	try
	{
		SocketSystem ss;
		SocketListener sl(port, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		std::cout << "\nStarted processing messages recieved in Server";
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			//std::cout << "\n\n  server recvd message with body contents:\n" + msg.bodyString();
			processMessage(msg);
			//dont put break server should listen to serve queue forever
				

		}
		std::cout << "Done processing the messages";
		//sl.close();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
	/*});
	t1.detach();*/
}
//-----------deleting all the files in a category------------
void MsgClient::deletingFilesInCategory(std::string category) {
	std::cout << "\nCalling deleting function to delete Category"+category;
	std::string pathTill = FileSystem::Path::getFullFileSpec("../");
	std::string pathToTheCategory = pathTill + "Server_Files";
	std::string htmlPath = pathToTheCategory + "\\HtmlFiles\\Category" + category + "\\";
	std::string sourcePath = pathToTheCategory + "\\SourceCode\\Category" + category+"\\";

	std::vector<std::string> filesHtml = FileSystem::Directory::getFiles(htmlPath, "*.html");
	for (std::string each : filesHtml) {
		std::string correct = pathToTheCategory + "\\HtmlFiles\\Category" + category + "\\" + each;
		remove(correct.c_str());
	}
	std::vector<std::string> filesSource = FileSystem::Directory::getFiles(sourcePath, "*.*");
	for (std::string each : filesSource) {
		std::string correct = pathToTheCategory + "\\SourceCode\\Category" + category + "\\" + each;
		remove(correct.c_str());
	}
	std::cout << "\nDeleting files in both (Html/SourceCode) Category";

}

//-----------end deleting all the files in a category------------

//-----------display all the files in a category------------
std::string MsgClient::dispplayFilesInCategory(std::string category) {
	std::cout << "\nCalling display function to delete Category" + category;
	std::string pathTill = FileSystem::Path::getFullFileSpec("../");
	std::string pathToTheCategory = pathTill + "Server_Files\\HtmlFiles\\Category" + category + "\\";
	std::vector<std::string> filesSource = FileSystem::Directory::getFiles(pathToTheCategory, "*.html");
	std::string correct;
	for (std::string each : filesSource) {
		 correct += each + ",";
		
	}
	correct.pop_back();
	std::cout << "\nDisplayed files in  Html ";
	return correct;
}

//-----------end display all the files in a category------------
//-------calling code publisher with category---------------
void MsgClient::publishTheCategory(std::string category) {
	std::cout << "\nPublish the files in category"+category;
	CodeAnalysis::CodeAnalysisExecutive obj;
	char * array[7];
	std::string path = "../Server_Files/SourceCode/Category" + category /*+ "/"*/;
	std::string arguments[] = {"abc", path,"*.h","*.cpp","/m","/f","/r" };
	for (int i = 0; i < 7; i++) {
		const char* charArguments = arguments[i].c_str();
		array[i] = _strdup(charArguments);
		}
	std::unordered_map<std::string, std::vector<std::string>> temp;
	temp = obj.getExecFunctionality(7, array, category);
	putInCorrectTable(category, temp);
	std::cout << "\nPublished the files in category";
}
std::unordered_map<std::string, std::vector<std::string>> MsgClient::getCorrectTable(std::string category)
{
	std::unordered_map<std::string, std::vector<std::string>> correctTable;
	if (category == "1")
		correctTable = category1HtmlDependencytable ;
	if (category == "2")
		correctTable = category2HtmlDependencytable ;
	if (category == "3")
		correctTable = category3HtmlDependencytable;

	return correctTable;
}
//-------end calling code publisher with category---------------
void MsgClient::putInCorrectTable(std::string category, std::unordered_map<std::string, std::vector<std::string>> htmlTable)
{
	if (category == "1")
		category1HtmlDependencytable = htmlTable;
	if (category == "2")
		category2HtmlDependencytable = htmlTable;
	if (category == "3")
		category3HtmlDependencytable = htmlTable;
	
}
//-------downloading files in a category---------------
void MsgClient::downloadCategory(std::string category, std::string type, std::vector<std::string> files) {
	std::cout << "\ndownload files in category" + category;
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;
		std::string msgBody = "sending back with files to download";
		std::cout << msgBody;
		msg = makeMessage(1, msgBody, "localhost:8080", category, type);		
		sendMessage(msg, si);
		std::cout << "\n\n  Server sends back download msg\n" + msg.toIndentedString();
		files.push_back("project4.js");
		files.push_back("style.css");
		//std::vector<std::string> files;
		for (size_t i = 0; i < files.size(); ++i)
		{
			std::cout << "\n\n  sending file " + files[i];
			sendFile(files[i], si, category, type);
		}

		// shut down server

		msg = makeMessage(1, "quit", "toAddr:localhost:8080", category, type);
		sendMessage(msg, si);
		//Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());

		//std::cout << "\n";
		std::cout << "\n  Sending quit msg to stop client to listen";
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}
void MsgClient::downloadSpecifiedFiles(std::string type, std::vector<std::string> files, std::string category) {
	try
	{
		std::cout << "\n Sending files back from server in sownload lazy";
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;
		std::string msgBody = "Sending from Server to " + type + "\n";
		msg = makeMessage(1, msgBody, "localhost:8080", category, type);
		sendMessage(msg, si);
		std::cout << "\n\n  Server sends \n" + msg.toIndentedString();
		//  send all *.cpp files from TestFiles folder

		for (size_t i = 0; i < filesForLazyDownload.size(); ++i)
		{

			std::string fullPathOfFile = files[i];
			//Show::write("\n\n  sending file " + files[i]);
			sendFile(fullPathOfFile, si, category,type);
		}

		// shut down server's client handler

			msg = makeMessage(1, "Successful", "toAddr:localhost:8080", category, type);
		sendMessage(msg, si);
		std::cout << "Server finished sending files back to client";
		si.close();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}


}

void MsgClient::lazyDownload(std::string files,std::string category,std::string type)
{
	std::vector<std::string> temporaryfiles;
	std::stringstream ss;
	ss.str(files);
	std::string temp;
	while (std::getline(ss, temp, ',')) {
		temporaryfiles.push_back(temp);

	}
	for (std::string each : temporaryfiles) {
		auto x = getCorrectTable(category);
		findHtmlDependencies(each, getCorrectTable(category));
	}

}

void MsgClient::findHtmlDependencies(std::string forEach,std::unordered_map<std::string, std::vector<std::string>> correctTable)
{
	std::string onlyNames = FileSystem::Path::getName(forEach);
	if (std::find(filesForLazyDownload.begin(), filesForLazyDownload.end(), onlyNames) == filesForLazyDownload.end())
	{
		filesForLazyDownload.push_back(onlyNames);
	}
	std::vector<std::string> temporary = correctTable[onlyNames];
	for (std::string each : temporary)
	{
		std::string onlyChildrenNames = FileSystem::Path::getName(each);
		if (std::find(filesForLazyDownload.begin(), filesForLazyDownload.end(), onlyChildrenNames) == filesForLazyDownload.end())
		{
			findHtmlDependencies(onlyChildrenNames, correctTable);
		}
	}
	
}

void MsgClient::processMessage(HttpMessage msg)
{
	std::string type = msg.findValue("type");
	if (type == "upload") {
		//std::cout << "Recieve message to upload ";
		//call the server function
		std::string  cat = msg.findValue("Category");
		std::string  tp = msg.findValue("type");
		//send successful after recieving finish
		if (msg.bodyString() == "finish") {
		HttpMessage message = makeMessage(1, "Successfull", "toAddr:localhost:8080", cat, tp);
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}
		sendMessage(message, si);
		}

	}if (type == "delete") {
		//std::cout << "Recieved message to delete";
		//call the server function
		std::string  cat = msg.findValue("Category");
		deletingFilesInCategory(cat);
		std::string  tp = msg.findValue("type");
		HttpMessage message = makeMessage(1, msg.bodyString(), "toAddr:localhost:8080", cat, tp);
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}
		sendMessage(message, si);
		
	}if (type == "publish") {
		//std::cout << "Recieved message to publish";
		//msg = makeMessage(1, "publish file done", "toAddr:localhost:8080");
		//create socket and send the message
		std::string  cat = msg.findValue("Category");
		std::string  tp = msg.findValue("type");
		publishTheCategory(cat);
		
		MsgClient c2;
		c2.execute(100, 1,tp,cat);
		
		
		// call the server function
	}if (type == "display") {
		//std::cout << "Recieved message to display";
		// call the server function
		std::string  cat = msg.findValue("Category");
		std::string csvString = dispplayFilesInCategory(cat);
		std::string  tp = msg.findValue("type");
		std::cout << "\n"+csvString;
		HttpMessage message = makeMessage(1, csvString , "toAddr:localhost:8080", cat, tp);
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}
		sendMessage(message, si);
		
	} if (type == "download") {
		std::string  cat = msg.findValue("Category");
		std::string  tp = msg.findValue("type");
		std::string pathTill = FileSystem::Path::getFullFileSpec("../");
		std::vector<std::string> absolutePathOfFiles;
		std::string pathToTheCategory = pathTill + "Server_Files\\HtmlFiles\\Category" + cat + "\\";
		std::vector<std::string> filesSource = FileSystem::Directory::getFiles(pathToTheCategory, "*.*");
		
		for (auto each : filesSource) {
			absolutePathOfFiles.push_back(pathToTheCategory + each);
		}
		absolutePathOfFiles.push_back(pathTill + "Server_Files\\HtmlFiles\\Category1\\project4.js");
		absolutePathOfFiles.push_back(pathTill + "Server_Files\\HtmlFiles\\Category1\\style.css");
		downloadCategory(cat,tp, absolutePathOfFiles);

	}
	if (type == "downloadLazy") {
		std::string lazyFiles = msg.findValue("lazyFiles");
		if (lazyFiles != "") {
			std::string toDownload = msg.findValue("allFilesToDownload");
			std::string  cat = msg.findValue("Category");
			//std::vector<std::string>
			lazyDownload(toDownload,cat,type);
			downloadSpecifiedFiles(type, filesForLazyDownload, cat);
		}

	}
	
}


//----< this defines the behavior of the client >--------------------

void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages, std::string type, std::string category)
{
	// send NumMessages messages
	::Sleep(100);
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);

	Show::attach(&std::cout);
	Show::start();

	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;

		
		
			std::string msgBody ="done";
			msg = makeMessage(1, msgBody, "localhost:8080",category,type);
			/*
			* Sender class will need to accept messages from an input queue
			* and examine the toAddr attribute to see if a new connection
			* is needed.  If so, it would either close the existing connection
			* or save it in a map[url] = socket, then open a new connection.
			*/
			sendMessage(msg, si);
			std::cout << "\n\n  Server sends back message\n" + msg.toIndentedString();
			::Sleep(TimeBetweenMessages);
		
		//  send all *.cpp files from TestFiles folder

	/*		std::vector<std::string> files;
		for (size_t i = 0; i < files.size(); ++i)
		{
			std::cout << "\n\n  sending file " + files[i];
			sendFile(files[i], si,category,type);
		}*/

		// shut down server

		/*msg = makeMessage(1, "quit", "toAddr:localhost:8080",category,type);
		sendMessage(msg, si);*/
		//Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());

		std::cout << "\n";
		std::cout << "\n  All done folks";
		si.close();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}




//----< entry point - runs two clients each on its own thread >------

int main()
{
	::SetConsoleTitle(L"Server on Threads");

	//Show::title("Demonstrating two HttpMessage Clients each running on a child thread");
	
	MsgClient c1;

	std::thread t1(
		[&]() {

		c1.initialiseListener(8081);
	});// Start listener on the client c1
	t1.join();

	

	//c1.execute(100, 1);
	getchar();

	
}