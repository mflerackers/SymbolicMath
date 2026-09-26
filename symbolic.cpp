#include "symbolic.h"
#include "symbolic_internal.h"
#include <algorithm>
#include <numeric>

NodeRef::NodeRef(float value) : fRef(newConstant(value)) {}

NodeRef operator-(const NodeRef &argument) {
  return NodeRef(newProduct(newConstant(-1.0f), argument.fRef));
}

NodeRef operator+(const NodeRef &left, const NodeRef &right) {
  return NodeRef(newSum(left.fRef, right.fRef));
}

NodeRef operator-(const NodeRef &left, const NodeRef &right) {
  return NodeRef(newSum(left.fRef, newProduct(newConstant(-1.0f), right.fRef)));
}

NodeRef operator*(float left, const NodeRef &right) {
  return NodeRef(newProduct(newConstant(left), right.fRef));
}

NodeRef operator*(const NodeRef &left, const NodeRef &right) {
  return NodeRef(newProduct(left.fRef, right.fRef));
}

NodeRef operator/(float left, const NodeRef &right) {
  return NodeRef(
      newProduct(newConstant(left), newPower(right.fRef, newConstant(-1.0f))));
}

NodeRef operator/(const NodeRef &left, const NodeRef &right) {
  return NodeRef(
      newProduct(left.fRef, newPower(right.fRef, newConstant(-1.0f))));
}

NodeRef operator^(const NodeRef &base, float exponent) {
  return NodeRef(newPower(base.fRef, newConstant(exponent)));
}

NodeRef operator^(float base, const NodeRef &exponent) {
  return NodeRef(newPower(newConstant(base), exponent.fRef));
}

NodeRef operator^(const NodeRef &base, const NodeRef &exponent) {
  return NodeRef(newPower(base.fRef, exponent.fRef));
}

std::ostream &operator<<(std::ostream &stream, const NodeRef &reference) {
  return reference.fRef->out(stream);
}

bool operator==(const NodeRef &left, const NodeRef &right) {
  return left.fRef->equals(right.fRef);
}

NodeRef constant(float value) { return NodeRef(newConstant(value)); }

NodeRef variable() { return NodeRef(newVariable()); }

NodeRef vec(std::initializer_list<NodeRef> elements) {
  std::vector<std::shared_ptr<Node>> el(elements.size());
  std::transform(elements.begin(), elements.end(), el.begin(),
                 [](auto &e) { return e.fRef; });
  return NodeRef(newVector(std::move(el)));
}

NodeRef vec2(const NodeRef &x, const NodeRef &y) {
  return NodeRef(newVector({x.fRef, y.fRef}));
}

NodeRef vec3(const NodeRef &x, const NodeRef &y, const NodeRef &z) {
  return NodeRef(newVector({x.fRef, y.fRef, z.fRef}));
}

NodeRef matrix(std::initializer_list<NodeRef> rowElements) {
  // Look for the longest row to determine columns
  size_t rows = rowElements.size();
  size_t columns = 0;
  for (auto &rowElement : rowElements) {
    auto vec = toVector(rowElement.fRef);
    if (vec) {
      columns = std::max(vec->getDimension(), columns);
    } else {
      columns = std::max(1ul, columns);
    }
  }
  // Create matrix data by unwrapping references
  std::vector<std::shared_ptr<Node>> el(columns * rows);
  size_t rowIndex = 0;
  auto zero = newConstant(0.0f);
  for (auto &rowElement : rowElements) {
    size_t columnIndex = 0;
    auto vec = toVector(rowElement.fRef);
    if (vec) {
      for (auto &element : vec->elements) {
        el[rowIndex * columns + columnIndex++] = element;
      }
    } else {
      el[rowIndex * columns + columnIndex++] = rowElement.fRef;
    }
    // Fill remaining columns with 0
    for (size_t col = columnIndex; col < columns; col++) {
      el[rowIndex * columns + col] = zero;
    }
    rowIndex++;
  }
  return NodeRef(newMatrix(rows, columns, std::move(el)));
}

NodeRef sqrt(const NodeRef &argument) {
  return NodeRef(newSquareRoot(argument.fRef));
}

NodeRef ln(const NodeRef &argument) {
  return NodeRef(newNaturalLogarithm(argument.fRef));
}

