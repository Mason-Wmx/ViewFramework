
#include "stdafx.h"
#include "Unit.h"
#include "UnitSymbols.h"
#include "SyntaxProcessor\SyntaxProcessor.h"
#include "SyntaxProcessor\Utils.h"

using namespace SIM;

SIM::Unit::Unit(double coeff, std::string unitName, EUnitType unitType, std::string symbol) :_coeff(coeff), _unitName(unitName), _symbol(symbol), _unitType(unitType)
{
}

SIM::Unit::Unit()
{

}

SIM::Unit::~Unit(void)
{

}

std::string SIM::Unit::GetSymbolName() const
{
  return _symbol;
}

std::string SIM::Unit::Format(double value, double accuracy, bool addUnitSymbol, bool suppressTrailing0) const
{
  std::string formated = TextUtils::Format(value, accuracy, suppressTrailing0);

  if (addUnitSymbol && !_symbol.empty())
  {
    formated.append(" ");
    formated.append(_symbol);
  }

  return formated;
}

std::string SIM::Unit::FormatBase(double baseValue, double accuracy, bool addUnitSymbol, bool suppressTrailing0) const
{
  double val = ConvertFromBase(baseValue);
  return Format(val, accuracy, addUnitSymbol, suppressTrailing0);
}

double SIM::Unit::ConvertToBase(double value) const
{
  return value / _coeff;
}

double SIM::Unit::ConvertFromBase(double baseValue) const
{
  return baseValue * _coeff;
}

bool SIM::Unit::TryParse(std::string txt, double& value, double accuracy) const
{
  std::string forParse(txt);
  std::string::size_type i = forParse.find(_symbol);

  if (i != std::string::npos)
    forParse.erase(i, _symbol.length());

  bool ok = TextUtils::TryParse(forParse, value);

  if (ok && accuracy > 0)
  {
    value = MathUtils::Round(value, accuracy);
    return true;
  }
  return ok;
}

bool SIM::Unit::TryParseToBase(std::string txt, double& value, double accuracy) const
{
  if (TryParse(txt, value, accuracy))
  {
    value = ConvertToBase(value);
    return true;
  }

  return false;
}


/*This type of unit are not planned for first version of Autodesk Generative Design{
SIM::UnitFahrenheit::UnitFahrenheit():Unit(1,EUnitType::UNIT_TYPE_FAHRENHEIT,UnitSymbols::Fahrenheit)
{

}

SIM::UnitFahrenheit::~UnitFahrenheit()
{

}

double SIM::UnitFahrenheit::ConvertToBase(double value) const
{
  return 273.15 + (value-32)*5/9;
}

double SIM::UnitFahrenheit::ConvertFromBase(double baseValue) const
{
  return (baseValue-273.15)*9/5+32;
}

SIM::UnitRankine::UnitRankine():Unit(1,EUnitType::UNIT_TYPE_RANKINE, UnitSymbols::Rankine)
{

}

SIM::UnitRankine::~UnitRankine()
{

}

double SIM::UnitRankine::ConvertToBase(double value) const
{
  return 273.15 + (value-491.67)/1.8;
}

double SIM::UnitRankine::ConvertFromBase(double baseValue) const
{
  return 1.8*(baseValue-273.15) + 491.67;
}


SIM::UnitCelsius::UnitCelsius():Unit(1,EUnitType::UNIT_TYPE_CELSIUS,UnitSymbols::Celsius)
{

}

SIM::UnitCelsius::~UnitCelsius()
{

}

double SIM::UnitCelsius::ConvertToBase(double value) const
{
  return value + 273.15;
}

double SIM::UnitCelsius::ConvertFromBase(double baseValue) const
{
  return baseValue-273.15;
}
*/

SIM::UnitFractionalFeet::UnitFractionalFeet(std::string unitName) :Unit(1, unitName, EUnitType::UNIT_TYPE_FRACTIONAL_FEET, UnitSymbols::FractionalFeet)
{

}

SIM::UnitFractionalFeet::~UnitFractionalFeet()
{

}

std::string SIM::UnitFractionalFeet::GetSymbolName() const
{
  return "frac ft";
}

double SIM::UnitFractionalFeet::ConvertToBase(double value) const {
  return value*0.3048;
}

double SIM::UnitFractionalFeet::ConvertFromBase(double baseValue) const {
  return baseValue/0.3048;
}


void replaceSeparator(std::string &txt) {
  std::stringstream ss;
  std::locale loc("");
  ss.imbue(loc);
  char point = std::use_facet< std::numpunct<char> >(loc).decimal_point();
  char group = std::use_facet< std::numpunct<char> >(loc).thousands_sep();
  txt.erase(std::remove(txt.begin(), txt.end(), group), txt.end());
  std::replace(txt.begin(), txt.end(), point, '.');
}

