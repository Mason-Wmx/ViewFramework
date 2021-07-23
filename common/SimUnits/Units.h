#pragma once
#include "stdafx.h"
#include <headers/UnitTypes.h>
#ifndef M_PI
#define M_PI       3.14159265358979323846 
#endif // !M_PI
 

namespace SIM
{
  class UnitGroup;
  class Unit;
  class SIMUNITS_EXPORT Units
  {
  public:
    ~Units(void);

    void AddUnit(EUnitCategory category, std::shared_ptr<Unit>& unit);
    void AddUnit(EUnitCategory category, std::string unitName, EUnitType unitType, std::string symbol, double coeff);
    void AddUnitGroup(std::string categoryName, EUnitCategory category);
    void AddUnitGroup(std::string categoryName, EUnitCategory category, EUnitCategory pattern);
    void Initialize();
    static const Units& GetInstance();
    const UnitGroup& GetUnitGroup(EUnitCategory category) const { return _groups.at(category); }
    EUnitCategory GetCategoryFromName(std::string name) const;
    const Unit* GetUnit(EUnitType unitType) const;
    EUnitType GetUnitTypeFromName(std::string name) const;
    const std::map<EUnitCategory, UnitGroup>& GetUnitGroups() const { return _groups; }
    double ConvertFromBase(EUnitCategory category, EUnitType destUnit, double value) const;
    double ConvertToBase(EUnitCategory category, EUnitType inputUnit, double value) const;
  private:
    Units(void);
    Units(Units const&);
    void operator=(Units const&);
    SIMUNITS_NOINTERFACE std::map<EUnitCategory, UnitGroup> _groups;
  public:
    static const double coefficientMeterToFoot;
    static const double coefficientMeterToInch;
    static const double coefficientKilogramToPoundMass;
    static const double coefficientRadianToDegree;
    static const double coefficientNewtonToForcePound;
    static const double coefficientJouleToCalorie;
    static const double coefficientJouleToBtu;
    static const double coefficientWattToBtuPerHour;
    static const double coefficientTempDifferenceKelvinToFahrenheit;
    static const double coefficientTempDifferenceKelvinToCelsius;
    static const double coefficientTempDifferenceCelsiusToFahrenheit;
    static const double coefficientKilogramToOunceMass;
    static const double coefficientKilogramToSlugMass;
  };
}

