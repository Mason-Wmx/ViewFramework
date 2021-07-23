#pragma once
#include "stdafx.h"
#include "export.h"
#include <headers/UnitTypes.h>
#include "../NfdcDataModel/UnitsModel.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT DocUnits
	{
	public:
		DocUnits();

		~DocUnits();

		std::string FormatDisplay(EUnitCategory category,double value,bool addUnitSymbol = true) const;
		std::string FormatBase(EUnitCategory category,double baseValue,bool addUnitSymbol = true) const;

		std::string FormatDisplay(EUnitCategory category,double value,bool addUnitSymbol, bool suppressTrailing0) const;
		std::string FormatBase(EUnitCategory category,double baseValue,bool addUnitSymbol, bool suppressTrailing0) const;

		double ConvertDisplayToBase(EUnitCategory category, double value) const;
		double ConvertBaseToDisplay(EUnitCategory category, double baseValue) const;

		bool TryParseToDisplay(EUnitCategory category, std::string txt, double& value, bool round=false) const;
		bool TryParseToBase(EUnitCategory category, std::string txt, double& value, bool round=false) const;
		
		void SetDisplayUnit(EUnitCategory category, EUnitType selectedUnit, double accuracy, bool showSymbol);
		void GetDisplayUnit(EUnitCategory category, EUnitType& selectedUnit, double& accuracy, bool& showSymbol) const;
		EUnitType GetDisplayUnit(EUnitCategory category) const;
		std::string GetUnitSymbol(EUnitCategory category) const;
		int GetUnitDecimalDigits(EUnitCategory category) const;
		double GetAccuracy(EUnitCategory category) const;

    double GetGlobalAccuracy() const;
    void SetGlobalAccuracy(double val);

    bool GetCustomGlobalAccuracy() const;
    void SetCustomGlobalAccuracy(bool val);

    bool GetSuppressTrailing0() const;
    void SetSuppressTrailing0(bool val);

		void SetDefaults();
		void SetDefaults(Application& app);

    void SetModel(std::shared_ptr<UnitsModel> ptr);
    std::shared_ptr<UnitsModel> GetModel() { return _unitsModel; };

	private:
		const SIM::Unit& GetUnit(EUnitCategory category) const;
		
    std::shared_ptr<UnitsModel> _unitsModel;

    
	};
}

