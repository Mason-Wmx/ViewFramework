#pragma once
#include "stdafx.h"
#include "export.h"
#include "AppModel.h"
#include <headers/UnitTypes.h>


namespace SIM
{
  class DefaultUnit
  {
  public:
    DefaultUnit() : type(EUnitType::UNIT_TYPE_EMPTY), precision(0) {};
    DefaultUnit(EUnitType name, int prec) : type(name), precision(prec) {}

    EUnitType Type() const { return type; }
    DefaultUnit& Type(EUnitType name) { this->type = name;  return *this; }

    int Precision() const { return precision; }
    DefaultUnit& Precision(int precision) { this->precision = precision;  return *this; }

  private:
    EUnitType type;
    int precision;
  };

  typedef std::map<EUnitCategory, DefaultUnit> DefaultUnitSettings;


  class NFDCAPPCORE_EXPORT DefaultUnitsObject : public AppObject
  {
  public:
    DefaultUnitsObject(const std::string& name);
    ~DefaultUnitsObject();
    virtual void Write(QSettings& settings);
    virtual void Read(QSettings& settings);

    EUnitSystem GetSystem() const { return _system; }
    void SetSystem(EUnitSystem val) { _system = val; }

    DefaultUnitSettings GetUnits() const;
    DefaultUnitSettings GetUnits(EUnitSystem system) const;
    void SetCustomUnits(DefaultUnitSettings& custom);

    static const DefaultUnitSettings& GetHardDefaults4System(EUnitSystem val);
    static const std::string ObjName;

  private:
    void SetDefaults();

    EUnitSystem _system;
    DefaultUnitSettings _units;

    static const std::string SettingKey;
    static const std::string SettingKeySaved;
  };
}

