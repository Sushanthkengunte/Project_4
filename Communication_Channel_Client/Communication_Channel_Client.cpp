/////////////////////////////////////////////////////////////////////////
// Communication_Channel_Client.cpp 						           //
//                                                                     //
//Sushanth Suresh, CSE687 - Object Oriented Design, Spring 2017        //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015,Lenovo, Windows 10				   //	
//(SUID: 987471535)  												   //	
/////////////////////////////////////////////////////////////////////////
/*
* This package implements Communication_Channel_Client.h
*
*
*/
/*
* Required Files:
* Communication_Channel_Client.h
*/




#include <iostream>
#include <string>

#include <thread>
#include "Communication_Channel_Client.h"

//using Show = Logging::StaticLogger<1>;
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




//----< factory for creating messages >------------------------------

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

bool MsgClient::sendFile(const std::string& filename, Socket& socket,std::string category)
{
	// assumes that socket is connected

	std::string fqname = /*"../TestFiles/" +*/ filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	//sending file 
	HttpMessage msg = makeMessage(1, "", "localhost::8080", category,"upload");
	std::string onlyFileName = FileSystem::Path::getName(filename);
	msg.addAttribute(HttpMessage::Attribute("file", onlyFileName));
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
std::string MsgClient::initialiseListener(int port){
	//Seen this do it in the last
	::Sleep(20);//to test for download
		Async::BlockingQueue<HttpMessage> msgQ;
		std::string typeOfResult;
		//try{
			SocketSystem ss;
			SocketListener sl(port, Socket::IP6);
			ClientHandler cp(msgQ);
			sl.start(cp);
			while (true){				
				HttpMessage msg = msgQ.deQ();		

				std::string type1 = msg.findValue("type");//unique value to mark end of the download file				
				if (type1 == "upload") {
					if (msg.bodyString() == "Successfull") {
						typeOfResult = msg.bodyString();
						break;
					}
			}
			else if (type1 == "download") {
					if (msg.bodyString() == "finish") {
						typeOfResult = "finish download";
						break;
					}
				}else if (type1 == "downloadLazy") {//filesForLazyDownload					
					if (msg.bodyString() == "Successful") {
						typeOfResult = "finish lazy download";
						break;
					}
				}
				else /*if (checkmessage(msg))*/ {
					filesTodisplay.clear();
					typeOfResult = msg.bodyString();
					break;
				}
			}
			std::cout << "\n-----------------\n Successfully completed and returned from Server \n-----------------\n";
			sl.close();
			return typeOfResult;
		//}catch (std::exception& exc){
			//std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		//}
}



//when GUI selects a category to delete from server, 
std::string MsgClient::deleteFile(int category)
{
	std::string returnMessage;

		execute(100, 1, "delete", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage = initialiseListener(8080);
		});
		t1.join();
	
		return returnMessage;
	
}
//------client makes a call to the server to publish------
std::string MsgClient::publishFile(int category)
{
	std::string returnMessage;
	execute(100, 1, "publish", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage =	initialiseListener(8080);
		});
		t1.join();
		
		return returnMessage;
}
//------splits the csv string into a vector of string--------
std::vector<std::string> MsgClient::split(const std::string &s, char delim) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
//---------makes the appropriate call to the server to add files
std::string MsgClient::addFiles(int category, std::string files)
{
	std::vector<std::string> filePath;
	filePath = split(files, ',');
	std::string returnMessage;
		filePathOf.clear();
		filePathOf = filePath;
		execute(100, 1, "upload", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage = initialiseListener(8080);
		});// Start listener on the client c1
		t1.join();
	
		return returnMessage;
}
//---------makes the appropriate call to the server to display files
std::string MsgClient::displayFilesInClient(int category)
{
	
	std::string returnMessage;
	
		execute(100, 1, "display", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage = initialiseListener(8080);
		});// Start listener on the client c1
		t1.join();
		std::string var = "Success," + returnMessage;
		return var;
}
//----------dispalys files in source code------------
std::string MsgClient::displayFilesInCategorySourceCode(std::string category) {
	std::cout << "\nCategory" + category + ":";
	std::string pathTill = FileSystem::Path::getFullFileSpec("../");
	std::string pathToTheCategory = pathTill + "Server_Files\\SourceCode\\Category" + category + "\\";
	std::vector<std::string> filesSource = FileSystem::Directory::getFiles(pathToTheCategory, "*.*");
	std::string correct;
	if (filesSource.size() == 0)
		return "No files to display";
	for (std::string each : filesSource) {

			correct += each + "\n";

	}
	//correct.pop_back();
	std::cout << "\nDisplayed all files in Source Code in Server side in"+pathToTheCategory;
	return correct;
}
//---------makes the appropriate call to the server to download files in a category
std::string MsgClient::downloadCategory(int category)
{
	
	std::string returnMessage;

		execute(100, 1, "download", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage = initialiseListener(8080);
		});// Start listener on the client c1
		t1.join();
		return returnMessage;
}
//---------makes the appropriate call to the server to download dependent files in a specific category
std::string MsgClient::downloadLazy(std::string files1, int category)
{
	std::vector<std::string> files;
	files = split(files1, ',');
	std::string returnMessage;
		fileForLazy.clear();
		fileForLazy = files;
		execute(100, 1, "downloadLazy", std::to_string(category));
		std::thread t1(
			[&]() {
			returnMessage = initialiseListener(8080);
		});
		t1.join();
		for (auto open : files) {

			std::string fullPathToRepository = FileSystem::Path::getFullFileSpec("../");
			std::string indexFile = fullPathToRepository + "Client_Files\\HtmlFiles\\Category" + std::to_string(category) + "\\" + open;
			std::string path = "\"file:///" + indexFile + "\"";
			std::wstring spath = std::wstring(path.begin(), path.end());
			LPCWSTR swpath = spath.c_str();
			LPCWSTR a = L"open";
			LPCWSTR ch = L"chrome.exe";
			ShellExecute(NULL, a, ch, swpath, NULL, SW_SHOWDEFAULT);
		}
		return returnMessage;
}




