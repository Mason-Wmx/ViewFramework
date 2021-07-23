#pragma once

#include <string>
#include <cctype>
#include<iostream>
#include <sstream> 
#include <vector>
#include <Iterator>
#include <algorithm>
#include <regex>

#include "Tokenizer.h"
Token::Token(): type(Type::NONE), name(""), value(0.0) {

}


Token::Type Token::GetType() const { 
  return type; 
}
Token::SubType Token::GetSubType() const { 
  return subType; 
}
string    Token::GetName() const { 
  return name; 
}
double Token::GetValue()   const { 
  return value; 
}
char Token::GetTypeSymbol() const {
  return type == Type::N ? 'N' : (type == Type::S ? GetName()[0] : (type == Type::O ? char(subType) : 'U'));
}

Token::Token(const Token &src) {
  this->type = src.type;
  this->subType = src.subType;
  this->name = src.name;
  this->value = src.value;
}
Token::Token(const string& name) : type(Type::S), subType(SubType::NONE), value(0.0) {
  this->name = name == "'" ? "F" : ((name == "''" || name == "\"") ? "I" : name);
}
Token::Token(double value) : type(Type::N), subType(SubType::NONE), value(value) {

}
Token::Token(SubType otype) : type(Type::O), subType(otype), value(0.0) {

}
Token::Token(Type ttype) : type(ttype), subType(SubType::NONE), value(0.0) {

}

void Token::DeleteTokenVector(vector<Token*> &tokens) {
  for (auto & token : tokens) {
    delete token;
	token = nullptr;
  }
  tokens.clear();
}

std::string Token::StringifyTokenVector( const vector<Token*> &tokens) {
  std::string tokensSymbolString;
  std::transform(tokens.begin(), tokens.end(), back_inserter(tokensSymbolString), [](Token* token) {return token->GetTypeSymbol(); });
  return tokensSymbolString;
}



void Tokenizer::error(string msg) {
  errorCount++;
}
 
using TT = Token::Type;
using OT = Token::SubType;

bool isalpha_(char ch) {
  return isalpha(ch) || ch == '\'' || ch == '"';
}

bool isalnum_(char ch) {
  return isalnum(ch) || ch == '\'' || ch == '"';
}

TT Tokenizer::get_token()
{
  char ch;
  do {// skip whitespace except '\en'
    if (!ssin.get(ch)) return curr_ttype = TT::END;
  } while (ch != '\n' && isspace(ch));

  switch (ch) {
  case ';':
  case '\n':
    return curr_ttype = TT::PRINT;
  case '*':
  case '/':
  case '+':
  case '-':
  case '=':
    {
      char nextCh;
      ssin.get(nextCh);
      if (nextCh != '=') {
          ssin.unget();
      }
      curr_otype = OT(ch);
    }
    return curr_ttype = TT::O;
  case '(':
  case ')':
    return curr_ttype = TT(ch);
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
  case '.':
    ssin.unget();
    ssin >> curr_numberValue;
    return curr_ttype = TT::N;
  default:// NAME, NAME=, or error
    if (isalpha_(ch)) {
      curr_stringValue = ch;
      while (ssin.get(ch) && isalnum_(ch))
        curr_stringValue += ch;
      ssin.unget();
      return curr_ttype = TT::S;
    }
    error("bad token");
    return curr_ttype = TT::PRINT;
  }
}

void Tokenizer::clear() {
  Token::DeleteTokenVector(tokens);
  tokens.clear();
  errorCount = 0;
  ssin.clear();
  curr_ttype = TT::NONE;
  curr_otype = OT::NONE;
  curr_numberValue = 0.0;
  curr_stringValue = "";
}



void Tokenizer::tokenize( ){
  while (curr_ttype != TT::END) {
    get_token();
    if (curr_ttype == TT::S) {
      AddToken(curr_stringValue);
    }
    else if (curr_ttype == TT::N) {
      AddToken(curr_numberValue);
    }
    else if (curr_ttype==TT::O){
      AddToken(curr_otype);
    }
    else {
      AddToken(curr_ttype);
    }
  }
}


void Tokenizer::Clear() {
  clear();
}

Tokenizer::~Tokenizer() {
  clear();
}

void Tokenizer::AddToken(const string &name) {  
  tokens.push_back(new Token(name));  
}
void Tokenizer::AddToken( double value ) {
  tokens.push_back(new Token(value));  
}
void Tokenizer::AddToken(Token::Type otype) {
  tokens.push_back(new Token(otype)); 
}
void Tokenizer::AddToken(Token::SubType ttype) {
  tokens.push_back(new Token(ttype)); 
}

Token* Tokenizer::GetToken(size_t ind) {
  return tokens[ind]; 
}

void Tokenizer::Tokenize(string str) {
  clear();
  ssin.str(str);
  tokenize();
}

vector<Token*> &Tokenizer::GetTokens() {
  return tokens; 
}

const vector<Token*> &Tokenizer::GetTokens() const {
  return tokens; 
}


TokenPostprocessor::TokenPostprocessor(vector<Token*> &tokens_) : tokens(tokens_) {

}

void TokenPostprocessor::ApplyManipulator(const ITokenStringManipulator *replacer) {
  
  using Coord = pair<size_t, size_t>;
  std::string srep = Token::StringifyTokenVector(tokens);
  std::regex regex_(replacer->getPattern(), std::regex_constants::syntax_option_type::awk);
  vector<Coord> vCoords;
  std::smatch res;
  std::regex_search(srep, res, regex_);
  
  auto res_begin = std::sregex_iterator(srep.begin(), srep.end(), regex_);
  auto res_end = std::sregex_iterator();
  
  for (std::sregex_iterator it = res_begin; it != res_end; it++)
  {
    vCoords.insert(vCoords.begin(), Coord( it->position(),it->length()));
  }

  for (Coord coord : vCoords) {
    replacer->execute(tokens, coord.first, coord.second);
  }
}