#pragma once
#include "stdafx.h"
#include <headers/UnitTypes.h>
#include <string>

namespace SIM
{
  class Unit;
  class SIMUNITS_EXPORT UnitGroup
  {
  public:
    UnitGroup(std::string categoryName, EUnitCategory category);
    UnitGroup(std::string categoryName, EUnitCategory category, const UnitGroup& pattern);
    UnitGroup();
    ~UnitGroup(void);

    void AddUnit(std::string unitName, EUnitType unitType, std::string symbol, double coeff);

    void AddUnit(std::shared_ptr<Unit>& unit);

    const Unit& GetUnit(EUnitType unit) const { return *_units.at(unit).get(); }

    bool ValidateUnit(EUnitType unit) const { return _units.find(unit) != _units.end(); }

    const std::map<EUnitType, std::shared_ptr<Unit>>& GetUnits() const { return _units; }

    double Convert(double value, EUnitType inputUnit, EUnitType outputUnit);

    std::string   GetCategoryName() const { return _categoryName; }

  private:
    EUnitCategory _category;
    SIMUNITS_NOINTERFACE std::string   _categoryName;
    SIMUNITS_NOINTERFACE std::map<EUnitType, std::shared_ptr<Unit>> _units;
  };

}

