//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "w4log.h"

//-----------------------------------------------------------------------------

#include <chrono>
#include <ctime> 
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include "libjson.h"

//-----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif // defined(_MSC_VER)

//-----------------------------------------------------------------------------

static std::string timestamp()
{
	std::ostringstream stream;
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	stream << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
	return stream.str();
}
//-----------------------------------------------------------------------------

w4Log::w4Log(const std::string& fileName, bool bRestLog/* = true*/)
: mFileName(fileName)
{
	if (bRestLog)
	{
		// Reset log file
		std::ofstream stream(mFileName, std::ios::trunc);
	}
}

//-----------------------------------------------------------------------------

w4Log::~w4Log()
{
}

//-----------------------------------------------------------------------------

void w4Log::write(const std::string& why, const std::string& what, const std::string& where)
{
	std::lock_guard<std::recursive_mutex> lock (mMutex);

	// Construct JSON
	JSONNode json;
	json.push_back(JSONNode("when", timestamp()));
	json.push_back(JSONNode("why", why));
	json.push_back(JSONNode("what", what));
	json.push_back(JSONNode("where", where));
	
	// Serialize JSON
	auto text = json.write();

	// Santize text, no CR or LF
	boost::replace_all(text, "\r","");
	boost::replace_all(text, "\n","");

	std::ofstream stream(mFileName, std::ios::app);
	stream << text << std::endl;
}

//-----------------------------------------------------------------------------

void* w4LogInit(char* fileName)
{
	return static_cast<void*>(new w4Log(fileName));
}

//-----------------------------------------------------------------------------

void w4LogDone(void* context)
{
	delete static_cast<w4Log*>(context);	
}

//-----------------------------------------------------------------------------

void w4LogWrite(void* context, char* why, char* what, char* where)
{
	if(auto log = static_cast<w4Log*>(context))
		log->write(why, what, where);
}

//-----------------------------------------------------------------------------

