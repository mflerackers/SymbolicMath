#include "symbolic.h"

NodeRef operator+(const NodeRef &left, const NodeRef &right) {
	return NodeRef(new Sum(left.fRef, right.fRef));
}

NodeRef operator-(const NodeRef &left, const NodeRef &right) {
	return NodeRef(new Sum(left.fRef, new Product(new Constant(-1.0f), right.fRef)));
}

NodeRef operator*(float left, const NodeRef &right) {
	return NodeRef(new Product(new Constant(left), right.fRef));
}

NodeRef operator*(const NodeRef &left, const NodeRef &right) {
	return NodeRef(new Product(left.fRef, right.fRef));
}

NodeRef operator/(float left, const NodeRef &right) {
	return NodeRef(new Product(new Constant(left), new Power(right.fRef, -1.0f)));
}

NodeRef operator/(const NodeRef &left, const NodeRef &right) {
	return NodeRef(new Product(left.fRef, new Power(right.fRef, -1.0f)));
}

NodeRef operator^(const NodeRef &base, float exponent) {
	return NodeRef(new Power(base.fRef, exponent));
}

std::ostream &operator<< (std::ostream &stream, const NodeRef &reference) {
	return reference.fRef->out(stream);
}

Node *Constant::derive() {
		return new Constant(0.0f);
}

float Constant::evaluate(float x) {
	return fValue;
}

Node *Constant::simplify() {
	return this;
}

std::ostream &Constant::out(std::ostream &stream) const {
	stream << fValue;
	return stream;
}

Node *Variable::derive() {
	return new Constant(1.0f);
}

float Variable::evaluate(float x) {
	return x;
}

Node *Variable::simplify() {
	return this;
}

std::ostream &Variable::out(std::ostream &stream) const {
	stream << "x";
	return stream;
}

Node *Sum::derive() {
	return new Sum(fLeft->derive(), fRight->derive());
}

float Sum::evaluate(float x) {
	return fLeft->evaluate(x) + fRight->evaluate(x);
}

Node *Sum::simplify() {
	if (isConstant(fLeft)) {
		auto left = dynamic_cast<Constant*>(fLeft);
		// n + m = p
		if (isConstant(fRight)) {
			auto right = dynamic_cast<Constant*>(fRight);
			return new Constant(left->fValue + right->fValue);
		}
		// 0 + n = n
		else if (left->fValue == 0.0f) {
			return fRight->simplify();
		}
	}
	else if (isConstant(fRight)) {
		auto right = dynamic_cast<Constant*>(fRight);
		// n + 0 = n
		if (right->fValue == 0.0f) {
			return fLeft->simplify();
		}
	}
	else if (isVariable(fLeft) && isVariable(fRight)) {
		return new Product(new Constant(2.0f), new Variable());
	}
	return new Sum(fLeft->simplify(), fRight->simplify());
}

std::ostream &Sum::out(std::ostream &stream) const {
	stream << "(" << *fLeft << " + " << *fRight << ")";
	return stream;
}

Node *Product::derive() {
	return new Sum(new Product(fLeft->derive(), fRight), new Product(fLeft, fRight->derive()));
}

float Product::evaluate(float x) {
	return fLeft->evaluate(x) * fRight->evaluate(x);
}

Node *Product::simplify() {
	// Switch so the constant is always left
	if (!isConstant(fLeft) && isConstant(fRight)) {
		return new Product(fRight, fLeft);
	}
	if (isConstant(fLeft)) {
		auto left = dynamic_cast<Constant*>(fLeft);
		// 0 * n = 0
		if (left->fValue == 0.0f) {
			return new Constant(0.0f);
		}
		// 1 * n = n
		else if (left->fValue == 1.0f) {
			return fRight->simplify();
		}
		// n * m = p
		else if (isConstant(fRight)) {
			auto right = dynamic_cast<Constant*>(fRight);
			return new Constant(left->fValue * right->fValue);
		}
		else if (isProduct(fRight)) {
			auto product = dynamic_cast<Product*>(fRight);
			// n * (m * x) = n*m * x
			if (isConstant(product->fLeft)) {
				auto pleft = dynamic_cast<Constant*>(product->fLeft);
				return new Product(new Constant(left->fValue * pleft->fValue), product->fRight);
			}
		}
	}
	if (isVariable(fLeft) && isVariable(fRight)) {
		return new Power(fLeft, 2.0f);
	}
	if (isProduct(fLeft) && isProduct(fRight)) {
		auto productLeft = dynamic_cast<Product*>(fLeft);
		auto productRight = dynamic_cast<Product*>(fRight);
		// (n * x) * (m * y) = n*m * (x * y)
		if (isConstant(productLeft->fLeft) && isConstant(productRight->fLeft)) {
			auto left = dynamic_cast<Constant*>(productLeft->fLeft);
			auto right = dynamic_cast<Constant*>(productRight->fLeft);
			return new Product(new Constant(left->fValue * right->fValue),
				new Product(productLeft->fRight, productRight->fRight));
		}
	}
	return new Product(fLeft->simplify(), fRight->simplify());
}

std::ostream &Product::out(std::ostream &stream) const {
	if (dynamic_cast<Constant*>(fLeft)) {
		auto left = dynamic_cast<Constant*>(fLeft);
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
		else if (isVariable(fRight) || isFunction(fRight)) {
			stream << *fLeft << *fRight;
			return stream;
		}
	}
	stream << "(" << *fLeft << " * " << *fRight << ")";
	return stream;
}

Node *Function::derive() {
	return new Product(deriveFunction(fArgument), fArgument->derive());
}

Node *Power::deriveFunction(Node *argument) {
	return new Product(new Constant(fExponent), new Power(argument, fExponent - 1.0f));
}

float Power::evaluate(float x) {
	return powf(fArgument->evaluate(x), fExponent);
}

Node *Power::simplify() {
	if (fExponent == 1) {
		return fArgument->simplify();
	}
	return this;
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

Node *Cosine::deriveFunction(Node *argument) {
	return new Product(new Constant(-1.0f), new Sine(argument));
}

float Cosine::evaluate(float x) {
	return cosf(fArgument->evaluate(x));
}

Node *Cosine::simplify() {
	return this;
}

std::ostream &Cosine::out(std::ostream &stream) const {
	stream << "cos(" << *fArgument << ")";
	return stream;
}

Node *Sine::deriveFunction(Node *argument) {
	return new Cosine(argument);
}

float Sine::evaluate(float x) {
	return sinf(fArgument->evaluate(x));
}

Node *Sine::simplify() {
	return this;
}

std::ostream &Sine::out(std::ostream &stream) const {
	stream << "sin(" << *fArgument << ")";
	return stream;
}