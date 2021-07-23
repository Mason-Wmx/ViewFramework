
#include "stdafx.h"
#include "Units.h"
#include "Unit.h"
#include "UnitSymbols.h"
#include "UnitGroup.h"

using namespace SIM;

const double Units::coefficientMeterToFoot = 1.0 / 0.3048;
const double Units::coefficientMeterToInch = 12.0 * coefficientMeterToFoot;
const double Units::coefficientKilogramToPoundMass = 1.0 / 0.45359237;
const double Units::coefficientRadianToDegree = 180.0 / M_PI;
const double Units::coefficientNewtonToForcePound = 1.0 / 4.4482216152605;
const double Units::coefficientJouleToCalorie = 1.0 / 4.18684; //ISO 31-4 
const double Units::coefficientJouleToBtu = 1.0 / 1055.056;    //ISO 31-4
const double Units::coefficientWattToBtuPerHour = 3600 * coefficientJouleToBtu;
const double Units::coefficientTempDifferenceKelvinToFahrenheit = 1.8; // Attention: it is TEMPERATURE DIFFERENCE conversion (NOT TEMPERATURE conversion)
const double Units::coefficientTempDifferenceKelvinToCelsius = 1.0; // Attention: it is TEMPERATURE DIFFERENCE conversion (NOT TEMPERATURE conversion)
const double Units::coefficientTempDifferenceCelsiusToFahrenheit = coefficientTempDifferenceKelvinToFahrenheit; // Attention: it is TEMPERATURE DIFFERENCE conversion (NOT TEMPERATURE conversion)
const double Units::coefficientKilogramToOunceMass = 1.0 / 0.028349523125;
const double Units::coefficientKilogramToSlugMass = 1.0 / 14.593903;

Units::Units(void)
{
  Initialize();
}

Units::~Units(void)
{
}

void SIM::Units::AddUnit(EUnitCategory category, std::string unitName, EUnitType unitType, std::string symbol, double coeff)
{
  if (_groups.find(category) != _groups.end())
    _groups[category].AddUnit(unitName, unitType, symbol, coeff);
}

void SIM::Units::AddUnit(EUnitCategory category, std::shared_ptr<Unit>& unit)
{
  if (_groups.find(category) != _groups.end())
    _groups[category].AddUnit(unit);
}

const Units& SIM::Units::GetInstance()
{
  static Units instance;
  return instance;
}

