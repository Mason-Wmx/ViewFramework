#pragma once
#include "MathUtils.h"

#include <locale>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace SIM
{
	class TextUtils
	{
	public:
		static std::string DoubleToString(double val,int precision = -1, bool suppressTrailing0 = false, char separator = '-')
		{
			std::string result = "";
			std::stringstream ss;
			std::locale loc("");
			ss.imbue(loc);
			char point = std::use_facet< std::numpunct<char> >(loc).decimal_point();

			if(precision < 0)
			{
				precision = 12;
				suppressTrailing0 = true;
			}

			ss << std::fixed << std::setprecision(precision) << val;
			result = ss.str();

			if(suppressTrailing0 && precision > 0)
			{
				if(result.find(point) !=  std::string::npos)
				{
					while(result.back() == '0')
					{
						result.pop_back();
					}

					if(result.back() == point)
						result.pop_back();
				}
			}


			if(separator != '-')
			{
				std::replace( result.begin(),result.end(), point, separator);
			}
		// Problem in QString. Invalid conversion from NO-BREAK SPACE
		char nonBreakSpace(0xA0);// 160 NO-BREAK SPACE
		char space(0x20);		// 32 SPACE
		std::replace(result.begin(), result.end(), nonBreakSpace, space);

			return result;
		}
		static bool TryParse(std::string txt,double& val, char separator = '-')
		{
			std::stringstream ss;
			std::locale loc("");
			ss.imbue(loc);
			char point = std::use_facet< std::numpunct<char> >(loc).decimal_point();
			char group = std::use_facet< std::numpunct<char> >(loc).thousands_sep();

			if(separator != '-')
				std::replace( txt.begin(),txt.end(), point, separator);

			txt.erase (std::remove(txt.begin(), txt.end(), group), txt.end());

			if(point != ' ')
				txt.erase (std::remove(txt.begin(), txt.end(), ' '), txt.end());;


			std::replace( txt.begin(),txt.end(), point, '.');

			try
			{
				val = std::stod(txt);
				return true;
			}
			catch(...)
			{
				return false;
			}
			return true;
		}
		static std::string Format(double value, double accuracy, bool suppressTrailing0 = false, char separator = '-')
		{
			int decimalNumber = MathUtils::AccuracyToDecimalDigits(accuracy);
			double val = MathUtils::Round1(value,accuracy);

			return DoubleToString(val,decimalNumber,suppressTrailing0,separator);
		}
	};
}

