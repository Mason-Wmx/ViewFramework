#include "stdafx.h"
#include "TimeBomb.h"


using namespace SIM;

/*
// Timebomb is 2017.03.18
const int tby = 2017, tbm = 3, tbd = 18;

const struct tm tb_tm = { 0, 0, 0, tbd, tbm - 1, tby - 1900 };

bool tbok(int* remaining_days = NULL)
{
	time_t now = time(0);
	struct tm* tnow = localtime(&now);
	int y = tnow->tm_year + 1900, m = tnow->tm_mon + 1, d = tnow->tm_mday;
	bool ok = (y < tby) || (y == tby && m < tbm) || (y == tby && m == tbm && d < tbd);
	if (remaining_days != NULL)
	{
		if (!ok)
			*remaining_days = 0;
		else
		{
			// calculate the remaining days
			time_t tb = mktime(&tb_tm);
			time_t tn = mktime(tnow);
			*remaining_days = (int)(std::difftime(tb, tn) / (60 * 60 * 24));
		}
	}
	return ok;
}

// Expiration message starts on 2017.04.16
static int wrny = 2017, wrnm = 3, wrnd = 4;
bool should_warn_expiration()
{
	time_t now = time(0);
	struct tm* tnow = localtime(&now);
	int y = tnow->tm_year + 1900, m = tnow->tm_mon + 1, d = tnow->tm_mday;
	if ((y > wrny) || (y == wrny && m > wrnm) || (y == wrny && m == wrnm && d >= wrnd))
		return true;
	return false;
}
*/

namespace SIM
{

TimeBomb::TimeBomb(int year, int month, int day)
: _year(year), _month(month), _day(day)
{
}

int TimeBomb::CalcRemainingDays()
{
	time_t now = time(0);
	struct tm* tnow = localtime(&now);
	/*int y = tnow->tm_year + 1900, m = tnow->tm_mon + 1, d = tnow->tm_mday;
	bool ok = (y < _year) || (y == _year && m < _month) || (y == _year && m == _month && d < _day);
	if (!ok) {
		return 0;
	}*/

	// calculate the remaining days
	struct tm tb_tm = { 0, 0, 23, _day, _month - 1, _year - 1900 };
	time_t tb = mktime(&tb_tm);
	time_t tn = mktime(tnow);
	int remaining_days = (int)(std::difftime(tb, tn) / (60 * 60 * 24));
	return remaining_days;
}

}

/*
int startInstance(LicenseConfig& lic_cfg)
{
	configureProduct(lic_cfg);
	PsLicenseConfig& cfg = dynamic_cast<PsLicenseConfig&>(lic_cfg);
	int remaining_days = 0;
	bool tok = tbok(&remaining_days);
	if (!tok)
	{
		if (g_client != NULL && !cfg.SilentFlag)
			g_client->tpExpirationMessage(true, tby, tbm, tbd, remaining_days);
		return 1; // EXPIRED
	}
	else if (g_client != NULL && g_client->tpCanLaunch())
	{
		if (should_warn_expiration() && !cfg.SilentFlag)
			g_client->tpExpirationMessage(false, tby, tbm, tbd, remaining_days);
		return 0;
	}
	return 1; // NO LICENSE
}
*/
