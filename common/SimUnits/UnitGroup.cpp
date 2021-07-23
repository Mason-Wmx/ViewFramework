
#include "stdafx.h"
#include "UnitGroup.h"
#include "Unit.h"
using namespace SIM;


SIM::UnitGroup::UnitGroup(std::string categoryName, EUnitCategory category):_category(category), _categoryName(categoryName)
{

}

SIM::UnitGroup::UnitGroup()
{

}

SIM::UnitGroup::UnitGroup(std::string categoryName, EUnitCategory category, const UnitGroup& pattern):_category(category), _categoryName(categoryName)
{
	_units = pattern._units;
}

SIM::UnitGroup::~UnitGroup(void)
{

}
void SIM::UnitGroup::AddUnit(std::string unitName, EUnitType unitType, std::string symbol, double coeff)
{
	_units[unitType] = std::make_shared<Unit>(coeff,unitName, unitType,symbol);
}

void SIM::UnitGroup::AddUnit(std::shared_ptr<Unit>& unit)
{
	_units[unit->GetUnitType()] = unit;
}

double SIM::UnitGroup::Convert(double value, EUnitType inputUnit, EUnitType outputUnit)
{
	double baseValue = _units[inputUnit]->ConvertToBase(value);
	return _units[outputUnit]->ConvertFromBase(baseValue);
}