//----< this defines the behavior of the client >--------------------

void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages,std::string type,std::string category){
	try{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081)){
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}// send a set of messages
		HttpMessage msg;		
			std::string msgBody = "Sending from client to "+type+"\n";
			msg = makeMessage(1, msgBody, "localhost:8080",category,type);
			if (fileForLazy.size() != 0) {
				std::string msgBody_;
				for (std::string each : fileForLazy) {
					msgBody_ += each + ",";
				}
				msgBody_.pop_back();				
				msg.addAttribute(HttpMessage::attribute("lazyFiles", "true"));
				msg.addAttribute(HttpMessage::attribute("allFilesToDownload", msgBody_));
				std::cout << "Sending message to do lazy download";
			}
			sendMessage(msg, si);
			std::cout<<"\n\n  clien sends \n" + msg.toIndentedString();
			::Sleep(TimeBetweenMessages);
			if (type == "upload") {//  send all *.cpp files from TestFiles folder
				std::vector<std::string> files = filePathOf;
				for (size_t i = 0; i < files.size(); ++i){
					std::string fullPathOfFile = files[i];
					sendFile(fullPathOfFile, si, category);
				}
				msg = makeMessage(1, "finish", "toAddr:localhost:8080", category, type);
				sendMessage(msg, si);
			}
		si.close();		
	}
	catch (std::exception& exc){
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}

//----< this defines processing to frame messages >------------------



