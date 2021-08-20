#include <iostream>
#include <memory>
#include <math.h>

class Node {
public:
	virtual std::shared_ptr<Node> derive() = 0;
	virtual float evaluate(float x) = 0;
	virtual std::shared_ptr<Node> simplify() = 0;
	virtual std::ostream &out(std::ostream &stream) const = 0;
	virtual bool equals(const std::shared_ptr<Node> &other) const {return false;}
};

inline std::ostream &operator<< (std::ostream &stream, Node &node) {
	return node.out(stream);
}

class NodeRef {
public:
	NodeRef(const std::shared_ptr<Node> &node) :
	fRef(node) {}

	NodeRef derive() {
		return NodeRef(fRef->derive());
	}

	float evaluate(float x) {
		return fRef->evaluate(x);
	}

	NodeRef simplifyStep() {
		return NodeRef(fRef->simplify());
	}

	NodeRef simplify() {
		auto prev = fRef;
		auto simplified = prev->simplify();
		while (!simplified->equals(prev)) {
			prev = simplified;
			simplified = simplified->simplify();
		}
		return NodeRef(simplified);
	}

	friend NodeRef operator+(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator-(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator*(float left, const NodeRef &right);
	friend NodeRef operator*(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator/(float left, const NodeRef &right);
	friend NodeRef operator/(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator^(const NodeRef &base, float exponent);
	friend NodeRef operator^(float base, const NodeRef &exponent);
	friend NodeRef operator^(const NodeRef &base, const NodeRef &exponent);
	friend std::ostream &operator<< (std::ostream &stream, const NodeRef &node);
	friend bool operator==(const NodeRef &left, const NodeRef &right);

	std::shared_ptr<Node> fRef;
};

NodeRef constant(float value);
NodeRef variable();
NodeRef vec2(const NodeRef&, const NodeRef&);
NodeRef sqrt(const NodeRef &argument);
NodeRef ln(const NodeRef &argument);
NodeRef cos(const NodeRef &argument);
NodeRef sin(const NodeRef &argument);
NodeRef dot(const NodeRef &left, const NodeRef &right);