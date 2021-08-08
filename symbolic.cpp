#include "symbolic.h"
#include "symbolic_internal.h"

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
	return NodeRef(newProduct(newConstant(left), newPower(right.fRef, -1.0f)));
}

NodeRef operator/(const NodeRef &left, const NodeRef &right) {
	return NodeRef(newProduct(left.fRef, newPower(right.fRef, -1.0f)));
}

NodeRef operator^(const NodeRef &base, float exponent) {
	return NodeRef(newPower(base.fRef, exponent));
}

std::ostream &operator<< (std::ostream &stream, const NodeRef &reference) {
	return reference.fRef->out(stream);
}

NodeRef variable() {
	return NodeRef(newVariable());
}

NodeRef squareroot(const NodeRef &argument) {
	return NodeRef(newSquareRoot(argument.fRef));
}

NodeRef cosine(const NodeRef &argument) {
	return NodeRef(newCosine(argument.fRef));
}

NodeRef sine(const NodeRef &argument) {
	return NodeRef(newSine(argument.fRef));
}

// Constant
std::shared_ptr<Node> Constant::derive() {
		return newConstant(0.0f);
}

float Constant::evaluate(float x) {
	return fValue;
}

std::shared_ptr<Node> Constant::simplify() {
	return shared_from_this();
}

std::ostream &Constant::out(std::ostream &stream) const {
	stream << fValue;
	return stream;
}

// Variable
std::shared_ptr<Node> Variable::derive() {
	return newConstant(1.0f);
}

float Variable::evaluate(float x) {
	return x;
}

std::shared_ptr<Node> Variable::simplify() {
	return shared_from_this();
}

std::ostream &Variable::out(std::ostream &stream) const {
	stream << "x";
	return stream;
}

// Sum
std::shared_ptr<Node> Sum::derive() {
	return newSum(fLeft->derive(), fRight->derive());
}

float Sum::evaluate(float x) {
	return fLeft->evaluate(x) + fRight->evaluate(x);
}

std::shared_ptr<Node> Sum::simplify() {
	if (isConstant(fLeft)) {
		auto left = dynamic_cast<Constant*>(fLeft.get());
		// n + m = p
		if (isConstant(fRight)) {
			auto right = dynamic_cast<Constant*>(fRight.get());
			return newConstant(left->fValue + right->fValue);
		}
		// 0 + n = n
		else if (left->fValue == 0.0f) {
			return fRight->simplify();
		}
	}
	else if (isConstant(fRight)) {
		auto right = dynamic_cast<Constant*>(fRight.get());
		// n + 0 = n
		if (right->fValue == 0.0f) {
			return fLeft->simplify();
		}
	}
	else if (isVariable(fLeft) && isVariable(fRight)) {
		return newProduct(newConstant(2.0f), newVariable());
	}
	return newSum(fLeft->simplify(), fRight->simplify());
}

std::ostream &Sum::out(std::ostream &stream) const {
	stream << "(" << *fLeft << " + " << *fRight << ")";
	return stream;
}

// Product
std::shared_ptr<Node> Product::derive() {
	return newSum(newProduct(fLeft->derive(), fRight), newProduct(fLeft, fRight->derive()));
}

float Product::evaluate(float x) {
	return fLeft->evaluate(x) * fRight->evaluate(x);
}

