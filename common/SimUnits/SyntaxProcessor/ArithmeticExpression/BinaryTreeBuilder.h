#pragma once
#include "Tokenizer.h"
#include "Node.h"

class BinaryTreeBuilder
{
private:
  const vector<Token*> &tokens;
  vector<size_t> bracketCount;
  const size_t tokensCount;

public:
  BinaryTreeBuilder(const vector<Token*> &tokens);
  ~BinaryTreeBuilder();
  Node* BuildTree();
  
private:
  using TT = Token::Type;
  using ST = Token::SubType;

  Node* getNode(size_t start, size_t end);
  void setBracketCount();
  size_t skipBracket(size_t start) const; 
  const size_t getNodeTokenPos(size_t start, size_t end) const; 
  Node* createNode(const Token *token) const;

};

