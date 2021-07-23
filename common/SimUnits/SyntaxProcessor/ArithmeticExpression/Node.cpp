#include "Node.h"
#include <string.h>

Node* Node::Create(Type type) {
  switch (type) {
  case Type::ADD: return new AddNode;
  case Type::SUB: return new SubNode;
  case Type::MUL: return new MulNode;
  case Type::DIV: return new DivNode;
  case Type::POW: return new PowNode;
  }
  return nullptr;
}

Node* Node::Create(double value) {
  return new NNode(value);
}

Node* Node::Create(std::string name) {
  return new SNode(name);
}


Node::Node() : pLeft(nullptr), pRight(nullptr), pParent(nullptr) {

}

Node::~Node() {
  PostTraverse([](Node* pNode) { delete pNode; });
}

std::string Node::GetName() const {
  return name;
}

void Node::SetParent(Node* pParent) {
  this->pParent = pParent;
}


Node*Node::AddLeft(Node* pNode) {
  if (this->pLeft == nullptr &&pNode != nullptr) {
    this->pLeft = pNode;
    this->pLeft->pParent = this;
  }
  return this->pLeft;
}

Node* Node::AddRight(Node* pNode) {
  if (this->pRight == nullptr && pNode != nullptr) {
    this->pRight = pNode;
    this->pRight->pParent = this;
  }
  return this->pRight;
}

const Node *Node::GetLeft() const {
  return pLeft;
}

const Node *Node::GetRight() const {
  return pRight;
}

const Node::Type Node::GetType() const {
  return type;
}

const Node *Node::GetRoot() const {
  return pParent == nullptr ? this : pParent->GetRoot();
}


void Node::PreTraverse(Action action) {
  action(this);
  if (pLeft) {
    pLeft->PreTraverse(action);
  }
  if (pRight) {
    pRight->PreTraverse(action);
  }
}

void Node::PostTraverse(Action action) {
  if (pLeft) {
    pLeft->PostTraverse(action);
  }
  if (pRight) {
    pRight->PostTraverse(action);
  }
  action(this);
}

void Node::InTraverse(Action action) {
  if (pLeft) {
    pLeft->InTraverse(action);
  }
  action(this);
  if (pRight) {
    pRight->InTraverse(action);
  }
}

NNode::NNode(double value) :Node() {
  type = Type::NUMBER;
  this->value = value;
}
double NNode::CalculateValue() {
  return value;
}


AddNode::AddNode() :Node::Node() { 
  type = Type::ADD; 
}
double AddNode::CalculateValue() {
  return (pLeft ? pLeft->CalculateValue() : 0.0) + (pRight ? pRight->CalculateValue() : 0.0);
}


SubNode::SubNode() :Node() { type = Type::SUB; }
double SubNode::CalculateValue()  {
  return (pLeft ? pLeft->CalculateValue() : 0.0) - (pRight ? pRight->CalculateValue() : 0.0);
}


MulNode::MulNode() :Node() { type = Type::MUL; }
double MulNode::CalculateValue()  {
  return (pLeft ? pLeft->CalculateValue() : 1.0) * (pRight ? pRight->CalculateValue() : 1.0);
}



DivNode::DivNode() :Node() { type = Type::DIV; }
double DivNode::CalculateValue()  {
  return (pLeft ? pLeft->CalculateValue() : 1.0) / (pRight ? pRight->CalculateValue() : 1.0);
}


PowNode::PowNode() :Node() { type = Type::POW; }
double PowNode::CalculateValue()  {
  return pow((pLeft ? pLeft->CalculateValue() : 1.0), (pRight ? pRight->CalculateValue() : 1.0));
}

SNode::SNode(std::string name) :Node() {
  type = Type::SYMBOL;
  this->name = name;
}

double SNode::CalculateValue() {
  return NAN;
}