NodeRef cos(const NodeRef &argument) {
  return NodeRef(newCosine(argument.fRef));
}

NodeRef sin(const NodeRef &argument) { return NodeRef(newSine(argument.fRef)); }

NodeRef dot(const NodeRef &left, const NodeRef &right) {
  if (isVector(left.fRef) && isVector(right.fRef)) {
    auto leftVector = toVector(left.fRef);
    auto rightVector = toVector(right.fRef);
    std::shared_ptr<Node> dot = newConstant(0.0f);
    if (leftVector->getDimension() == rightVector->getDimension()) {
      dot = newProduct(leftVector->elements.front(),
                       rightVector->elements.front());
      for (size_t i = 1; i < leftVector->getDimension(); i++) {
        dot = newSum(
            dot, newProduct(leftVector->elements[i], rightVector->elements[i]));
      }
    }
    return NodeRef(dot);
  } else {
    return left * right;
  }
}

// Constant
/*
        The derivative of a constant is zero
*/
std::shared_ptr<Node> Constant::derive() { return newConstant(0.0f); }
/*
                The integral of a constant is the product of the constant and
   the variable
*/
std::shared_ptr<Node> Constant::integrate() {
  return newProduct(shared_from_this(), newVariable());
}

float Constant::evaluate(float x) { return fValue; }

std::shared_ptr<Node> Constant::simplify() { return shared_from_this(); }

std::ostream &Constant::out(std::ostream &stream) const {
  stream << fValue;
  return stream;
}

bool Constant::equals(const std::shared_ptr<Node> &other) const {
  auto constant = toConstant(other);
  return constant && constant->fValue == fValue;
}

// Variable
/*
        The derivative of a variable is one
*/
std::shared_ptr<Node> Variable::derive() { return newConstant(1.0f); }
/*
        The integral of a constant is the product of the constant and
   the variable
*/
std::shared_ptr<Node> Variable::integrate() {
  return newProduct(newPower(newConstant(2), newConstant(-1)),
                    newPower(shared_from_this(), newConstant(2)));
}

float Variable::evaluate(float x) { return x; }

std::shared_ptr<Node> Variable::simplify() { return shared_from_this(); }

std::ostream &Variable::out(std::ostream &stream) const {
  stream << "x";
  return stream;
}

bool Variable::equals(const std::shared_ptr<Node> &other) const {
  auto variable = toVariable(other);
  return variable != nullptr;
}

// Vector
Vector::Vector(std::initializer_list<std::shared_ptr<Node>> nodes)
    : elements(nodes) {}

Vector::Vector(std::vector<std::shared_ptr<Node>> &&nodes) : elements(nodes) {}

std::shared_ptr<Node> Vector::derive() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->derive(); });
  return newVector(std::move(s));
}

std::shared_ptr<Node> Vector::integrate() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->integrate(); });
  return newVector(std::move(s));
}

float Vector::evaluate(float x) { return 0.0f; }

std::shared_ptr<Node> Vector::simplify() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->simplify(); });
  return newVector(std::move(s));
}

std::ostream &Vector::out(std::ostream &stream) const {
  bool first = true;
  stream << "[";
  for (auto &element : elements) {
    if (!first) {
      stream << ", ";
    } else {
      first = false;
    }
    element->out(stream);
  }
  stream << "]";
  return stream;
}

bool Vector::equals(const std::shared_ptr<Node> &other) const {
  auto vector = toVector(other);
  return vector && vector->getDimension() == getDimension() &&
         std::equal(elements.begin(), elements.end(), vector->elements.begin(),
                    [](auto &a, auto &b) { return a->equals(b); });
}

// Matrix
Matrix::Matrix(size_t rows, size_t columns,
               std::initializer_list<std::shared_ptr<Node>> nodes)
    : rows(rows), columns(columns), elements(nodes) {}

Matrix::Matrix(size_t rows, size_t columns,
               std::vector<std::shared_ptr<Node>> &&nodes)
    : rows(rows), columns(columns), elements(nodes) {}

std::shared_ptr<Node> Matrix::derive() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->derive(); });
  return newMatrix(rows, columns, std::move(s));
}