bool SIM::UnitFractionalFeet::TryParse(std::string txt, double& value, double accuracy) const {
  std::string forParse(txt);
  std::string::size_type i = forParse.find(_symbol);

  if (i != std::string::npos)
    forParse.erase(i, _symbol.length());

  replaceSeparator(forParse);

  SyntaxProcessor processor;
  processor.addTokenManipulator(new FractialSpaceModifier());
  auto res = processor.Parse(forParse);

  bool ok = !isnan(res);
  value = res;

  if (ok && accuracy > 0)
  {
    value = MathUtils::Round(value, accuracy);
    return true;
  }
  return ok;
}

std::string SIM::UnitFractionalFeet::Format(double value, double accuracy, bool addUnitSymbol, bool suppressTrailing0) const {

  accuracy = accuracy < 1.0e-10 ? 8 : 1 / accuracy;

  SyntaxProcessor engine;
  engine.setFormatter(new FractionFormatter((int)accuracy, new BinaryProvider()));
  std::string formated = engine.Format(value);

  if (addUnitSymbol && !_symbol.empty())
  {
    formated.append(" ");
    formated.append(_symbol);
  }

  return formated;
}
    
///////////////

SIM::UnitFractionalInches::UnitFractionalInches(std::string unitName) :Unit(1, unitName, EUnitType::UNIT_TYPE_FRACTIONAL_INCHES, UnitSymbols::FractionalInches)
{

}
 
SIM::UnitFractionalInches::~UnitFractionalInches()
{

}
 
std::string SIM::UnitFractionalInches::GetSymbolName() const
{
  return "frac in";
}
 
double SIM::UnitFractionalInches::ConvertToBase(double value) const {
  return value*0.0254;
}
 
double SIM::UnitFractionalInches::ConvertFromBase(double baseValue) const {
  return baseValue / 0.0254;
}
 
 
bool SIM::UnitFractionalInches::TryParse(std::string txt, double& value, double accuracy) const {
  std::string forParse(txt);
  std::string::size_type i = forParse.find(_symbol);

  if (i != std::string::npos)
    forParse.erase(i, _symbol.length());

  replaceSeparator(forParse);

  SyntaxProcessor processor;
  processor.addTokenManipulator(new FractialSpaceModifier());
  auto res = processor.Parse(forParse);

  bool ok = !isnan(res);
  value = res;

  if (ok && accuracy > 0)
  {
    value = MathUtils::Round(value, accuracy);
    return true;
  }
  return ok;
}
 
std::string SIM::UnitFractionalInches::Format(double value, double accuracy, bool addUnitSymbol, bool suppressTrailing0) const {

  accuracy = accuracy < 1.0e-10 ? 8 : 1 / accuracy;

  SyntaxProcessor engine;
  engine.setFormatter(new FractionFormatter((int)accuracy, new BinaryProvider()));
  std::string formated = engine.Format(value);

  if (addUnitSymbol && !_symbol.empty())
  {
    formated.append(" ");
    formated.append(_symbol);
  }

  return formated;
}

////

SIM::UnitArchitectural::UnitArchitectural(std::string unitName) :Unit(1, unitName, EUnitType::UNIT_TYPE_ARCHITECTURAL, "")
{

}

SIM::UnitArchitectural::~UnitArchitectural()
{

}

std::string SIM::UnitArchitectural::GetSymbolName() const
{
  return "0'-0/0''";
}

double SIM::UnitArchitectural::ConvertToBase(double value) const {
  return value*0.3048;
}

double SIM::UnitArchitectural::ConvertFromBase(double baseValue) const {
  return baseValue;
}

bool SIM::UnitArchitectural::TryParse(std::string txt, double& value, double accuracy) const {
  std::string forParse(txt);

  SyntaxProcessor processor;
  processor.addTokenManipulator(new ArchitecturalBracketer());
  processor.addTokenManipulator(new ArchitecturalDashReplacer());
  processor.addTokenManipulator(new UnitSymbolReplacer("F", 1.0));
  processor.addTokenManipulator(new UnitSymbolReplacer("I", 1.0 / 12));
  processor.addTokenManipulator(new FractialSpaceModifier());

  auto res = processor.Parse(forParse);

  bool ok = !isnan(res);
  value = res;

  if (ok && accuracy > 0)
  {
    value = MathUtils::Round(value, accuracy);
    return true;
  }
  return ok;
}

std::string SIM::UnitArchitectural::Format(double value, double accuracy, bool addUnitSymbol, bool suppressTrailing0) const {

  accuracy = accuracy<1.0e-10 ? 8 : 1/accuracy;

  SyntaxProcessor engine;
  engine.setFormatter(new ArchitecturalFormatter(accuracy, new BinaryProvider()));
  std::string formated = engine.Format(value);

  return formated;
}