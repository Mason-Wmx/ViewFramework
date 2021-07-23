#pragma once
#include <string>
#include "stdafx.h"

namespace SIM
{
  class SIMUNITS_EXPORT UnitSymbols
  {
  public:
    SIMUNITS_NOINTERFACE static const std::string Meters;
    SIMUNITS_NOINTERFACE static const std::string Decimeters;
    SIMUNITS_NOINTERFACE static const std::string Centimeters;
    SIMUNITS_NOINTERFACE static const std::string Millimeters;
    SIMUNITS_NOINTERFACE static const std::string DecimalFeet;
    SIMUNITS_NOINTERFACE static const std::string FractionalFeet;
    SIMUNITS_NOINTERFACE static const std::string DecimalInches;
    SIMUNITS_NOINTERFACE static const std::string FractionalInches;
    SIMUNITS_NOINTERFACE static const std::string Empty;

    /*This type of unit are not planned for first version of Autodesk Generative Design{
    SIMUNITS_NOINTERFACE static const std::string Fahrenheit;
    SIMUNITS_NOINTERFACE static const std::string Celsius;
    SIMUNITS_NOINTERFACE static const std::string Kelvin;
    SIMUNITS_NOINTERFACE static const std::string Rankine;
    }*/

    SIMUNITS_NOINTERFACE static const std::string Pascals;
    SIMUNITS_NOINTERFACE static const std::string Kilopascals;
    SIMUNITS_NOINTERFACE static const std::string Megapascals;
    SIMUNITS_NOINTERFACE static const std::string PoundsForcePerSquareInch;
    SIMUNITS_NOINTERFACE static const std::string KilopoundsForcePerSquareInch;
    /*This type of unit are not planned for first version of Autodesk Generative Design{
    SIMUNITS_NOINTERFACE static const std::string InchesOfMercury;
    SIMUNITS_NOINTERFACE static const std::string MillimetersOfMercury;
    SIMUNITS_NOINTERFACE static const std::string Atmospheres;
    SIMUNITS_NOINTERFACE static const std::string Bars;
    SIMUNITS_NOINTERFACE static const std::string MetersOfWater;
    SIMUNITS_NOINTERFACE static const std::string FeetOfWater;
    SIMUNITS_NOINTERFACE static const std::string InchesOfWater;

    SIMUNITS_NOINTERFACE static const std::string LitersPerSecond;
    SIMUNITS_NOINTERFACE static const std::string LitersPerMinute;
    SIMUNITS_NOINTERFACE static const std::string LitersPerHour;
    SIMUNITS_NOINTERFACE static const std::string CubicMetersPerSecond;
    SIMUNITS_NOINTERFACE static const std::string CubicMetersPerMinute;
    SIMUNITS_NOINTERFACE static const std::string CubicMetersPerHour;
    SIMUNITS_NOINTERFACE static const std::string USGallonsPerMinute;
    SIMUNITS_NOINTERFACE static const std::string USGallonsPerHour;
    SIMUNITS_NOINTERFACE static const std::string CubicFeetPerMinute;

    SIMUNITS_NOINTERFACE static const std::string MetersPerSecond;
    SIMUNITS_NOINTERFACE static const std::string FeetPerSecond;
    SIMUNITS_NOINTERFACE static const std::string CentimetersPerMinute;
    SIMUNITS_NOINTERFACE static const std::string FeetPerMinute;
    }*/

    SIMUNITS_NOINTERFACE static const std::string KilogramsPerCubicMeter;
    SIMUNITS_NOINTERFACE static const std::string PoundsMassPerCubicFoot;
    SIMUNITS_NOINTERFACE static const std::string PoundsMassPerCubicInch;

    /*This type of unit are not planned for first version of Autodesk Generative Design{
    SIMUNITS_NOINTERFACE static const std::string PascalSeconds;
    SIMUNITS_NOINTERFACE static const std::string PoundsPerFootSecond;
    SIMUNITS_NOINTERFACE static const std::string Centipoises;
    SIMUNITS_NOINTERFACE static const std::string PoundsPerFootHour;*/