std::shared_ptr<Node> Matrix::integrate() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->integrate(); });
  return newMatrix(rows, columns, std::move(s));
}

float Matrix::evaluate(float x) { return 0.0f; }

std::shared_ptr<Node> Matrix::simplify() {
  std::vector<std::shared_ptr<Node>> s;
  s.resize(elements.size());
  std::transform(elements.begin(), elements.end(), s.begin(),
                 [](auto &e) { return e->simplify(); });
  return newMatrix(rows, columns, std::move(s));
}

std::ostream &Matrix::out(std::ostream &stream) const {
  stream << "[";
  auto it = elements.begin();
  for (size_t r = 0; r < rows; r++) {
    if (r > 0) {
      stream << ", ";
    }
    stream << "[";
    for (size_t c = 0; c < columns; c++) {
      if (c > 0) {
        stream << ", ";
      }
      (*it++)->out(stream);
    }
    stream << "]";
  }
  stream << "]";
  return stream;
}

bool Matrix::equals(const std::shared_ptr<Node> &other) const {
  auto matrix = toMatrix(other);
  return matrix && matrix->getRows() == getRows() &&
         matrix->getColumns() == getColumns() &&
         std::equal(elements.begin(), elements.end(), matrix->elements.begin(),
                    [](auto &a, auto &b) { return a->equals(b); });
}

std::shared_ptr<Vector> Matrix::getRow(size_t index) {
  std::vector<std::shared_ptr<Node>> e(elements.begin() + index * rows,
                                       elements.begin() + index * rows +
                                           columns);
  return newVector(std::move(e));
}

std::shared_ptr<Vector> Matrix::getColumn(size_t index) {
  std::vector<std::shared_ptr<Node>> e(rows);
  for (size_t i = 0; i < rows; i++) {
    e[i] = elements[i * columns + index];
  }
  return newVector(std::move(e));
}

// Sum
/*
        The derivative of a sum is the sum of the derivatives
        (a + b)' = a' + b'
*/
std::shared_ptr<Node> Sum::derive() {
  return newSum(fLeft->derive(), fRight->derive());
}

std::shared_ptr<Node> Sum::integrate() {
  return newSum(fLeft->integrate(), fRight->integrate());
}

float Sum::evaluate(float x) {
  return fLeft->evaluate(x) + fRight->evaluate(x);
}

