#pragma once
#include "SimUnits\stdafx.h"
#include <string>
#include <vector>
#include "ArithmeticExpression\Tokenizer.h"
#include "SyntaxProcessor.h"

struct SIMUNITS_EXPORT BinaryProvider : SyntaxProcessor::IDenominatorProvider {
  int seed;
  BinaryProvider();
  void Reset() override;
  int GetNext() override;
};

class SIMUNITS_EXPORT ArchitecturalDashReplacer : public ITokenStringManipulator
{
public:
  void execute(std::vector<Token*> &tokens, size_t pos, size_t len) const override;
  std::string getPattern() const override;
};

class SIMUNITS_EXPORT ArchitecturalBracketer : public ITokenStringManipulator
{
public:
  void execute(std::vector<Token*> &tokens, size_t pos, size_t len) const override;
  std::string getPattern() const override;
};

class SIMUNITS_EXPORT FractialSpaceModifier : public ITokenStringManipulator
{
public:
  void execute(std::vector<Token*> &tokens, size_t pos, size_t len) const override;
  std::string getPattern() const override;
};

class SIMUNITS_EXPORT UnitSymbolReplacer : public ITokenStringManipulator
{
private:
  SIMUNITS_NOINTERFACE std::string _pattern;
  double _coeff;
public:
  UnitSymbolReplacer(const std::string &symbol, const double _coeff);
  void execute(std::vector<Token*> &tokens, size_t pos, size_t len) const override;
  std::string getPattern() const override;
};

class SIMUNITS_EXPORT FractionFormatter : public SyntaxProcessor::IFormatter {
private:
  const int _accuracy;
  SyntaxProcessor::IDenominatorProvider *_denominatorProvider;
  FractionFormatter();
public:
  FractionFormatter( const int accuracy, SyntaxProcessor::IDenominatorProvider *denominatorProvider);
  ~FractionFormatter();
  std::string Format(const double value) const override;
};

class SIMUNITS_EXPORT ArchitecturalFormatter : public SyntaxProcessor::IFormatter {
private:
  const int _accuracy;
  SyntaxProcessor::IDenominatorProvider *_denominatorProvider;
  ArchitecturalFormatter();
public:
  ArchitecturalFormatter(const int accuracy, SyntaxProcessor::IDenominatorProvider *denominatorProvider);
  ~ArchitecturalFormatter();
  std::string Format(const double value) const override;
};