void SIM::Units::Initialize()
{
  AddUnitGroup("Number", EUnitCategory::UNIT_NUMBER);
  AddUnit(EUnitCategory::UNIT_NUMBER, "", EUnitType::UNIT_TYPE_EMPTY, UnitSymbols::Empty, 1);

  AddUnitGroup("Length", EUnitCategory::UNIT_LENGTH);
  AddUnit(EUnitCategory::UNIT_LENGTH, "Meters", EUnitType::UNIT_TYPE_METERS, UnitSymbols::Meters, 1);
  AddUnit(EUnitCategory::UNIT_LENGTH, "Centimeters", EUnitType::UNIT_TYPE_CENTIMETERS, UnitSymbols::Centimeters, 1e2);
  AddUnit(EUnitCategory::UNIT_LENGTH, "Millimeters", EUnitType::UNIT_TYPE_MILLIMETERS, UnitSymbols::Millimeters, 1e3);
  AddUnit(EUnitCategory::UNIT_LENGTH, "DecimalFeet", EUnitType::UNIT_TYPE_DECIMAL_FEET, UnitSymbols::DecimalFeet, coefficientMeterToFoot);
  AddUnit(EUnitCategory::UNIT_LENGTH, std::shared_ptr<Unit>(new UnitFractionalFeet("FractionalFeet")));
  AddUnit(EUnitCategory::UNIT_LENGTH, "DecimalInches", EUnitType::UNIT_TYPE_DECIMAL_INCHES, UnitSymbols::DecimalInches, coefficientMeterToInch);
  AddUnit(EUnitCategory::UNIT_LENGTH, std::shared_ptr<Unit>(new UnitFractionalInches("FractionalInches")));
  AddUnit(EUnitCategory::UNIT_LENGTH, std::shared_ptr<Unit>(new UnitArchitectural("ArchitecturalFootInch")));

  AddUnitGroup("Area", EUnitCategory::UNIT_AREA);
  AddUnit(EUnitCategory::UNIT_AREA, "SquareMeters", EUnitType::UNIT_TYPE_SQUARE_METERS, UnitSymbols::SquareMeters, 1);
  AddUnit(EUnitCategory::UNIT_AREA, "SquareCentimeters", EUnitType::UNIT_TYPE_SQUARE_CENTIMETERS, UnitSymbols::SquareCentimeters, 1e4);
  AddUnit(EUnitCategory::UNIT_AREA, "SquareMillimeters", EUnitType::UNIT_TYPE_SQUARE_MILLIMETERS, UnitSymbols::SquareMillimeters, 1e6);
  AddUnit(EUnitCategory::UNIT_AREA, "SquareDecimalFeet", EUnitType::UNIT_TYPE_SQUARE_DECIMAL_FEET, UnitSymbols::SquareDecimalFeet, coefficientMeterToFoot*coefficientMeterToFoot);
  AddUnit(EUnitCategory::UNIT_AREA, "SquareDecimalInches", EUnitType::UNIT_TYPE_SQUARE_DECIMAL_INCHES, UnitSymbols::SquareDecimalInches, coefficientMeterToInch*coefficientMeterToInch);


  AddUnitGroup("Volume", EUnitCategory::UNIT_VOLUME);
  AddUnit(EUnitCategory::UNIT_VOLUME, "CubicMeters", EUnitType::UNIT_TYPE_CUBIC_METERS, UnitSymbols::CubicMeters, 1);
  AddUnit(EUnitCategory::UNIT_VOLUME, "CubicCentimeters", EUnitType::UNIT_TYPE_CUBIC_CENTIMETERS, UnitSymbols::CubicCentimeters, 1e6);
  AddUnit(EUnitCategory::UNIT_VOLUME, "CubicMillimeters", EUnitType::UNIT_TYPE_CUBIC_MILLIMETERS, UnitSymbols::CubicMillimeters, 1e9);
  AddUnit(EUnitCategory::UNIT_VOLUME, "CubicDecimalFeet", EUnitType::UNIT_TYPE_CUBIC_DECIMAL_FEET, UnitSymbols::CubicDecimalFeet, coefficientMeterToFoot*coefficientMeterToFoot*coefficientMeterToFoot);
  AddUnit(EUnitCategory::UNIT_VOLUME, "CubicDecimalInches", EUnitType::UNIT_TYPE_CUBIC_DECIMAL_INCHES, UnitSymbols::CubicDecimalInches, coefficientMeterToInch*coefficientMeterToInch*coefficientMeterToInch);


  AddUnitGroup("Angle", EUnitCategory::UNIT_ANGLE);
  AddUnit(EUnitCategory::UNIT_ANGLE, "Radians", EUnitType::UNIT_TYPE_RADIANS, UnitSymbols::Radian, 1);
  AddUnit(EUnitCategory::UNIT_ANGLE, "DecimalDegrees", EUnitType::UNIT_TYPE_DEGREES, UnitSymbols::DecimalDegree, coefficientRadianToDegree);

  AddUnitGroup("Mass", EUnitCategory::UNIT_MASS);
  AddUnit(EUnitCategory::UNIT_MASS, "KiloGrams", EUnitType::UNIT_TYPE_KILOGRAMS, UnitSymbols::KiloGrams, 1);
  AddUnit(EUnitCategory::UNIT_MASS, "Grams", EUnitType::UNIT_TYPE_GRAMS, UnitSymbols::Grams, 1e3);
  AddUnit(EUnitCategory::UNIT_MASS, "Tonnes", EUnitType::UNIT_TYPE_TONNES, UnitSymbols::Tonnes, 1e-3);
  AddUnit(EUnitCategory::UNIT_MASS, "PoundsMass", EUnitType::UNIT_TYPE_POUNDS_MASS, UnitSymbols::PoundsMass, coefficientKilogramToPoundMass);
  AddUnit(EUnitCategory::UNIT_MASS, "OuncesMass", EUnitType::UNIT_TYPE_OUNCES_MASS, UnitSymbols::OuncesMass, coefficientKilogramToOunceMass);
  AddUnit(EUnitCategory::UNIT_MASS, "SlugsMass", EUnitType::UNIT_TYPE_SLUGS_MASS, UnitSymbols::SlugsMass, coefficientKilogramToSlugMass);

  AddUnitGroup("Force", EUnitCategory::UNIT_FORCE);
  AddUnit(EUnitCategory::UNIT_FORCE, "Newtons", EUnitType::UNIT_TYPE_NEWTONS, UnitSymbols::Newtons, 1);
  AddUnit(EUnitCategory::UNIT_FORCE, "KiloNewtons", EUnitType::UNIT_TYPE_KILO_NEWTONS, UnitSymbols::KiloNewtons, 1e-3);
  AddUnit(EUnitCategory::UNIT_FORCE, "MegaNewtons", EUnitType::UNIT_TYPE_MEGA_NEWTONS, UnitSymbols::MegaNewtons, 1e-6);
  AddUnit(EUnitCategory::UNIT_FORCE, "PoundsForce", EUnitType::UNIT_TYPE_POUNDS_FORCE, UnitSymbols::PoundsForce, coefficientNewtonToForcePound);
  AddUnit(EUnitCategory::UNIT_FORCE, "KiloPoundsForce", EUnitType::UNIT_TYPE_KILO_POUNDS_FORCE, UnitSymbols::KiloPoundsForce, coefficientNewtonToForcePound * 1e-3);

  AddUnitGroup("Energy", EUnitCategory::UNIT_ENERGY);
  AddUnit(EUnitCategory::UNIT_ENERGY, "Joules", EUnitType::UNIT_TYPE_JOULES, UnitSymbols::Joules, 1);
  AddUnit(EUnitCategory::UNIT_ENERGY, "KiloJoules", EUnitType::UNIT_TYPE_KILO_JOULES, UnitSymbols::KiloJoules, 1e-3);
  AddUnit(EUnitCategory::UNIT_ENERGY, "MegaJoules", EUnitType::UNIT_TYPE_MEGA_JOULES, UnitSymbols::MegaJoules, 1e-6);
  AddUnit(EUnitCategory::UNIT_ENERGY, "Btu", EUnitType::UNIT_TYPE_BTU, UnitSymbols::Btu, coefficientJouleToBtu);
  AddUnit(EUnitCategory::UNIT_ENERGY, "KiloBtu", EUnitType::UNIT_TYPE_KILO_BTU, UnitSymbols::KiloBtu, coefficientJouleToBtu * 1e-3);

  AddUnitGroup("Density", EUnitCategory::UNIT_DENSITY);
  AddUnit(EUnitCategory::UNIT_DENSITY, "KilogramsPerCubicMeter", EUnitType::UNIT_TYPE_KILOGRAMS_PER_CUBIC_METER, UnitSymbols::KilogramsPerCubicMeter, 1);
  AddUnit(EUnitCategory::UNIT_DENSITY, "PoundsMassPerCubicFoot", EUnitType::UNIT_TYPE_POUNDS_MASS_PER_CUBIC_FOOT, UnitSymbols::PoundsMassPerCubicFoot, coefficientKilogramToPoundMass / (coefficientMeterToFoot*coefficientMeterToFoot*coefficientMeterToFoot));
  AddUnit(EUnitCategory::UNIT_DENSITY, "PoundsMassPerCubicInch", EUnitType::UNIT_TYPE_POUNDS_MASS_PER_CUBIC_INCH, UnitSymbols::PoundsMassPerCubicInch, coefficientKilogramToPoundMass / (coefficientMeterToInch*coefficientMeterToInch*coefficientMeterToInch));

  AddUnitGroup("Pressure", EUnitCategory::UNIT_PRESSURE);
  AddUnit(EUnitCategory::UNIT_PRESSURE, "Pascals", EUnitType::UNIT_TYPE_PASCALS, UnitSymbols::Pascals, 1);
  AddUnit(EUnitCategory::UNIT_PRESSURE, "Kilopascals", EUnitType::UNIT_TYPE_KILOPASCALS, UnitSymbols::Kilopascals, 1e-3);
  AddUnit(EUnitCategory::UNIT_PRESSURE, "Megapascals", EUnitType::UNIT_TYPE_MEGAPASCALS, UnitSymbols::Megapascals, 1e-6);
  AddUnit(EUnitCategory::UNIT_PRESSURE, "PoundsForcePerSquareInch", EUnitType::UNIT_TYPE_POUNDS_FORCE_PER_SQUARE_INCH, UnitSymbols::PoundsForcePerSquareInch, coefficientNewtonToForcePound / (coefficientMeterToInch*coefficientMeterToInch));
  AddUnit(EUnitCategory::UNIT_PRESSURE, "KilopoundsForcePerSquareInch", EUnitType::UNIT_TYPE_KILO_POUNDS_FORCE_PER_SQUARE_INCH, UnitSymbols::KilopoundsForcePerSquareInch, coefficientNewtonToForcePound * 1e-3 / (coefficientMeterToInch*coefficientMeterToInch));

  AddUnitGroup("Conductivity", EUnitCategory::UNIT_CONDUCTIVITY);
  AddUnit(EUnitCategory::UNIT_CONDUCTIVITY, "WattsPerMeterKelvin", EUnitType::UNIT_TYPE_WATTS_PER_METER_KELVIN, UnitSymbols::WattsPerMeterKelvin, 1);
  AddUnit(EUnitCategory::UNIT_CONDUCTIVITY, "WattsPerMeterCelsius", EUnitType::UNIT_TYPE_WATTS_PER_METER_CELSIUS, UnitSymbols::WattsPerMeterCelsius, 1.0 / coefficientTempDifferenceKelvinToCelsius);
  AddUnit(EUnitCategory::UNIT_CONDUCTIVITY, "BtuPerHourFootFahrenheit", EUnitType::UNIT_TYPE_BTU_PER_HOUR_FOOT_FAHRENHEIT, UnitSymbols::BtuPerHourFootFahrenheit, coefficientWattToBtuPerHour / (coefficientMeterToFoot*coefficientTempDifferenceKelvinToFahrenheit));

  AddUnitGroup("SpecificHeat", EUnitCategory::UNIT_SPECIFIC_HEAT);
  AddUnit(EUnitCategory::UNIT_SPECIFIC_HEAT, "JoulesPerKilogramKelvin", EUnitType::UNIT_TYPE_JOULES_PER_KILOGRAM_KELVIN, UnitSymbols::JoulesPerKilogramKelvin, 1);
  AddUnit(EUnitCategory::UNIT_SPECIFIC_HEAT, "JoulesPerKilogramCelsius", EUnitType::UNIT_TYPE_JOULES_PER_KILOGRAM_CELSIUS, UnitSymbols::JoulesPerKilogramCelsius, 1.0 / coefficientTempDifferenceKelvinToCelsius);
  AddUnit(EUnitCategory::UNIT_SPECIFIC_HEAT, "KiloJoulesPerKilogramKelvin", EUnitType::UNIT_TYPE_KILOJOULES_PER_KILOGRAM_KELVIN, UnitSymbols::KiloJoulesPerKilogramKelvin, 1e-3);
  AddUnit(EUnitCategory::UNIT_SPECIFIC_HEAT, "KiloJoulesPerKilogramCelsius", EUnitType::UNIT_TYPE_KILOJOULES_PER_KILOGRAM_CELSIUS, UnitSymbols::KiloJoulesPerKilogramCelsius, 1e-3 / coefficientTempDifferenceKelvinToCelsius);
  AddUnit(EUnitCategory::UNIT_SPECIFIC_HEAT, "BtuPerPoundMassFahrenheit", EUnitType::UNIT_TYPE_BTU_PER_POUND_MASS_FAHRENHEIT, UnitSymbols::BtuPerPoundMassFahrenheit, coefficientJouleToBtu / (coefficientKilogramToPoundMass*coefficientTempDifferenceKelvinToFahrenheit));

  AddUnitGroup("ThermalExpansion", EUnitCategory::UNIT_THERMAL_EXPANSION);
  AddUnit(EUnitCategory::UNIT_THERMAL_EXPANSION, "OnePerKelvin", EUnitType::UNIT_TYPE_ONE_PER_KELVIN, UnitSymbols::OnePerKelvin, 1);
  AddUnit(EUnitCategory::UNIT_THERMAL_EXPANSION, "OnePerCelsius", EUnitType::UNIT_TYPE_ONE_PER_CELSIUS, UnitSymbols::OnePerCelsius, 1.0 / coefficientTempDifferenceKelvinToCelsius);
  AddUnit(EUnitCategory::UNIT_THERMAL_EXPANSION, "OnePerFahrenheit", EUnitType::UNIT_TYPE_ONE_PER_FAHRENHEIT, UnitSymbols::OnePerFahrenheit, 1.0 / coefficientTempDifferenceKelvinToFahrenheit);

  AddUnitGroup("Stiffness", EUnitCategory::UNIT_STIFFNESS);
  AddUnit(EUnitCategory::UNIT_STIFFNESS, "NewtonsPerMeter", EUnitType::UNIT_TYPE_NEWTONS_PER_METER, UnitSymbols::NewtonsPerMeter, 1);
  AddUnit(EUnitCategory::UNIT_STIFFNESS, "KiloNewtonsPerMeter", EUnitType::UNIT_TYPE_KILONEWTONS_PER_METER, UnitSymbols::KiloNewtonsPerMeter, 1e-3);
  AddUnit(EUnitCategory::UNIT_STIFFNESS, "PoundsForcePerInch", EUnitType::UNIT_TYPE_POUNDS_FORCE_PER_INCH, UnitSymbols::PoundsForcePerInch, coefficientNewtonToForcePound / coefficientMeterToInch);
  AddUnit(EUnitCategory::UNIT_STIFFNESS, "PoundsForcePerFoot", EUnitType::UNIT_TYPE_POUNDS_FORCE_PER_FOOT, UnitSymbols::PoundsForcePerFoot, coefficientNewtonToForcePound / coefficientMeterToFoot);
}