std::shared_ptr<Node> Sum::simplify() {
  if (isConstant(fRight)) {
    auto right = toConstant(fRight);
    if (isConstant(fLeft)) {
      auto left = toConstant(fLeft);
      // n + m = p
      return newConstant(left->fValue + right->fValue);
    } else {
      return newSum(fRight->simplify(), fLeft->simplify());
    }
  }
  if (isConstant(fLeft)) {
    auto left = toConstant(fLeft);
    if (left->fValue == 0.0f) {
      // 0 + n = n
      return fRight->simplify();
    }
  }
  if (fLeft->equals(fRight)) {
    // x + x = 2 * x
    return newProduct(newConstant(2.0f), fLeft->simplify());
  }
  if (isProduct(fLeft) && isProduct(fRight)) {
    auto left = toProduct(fLeft);
    auto right = toProduct(fRight);
    if (isConstant(left->fLeft) && isConstant(right->fLeft) &&
        left->fRight->equals(right->fRight)) {
      // (n * x) + (m * x) = (n + m) * x
      return newProduct(newConstant(toConstant(left->fLeft)->fValue +
                                    toConstant(right->fLeft)->fValue),
                        left->fRight);
    }
  }
  if (isProduct(fLeft)) {
    auto left = toProduct(fLeft);
    // (n * x) + x = (n + 1) * x
    if (isConstant(left->fLeft) && left->fRight->equals(fRight)) {
      return newProduct(newConstant(toConstant(left->fLeft)->fValue + 1),
                        fRight);
    }
  }
  if (isProduct(fRight)) {
    // Hack for x + (-1 * x)
    auto right = toProduct(fRight);
    if (isConstant(right->fLeft)) {
      if (toConstant(right->fLeft)->fValue == -1.0f) {
        if (fLeft->equals(right->fRight)) {
          return newConstant(0.0f);
        }
      }
    }
  }
  if (isVector(fLeft) && isVector(fRight)) {
    auto left = toVector(fLeft);
    auto right = toVector(fRight);
    if (left->getDimension() == right->getDimension()) {
      std::vector<std::shared_ptr<Node>> s;
      s.resize(left->getDimension());
      std::transform(left->elements.begin(), left->elements.end(),
                     right->elements.begin(), s.begin(), [](auto &a, auto &b) {
                       return newSum(a->simplify(), b->simplify());
                     });
      return newVector(std::move(s));
    }
  }
  if (isMatrix(fLeft) && isMatrix(fRight)) {
    auto left = toMatrix(fLeft);
    auto right = toMatrix(fRight);
    if (left->getRows() == right->getRows() &&
        left->getColumns() == right->getColumns()) {
      std::vector<std::shared_ptr<Node>> s;
      s.resize(left->getRows() * left->getColumns());
      std::transform(left->elements.begin(), left->elements.end(),
                     right->elements.begin(), s.begin(), [](auto &a, auto &b) {
                       return newSum(a->simplify(), b->simplify());
                     });
      return newMatrix(left->getRows(), left->getColumns(), std::move(s));
    }
  }
  if (isPower(fLeft) && isPower(fRight)) {
    auto leftPower = toPower(fLeft);
    auto rightPower = toPower(fRight);
    if (isConstant(leftPower->fExponent) && isConstant(rightPower->fExponent)) {
      auto leftExponent = toConstant(leftPower->fExponent)->fValue;
      auto rightExponent = toConstant(rightPower->fExponent)->fValue;
      if (leftExponent == 2.0f && rightExponent == 2.0f) {
        if (isCosine(leftPower->fBase) && isSine(rightPower->fBase)) {
          auto cos = toCosine(leftPower->fBase);
          auto sin = toSine(rightPower->fBase);
          if (cos->fArgument->equals(sin->fArgument)) {
            return newConstant(1.0f);
          }
        } else if (isSine(leftPower->fBase) && isCosine(rightPower->fBase)) {
          auto cos = toCosine(rightPower->fBase);
          auto sin = toSine(leftPower->fBase);
          if (cos->fArgument->equals(sin->fArgument)) {
            return newConstant(1.0f);
          }
        }
      }
    }
  }
  return newSum(fLeft->simplify(), fRight->simplify());
}

std::ostream &Sum::out(std::ostream &stream) const {
  stream << "(" << *fLeft << " + " << *fRight << ")";
  return stream;
}

bool Sum::equals(const std::shared_ptr<Node> &other) const {
  auto sum = toSum(other);
  return sum &&
         ((sum->fLeft->equals(fLeft) &&
           sum->fRight->equals(fRight))); //||
                                          //(sum->fLeft->equals(fRight) &&
                                          // sum->fRight->equals(fLeft)));
}

// Product
/*
        The derivative of a product is
        (a * b)' = a' * b + a * b'
*/
std::shared_ptr<Node> Product::derive() {
  return newSum(newProduct(fLeft->derive(), fRight),
                newProduct(fLeft, fRight->derive()));
}
/*
                The integral of a product is
                ∫(a * b) = a∫b - ∫(a'∫b)
*/
std::shared_ptr<Node> Product::integrate() {
  /*std::cout << "product\n";
  fRight->integrate()->simplify()->out(std::cout);
  std::cout << "\n";
  fLeft->derive()->simplify()->out(std::cout);
  std::cout << "\n";*/
  return newSum(newProduct(fLeft, fRight->integrate()->simplify()),
                newProduct(newConstant(-1),
                           newProduct(fLeft->derive()->simplify(),
                                      fRight->integrate()->simplify())
                               ->simplify()
                               ->integrate()))
      ->simplify();
}

float Product::evaluate(float x) {
  return fLeft->evaluate(x) * fRight->evaluate(x);
}