HttpMessage ClientHandler::readMessage(Socket& socket){
	connectionClosed_ = false;
	HttpMessage msg;
	while (true){// read message attributes
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;		
	}if (msg.attributes().size() == 0){// If client is done, connection breaks and recvString returns empty string
		connectionClosed_ = true;
		return msg;
	}if (msg.attributes()[0].first == "POST"){// is this a file message?		
		std::string fileList = msg.findValue("listOf");
		if (fileList != "") 
			splitforList(fileList);
		
		std::string filename = msg.findValue("file");
		if (filename != ""){
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			callReadFileFunction(msg, filename,contentSize, socket);
		}if (filename != ""){// construct message body
			msg = addFileBody(msg, filename);		
		}else if (fileList != "") {
			msg = addFileBody(msg, fileList);
		}else{// read message body
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()){
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}
//---------adds the body of file message
HttpMessage ClientHandler::addFileBody(HttpMessage msg, std::string filename)
{
	HttpMessage temp = msg;
	temp.removeAttribute("content-length");
	std::string bodyString = "<file>" + filename + "</file>";
	std::string sizeString = Converter<size_t>::toString(bodyString.size());
	temp.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	temp.addBody(bodyString);
	return temp;
}

//split body
void ClientHandler::splitforList(std::string fileList)
{
	displayFiles.clear();
	std::stringstream ss;
	ss.str(fileList);
	std::string item;
	while (std::getline(ss, item, ',')) {
		displayFiles.push_back(item);
	}
	//return std::vector<std::string>();
}
//-------makes the call to the read the file
void ClientHandler::callReadFileFunction(HttpMessage msg, std::string filename, size_t contentSize, Socket& socket)
{
	std::string correctfilename;
	std::string category1 = msg.findValue("Category");
	std::string pathTill = FileSystem::Path::getFullFileSpec("../");
	std::string fileName = FileSystem::Path::getName(filename);
	correctfilename = pathTill + "Client_Files\\HtmlFiles\\Category" + category1 + "\\" + fileName;
	readFile(correctfilename, contentSize, socket);
}




//----< read a binary file from socket and save >--------------------

bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::cout << "\n\nReceiving file-------->" + filename;
	std::string fqname = filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		std::cout << "Cant open file" + fqname;
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	std::cout << "\nrecieving blocks of file";
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	std::cout << "\nfile recieved and closed-------->" + filename;
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{//closes when the connection closes
			std::cout << "\n \n Client Handler thread is terminating";
			//Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}
//-----------test executive--------------

void MsgClient::testExecutive()
{
	std::cout << "\n\n******************************<Requirement 9> Unit Test Executive *************************** \n";

	std::cout << "\n*********** <Requirement 1> Using Visual Studio 2015 and its C++ Windows Console Projects and WPF GUI ********** \n";
	std::cout << "\n************** <Requirement 4> Project 3 requirements satisfied ***************** \n";
	std::cout << "\n*********** <Requirement 2> managing heap based memory with new & delete ********** \n";
	std::cout << "\n************** <Requirement 6 & 7>Communication technique used is Message Passing based on sockets and with HTTP messaging with Synchronous communication ***************** \n";
	std::cout << "\n*********** <Requirement 3> Publish Button on GUI with Linking Webpages and showing files which dont have parent ********** \n";
	std::cout << "\n********** Code publisher implemented on uploaded files in a particular category\n *********** <Requirement 10>Bonus: Lazy Download Implemented \n *****************  ";
	std::cout << "\n************** Select the files and download, browser opens with thoes files\n  \n ***************** \n";
	std::cout << "\n****** <Requirement 5>Uploading Logger files *******\n";

	std::string fullPathToRepository = FileSystem::Path::getFullFileSpec("../");
	std::string loggerHeaderFile = fullPathToRepository + "Logger\\Logger.h";
	std::string loggerCppFile = fullPathToRepository + "Logger\\Logger.cpp";	
	std::string uploadingFiles = loggerHeaderFile + "," + loggerCppFile;
	
	std::cout<<"\nAdding files operation " + addFiles(1, uploadingFiles);
	::Sleep(10);
	std::cout << "\n************** <Requirement 5>Veiwing contents of repository in category 1 ***************** \n";
	std::cout << "\n " + displayFilesInCategorySourceCode(std::to_string(1));
	::Sleep(10);
	std::cout << "\n************** Deleted contents of server repository in category 1 ***************** \n";
	std::cout << "\nDeleting files operation " + deleteFile(1);
	::Sleep(10);
	std::cout << "\n************** Veiwing contents of repository in category 1 ***************** \n";
	std::cout << "\n " + displayFilesInCategorySourceCode(std::to_string(1));
	
	std::cout << "\n****** <Requirement 3 and 10>Uploading Logger files and demonstrating publishing,lazy download using the file Logger.h.html )*******\n";
	std::cout << "\nAdding files operation " + addFiles(1, uploadingFiles);
	::Sleep(10);
	std::cout << "\nPUBLISH:\n";
	std::cout << publishFile(1);	
	std::string temp = "Logger.h.html";
	std::cout << "\n*************displaying the HTML files Generated in Category 1\n";
	std::cout << displayFilesInClient(1);
	std::cout << "\nDOWNLOAD:\n";
	downloadLazy(temp, 1);

	std::cout << "\n******* <Requirement 8>Sends File  between Client and Server using Stream Bytes(in blocks) with initial message ********* \n";
	std::cout << "\n \n";
	std::cout << "\n \n";
}
//----< entry point - runs two clients each on its own thread >------
#ifdef TESTING_CLIENT




int main()
{
	::SetConsoleTitle(L"Clients Running on Threads");

	//Show::title("Demonstrating two HttpMessage Clients each running on a child thread");
	
	MsgClient c1;
	//Server for this 
	//std::thread t1(
	//	[&]() {
	//	c1.initialiseListener(8081);
	//});// Start listener on the client c1
	//t1.detach();

	std::vector<std::string> files ;
	for(auto each: FileSystem::Directory::getFiles("../TestFiles", "*.cpp")){
		files.push_back(FileSystem::Path::getFullFileSpec("../TestFiles/") + each);
	}
	//c1.addFiles(2, files);
	//c1.deleteFile(2);
	//c1.displayFilesInClient(2);
	c1.publishFile(1);
	//c1.downloadCategory(1);
	std::vector<std::string> files1;
	//for (auto each : FileSystem::Directory::getFiles("../Server_Files/HtmlFiles/Category1", "*.html")) {
	//	files1.push_back(FileSystem::Path::getFullFileSpec("../Server_Files/HtmlFiles/Category") + each);
	//}
	std::string temp = "C:/Users/susha/Source/Repos/Project_4/Server_Files/HtmlFiles/Category1/XmlDocument.h.html";
	files1.push_back(temp);
	c1.downloadLazy(files1,1);
	//c1.execute(100, 1);
	
	
	getchar();
	/*std::thread t1(
	[&]() { c1.execute(100, 20); } // 20 messages 100 millisec apart
	);
	t1.join();*/

	/*MsgClient c2;
	std::thread t2(
	[&]() { c2.execute(120, 20); } // 20 messages 120 millisec apart
	);
	t1.join();
	t2.join();*/
}
#endif // TESTING_CLIENT