#include "stdafx.h"
#include "DefaultUnitsObject.h"

using namespace SIM;

const std::string  DefaultUnitsObject::ObjName = "DefaultUnits";
const std::string SIM::DefaultUnitsObject::SettingKeySaved("DefaultUnitsObjectSaved");
const std::string SIM::DefaultUnitsObject::SettingKey("DefaultUnitsObject");

/* defaults ******************************************************************************************** */

// this is the METRIC DEFAULT units map
static DefaultUnitSettings _metric_default_units
{ 
  { EUnitCategory::UNIT_NUMBER,   DefaultUnit().Type(EUnitType::UNIT_TYPE_EMPTY).Precision(2) } ,
  { EUnitCategory::UNIT_LENGTH,   DefaultUnit().Type(EUnitType::UNIT_TYPE_MILLIMETERS).Precision(0) } ,
  { EUnitCategory::UNIT_MASS,     DefaultUnit().Type(EUnitType::UNIT_TYPE_GRAMS).Precision(2) } ,
  { EUnitCategory::UNIT_DENSITY,  DefaultUnit().Type(EUnitType::UNIT_TYPE_KILOGRAMS_PER_CUBIC_METER).Precision(0) } ,
  { EUnitCategory::UNIT_PRESSURE, DefaultUnit().Type(EUnitType::UNIT_TYPE_MEGAPASCALS).Precision(1) } ,
  { EUnitCategory::UNIT_FORCE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_NEWTONS).Precision(1) } ,
  { EUnitCategory::UNIT_AREA,     DefaultUnit().Type(EUnitType::UNIT_TYPE_SQUARE_MILLIMETERS).Precision(1) } ,
  { EUnitCategory::UNIT_VOLUME,   DefaultUnit().Type(EUnitType::UNIT_TYPE_CUBIC_MILLIMETERS).Precision(2) } ,
  { EUnitCategory::UNIT_ENERGY,   DefaultUnit().Type(EUnitType::UNIT_TYPE_KILO_JOULES).Precision(0) } ,
  { EUnitCategory::UNIT_ANGLE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_DEGREES).Precision(0) },
  { EUnitCategory::UNIT_CONDUCTIVITY,      DefaultUnit().Type(EUnitType::UNIT_TYPE_WATTS_PER_METER_KELVIN).Precision(2) },
  { EUnitCategory::UNIT_SPECIFIC_HEAT,     DefaultUnit().Type(EUnitType::UNIT_TYPE_KILOJOULES_PER_KILOGRAM_KELVIN).Precision(3) },
  { EUnitCategory::UNIT_THERMAL_EXPANSION, DefaultUnit().Type(EUnitType::UNIT_TYPE_ONE_PER_KELVIN).Precision(7) },
  { EUnitCategory::UNIT_STIFFNESS, DefaultUnit().Type(EUnitType::UNIT_TYPE_NEWTONS_PER_METER).Precision(2) }
};

// this is the ENGLISH DEFAULT units map
static DefaultUnitSettings _english_default_units
{ 
  { EUnitCategory::UNIT_NUMBER,   DefaultUnit().Type(EUnitType::UNIT_TYPE_EMPTY).Precision(2) } ,
  { EUnitCategory::UNIT_LENGTH,   DefaultUnit().Type(EUnitType::UNIT_TYPE_DECIMAL_INCHES).Precision(2) } ,
  { EUnitCategory::UNIT_MASS,     DefaultUnit().Type(EUnitType::UNIT_TYPE_POUNDS_MASS).Precision(2) } ,
  { EUnitCategory::UNIT_DENSITY,  DefaultUnit().Type(EUnitType::UNIT_TYPE_POUNDS_MASS_PER_CUBIC_INCH).Precision(4) } ,
  { EUnitCategory::UNIT_PRESSURE, DefaultUnit().Type(EUnitType::UNIT_TYPE_POUNDS_FORCE_PER_SQUARE_INCH).Precision(0) } ,
  { EUnitCategory::UNIT_FORCE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_POUNDS_FORCE).Precision(1) } ,
  { EUnitCategory::UNIT_AREA,     DefaultUnit().Type(EUnitType::UNIT_TYPE_SQUARE_DECIMAL_INCHES).Precision(3) } ,
  { EUnitCategory::UNIT_VOLUME,   DefaultUnit().Type(EUnitType::UNIT_TYPE_CUBIC_DECIMAL_INCHES).Precision(4) } ,
  { EUnitCategory::UNIT_ENERGY,   DefaultUnit().Type(EUnitType::UNIT_TYPE_BTU).Precision(0) } ,
  { EUnitCategory::UNIT_ANGLE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_DEGREES).Precision(0) },
  { EUnitCategory::UNIT_CONDUCTIVITY,      DefaultUnit().Type(EUnitType::UNIT_TYPE_BTU_PER_HOUR_FOOT_FAHRENHEIT).Precision(2) },
  { EUnitCategory::UNIT_SPECIFIC_HEAT,     DefaultUnit().Type(EUnitType::UNIT_TYPE_BTU_PER_POUND_MASS_FAHRENHEIT).Precision(3) },
  { EUnitCategory::UNIT_THERMAL_EXPANSION, DefaultUnit().Type(EUnitType::UNIT_TYPE_ONE_PER_KELVIN).Precision(7) },
  { EUnitCategory::UNIT_STIFFNESS, DefaultUnit().Type(EUnitType::UNIT_TYPE_POUNDS_FORCE_PER_INCH).Precision(2) }
};

