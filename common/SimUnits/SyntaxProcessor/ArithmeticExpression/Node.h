#pragma once
#include <string>
#include <vector>
#include <functional>
#include <math.h>

class Node
{
public:
  enum class Type
  {
    NONE, SYMBOL, NUMBER, ADD, SUB, MUL, DIV, POW
  };
  enum class StringType {
    DEFAULT, RAW
  };
protected:
  Node *pLeft, *pRight, *pParent;
  Type type;
  std::string name;

  Node();

public:
  ~Node();
  void SetParent(Node* pParent);
  virtual double CalculateValue()=0;
  std::string GetName() const;
  Node* AddLeft(Node* pNode);
  Node* AddRight(Node* pNode);
  const Node *GetLeft() const;
  const Node *GetRight() const;
  const Type GetType() const;
  const Node *GetRoot() const;
  using Action = std::function<void(Node*)>;
  virtual void PreTraverse(Action action);
  virtual void PostTraverse(Action action);
  virtual void InTraverse(Action action);
  static Node* Create(Type type);
  static Node* Create(double value);
  static Node* Create(std::string name);
};


class NNode : public Node {
private:
  double value;
  NNode();
public:
  NNode(double value);
  double CalculateValue() override;
};

class AddNode : public Node {
public:
  AddNode();
  double CalculateValue();
};

class SubNode : public Node {
public:
  SubNode();
  double CalculateValue() override;
};

class MulNode : public Node {
public:
  MulNode();
  double CalculateValue() override;
};

class DivNode : public Node {
public:
  DivNode();
  double CalculateValue() override;
};

class PowNode : public Node {
public:
  PowNode();
  double CalculateValue() override;
};

class SNode : public Node {
private:
  SNode();
public:
  SNode(std::string name);
  double CalculateValue();
};