void SIM::Units::AddUnitGroup(std::string categoryName, EUnitCategory category)
{
  if (_groups.find(category) == _groups.end())
    _groups.insert(std::make_pair(category, UnitGroup(categoryName, category)));
}

void SIM::Units::AddUnitGroup(std::string categoryName, EUnitCategory category, EUnitCategory pattern)
{
  if (_groups.find(category) == _groups.end() && _groups.find(pattern) != _groups.end())
    _groups.insert(std::make_pair(category, UnitGroup(categoryName, category, _groups.at(pattern))));
}

EUnitCategory Units::GetCategoryFromName(std::string name) const
{
  for each (const std::pair<EUnitCategory, UnitGroup>& group in _groups)
  {
      if (name == group.second.GetCategoryName())
        return group.first;
  }
  return EUnitCategory::UNIT_UNDEFINED;
}

const Unit* Units::GetUnit(EUnitType unitType) const
{
  for each (const std::pair<EUnitCategory, UnitGroup>& group in _groups)
  {
    for each(const std::pair<EUnitType, std::shared_ptr<Unit>>& unit in group.second.GetUnits())
    {
      if (unitType == unit.second.get()->GetUnitType())
        return unit.second.get();
    }
  }
  return NULL;
}

EUnitType Units::GetUnitTypeFromName(std::string name) const
{
  for each (const std::pair<EUnitCategory, UnitGroup>& group in _groups)
  {
    for each(const std::pair<EUnitType, std::shared_ptr<Unit>>& unit in group.second.GetUnits())
    {
      if (name == unit.second.get()->GetUnitName())
        return unit.second.get()->GetUnitType();
    }
  }
  return UNIT_TYPE_EMPTY;
}

double SIM::Units::ConvertFromBase(EUnitCategory category, EUnitType destUnit, double value) const
{
  if (_groups.find(category) != _groups.end())
  {
    return _groups.at(category).GetUnit(destUnit).ConvertFromBase(value);
  }
  return value;
}

double SIM::Units::ConvertToBase(EUnitCategory category, EUnitType inputUnit, double value) const
{
  if (_groups.find(category) != _groups.end())
  {
    return _groups.at(category).GetUnit(inputUnit).ConvertToBase(value);
  }
  return value;
}