// this is the CUSTOM DEFAULT units map
static DefaultUnitSettings _custom_default_units
{ 
  { EUnitCategory::UNIT_NUMBER,   DefaultUnit().Type(EUnitType::UNIT_TYPE_EMPTY).Precision(2) } ,
  { EUnitCategory::UNIT_LENGTH,   DefaultUnit().Type(EUnitType::UNIT_TYPE_MILLIMETERS).Precision(0) } ,
  { EUnitCategory::UNIT_MASS,     DefaultUnit().Type(EUnitType::UNIT_TYPE_GRAMS).Precision(2) } ,
  { EUnitCategory::UNIT_DENSITY,  DefaultUnit().Type(EUnitType::UNIT_TYPE_KILOGRAMS_PER_CUBIC_METER).Precision(0) } ,
  { EUnitCategory::UNIT_PRESSURE, DefaultUnit().Type(EUnitType::UNIT_TYPE_MEGAPASCALS).Precision(1) } ,
  { EUnitCategory::UNIT_FORCE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_NEWTONS).Precision(1) } ,
  { EUnitCategory::UNIT_AREA,     DefaultUnit().Type(EUnitType::UNIT_TYPE_SQUARE_MILLIMETERS).Precision(1) } ,
  { EUnitCategory::UNIT_VOLUME,   DefaultUnit().Type(EUnitType::UNIT_TYPE_CUBIC_MILLIMETERS).Precision(2) } ,
  { EUnitCategory::UNIT_ENERGY,   DefaultUnit().Type(EUnitType::UNIT_TYPE_KILO_JOULES).Precision(0) } ,
  { EUnitCategory::UNIT_ANGLE,    DefaultUnit().Type(EUnitType::UNIT_TYPE_DEGREES).Precision(0) },
  { EUnitCategory::UNIT_CONDUCTIVITY,      DefaultUnit().Type(EUnitType::UNIT_TYPE_WATTS_PER_METER_KELVIN).Precision(2) },
  { EUnitCategory::UNIT_SPECIFIC_HEAT,     DefaultUnit().Type(EUnitType::UNIT_TYPE_KILOJOULES_PER_KILOGRAM_KELVIN).Precision(3) },
  { EUnitCategory::UNIT_THERMAL_EXPANSION, DefaultUnit().Type(EUnitType::UNIT_TYPE_ONE_PER_KELVIN).Precision(7) },
  { EUnitCategory::UNIT_STIFFNESS, DefaultUnit().Type(EUnitType::UNIT_TYPE_NEWTONS_PER_METER).Precision(2) }
};

const SIM::DefaultUnitSettings& SIM::DefaultUnitsObject::GetHardDefaults4System(EUnitSystem val)
{
  switch (val) {
  default:
  case EUnitSystem::UNIT_SYSTEM_CUSTOM:
    return _custom_default_units;
  case EUnitSystem::UNIT_SYSTEM_ENGLISH:
    return _english_default_units;
  case EUnitSystem::UNIT_SYSTEM_METRIC:
    return _metric_default_units;
  }
}

SIM::DefaultUnitsObject::DefaultUnitsObject(const std::string& name) : AppObject(name)
{
  SetDefaults();
}

SIM::DefaultUnitsObject::~DefaultUnitsObject()
{

}

void SIM::DefaultUnitsObject::SetDefaults()
{
  _system = EUnitSystem::UNIT_SYSTEM_CUSTOM;
  _units = _custom_default_units;
}

DefaultUnitSettings SIM::DefaultUnitsObject::GetUnits() const
{
  return GetUnits(_system);
}

DefaultUnitSettings SIM::DefaultUnitsObject::GetUnits(EUnitSystem system) const
{
  switch (system) {
  default:
  case EUnitSystem::UNIT_SYSTEM_CUSTOM:
    return _units;
  case EUnitSystem::UNIT_SYSTEM_ENGLISH:
    return _english_default_units;
  case EUnitSystem::UNIT_SYSTEM_METRIC:
    return _metric_default_units;
  }
}

void SIM::DefaultUnitsObject::SetCustomUnits(DefaultUnitSettings& custom)
{
  _units = custom;
}

void SIM::DefaultUnitsObject::Write(QSettings& settings)
{
  settings.setValue(SettingKeySaved.c_str(), 1);
  settings.beginGroup(SettingKey.c_str());
  settings.setValue("system", _system);

  for (auto cat : _units)
  {
    settings.beginGroup(std::to_string(cat.first).c_str());
    settings.setValue("type", cat.second.Type());
    settings.setValue("precision", cat.second.Precision());
    settings.endGroup();
  }
  settings.endGroup();
}

void SIM::DefaultUnitsObject::Read(QSettings& settings)
{
  if (settings.contains(SettingKeySaved.c_str()))
  {
    settings.beginGroup(SettingKey.c_str());
    int system = settings.value("system", EUnitSystem::UNIT_SYSTEM_CUSTOM).toInt();
    _system = EUnitSystem(system);
    _units = GetUnits(_system);

    QStringList groups = settings.childGroups();
    for (auto cat : _units)
    {
      if (groups.contains(std::to_string(cat.first).c_str())) 
      {
        settings.beginGroup(std::to_string(cat.first).c_str());
        int type = settings.value("type").toInt();
        int precision = settings.value("precision").toInt();
        _units[cat.first] = DefaultUnit(EUnitType(type), precision);
        settings.endGroup();
      }
    }
    settings.endGroup();
  }
}