std::shared_ptr<Node> Product::simplify() {
  // Switch so the constant is always left
  if (isConstant(fRight)) {
    auto right = toConstant(fRight);
    if (isConstant(fLeft)) {
      auto left = toConstant(fLeft);
      // n * m = p
      return newConstant(left->fValue * right->fValue);
    } else {
      return newProduct(fRight, fLeft);
    }
  }
  if (isConstant(fLeft)) {
    auto left = toConstant(fLeft);
    // 0 * n = 0
    if (left->fValue == 0.0f) {
      return newConstant(0.0f);
    }
    // 1 * n = n
    else if (left->fValue == 1.0f) {
      return fRight->simplify();
    } else if (isProduct(fRight)) {
      auto product = toProduct(fRight);
      // n * (m * x) = n*m * x
      if (isConstant(product->fLeft)) {
        auto pleft = toConstant(product->fLeft);
        return newProduct(newConstant(left->fValue * pleft->fValue),
                          product->fRight);
      }
    }
  }
  if (fLeft->equals(fRight)) {
    // (x * x) = x ^ 2
    return newPower(fLeft, newConstant(2.0f));
  }
  if (isProduct(fLeft) && isProduct(fRight)) {
    auto productLeft = toProduct(fLeft);
    auto productRight = toProduct(fRight);
    // (n * x) * (m * y) = (n * m) * (x * y)
    if (isConstant(productLeft->fLeft) && isConstant(productRight->fLeft)) {
      auto left = toConstant(productLeft->fLeft);
      auto right = toConstant(productRight->fLeft);
      return newProduct(newConstant(left->fValue * right->fValue),
                        newProduct(productLeft->fRight, productRight->fRight));
    }
  }
  if (isProduct(fLeft)) {
    auto productLeft = toProduct(fLeft);
    // (n * x) * x = n * (x ^ 2)
    if (productLeft->fRight->equals(fRight)) {
      return newProduct(productLeft->fLeft,
                        newPower(fRight, newConstant(2.0f)));
    }
    if (isConstant(productLeft->fLeft)) {
      // (n * x) * y = n * (x * y)
      return newProduct(productLeft->fLeft,
                        newProduct(productLeft->fRight, fRight));
    }
  }
  if (isProduct(fRight)) {
    auto productRight = toProduct(fRight);
    // x * (n * x) = n * (x ^ 2)
    if (productRight->fRight->equals(fLeft)) {
      return newProduct(productRight->fLeft,
                        newPower(fLeft, newConstant(2.0f)));
    }
    if (isConstant(productRight->fLeft)) {
      // (n * x) * y = n * (x * y)
      return newProduct(productRight->fLeft,
                        newProduct(fLeft, productRight->fRight));
    }
  }
  if (isPower(fLeft) && isPower(fRight)) {
    auto left = toPower(fLeft);
    auto right = toPower(fRight);
    if (left->fBase->equals(right->fBase)) {
      // (x ^ n) * (x ^ m) = x ^ (n + m)
      return newPower(left->fBase, newSum(left->fExponent, right->fExponent));
    }
  }
  if (isPower(fRight)) {
    auto power = toPower(fRight);
    // x * (x ^ n) = x ^ (n + 1)
    if (power->fBase->equals(fLeft)) {
      return newPower(fLeft, newSum(newConstant(1), power->fExponent));
    }
  }
  if (isVector(fRight)) {
    auto vector = toVector(fRight);
    if (isConstant(fLeft) || isVariable(fLeft)) {
      std::vector<std::shared_ptr<Node>> s;
      s.resize(vector->getDimension());
      std::transform(vector->elements.begin(), vector->elements.end(),
                     s.begin(), [this](auto &e) {
                       return newProduct(fLeft->simplify(), e->simplify());
                     });
      return newVector(std::move(s));
    }
  }
  if (isMatrix(fRight)) {
    auto right = toMatrix(fRight);
    if (isMatrix(fLeft)) {
      auto left = toMatrix(fLeft);
      std::vector<std::shared_ptr<Node>> s;
      const size_t rows = left->getRows();
      const size_t columns = left->getColumns();
      s.resize(rows * columns);
      auto i = s.begin();
      for (size_t r = 0; r < left->getRows(); r++) {
        for (size_t c = 0; c < left->getColumns(); c++) {
          (*i++) = newDot(left->getRow(r), right->getColumn(c));
        }
      }
      return newMatrix(left->getRows(), left->getColumns(), std::move(s));
    }
    if (isConstant(fLeft) || isVariable(fLeft)) {
      std::vector<std::shared_ptr<Node>> s;
      s.resize(right->getRows() * right->getColumns());
      std::transform(right->elements.begin(), right->elements.end(), s.begin(),
                     [this](auto &e) {
                       return newProduct(fLeft->simplify(), e->simplify());
                     });
      return newMatrix(right->getRows(), right->getColumns(), std::move(s));
    }
  }
  return newProduct(fLeft->simplify(), fRight->simplify());
}

