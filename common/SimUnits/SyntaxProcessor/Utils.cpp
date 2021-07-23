#include "Utils.h"
#include <algorithm>
#include <regex>
#include "ArithmeticExpression\Tokenizer.h"

BinaryProvider::BinaryProvider() {
  Reset();
}

void BinaryProvider::Reset() {
  seed = 1;
}

int BinaryProvider::GetNext() {
  return seed *= 2;
}


std::string FractialSpaceModifier::getPattern() const {
  return std::string("NN/N");
}

void FractialSpaceModifier::execute(std::vector<Token*> &tokens, size_t pos, size_t len) const {
  bool bNegated = pos > 0 && tokens[pos-1]->GetType() == Token::Type::O && tokens[pos-1]->GetSubType() == Token::SubType::SUB;
  tokens.insert(tokens.begin() + pos + 4, new Token(Token::Type::R)); 
  tokens.insert(tokens.begin() + pos + 1, new Token(Token::SubType::ADD));
  tokens.insert(tokens.begin() + pos,     new Token(Token::Type::L));
  if(bNegated) {
    tokens.insert(tokens.begin() + pos - 1, new Token(0.0));
  }
}

std::string ArchitecturalDashReplacer::getPattern() const {
  return std::string("F-N");
}

void ArchitecturalDashReplacer::execute(std::vector<Token*> &tokens, size_t pos, size_t len) const {
  vector<Token*>::iterator it = tokens.begin() + pos + 1;
  delete *it;
  *it = new Token(Token::SubType::ADD);
}

std::string ArchitecturalBracketer::getPattern() const {
  return std::string("(NF-NN/NI)|(NF-NI)|(NF-N/NI)");
}


void ArchitecturalBracketer::execute(std::vector<Token*> &tokens, size_t pos, size_t len) const {
  bool bNegated = pos == 0 && tokens[0]->GetType() == Token::Type::O && tokens[0]->GetSubType() == Token::SubType::SUB;
  tokens.insert(tokens.begin() + pos + len, new Token(Token::Type::R));
  tokens.insert(tokens.begin() + pos, new Token(Token::Type::L));
  if (bNegated) {
    tokens.insert(tokens.begin() + pos - 1, new Token(0.0));
  }
}


UnitSymbolReplacer::UnitSymbolReplacer(const std::string &symbol, const double coeff) {
  _pattern.push_back('N');
  _pattern.append(symbol);
  _coeff = coeff;
}

std::string UnitSymbolReplacer::getPattern() const {
  return _pattern;
}

void UnitSymbolReplacer::execute(std::vector<Token*> &tokens, size_t pos, size_t len) const {
  vector<Token*>::iterator it = tokens.begin() + pos + 1;
  delete *it;
  *it = new Token(_coeff);
  tokens.insert(it, new Token(Token::SubType::MUL));
}

FractionFormatter::FractionFormatter(const int accuracy, SyntaxProcessor::IDenominatorProvider *denominatorProvider) : _accuracy(accuracy), _denominatorProvider(denominatorProvider) {

}

FractionFormatter::~FractionFormatter()
{
  delete _denominatorProvider;
}

std::string FractionFormatter::Format(const double value) const
{
  SyntaxProcessor::Fraction fraction = SyntaxProcessor::CreateFraction(value, *_denominatorProvider, _accuracy);
  std::stringstream ss;
  if (fraction.coeff < 0) {
    ss << "-";
  }
  bool bInteger = fraction.integer != 0;
  if ( bInteger ) {
    ss << fraction.integer;
  }
  if (fraction.numerator != 0)
  {
    ss << (bInteger ? " " : "") << fraction.numerator << "/" << fraction.denominator;
  }
  if (!bInteger && fraction.numerator == 0) {
    ss << "0";
  }

  return ss.str();
}



ArchitecturalFormatter::ArchitecturalFormatter(const int accuracy, SyntaxProcessor::IDenominatorProvider *denominatorProvider) : _accuracy(accuracy), _denominatorProvider(denominatorProvider) {

}

ArchitecturalFormatter::~ArchitecturalFormatter()
{
  delete _denominatorProvider;
}

std::string ArchitecturalFormatter::Format(const double value) const
{
  const double ft2m = 0.3048;
  const double in2m = 0.0254;

  double feetRaw = floor(value / ft2m);
  double rest = value - feetRaw*ft2m;

  double inch = rest / in2m;
  SyntaxProcessor::Fraction inchFraction = SyntaxProcessor::CreateFraction(inch, *_denominatorProvider, _accuracy);

  int feetInFraction = inchFraction.integer / 12;
  if ( feetInFraction > 0) {
    feetRaw += feetInFraction;
    inchFraction.integer -= (feetInFraction * 12);
  }

  std::stringstream ss;
  int feet = (int)feetRaw;
  bool bFeet = feetRaw != 0;
  if (bFeet) {
    ss << feet << "'";
  }

  bool bIntegerInch = inchFraction.integer != 0,
       bFractialInch = inchFraction.numerator != 0,
       bAnyInch = bIntegerInch || bFractialInch;

  if (bFeet && bAnyInch) {
    ss << "-";
  }
  
  if (bIntegerInch) {
    ss <<  inchFraction.integer;
  }
  if (bFractialInch)
  {
    ss << ( bIntegerInch ? " " : "") << inchFraction.numerator << "/" << inchFraction.denominator;
  }

  if (bAnyInch) 
  {
    ss << "\"";
  }
  
  if (!bFeet && !bAnyInch)
  {
    ss << "0'-0\"";
  }

  return ss.str();
}