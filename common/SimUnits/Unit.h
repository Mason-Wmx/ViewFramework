#pragma once
#include "stdafx.h"
#include <headers/UnitTypes.h>
#include <string>

namespace SIM
{
  class SIMUNITS_EXPORT Unit
  {
  public:
    Unit(double coeff, std::string unitName, EUnitType unitType, std::string symbol);
    Unit();
    ~Unit(void);
    double GetCoeff() const { return _coeff; }
    std::string GetSymbol() const { return _symbol; }
    virtual std::string GetSymbolName() const;
    SIM::EUnitType GetUnitType() const { return _unitType; }
    std::string  GetUnitName() const { return _unitName; }

    virtual double ConvertToBase(double value) const;
    virtual double ConvertFromBase(double baseValue) const;

    virtual std::string Format(double value, double accuracy, bool addUnitSymbol = true, bool suppressTrailing0 = false) const;
    std::string FormatBase(double baseValue, double accuracy, bool addUnitSymbol = true, bool suppressTrailing0 = false) const;

    virtual bool TryParse(std::string txt, double& value, double accuracy = -1.0) const;
    bool TryParseToBase(std::string txt, double& value, double accuracy = -1.0) const;
  private:
    double _coeff;
    EUnitType _unitType;
  protected:
    SIMUNITS_NOINTERFACE std::string _symbol;
    SIMUNITS_NOINTERFACE std::string _unitName;
  };
  /*This type of unit are not planned for first version of Autodesk Generative Design{
  class UnitCelsius:public Unit
  {
  public:
    UnitCelsius();
    ~UnitCelsius();
    virtual double ConvertToBase(double value) const;
    virtual double ConvertFromBase(double baseValue) const;
  };

  class UnitFahrenheit:public Unit
  {
  public:
    UnitFahrenheit();
    ~UnitFahrenheit();
    virtual double ConvertToBase(double value) const;
    virtual double ConvertFromBase(double baseValue) const;
  };

  class UnitRankine:public Unit
  {
  public:
    UnitRankine();
    ~UnitRankine();
    virtual double ConvertToBase(double value) const;
    virtual double ConvertFromBase(double baseValue) const;
  };
  }*/

  class UnitFractionalFeet :public Unit
  {
  public:
    UnitFractionalFeet(std::string unitName);
    ~UnitFractionalFeet();
    std::string GetSymbolName() const override;
    double ConvertToBase(double value) const override;
    double ConvertFromBase(double baseValue) const override;
    bool TryParse(std::string txt, double& value, double accuracy = -1.0) const override;
    std::string Format(double value, double accuracy, bool addUnitSymbol = true, bool suppressTrailing0 = false) const override;
  };

  class UnitFractionalInches :public Unit
  {
  public:
    UnitFractionalInches(std::string unitName);
    ~UnitFractionalInches();
    std::string GetSymbolName() const override;
    double ConvertToBase(double value) const override;
    double ConvertFromBase(double baseValue) const override;
    bool TryParse(std::string txt, double& value, double accuracy = -1.0) const override;
    std::string Format(double value, double accuracy, bool addUnitSymbol = true, bool suppressTrailing0 = false) const override;
  };

  class UnitArchitectural :public Unit
  {
  public:
    UnitArchitectural(std::string unitName);
    ~UnitArchitectural();
    std::string GetSymbolName() const override;
    double ConvertToBase(double value) const override;
    double ConvertFromBase(double baseValue) const override;
    bool TryParse(std::string txt, double& value, double accuracy = -1.0) const override;
    std::string Format(double value, double accuracy, bool addUnitSymbol = true, bool suppressTrailing0 = false) const override;
  };

}

