#pragma once
#include <string>
#include "SimUnits\stdafx.h"


class Node;
class ITokenStringManipulator;
class SIMUNITS_EXPORT SyntaxProcessor
{
public:
  class IFormatter {
  public:
    virtual std::string Format( const double value) const = 0;
  };

  class IDenominatorProvider
  {
  public:
    virtual void Reset() = 0;
    virtual int GetNext() = 0;
  };

  class SIMUNITS_EXPORT Fraction {
  public:
    int coeff, integer, numerator, denominator;
  public:
    Fraction();
    Fraction(int coeff, int integer_, int numerator_, int denominator_);
    double getValue() const;
  };

public:
  SyntaxProcessor();
  ~SyntaxProcessor();

  void addTokenManipulator(ITokenStringManipulator *pManipulator);
  void setFormatter(IFormatter *pFormatter);

  static Fraction CreateFraction(double val, IDenominatorProvider &denominatorProvider, int maxDenominator);
  
  double Parse(std::string text) const;
  std::string Format(double value) const ;

private:

  SIMUNITS_NOINTERFACE std::vector<ITokenStringManipulator*> _tokenManipulators;
  void cleanTokenManipulators();

  IFormatter *_formatter;
  void cleanFormatter();
  
};

// 12'-6 1/2''
// 12 1/2 ft  [+-x/()N]
// 
// 1. NN/N -> (N+N/N)
// 2. '-N -> '+N
// 3. " -> 0.3048/12
// 4. ' -> 0.3048