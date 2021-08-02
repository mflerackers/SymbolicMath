#include <iostream>
#include <math.h>

class Node {
public:
	virtual Node *derive() = 0;
	virtual float evaluate(float x) = 0;
	virtual Node *simplify() = 0;
	virtual std::ostream &out(std::ostream &stream) const = 0;
};

inline std::ostream &operator<< (std::ostream &stream, Node &node) {
	return node.out(stream);
}

class NodeRef {
public:
	NodeRef(Node *node) :
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

	Node *fRef{nullptr};
};

class Constant : public Node {
public:
	Constant(float value) :
	fValue(value) {}

 	Node *derive() override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	float fValue{0.0f};
};

inline bool isConstant(Node *node) {
	return dynamic_cast<Constant*>(node) != nullptr;
}

class Variable : public Node {
public:
	Variable() {}

	Node *derive() override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};

inline bool isVariable(Node *node) {
	return dynamic_cast<Variable*>(node) != nullptr;
}

class Sum : public Node {
public:
	Sum(Node *left, Node *right) :
	fLeft(left),
	fRight(right) {}

	Node *derive() override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	Node *fLeft{nullptr};
	Node *fRight{nullptr};
};

inline bool isSum(Node *node) {
	return dynamic_cast<Sum*>(node) != nullptr;
}

class Product : public Node {
public:
	Product(Node *left, Node *right) :
	fLeft(left),
	fRight(right) {}

	Node *derive() override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	Node *fLeft{nullptr};
	Node *fRight{nullptr};
};

inline bool isProduct(Node *node) {
	return dynamic_cast<Product*>(node) != nullptr;
}

class Function : public Node {
public:
	Function(Node *argument) :
	fArgument(argument) {}

	Node *derive() override;
	virtual Node *deriveFunction(Node *argument) = 0;

	Node *fArgument{nullptr};
};

inline bool isFunction(Node *node) {
	return dynamic_cast<Function*>(node) != nullptr;
}

class Power : public Function {
public:
	Power(Node *base, float exponent) :
	Function(base),
	fExponent(exponent) {}

	Node *deriveFunction(Node *argument) override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	float fExponent{0.0f};
};

inline bool isPower(Node *node) {
	return dynamic_cast<Power*>(node) != nullptr;
}

class SquareRoot : public Power {
public:
	SquareRoot(Node *base) : Power(base, 0.5) {}
};

class Cosine : public Function {
public:
	Cosine(Node *argument) :
	Function(argument) {}

	Node *deriveFunction(Node *argument) override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};

class Sine : public Function {
public:
	Sine(Node *argument) :
	Function(argument) {}

	Node *deriveFunction(Node *argument) override;
	float evaluate(float x) override;
	Node *simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};