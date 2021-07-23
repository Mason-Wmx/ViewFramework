#pragma once

#include <string>
#include <cctype>
#include<iostream>
#include <sstream> 
#include <vector>
#include <Iterator>
#include <algorithm>

using namespace std;

class Token {
public:
  enum class Type {
    NONE, S, N, L = '(', R = ')',O, PRINT, END, BADTOKEN
  };

  enum class SubType {
    NONE, ADD='+', SUB='-', MUL='*', DIV='/', POW='^'
  };

private:
  Type type;
  SubType subType;
  string name;
  double value;
  Token();

public:
  Type GetType() const;
  SubType GetSubType() const;
  string    GetName() const;
  double GetValue()   const;
  char GetTypeSymbol() const;

  Token(const Token &src);
  Token(const string& name);
  Token(double value);
  Token(SubType otype);
  Token(Type ttype);

  static void DeleteTokenVector(vector<Token*> &tokens);
  static string StringifyTokenVector( const vector<Token*> &tokens);
};


class Tokenizer {
private:
  using TT = Token::Type;
  using OT = Token::SubType;

  vector<Token*> tokens;
  stringstream ssin;
  int errorCount = 0;

  TT curr_ttype;
  OT curr_otype;
  double curr_numberValue;
  string curr_stringValue;

  void error(string msg);
   
  TT get_token();
  void clear();
  void tokenize();

public:
  void Clear();

  ~Tokenizer();

  void AddToken(const string &name);
  void AddToken(double value);
  void AddToken(Token::Type otype);
  void AddToken(Token::SubType ttype);

  Token* GetToken(size_t ind);

  void Tokenize(string str);
  vector<Token*> &GetTokens();
  const vector<Token*> &GetTokens() const;
};


class ITokenStringManipulator {
private:

public:
  virtual void execute(std::vector<Token*> &tokens, size_t pos, size_t len) const = 0;
  virtual std::string getPattern() const = 0;
};

class TokenPostprocessor {
private:
  vector<Token*> &tokens;
public:

  TokenPostprocessor(vector<Token*> &tokens_);
  void ApplyManipulator( const ITokenStringManipulator *replacer);
};