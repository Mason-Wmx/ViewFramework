
#include "stdafx.h"
#include "UnitSymbols.h"

namespace SIM
{
const std::string UnitSymbols::Meters("m");
const std::string UnitSymbols::Decimeters("dm");
const std::string UnitSymbols::Centimeters("cm");
const std::string UnitSymbols::Millimeters("mm");
const std::string UnitSymbols::DecimalFeet("ft");
const std::string UnitSymbols::FractionalFeet("ft");
const std::string UnitSymbols::DecimalInches("in");
const std::string UnitSymbols::FractionalInches("in");

const std::string UnitSymbols::Empty("");

/*This type of unit are not planned for first version of Autodesk Generative Design{
const std::string UnitSymbols::Fahrenheit("\xc2\xba" "F");
const std::string UnitSymbols::Celsius("\xc2\xba" "C");
const std::string UnitSymbols::Kelvin("K");
const std::string UnitSymbols::Rankine("\xc2\xba" "R");
}*/

const std::string UnitSymbols::Pascals("Pa");
const std::string UnitSymbols::Kilopascals("kPa");
const std::string UnitSymbols::Megapascals("MPa");
const std::string UnitSymbols::PoundsForcePerSquareInch("psi");
const std::string UnitSymbols::KilopoundsForcePerSquareInch("ksi");

/*This type of unit are not planned for first version of Autodesk Generative Design{
const std::string UnitSymbols::InchesOfMercury("inHg");
const std::string UnitSymbols::MillimetersOfMercury("mmHg");
const std::string UnitSymbols::Atmospheres("atm");
const std::string UnitSymbols::Bars("bar");
const std::string UnitSymbols::MetersOfWater("mH2O");
const std::string UnitSymbols::FeetOfWater("ftH2O");
const std::string UnitSymbols::InchesOfWater("inH2O");

const std::string UnitSymbols::LitersPerSecond("L/s");
const std::string UnitSymbols::LitersPerMinute("L/min");
const std::string UnitSymbols::LitersPerHour("L/h");
const std::string UnitSymbols::CubicMetersPerSecond("m" "\xc2\xb3" "/s");
const std::string UnitSymbols::CubicMetersPerMinute("m" "\xc2\xb3" "/min");
const std::string UnitSymbols::CubicMetersPerHour("m" "\xc2\xb3" "/h");
const std::string UnitSymbols::USGallonsPerMinute("gal/min");
const std::string UnitSymbols::USGallonsPerHour("gal/h");
const std::string UnitSymbols::CubicFeetPerMinute("ft" "\xc2\xb3" "/min");

const std::string UnitSymbols::MetersPerSecond("m/s");
const std::string UnitSymbols::FeetPerSecond("ft/s");
const std::string UnitSymbols::CentimetersPerMinute("cm/min");
const std::string UnitSymbols::FeetPerMinute("ft/min");
}*/

const std::string UnitSymbols::KilogramsPerCubicMeter("kg/m" "\xc2\xb3");
const std::string UnitSymbols::PoundsMassPerCubicFoot("lb/ft" "\xc2\xb3");
const std::string UnitSymbols::PoundsMassPerCubicInch("lb/in" "\xc2\xb3");

/*This type of unit are not planned for first version of Autodesk Generative Design{
const std::string UnitSymbols::PascalSeconds("Pa-s");
const std::string UnitSymbols::PoundsPerFootSecond("lb/ft-s");
const std::string UnitSymbols::Centipoises("cP");
const std::string UnitSymbols::PoundsPerFootHour("lb/ft-h");*/

const std::string UnitSymbols::WattsPerMeterKelvin("W/(m" "\xc2\xb7" "K)");
const std::string UnitSymbols::WattsPerMeterCelsius("W/(m" "\xc2\xb7" "\xc2\xba" "C)");
const std::string UnitSymbols::BtuPerHourFootFahrenheit("Btu/(h" "\xc2\xb7" "ft" "\xc2\xb7" "\xc2\xba" "F)");

//const std::string UnitSymbols::JoulesPerGramCelsius("J/(g" "\xc2\xb7" "\xc2\xba" "C)");
const std::string UnitSymbols::JoulesPerKilogramKelvin("J/(kg" "\xc2\xb7" "K)");
const std::string UnitSymbols::JoulesPerKilogramCelsius("J/(kg" "\xc2\xb7" "\xc2\xba" "C)");
const std::string UnitSymbols::KiloJoulesPerKilogramKelvin("kJ/(kg" "\xc2\xb7" "K)");
const std::string UnitSymbols::KiloJoulesPerKilogramCelsius("kJ/(kg" "\xc2\xb7" "\xc2\xba" "C)");
const std::string UnitSymbols::BtuPerPoundMassFahrenheit("Btu/(lb" "\xc2\xb7" "\xc2\xba" "F)");

const std::string UnitSymbols::OnePerKelvin("1/K");
const std::string UnitSymbols::OnePerCelsius("1/" "\xc2\xba" "C");
const std::string UnitSymbols::OnePerFahrenheit("1/" "\xc2\xba" "F");

//const std::string UnitSymbols::KilogramsPerSecond("kg/s");

const std::string UnitSymbols::Radian("rad");
const std::string UnitSymbols::DecimalDegree("\xc2\xba");
const std::string UnitSymbols::Gradian("gon");

const std::string UnitSymbols::Grams("g");
const std::string UnitSymbols::KiloGrams("kg");
const std::string UnitSymbols::Tonnes("t");
const std::string UnitSymbols::PoundsMass("lb");
const std::string UnitSymbols::OuncesMass("oz");
const std::string UnitSymbols::SlugsMass("slug");

const std::string UnitSymbols::Newtons("N");
const std::string UnitSymbols::KiloNewtons("kN");
const std::string UnitSymbols::MegaNewtons("MN");
const std::string UnitSymbols::PoundsForce("lbf");
const std::string UnitSymbols::KiloPoundsForce("kip");

const std::string UnitSymbols::Joules("J");
const std::string UnitSymbols::KiloJoules("kJ");
const std::string UnitSymbols::MegaJoules("MJ");
const std::string UnitSymbols::Calories("cal");
const std::string UnitSymbols::KiloCalories("kcal");
const std::string UnitSymbols::Btu("Btu");
const std::string UnitSymbols::KiloBtu("kBtu");

const std::string UnitSymbols::CubicMeters("m" "\xc2\xb3");
const std::string UnitSymbols::CubicDecimeters("dm" "\xc2\xb3");
const std::string UnitSymbols::CubicCentimeters("cm" "\xc2\xb3");
const std::string UnitSymbols::CubicMillimeters("mm" "\xc2\xb3");
const std::string UnitSymbols::CubicDecimalFeet("ft" "\xc2\xb3");
const std::string UnitSymbols::CubicDecimalInches("in" "\xc2\xb3");

const std::string UnitSymbols::SquareMeters("m" "\xc2\xb2");
const std::string UnitSymbols::SquareDecimeters("dm" "\xc2\xb2");
const std::string UnitSymbols::SquareCentimeters("cm" "\xc2\xb2");
const std::string UnitSymbols::SquareMillimeters("mm" "\xc2\xb2");
const std::string UnitSymbols::SquareDecimalFeet("ft" "\xc2\xb2");
const std::string UnitSymbols::SquareDecimalInches("in" "\xc2\xb2");

const std::string UnitSymbols::NewtonsPerMeter("N/m");
const std::string UnitSymbols::KiloNewtonsPerMeter("kN/m");
const std::string UnitSymbols::PoundsForcePerInch("lbf/in");
const std::string UnitSymbols::PoundsForcePerFoot("lbf/ft");
}