    SIMUNITS_NOINTERFACE static const std::string WattsPerMeterKelvin;
    SIMUNITS_NOINTERFACE static const std::string WattsPerMeterCelsius;
    SIMUNITS_NOINTERFACE static const std::string BtuPerHourFootFahrenheit;

    //static const std::string JoulesPerGramCelsius;
    SIMUNITS_NOINTERFACE static const std::string JoulesPerKilogramKelvin;
    SIMUNITS_NOINTERFACE static const std::string JoulesPerKilogramCelsius;
    SIMUNITS_NOINTERFACE static const std::string KiloJoulesPerKilogramKelvin;
    SIMUNITS_NOINTERFACE static const std::string KiloJoulesPerKilogramCelsius;
    SIMUNITS_NOINTERFACE static const std::string BtuPerPoundMassFahrenheit;

    SIMUNITS_NOINTERFACE static const std::string OnePerKelvin;
    SIMUNITS_NOINTERFACE static const std::string OnePerCelsius;
    SIMUNITS_NOINTERFACE static const std::string OnePerFahrenheit;

    //static const  std::string KilogramsPerSecond;

    SIMUNITS_NOINTERFACE static const std::string Radian;
    SIMUNITS_NOINTERFACE static const std::string DecimalDegree;
    SIMUNITS_NOINTERFACE static const std::string Gradian;

    SIMUNITS_NOINTERFACE static const std::string Grams;
    SIMUNITS_NOINTERFACE static const std::string KiloGrams;
    SIMUNITS_NOINTERFACE static const std::string Tonnes;
    SIMUNITS_NOINTERFACE static const std::string PoundsMass;
    SIMUNITS_NOINTERFACE static const std::string OuncesMass;
    SIMUNITS_NOINTERFACE static const std::string SlugsMass;

    SIMUNITS_NOINTERFACE static const std::string Newtons;
    SIMUNITS_NOINTERFACE static const std::string KiloNewtons;
    SIMUNITS_NOINTERFACE static const std::string MegaNewtons;
    SIMUNITS_NOINTERFACE static const std::string PoundsForce;
    SIMUNITS_NOINTERFACE static const std::string KiloPoundsForce;

    SIMUNITS_NOINTERFACE static const std::string Joules;
    SIMUNITS_NOINTERFACE static const std::string KiloJoules;
    SIMUNITS_NOINTERFACE static const std::string MegaJoules;
    SIMUNITS_NOINTERFACE static const std::string Calories;
    SIMUNITS_NOINTERFACE static const std::string KiloCalories;
    SIMUNITS_NOINTERFACE static const std::string Btu;
    SIMUNITS_NOINTERFACE static const std::string KiloBtu;

    SIMUNITS_NOINTERFACE static const std::string CubicMeters;
    SIMUNITS_NOINTERFACE static const std::string CubicDecimeters;
    SIMUNITS_NOINTERFACE static const std::string CubicCentimeters;
    SIMUNITS_NOINTERFACE static const std::string CubicMillimeters;
    SIMUNITS_NOINTERFACE static const std::string CubicDecimalFeet;
    SIMUNITS_NOINTERFACE static const std::string CubicDecimalInches;

    SIMUNITS_NOINTERFACE static const std::string SquareMeters;
    SIMUNITS_NOINTERFACE static const std::string SquareDecimeters;
    SIMUNITS_NOINTERFACE static const std::string SquareCentimeters;
    SIMUNITS_NOINTERFACE static const std::string SquareMillimeters;
    SIMUNITS_NOINTERFACE static const std::string SquareDecimalFeet;
    SIMUNITS_NOINTERFACE static const std::string SquareDecimalInches;

    SIMUNITS_NOINTERFACE static const std::string NewtonsPerMeter;
    SIMUNITS_NOINTERFACE static const std::string KiloNewtonsPerMeter;
    SIMUNITS_NOINTERFACE static const std::string PoundsForcePerInch;
    SIMUNITS_NOINTERFACE static const std::string PoundsForcePerFoot;
  };
}