#include <iostream>
#include <math.h>
#include <memory>
#include <set>

class Node {
public:
  virtual std::shared_ptr<Node> derive(const std::string &name = "x") = 0;
  virtual std::shared_ptr<Node> integrate(const std::string &name = "x") = 0;
  virtual std::shared_ptr<Node> evaluate(float value,
                                         const std::string &name = "x") = 0;
  virtual std::shared_ptr<Node> simplify() = 0;
  virtual std::ostream &out(std::ostream &stream) const = 0;
  virtual bool equals(const std::shared_ptr<Node> &other) const {
    return false;
  }
  virtual float toScalar() const { throw std::runtime_error("Not a scalar"); }
  virtual void getVariables(std::set<std::string> &variables) const = 0;
};

inline std::ostream &operator<<(std::ostream &stream, Node &node) {
  return node.out(stream);
}

class NodeRef {
public:
  NodeRef(const std::shared_ptr<Node> &node) : fRef(node) {}
  NodeRef(float value);

  NodeRef derive(const std::string &name = "x") {
    return NodeRef(fRef->derive(name));
  }

  NodeRef integrate(const std::string &name = "x") {
    return NodeRef(fRef->integrate());
  }
  NodeRef integrate(float from, float to, const std::string &name = "x") {
    auto integral = NodeRef(fRef->integrate());
    return integral(to) - integral(from);
  }

  NodeRef evaluate(float value, const std::string &name = "x") {
    return NodeRef(fRef->evaluate(value, name));
  }
  float operator()(float x) { return evaluate(x, "x").toScalar(); }

  float toScalar() const { return fRef->toScalar(); }

  NodeRef simplifyStep() { return NodeRef(fRef->simplify()); }

  NodeRef simplify() {
    auto prev = fRef;
    auto simplified = prev->simplify();
    while (!simplified->equals(prev)) {
      prev = simplified;
      simplified = simplified->simplify();
    }
    return NodeRef(simplified);
  }

  std::set<std::string> getVariables() {
    std::set<std::string> vars;
    fRef->getVariables(vars);
    return vars;
  }

  friend NodeRef operator-(const NodeRef &argument);
  friend NodeRef operator+(const NodeRef &left, const NodeRef &right);
  friend NodeRef operator-(const NodeRef &left, const NodeRef &right);
  friend NodeRef operator*(float left, const NodeRef &right);
  friend NodeRef operator*(const NodeRef &left, const NodeRef &right);
  friend NodeRef operator/(float left, const NodeRef &right);
  friend NodeRef operator/(const NodeRef &left, const NodeRef &right);
  friend NodeRef operator^(const NodeRef &base, float exponent);
  friend NodeRef operator^(float base, const NodeRef &exponent);
  friend NodeRef operator^(const NodeRef &base, const NodeRef &exponent);
  friend std::ostream &operator<<(std::ostream &stream, const NodeRef &node);
  friend bool operator==(const NodeRef &left, const NodeRef &right);

  std::shared_ptr<Node> fRef;
};

NodeRef constant(float value);
NodeRef variable(const std::string &name = "x");
NodeRef vec(std::initializer_list<NodeRef> elements);
NodeRef vec2(const NodeRef &, const NodeRef &);
NodeRef vec3(const NodeRef &, const NodeRef &, const NodeRef &);
NodeRef matrix(std::initializer_list<NodeRef> rows);
NodeRef sqrt(const NodeRef &argument);
NodeRef ln(const NodeRef &argument);
NodeRef cos(const NodeRef &argument);
NodeRef sin(const NodeRef &argument);
NodeRef dot(const NodeRef &left, const NodeRef &right);

float newtonStep(const NodeRef function, float estimate,
                 const std::string &name = "x");
float newton(const NodeRef function, float estimate, int rounds,
             const std::string &name = "x");
NodeRef taylor(NodeRef function, float around, int degree,
               const std::string &name = "x");
NodeRef jacobian(NodeRef vector);
NodeRef hessian(NodeRef function);