std::ostream &Product::out(std::ostream &stream) const {
  if (isConstant(fLeft) && isVariable(fRight)) {
    stream << "(" << *fLeft << *fRight << ")";
  } else {
    stream << "(" << *fLeft << " * " << *fRight << ")";
  }
  return stream;
}

bool Product::equals(const std::shared_ptr<Node> &other) const {
  auto product = toProduct(other);
  return product && ((product->fLeft->equals(fLeft) &&
                      product->fRight->equals(fRight))); /*||
(product->fLeft->equals(fRight) && product->fRight->equals(fLeft)));*/
}

// Function
std::shared_ptr<Node> Function::derive() {
  return newProduct(deriveFunction(fArgument), fArgument->derive());
}

std::shared_ptr<Node> Function::integrate() { return nullptr; }

// Power
/*
        The derivative of a power is
        (b ^ e)' = (b ^ e) * (e' * ln(b) + e * (ln(b))')
        If e is a constant this becomes
        (b ^ c)' = (b ^ c) * (0 * ln(b) + c * 1 / b) = c * b ^ (c - 1)
        If b is a constant and e is x this becomes
        (c ^ x)' = (c ^ e) * (1 * ln(c) + x * 0) = ln(c) * (c ^ x)
*/
std::shared_ptr<Node> Power::derive() {
  // Specialized for constant exponent since simplification is still lacking
  if (isConstant(fExponent)) {
    auto exponent = toConstant(fExponent)->fValue;
    return newProduct(newProduct(newConstant(exponent),
                                 newPower(fBase, newConstant(exponent - 1.0f))),
                      fBase->derive());
  }
  // Specialized for constant base since simplification is still lacking
  else if (isConstant(fBase)) {
    auto base = toConstant(fBase)->fValue;
    return newProduct(
        shared_from_this(),
        newSum(newProduct(fExponent->derive(), newNaturalLogarithm(fBase)),
               newProduct(fExponent, newNaturalLogarithm(fBase)->derive())));
  }
  return newProduct(
      shared_from_this(),
      newSum(newProduct(fExponent->derive(), newNaturalLogarithm(fBase)),
             newProduct(fExponent, newNaturalLogarithm(fBase)->derive())));
}

std::shared_ptr<Node> Power::integrate() {
  if (isConstant(fExponent)) {
    auto exponent = toConstant(fExponent)->fValue;
    return newProduct(
        newProduct(newPower(newConstant(exponent + 1.0f), newConstant(-1)),
                   newPower(fBase, newConstant(exponent + 1.0f))),
        newPower(fBase->derive(), newConstant(-1)));
  }
  std::cout << "Power::integrate() NYI\n";
  return nullptr;
}

float Power::evaluate(float x) {
  return powf(fBase->evaluate(x), fExponent->evaluate(x));
}

bool isEven(float value) {
  return (value == int(value) && ((int(value) & 1) == 0));
}

std::shared_ptr<Node> Power::simplify() {
  if (isConstant(fExponent)) {
    auto exponent = toConstant(fExponent);
    if (exponent->fValue == 0) {
      // x ^ 0 = 1
      return newConstant(1);
    } else if (exponent->fValue == 1) {
      // x ^ 1 = x
      return fBase->simplify();
    } else if (isConstant(fBase)) {
      // n ^ m = p
      auto base = toConstant(fBase);
      return newConstant(powf(base->fValue, exponent->fValue));
    } else if (isEven(exponent->fValue) && isProduct(fBase)) {
      auto product = toProduct(fBase);
      if (isConstant(product->fLeft)) {
        auto left = toConstant(product->fLeft);
        if (left->fValue < 0.0f) {
          return newPower(
              newProduct(newConstant(-left->fValue), product->fRight),
              fExponent);
        }
      }
    }
  }
  if (isPower(fBase)) {
    auto power = toPower(fBase);
    return newPower(power->fBase, newProduct(power->fExponent, fExponent));
  }
  return newPower(fBase->simplify(), fExponent->simplify());
  // return shared_from_this();
}

