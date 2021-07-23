#include <math.h>
#include <sstream>
#include <algorithm>

#include "SyntaxProcessor.h"
#include "ArithmeticExpression\Tokenizer.h"
#include "ArithmeticExpression\BinaryTreeBuilder.h"

SyntaxProcessor::Fraction::Fraction() : coeff(1), integer(0), numerator(0), denominator(0) {

}

const double unityEps = 0.001;
SyntaxProcessor::Fraction::Fraction( int coeff_, int integer_, int numerator_, int denominator_)
  : coeff(coeff_), integer(integer_), numerator(numerator_), denominator(denominator_) {
  if (denominator != 0 && (fabs(denominator - numerator) < unityEps) ) {
    integer += (int)(floor(numerator / denominator + 0.5));
    denominator = 1;
    numerator = 0;
  }
}

double SyntaxProcessor::Fraction::getValue() const{
  return coeff * (double(integer) + double(numerator)/double(denominator));
}

void SyntaxProcessor::cleanTokenManipulators() {
  for (ITokenStringManipulator* manipulator : _tokenManipulators) {
    delete manipulator;
  }
  _tokenManipulators.clear();
}

void SyntaxProcessor::cleanFormatter(){
  setFormatter(nullptr);
}

SyntaxProcessor::SyntaxProcessor(): _formatter(nullptr) {
  cleanTokenManipulators();
  cleanFormatter();
}

SyntaxProcessor::~SyntaxProcessor() {
  cleanTokenManipulators();
  cleanFormatter();
}

void SyntaxProcessor::addTokenManipulator(ITokenStringManipulator *pManipulator) {
  _tokenManipulators.push_back(pManipulator);
}

void SyntaxProcessor::setFormatter(IFormatter * pFormatter)
{
  if (_formatter != nullptr) {
    delete _formatter;
  }
  _formatter = pFormatter;
}


std::string SyntaxProcessor::Format(double value) const
{
  return _formatter == nullptr ? "" : _formatter->Format(value);
}

SyntaxProcessor::Fraction SyntaxProcessor::CreateFraction(double val, IDenominatorProvider &denominators, int maxDenominator)  {
  denominators.Reset();
  int coeff = val < 0.0 ? -1 : 1;
  val *= coeff;
  int integer = (int)floor(val);
  double fractionValue = val - integer;
  int numer = 0;
  int denom = 0;
  const double eps = 1.0 / maxDenominator;
  for( int i=0; i<20; i++){
    denom = denominators.GetNext();
    if ( denom==0) {
      break;
    }
    numer = (int)floor((double)fractionValue*denom + 0.5);
    if ( fabs(double(numer) / denom-fractionValue) < eps) {
      break;
    }
  }
  return SyntaxProcessor::Fraction( coeff, integer, numer, denom);
}


double SyntaxProcessor::Parse(std::string text) const {

  double retVal = NAN;
  Tokenizer tokenizer;
  tokenizer.Tokenize(text);
  auto &tokens = tokenizer.GetTokens();
  tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const Token* token) {return token->GetType() == Token::Type::END; }), tokens.end());

  TokenPostprocessor pprocessor(tokens);
  for (ITokenStringManipulator* manipulator : _tokenManipulators) {
    pprocessor.ApplyManipulator(manipulator);
  }
  BinaryTreeBuilder builder(tokens);
  auto pTreeNode = builder.BuildTree();
  if (pTreeNode != nullptr) {
    retVal = pTreeNode->CalculateValue();
  }

  return retVal;
}
