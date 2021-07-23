#pragma once
#include <string>

namespace SIM
{
  enum EUnitSystem {
    UNIT_SYSTEM_METRIC = 0,
    UNIT_SYSTEM_ENGLISH,
    UNIT_SYSTEM_CUSTOM,
  };

  enum EUnitCategory {
    UNIT_LENGTH,
    UNIT_NUMBER,
    UNIT_AREA,
    UNIT_VOLUME,
    UNIT_ANGLE,
    UNIT_MASS,
    UNIT_FORCE,
    UNIT_ENERGY,
    UNIT_DENSITY,
    UNIT_PRESSURE,
    UNIT_STIFFNESS,
    UNIT_CONDUCTIVITY,
    UNIT_SPECIFIC_HEAT,
    UNIT_THERMAL_EXPANSION,

    /*This groups of unit are not planned to use in the Dreamcatcher{
    UNIT_PIPE_DIMENSION = 0x20,
    UNIT_PIPE_SIZE,
    UNIT_PIPING_TEMPERATURE,
    UNIT_PIPING_PRESSURE,
    UNIT_PIPING_HEAD,
    UNIT_PIPING_FLOW,
    UNIT_PIPING_CAPACITY,
    UNIT_PIPING_VELOCITY,
    UNIT_PIPING_DENSITY,
    UNIT_PIPING_VISCOSITY,
    UNIT_PIPE_INSULATION_THICKNESS,
    UNIT_PIPE_ROUGHNESS,

    UNIT_HVAC_DUCT_SIZE = 0x40,
    UNIT_HVAC_TEMPERATURE,
    UNIT_HVAC_PRESSURE,
    UNIT_HVAC_HEAD,
    UNIT_HVAC_AIRFLOW,
    UNIT_HVAC_CAPACITY,
    UNIT_HVAC_VELOCITY,
    UNIT_HVAC_DENSITY,
    UNIT_HVAC_VISCOSITY,
    UNIT_HVAC_INSULATION_THICKNESS,
    UNIT_HVAC_ROUGHNESS,
    }*/
    /*This groups of unit are not planned for first version of Dreamcatcher{

    UNIT_MASS_FLOW_RATE = 0x80,
    */

    UNIT_UNDEFINED
  };

  enum EUnitType {
    UNIT_TYPE_EMPTY = 0,


    // Length
    UNIT_TYPE_METERS = 10,
    //This type of unit are not planned for first version of Dreamcatcher
    //UNIT_TYPE_DECIMETERS = 20,
    UNIT_TYPE_CENTIMETERS = 30,
    UNIT_TYPE_MILLIMETERS = 40,
    UNIT_TYPE_DECIMAL_FEET = 50,
    UNIT_TYPE_FRACTIONAL_FEET = 51,
    UNIT_TYPE_DECIMAL_INCHES = 60,
    UNIT_TYPE_FRACTIONAL_INCHES = 70,
    UNIT_TYPE_ARCHITECTURAL = 80,

    /*This type of unit are not planned for first version of Dreamcatcher{
    // Temperature
    UNIT_TYPE_FAHRENHEIT = 100,
    UNIT_TYPE_CELSIUS = 110,
    UNIT_TYPE_KELVIN = 120,
    UNIT_TYPE_RANKINE = 130,
    }*/

    // Pressure
    UNIT_TYPE_PASCALS = 200,
    UNIT_TYPE_KILOPASCALS = 205,
    UNIT_TYPE_MEGAPASCALS = 210,
    UNIT_TYPE_POUNDS_FORCE_PER_SQUARE_INCH = 220,
    UNIT_TYPE_KILO_POUNDS_FORCE_PER_SQUARE_INCH = 221,
    /*This type of unit are not planned for first version of Dreamcatcher{
    UNIT_TYPE_INCHES_OF_MERCURY = 230,
    UNIT_TYPE_MILLIMETERS_OF_MERCURY = 240,
    UNIT_TYPE_ATMOSPHERES = 250,
    UNIT_TYPE_BARS = 260,
    UNIT_TYPE_METERS_OF_WATER = 270,
    UNIT_TYPE_FEET_OF_WATER = 280,
    UNIT_TYPE_INCHES_OF_WATER = 290,*/

    // Stiffness
    UNIT_TYPE_NEWTONS_PER_METER = 300,
    UNIT_TYPE_KILONEWTONS_PER_METER = 310,
    UNIT_TYPE_POUNDS_FORCE_PER_INCH = 320,
    UNIT_TYPE_POUNDS_FORCE_PER_FOOT = 330,

    // Flow rate
    /*UNIT_TYPE_LITERS_PER_SECOND = 300,
    UNIT_TYPE_LITERS_PER_MINUTE = 310,
    UNIT_TYPE_LITERS_PER_HOUR = 320,
    UNIT_TYPE_CUBIC_METERS_PER_SECOND = 330,
    UNIT_TYPE_CUBIC_METERS_PER_MINUTE = 340,
    UNIT_TYPE_CUBIC_METERS_PER_HOUR = 350,
    UNIT_TYPE_US_GALLONS_PER_MINUTE = 360,
    UNIT_TYPE_US_GALLONS_PER_HOUR = 370,
    UNIT_TYPE_CUBIC_FEET_PER_MINUTE = 380,

    // Velocity
    UNIT_TYPE_METERS_PER_SECOND = 400,
    UNIT_TYPE_FEET_PER_SECOND = 410,
    UNIT_TYPE_CENTIMETERS_PER_MINUTE = 420,
    UNIT_TYPE_FEET_PER_MINUTE = 430,
    }*/