std::ostream &Power::out(std::ostream &stream) const {
  /*if (isConstant(fExponent)) {
          float exponent = dynamic_cast<Constant*>(fExponent.get())->fValue;
          float absExponent = std::abs(exponent);
          if (exponent < 0) {
                  stream << "1/(";
          }
          if (absExponent == 0.5f) {
                  stream << "√(" << *fBase << ")";
          }
          else if (absExponent == 1.0f) {
                  stream << *fBase;
          }
          else if (absExponent == 2.0f) {
                  stream << "(" << *fBase << ")²";
          }
          else if (absExponent == 3.0f) {
                  stream << "(" << *fBase << ")³";
          }
          if (exponent < 0) {
                  stream << ")";
          }
  }
  else {*/
  stream << "(" << *fBase << " ^ " << *fExponent << ")";
  //}
  return stream;
}

bool Power::equals(const std::shared_ptr<Node> &other) const {
  auto power = toPower(other);
  return power && power->fBase->equals(fBase) &&
         power->fExponent->equals(fExponent);
}

std::shared_ptr<Node>
NaturalLogarithm::deriveFunction(const std::shared_ptr<Node> &argument) {
  return newPower(argument, newConstant(-1.0f));
}

std::shared_ptr<Node>
NaturalLogarithm::integrateFunction(const std::shared_ptr<Node> &argument) {
  return nullptr;
}

float NaturalLogarithm::evaluate(float x) { return log(x); }

std::shared_ptr<Node> NaturalLogarithm::simplify() {
  return shared_from_this();
}

std::ostream &NaturalLogarithm::out(std::ostream &stream) const {
  stream << "ln(" << *fArgument << ")";
  return stream;
}

bool NaturalLogarithm::equals(const std::shared_ptr<Node> &other) const {
  return equalsHelper<NaturalLogarithm>(other);
}

// Cosine
std::shared_ptr<Node>
Cosine::deriveFunction(const std::shared_ptr<Node> &argument) {
  return newProduct(newConstant(-1.0f), newSine(argument));
}

std::shared_ptr<Node>
Cosine::integrateFunction(const std::shared_ptr<Node> &argument) {
  return newSine(argument);
}

float Cosine::evaluate(float x) { return cosf(fArgument->evaluate(x)); }

std::shared_ptr<Node> Cosine::simplify() { return shared_from_this(); }

std::ostream &Cosine::out(std::ostream &stream) const {
  stream << "cos(" << *fArgument << ")";
  return stream;
}

bool Cosine::equals(const std::shared_ptr<Node> &other) const {
  return equalsHelper<Cosine>(other);
}

// Sine
std::shared_ptr<Node>
Sine::deriveFunction(const std::shared_ptr<Node> &argument) {
  return newCosine(argument);
}

std::shared_ptr<Node>
Sine::integrateFunction(const std::shared_ptr<Node> &argument) {
  return newProduct(newConstant(-1.0f), newCosine(argument));
}

float Sine::evaluate(float x) { return sinf(fArgument->evaluate(x)); }

std::shared_ptr<Node> Sine::simplify() { return shared_from_this(); }

std::ostream &Sine::out(std::ostream &stream) const {
  stream << "sin(" << *fArgument << ")";
  return stream;
}

bool Sine::equals(const std::shared_ptr<Node> &other) const {
  return equalsHelper<Sine>(other);
}

std::shared_ptr<Node> newDot(const std::shared_ptr<Node> &left,
                             const std::shared_ptr<Node> &right) {
  if (isVector(left) && isVector(right)) {
    auto leftVector = toVector(left);
    auto rightVector = toVector(right);
    std::shared_ptr<Node> dot = newConstant(0.0f);
    if (leftVector->getDimension() == rightVector->getDimension()) {
      dot = newProduct(leftVector->elements.front(),
                       rightVector->elements.front());
      for (size_t i = 1; i < leftVector->getDimension(); i++) {
        dot = newSum(
            dot, newProduct(leftVector->elements[i], rightVector->elements[i]));
      }
    }
    return dot;
  } else {
    return newProduct(left, right);
  }
}