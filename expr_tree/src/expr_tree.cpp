#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

class ValueExpr : public Expression {
public:
  ValueExpr(int value) : value(value) {}

  virtual int Evaluate() const override {
    return value;
  }

  virtual std::string ToString() const override {
    return std::to_string(value);
  }

private:
  int value;
};

class SumExpr : public Expression {
public:
  SumExpr(ExpressionPtr& left, ExpressionPtr& right) : left(move(left))
                                                     , right(move(right))
  {}

  virtual int Evaluate() const override {
    return left->Evaluate() + right->Evaluate();
  }

  virtual std::string ToString() const override {
    return "(" + left->ToString() + ")" + "+" + "(" + right->ToString() + ")";
  }

private:
  ExpressionPtr left;
  ExpressionPtr right;
};

class ProductExpr : public Expression {
public:
  ProductExpr(ExpressionPtr& left, ExpressionPtr& right) : left(move(left))
                                                       , right(move(right))
  {}

  virtual int Evaluate() const override {
    return left->Evaluate() * right->Evaluate();
  }

  virtual std::string ToString() const override {
    return "(" + left->ToString() + ")" + "*" + "(" + right->ToString() + ")";
  }

private:
  ExpressionPtr left;
  ExpressionPtr right;
};

ExpressionPtr Value(int value) {
  return make_unique<ValueExpr>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<SumExpr>(left, right);
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<ProductExpr>(left, right);
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}
