#pragma once

#include "stdafx.h"
#include "qstring.h"

namespace SIM {

class AppStyle
{
public:
	static QString GetStyleSheet( void );
	static std::vector<QString> GetFontPaths(void);
};

} // namespace SIM