    // Mass
    UNIT_TYPE_GRAMS = 400,
    UNIT_TYPE_KILOGRAMS = 410,
    UNIT_TYPE_TONNES = 420,
    UNIT_TYPE_POUNDS_MASS = 430,
    UNIT_TYPE_OUNCES_MASS = 440,
    UNIT_TYPE_SLUGS_MASS = 450,


    // Density
    UNIT_TYPE_KILOGRAMS_PER_CUBIC_METER = 500,
    UNIT_TYPE_POUNDS_MASS_PER_CUBIC_FOOT = 510,
    UNIT_TYPE_POUNDS_MASS_PER_CUBIC_INCH = 520,

    /*This type of unit are not planned for first version of Dreamcatcher{
    // Viscosity
    UNIT_TYPE_PASCAL_SECONDS = 600,
    UNIT_TYPE_POUNDS_PER_FOOT_SECOND = 610,
    UNIT_TYPE_CENTIPOISES = 620,
    UNIT_TYPE_POUNDS_PER_FOOT_HOUR = 630,*/

    // Thermal conductivity
    UNIT_TYPE_WATTS_PER_METER_KELVIN = 700,
    UNIT_TYPE_WATTS_PER_METER_CELSIUS = 710,
    UNIT_TYPE_BTU_PER_HOUR_FOOT_FAHRENHEIT = 720,

    // Specific heat
    UNIT_TYPE_JOULES_PER_KILOGRAM_KELVIN = 800,
    UNIT_TYPE_JOULES_PER_KILOGRAM_CELSIUS = 810,
    UNIT_TYPE_KILOJOULES_PER_KILOGRAM_KELVIN = 820,
    UNIT_TYPE_KILOJOULES_PER_KILOGRAM_CELSIUS = 830,
    UNIT_TYPE_BTU_PER_POUND_MASS_FAHRENHEIT = 840,

    //This type of unit are not planned for first version of Dreamcatcher
    // Mass flow rate
    /*UNIT_TYPE_KILOGRAMS_PER_SECOND = 900,
    }*/

    // Area
    UNIT_TYPE_SQUARE_METERS = 1000,
    //This type of unit are not planned for first version of Dreamcatcher
    //UNIT_TYPE_SQUARE_DECIMETERS = 1010,
    UNIT_TYPE_SQUARE_CENTIMETERS = 1020,
    UNIT_TYPE_SQUARE_MILLIMETERS = 1030,
    UNIT_TYPE_SQUARE_DECIMAL_FEET = 1040,
    UNIT_TYPE_SQUARE_DECIMAL_INCHES = 1050,

    // Volume
    UNIT_TYPE_CUBIC_METERS = 1100,
    //This type of unit are not planned for first version of Dreamcatcher
    //UNIT_TYPE_CUBIC_DECIMETERS = 1110,
    UNIT_TYPE_CUBIC_CENTIMETERS = 1120,
    UNIT_TYPE_CUBIC_MILLIMETERS = 1130,
    UNIT_TYPE_CUBIC_DECIMAL_FEET = 1140,
    UNIT_TYPE_CUBIC_DECIMAL_INCHES = 1150,

    // Angle
    UNIT_TYPE_RADIANS = 1200,
    //This type of unit are not planned for first version of Dreamcatcher
    //UNIT_TYPE_GRADIAN = 1210,
    UNIT_TYPE_DEGREES = 1220,

    // Force
    UNIT_TYPE_NEWTONS = 1300,
    UNIT_TYPE_KILO_NEWTONS = 1310,
    UNIT_TYPE_MEGA_NEWTONS = 1320,
    UNIT_TYPE_POUNDS_FORCE = 1330,
    UNIT_TYPE_KILO_POUNDS_FORCE = 1340,

    // Energy
    UNIT_TYPE_JOULES = 1400,
    UNIT_TYPE_KILO_JOULES = 1410,
    UNIT_TYPE_MEGA_JOULES = 1420,
    //This type of unit are not planned for first version of Dreamcatcher
    //UNIT_TYPE_CALORIE = 1430,
    //UNIT_TYPE_KILO_CALORIE = 1440,
    UNIT_TYPE_BTU = 1450,
    UNIT_TYPE_KILO_BTU = 1460,

    // Thermal Expansion Coefficient
    UNIT_TYPE_ONE_PER_KELVIN = 1500,
    UNIT_TYPE_ONE_PER_CELSIUS = 1510,
    UNIT_TYPE_ONE_PER_FAHRENHEIT = 1520
  };

  struct UnitCategory
  {
    inline static EUnitCategory Length() { return EUnitCategory::UNIT_LENGTH; }
    inline static EUnitCategory Number() { return EUnitCategory::UNIT_NUMBER; }
    inline static EUnitCategory Area() { EUnitCategory::UNIT_AREA; }
    inline static EUnitCategory Volume() { EUnitCategory::UNIT_VOLUME; }
    inline static EUnitCategory Angle() { EUnitCategory::UNIT_ANGLE; }
    inline static EUnitCategory Force() { EUnitCategory::UNIT_FORCE; }
    inline static EUnitCategory Energy() { EUnitCategory::UNIT_ENERGY; }
    inline static EUnitCategory Density() { EUnitCategory::UNIT_DENSITY; }
    inline static EUnitCategory Stress() { EUnitCategory::UNIT_PRESSURE; }
    inline static EUnitCategory Conductivity() { EUnitCategory::UNIT_CONDUCTIVITY; }
    inline static EUnitCategory SpecificHeat() { EUnitCategory::UNIT_SPECIFIC_HEAT; }
    inline static EUnitCategory ThermalExpansion() { EUnitCategory::UNIT_THERMAL_EXPANSION; }
  };
}