
#include "stdafx.h"
#include "DocUnits.h"
#include "DefaultUnitsObject.h"
#include "Application.h"



namespace SIM
{

  DocUnits::DocUnits()
  {
    _unitsModel = std::make_shared<UnitsModel>();

    SetDefaults();
  }

  DocUnits::~DocUnits()
  {

  }

  void DocUnits::SetDisplayUnit(EUnitCategory category, EUnitType selectedUnit, double accuracy, bool showSymbol)
  {
    _unitsModel->SetDisplayUnit(category, selectedUnit, accuracy, showSymbol);
  }

  std::string DocUnits::FormatBase(EUnitCategory category, double baseValue, bool addUnitSymbol /*= true*/) const
  {
    bool _suppressTrailing0 = _unitsModel->GetSuppressTrailing0();
    bool _showSymbol = _unitsModel->GetDisplayUnit(category).ShowSymbol;

    return GetUnit(category).FormatBase(baseValue, GetAccuracy(category), addUnitSymbol && _showSymbol, _suppressTrailing0);
  }

  std::string DocUnits::FormatDisplay(EUnitCategory category, double value, bool addUnitSymbol /*= true*/) const
  {
    bool _suppressTrailing0 = _unitsModel->GetSuppressTrailing0();
    bool _showSymbol = _unitsModel->GetDisplayUnit(category).ShowSymbol;

    return GetUnit(category).Format(value, GetAccuracy(category), addUnitSymbol && _showSymbol, _suppressTrailing0);
  }

  std::string DocUnits::FormatBase(EUnitCategory category, double baseValue, bool addUnitSymbol, bool suppressTrailing0) const
  {
    bool _showSymbol = _unitsModel->GetDisplayUnit(category).ShowSymbol;
    return GetUnit(category).FormatBase(baseValue, GetAccuracy(category), addUnitSymbol && _showSymbol, suppressTrailing0);
  }

  std::string DocUnits::FormatDisplay(EUnitCategory category, double value, bool addUnitSymbol, bool suppressTrailing0) const
  {
    bool _showSymbol = _unitsModel->GetDisplayUnit(category).ShowSymbol;
    return GetUnit(category).Format(value, GetAccuracy(category), addUnitSymbol && _showSymbol, suppressTrailing0);
  }
  double DocUnits::ConvertDisplayToBase(EUnitCategory category, double value) const
  {
    return GetUnit(category).ConvertToBase(value);
  }

  double DocUnits::ConvertBaseToDisplay(EUnitCategory category, double baseValue) const
  {
    return GetUnit(category).ConvertFromBase(baseValue);
  }

  bool DocUnits::TryParseToDisplay(EUnitCategory category, std::string txt, double& value, bool round) const
  {
    double accuracy = (round) ? GetAccuracy(category) : -1.0;

    return GetUnit(category).TryParse(txt, value, accuracy);
  }

  bool DocUnits::TryParseToBase(EUnitCategory category, std::string txt, double& value, bool round) const
  {
    double accuracy = (round) ? GetAccuracy(category) : -1.0;
    return GetUnit(category).TryParseToBase(txt, value, accuracy);
  }

  std::string DocUnits::GetUnitSymbol(EUnitCategory category) const
  {
    bool _showSymbol = _unitsModel->GetDisplayUnit(category).ShowSymbol;

    if (!_showSymbol)
      return "";

    return GetUnit(category).GetSymbol();
  }

  int DocUnits::GetUnitDecimalDigits(EUnitCategory category) const
  {
    return MathUtils::AccuracyToDecimalDigits(GetAccuracy(category));
  }

  EUnitType SIM::DocUnits::GetDisplayUnit(EUnitCategory category) const
  {
    return _unitsModel->GetDisplayUnit(category).Unit;
  }

  void DocUnits::GetDisplayUnit(EUnitCategory category, EUnitType& selectedUnit, double& accuracy, bool& showSymbol) const
  {
      UnitCategorySettings settings = _unitsModel->GetDisplayUnit(category);

      if (settings.Category != EUnitCategory::UNIT_UNDEFINED)
      {
          selectedUnit = settings.Unit;
          accuracy = settings.Accuracy;
          showSymbol = settings.ShowSymbol;
      }
  }


  void DocUnits::SetModel(std::shared_ptr<UnitsModel> ptr)
  {
    this->_unitsModel = ptr;
  }

  void DocUnits::SetDefaults()
  {
  }

  double DocUnits::GetGlobalAccuracy() const
  {
    return _unitsModel->GetGlobalAccuracy();
  }
  void DocUnits::SetGlobalAccuracy(double val)
  {
    _unitsModel->SetGlobalAccuracy(val);
  }

void SIM::DocUnits::SetDefaults(Application& app)
{
  std::shared_ptr<SIM::DefaultUnitsObject> unitObject = std::dynamic_pointer_cast <SIM::DefaultUnitsObject>(app.GetModel().GetAppObject("DefaultUnits"));
  if (unitObject.get())
  {
    for (auto i : unitObject->GetUnits())
    {
      int prec = i.second.Precision() > 9 ? 9 : i.second.Precision();
      double accuracy = prec == 0 ? 0.0 : std::pow(10, -prec);
      SetDisplayUnit(i.first, i.second.Type(), accuracy, true);
    }
  }
}

bool DocUnits::GetCustomGlobalAccuracy() const
{
  return _unitsModel->GetCustomGlobalAccuracy();
}

void DocUnits::SetCustomGlobalAccuracy(bool val)
{
  _unitsModel->SetCustomGlobalAccuracy(val);
}

  bool DocUnits::GetSuppressTrailing0() const
  {
    return _unitsModel->GetSuppressTrailing0();
  }
  void DocUnits::SetSuppressTrailing0(bool val)
  {
    _unitsModel->SetSuppressTrailing0(val);
  }

  const Unit& DocUnits::GetUnit(EUnitCategory category) const
  {
    return Units::GetInstance().GetUnitGroup(category).GetUnit(_unitsModel->GetDisplayUnit(category).Unit);
  }

  double DocUnits::GetAccuracy(EUnitCategory category) const
  {
    if (_unitsModel->GetUseGlobalAccurancy())
      return _unitsModel->GetGlobalAccuracy();

    UnitCategorySettings settings = _unitsModel->GetDisplayUnit(category);

    if (settings.Category != EUnitCategory::UNIT_UNDEFINED)
      return settings.Accuracy;

    return 0;
  }
}
