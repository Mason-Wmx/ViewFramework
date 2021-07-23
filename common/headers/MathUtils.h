#pragma once
#include "Epsilon.h"


namespace SIM
{
	class MathUtils
	{
	public:
		static double Epsilon;
		static bool IsEqual(double val1, double val2)
		{
			return (std::abs(val1 - val2) < SIM_EPS);
		}
		static bool IsEqual(double val1, double val2,double eps)
		{
			return (std::abs(val1 - val2) < eps);
		}
		static double Round1(double value, double accuracy)
		{
			if(accuracy < 1)
			{
				return value;
			}

			if(accuracy == 0)
				accuracy = 1;

			double rounded = value/accuracy;
			rounded = value < 0.0 ? std::ceil(rounded - 0.5) : std::floor(rounded + 0.5);
			__int64 numb = (__int64)rounded;
			double finalValue = numb*accuracy;
			return finalValue;
		}
		static double Round2(double value, double accuracy)
		{
			if(accuracy < 0.000000000000001)
				return value;

			return ceil(value / accuracy) * accuracy;
		}
		static double Round(double value, double accuracy)
		{
			if(accuracy < 1)
				return Round2(value,accuracy);
			else
				return Round1(value,accuracy);
		}
        static int SIM::MathUtils::AccuracyToDecimalDigits(double accuracy)
        {
		    int decimalNumber = 0;
		    if(accuracy < 1 && accuracy > 0)
		    {
		        double acc = accuracy;
		        for(;;)
		        {
		            decimalNumber++;
		            acc*=10;
		            if(acc > 0.9999999999)
		                break;
		        }
		    }
		    return decimalNumber;
		}

		static double GetRounding(double value)
		{
			double rounding = 1;
			double val = abs(value);

			if(val > 1)
			{
				for(int i=0; i< 20; i++)
				{
					if(val/(rounding*10) > 1)
					{
						rounding *= 10;
					}
					else
					{
						return rounding;
					}
				}
			}
			else
			{
				for(int i=0; i< 20; i++)
				{
					if(val/rounding < 1)
					{
						rounding /= 10;
					}
					else
					{
						return rounding;
					}
				}
			}

			return value;
		}
	};
}

