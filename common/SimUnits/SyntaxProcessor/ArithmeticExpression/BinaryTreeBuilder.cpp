#include "BinaryTreeBuilder.h"


BinaryTreeBuilder::~BinaryTreeBuilder()
{
}


BinaryTreeBuilder::BinaryTreeBuilder(const vector<Token*> &tokens) : tokens(tokens), tokensCount(tokens.size()) {
  bracketCount.assign(tokens.size(), 0);
}

Node* BinaryTreeBuilder::BuildTree() {
  setBracketCount();
  return getNode(0, tokensCount - 1);
}

using TT = Token::Type;
using ST = Token::SubType;

Node* BinaryTreeBuilder::getNode(size_t start, size_t end) {
  size_t len = end - start;
  if ((int)end < (int)start) {
    return nullptr;
  }
  else {
    if (tokens[start]->GetType() == TT::L)
    {
      size_t skip = skipBracket(start);
      if (skip == end) {
        return getNode(start + 1, end - 1);
      }
    }
    size_t nodeTokenPos = getNodeTokenPos(start, end);
    if (nodeTokenPos == -1) {
      return nullptr;
    }

    Node *node = createNode(tokens[nodeTokenPos]);

    if (tokens[nodeTokenPos]->GetType() == TT::O) {
      node->AddLeft(getNode(start, nodeTokenPos - 1));
      node->AddRight(getNode(nodeTokenPos + 1, end));
    }

    return node;
  }
}

void BinaryTreeBuilder::setBracketCount() {
  size_t counter = 0;
  for (size_t i = 0; i < tokensCount; i++) {
    counter += tokens[i]->GetType() == TT::L ? 1 : (tokens[i]->GetType() == TT::R ? -1 : 0);
    bracketCount[i] = counter;
  }
}

size_t BinaryTreeBuilder::skipBracket(size_t start) const {
  size_t counter = bracketCount[start];
  while (++start<tokensCount) {
    if (bracketCount[start] < counter) {
      break;
    }
  }
  return start;
}

const size_t BinaryTreeBuilder::getNodeTokenPos(size_t start, size_t end) const {
  if (start<0 || end >= tokensCount || (end - start) < 0) {
    return -1;
  }
  else {
    size_t mulDivPos = -1;
    size_t addSubPos = -1;
    for (size_t i = start; i < end; i++) {
      auto type = tokens[i]->GetType();
      if (type == TT::L) {
        size_t skip = skipBracket(i);
        if (i == start && skip == end) {
          return getNodeTokenPos(start + 1, end - 1);
        }
        else if (skip >= end)
        {
          break;
        }
        else {
          i = skip;
        }
      }

      auto subtype = tokens[i]->GetSubType();
      if (type == TT::O && (subtype == ST::ADD || subtype == ST::SUB)) {
        addSubPos = i;
        break;
      }

      if (type == TT::O && (subtype == ST::MUL || subtype == ST::DIV || subtype == ST::POW)) {
        mulDivPos = i;
      }
    }

    return addSubPos != -1 ? addSubPos : (mulDivPos != -1 ? mulDivPos : start);
  }
}

Node* BinaryTreeBuilder::createNode(const Token *token) const {
  switch (token->GetType()) {
  case TT::O:
  {
    switch (token->GetSubType()) {
    case ST::ADD: return Node::Create(Node::Type::ADD);
    case ST::SUB: return Node::Create(Node::Type::SUB);
    case ST::MUL: return Node::Create(Node::Type::MUL);
    case ST::DIV: return Node::Create(Node::Type::DIV);
    default: return nullptr;
    }
  }
  case TT::N: {
    return Node::Create(token->GetValue());
  }
  case TT::S: {
    return Node::Create(token->GetName());
  }

  default: return nullptr;
  }
}