std::shared_ptr<Node> Product::simplify() {
	// Switch so the constant is always left
	if (!isConstant(fLeft) && isConstant(fRight)) {
		return newProduct(fRight, fLeft);
	}
	if (isConstant(fLeft)) {
		auto left = dynamic_cast<Constant*>(fLeft.get());
		// 0 * n = 0
		if (left->fValue == 0.0f) {
			return newConstant(0.0f);
		}
		// 1 * n = n
		else if (left->fValue == 1.0f) {
			return fRight->simplify();
		}
		// n * m = p
		else if (isConstant(fRight)) {
			auto right = dynamic_cast<Constant*>(fRight.get());
			return newConstant(left->fValue * right->fValue);
		}
		else if (isProduct(fRight)) {
			auto product = dynamic_cast<Product*>(fRight.get());
			// n * (m * x) = n*m * x
			if (isConstant(product->fLeft)) {
				auto pleft = dynamic_cast<Constant*>(product->fLeft.get());
				return newProduct(newConstant(left->fValue * pleft->fValue), product->fRight);
			}
		}
	}
	if (isVariable(fLeft) && isVariable(fRight)) {
		return newPower(fLeft, 2.0f);
	}
	if (isProduct(fLeft) && isProduct(fRight)) {
		auto productLeft = dynamic_cast<Product*>(fLeft.get());
		auto productRight = dynamic_cast<Product*>(fRight.get());
		// (n * x) * (m * y) = n*m * (x * y)
		if (isConstant(productLeft->fLeft) && isConstant(productRight->fLeft)) {
			auto left = dynamic_cast<Constant*>(productLeft->fLeft.get());
			auto right = dynamic_cast<Constant*>(productRight->fLeft.get());
			return newProduct(newConstant(left->fValue * right->fValue),
				newProduct(productLeft->fRight, productRight->fRight));
		}
	}
	return newProduct(fLeft->simplify(), fRight->simplify());
}

std::ostream &Product::out(std::ostream &stream) const {
	if (dynamic_cast<Constant*>(fLeft.get())) {
		auto left = dynamic_cast<Constant*>(fLeft.get());
		// -1 * n = -n
		if (left->fValue == -1.0f) {
			stream << "-" << *fRight;
			return stream;
		}
		// 1 * n = n
		else if (left->fValue == 1.0f) {
			stream << *fRight;
			return stream;
		}
		// n * x = nx
		else if (isVariable(fRight)) {
			stream << *fLeft << *fRight;
			return stream;
		}
		// n * f(x) = n*f(x)
		else if (isFunction(fRight)) {
			stream << *fLeft << "*" << *fRight;
			return stream;
		}
	}
	stream << "(" << *fLeft << "*" << *fRight << ")";
	return stream;
}


// Function
std::shared_ptr<Node> Function::derive() {
	return newProduct(deriveFunction(fArgument), fArgument->derive());
}

// Power
std::shared_ptr<Node> Power::deriveFunction(const std::shared_ptr<Node> &argument) {
	return newProduct(newConstant(fExponent), newPower(argument, fExponent - 1.0f));
}

float Power::evaluate(float x) {
	return powf(fArgument->evaluate(x), fExponent);
}

std::shared_ptr<Node> Power::simplify() {
	if (fExponent == 1) {
		return fArgument->simplify();
	}
	return shared_from_this();
}

std::ostream &Power::out(std::ostream &stream) const {
	float exponent = std::abs(fExponent);
	if (fExponent < 0) {
		stream << "1/(";
	}
	if (exponent == 0.5f) {
		stream << "√(" << *fArgument << ")";
	}
	else if (exponent == 1.0f) {
		stream << *fArgument;
	}
	else if (exponent == 2.0f) {
		stream << "(" << *fArgument << ")²";
	}
	else if (exponent == 3.0f) {
		stream << "(" << *fArgument << ")³";
	}
	else {
		stream << "(" << *fArgument << " ^ " << exponent << ")";
	}
	if (fExponent < 0) {
		stream << ")";
	}
	return stream;
}

// Cosine
std::shared_ptr<Node> Cosine::deriveFunction(const std::shared_ptr<Node> &argument) {
	return newProduct(newConstant(-1.0f), newSine(argument));
}

float Cosine::evaluate(float x) {
	return cosf(fArgument->evaluate(x));
}

std::shared_ptr<Node> Cosine::simplify() {
	return shared_from_this();
}

std::ostream &Cosine::out(std::ostream &stream) const {
	stream << "cos(" << *fArgument << ")";
	return stream;
}

// Sine
std::shared_ptr<Node> Sine::deriveFunction(const std::shared_ptr<Node> &argument) {
	return newCosine(argument);
}

float Sine::evaluate(float x) {
	return sinf(fArgument->evaluate(x));
}

std::shared_ptr<Node> Sine::simplify() {
	return shared_from_this();
}

std::ostream &Sine::out(std::ostream &stream) const {
	stream << "sin(" << *fArgument << ")";
	return stream;
}