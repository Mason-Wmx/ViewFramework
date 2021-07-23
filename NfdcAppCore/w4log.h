//-----------------------------------------------------------------------------
#if !defined(__W4LOG_H__)
#define __W4LOG_H__

//-----------------------------------------------------------------------------

#include "export.h"

//-----------------------------------------------------------------------------

#define W4LOG_INFORMATION	"info"
#define W4LOG_WARNING		"warning"
#define W4LOG_ERROR			"error"
#define W4LOG_OUTPUT		"output"

//-----------------------------------------------------------------------------

#if defined(__cplusplus)
#include <string>
#include <mutex>
class NFDCAPPCORE_EXPORT w4Log
{
public:
	w4Log(const std::string& fileName, bool bRestLog = true);
	~w4Log();
	void write(const std::string& why, const std::string& what, const std::string& where);
private:
	std::string mFileName;
	std::recursive_mutex mMutex;
};
#endif // defined(__cplusplus)

//-----------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

//-----------------------------------------------------------------------------

NFDCAPPCORE_EXPORT void* w4LogInit(char* fileName);
NFDCAPPCORE_EXPORT void  w4LogDone(void* context);
NFDCAPPCORE_EXPORT void  w4LogWrite(void* context, char* why, char* what, char* where);

//-----------------------------------------------------------------------------

#if defined(__cplusplus)
} // extern "C"
#endif // defined(__cplusplus)
	
//-----------------------------------------------------------------------------

#endif // !defined(__W4LOG_H__)

//-----------------------------------------------------------------------------

