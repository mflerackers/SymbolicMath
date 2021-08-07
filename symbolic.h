#include <iostream>
#include <memory>
#include <math.h>

class Node {
public:
	virtual std::shared_ptr<Node> derive() = 0;
	virtual float evaluate(float x) = 0;
	virtual std::shared_ptr<Node> simplify() = 0;
	virtual std::ostream &out(std::ostream &stream) const = 0;
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

	NodeRef simplify() {
		return NodeRef(fRef->simplify());
	}

	friend NodeRef operator+(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator-(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator*(float left, const NodeRef &right);
	friend NodeRef operator*(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator/(float left, const NodeRef &right);
	friend NodeRef operator/(const NodeRef &left, const NodeRef &right);
	friend NodeRef operator^(const NodeRef &base, float exponent);
	friend std::ostream &operator<< (std::ostream &stream, const NodeRef &node);

	std::shared_ptr<Node> fRef;
};

NodeRef variable();
NodeRef squareroot(const NodeRef &argument);
NodeRef cosine(const NodeRef &argument);
NodeRef sine(const NodeRef &argument);