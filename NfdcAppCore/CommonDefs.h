
#pragma once
#include <qmetatype.h>
#include <headers/ObjectId.h>

Q_DECLARE_METATYPE(SIM::EUnitCategory)
Q_DECLARE_METATYPE(SIM::EUnitType)


namespace SIM
{
    class Observable;
    class Observer;
    class Document;
    class Command;
    class Request;
    class Application;	    

	typedef internal::Vec3<double> Vec3;
	typedef Vec3 Point3;


	const char* const DATE_FORMAT = "yyyy-MM-dd";
	const char* const DATE_START = "";
	const char* const DATE_END = "";
	const char* const RF_TB_URL = "https://beta.autodesk.com/project/home.html?cap=%7bA6C6496B-89D7-4649-B9EB-62EF2FCC4796%7d";
	const char* const RF_TB_NAME_URL = "Autodesk Labs";
	const char* const RF_HELP_URL = "http://www.autodesk.com/rctfld-help-enu";
	const char* const RF_FORUM_URL = "https://beta.autodesk.com/project/forum/default.html?cap=A6C6496B89D74649B9EB62EF2FCC4796";
	const char* const RF_ABOUT_URL = "https://beta.autodesk.com/project/home.html?cap=%7bA6C6496B-89D7-4649-B9EB-62EF2FCC4796%7d";
	const char* const TOOLTIP_KEY_NOT_IMPLEMENTED = "NotImplemented";
}
