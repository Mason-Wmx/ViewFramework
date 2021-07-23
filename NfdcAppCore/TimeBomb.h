#pragma once
#include "stdafx.h"

namespace SIM
{
    class TimeBomb
    {
    public:
		TimeBomb(int year, int month, int day);
		int CalcRemainingDays();

	private:
		int _year, _month, _day;
    };

}

