class Constant : public Node, public std::enable_shared_from_this<Constant> {
public:
	Constant(float value) :
	fValue(value) {}

 	std::shared_ptr<Node> derive() override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	float fValue{0.0f};
};

std::shared_ptr<Constant> newConstant(float value) {
	return std::make_shared<Constant>(value);
}

bool isConstant(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Constant*>(node.get()) != nullptr;
}

class Variable : public Node, public std::enable_shared_from_this<Variable> {
public:
	Variable() {}

	std::shared_ptr<Node> derive() override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};

inline std::shared_ptr<Variable> newVariable() {
	return std::make_shared<Variable>();
}

inline bool isVariable(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Variable*>(node.get()) != nullptr;
}

class Sum : public Node {
public:
	Sum(const std::shared_ptr<Node> &left, const std::shared_ptr<Node> &right) :
	fLeft(left),
	fRight(right) {}

	std::shared_ptr<Node> derive() override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	std::shared_ptr<Node> fLeft;
	std::shared_ptr<Node> fRight;
};

inline std::shared_ptr<Sum> newSum(const std::shared_ptr<Node> &left, const std::shared_ptr<Node> &right) {
	return std::make_shared<Sum>(left, right);
}

inline bool isSum(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Sum*>(node.get()) != nullptr;
}

class Product : public Node {
public:
	Product(const std::shared_ptr<Node> &left, const std::shared_ptr<Node> &right) :
	fLeft(left),
	fRight(right) {}

	std::shared_ptr<Node> derive() override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	std::shared_ptr<Node> fLeft;
	std::shared_ptr<Node> fRight;
};

inline std::shared_ptr<Product> newProduct(const std::shared_ptr<Node> &left, const std::shared_ptr<Node> &right) {
	return std::make_shared<Product>(left, right);
}

inline bool isProduct(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Product*>(node.get()) != nullptr;
}

class Function : public Node {
public:
	Function(const std::shared_ptr<Node> &argument) :
	fArgument(argument) {}

	std::shared_ptr<Node> derive() override;
	virtual std::shared_ptr<Node> deriveFunction(const std::shared_ptr<Node> &argument) = 0;

	std::shared_ptr<Node> fArgument;
};

inline bool isFunction(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Function*>(node.get()) != nullptr;
}

class Power : /*public Function, */public Node, public std::enable_shared_from_this<Power> {
public:
	Power(const std::shared_ptr<Node> &base, const std::shared_ptr<Node> &exponent) :
	//Function(base),
	fBase(base),
	fExponent(exponent) {}

	//std::shared_ptr<Node> deriveFunction(const std::shared_ptr<Node> &argument) override;
	std::shared_ptr<Node> derive() override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	std::shared_ptr<Node> fBase;
	std::shared_ptr<Node> fExponent;
};

inline std::shared_ptr<Power> newPower(const std::shared_ptr<Node> &base, const std::shared_ptr<Node> &exponent) {
	return std::make_shared<Power>(base, exponent);
}

inline bool isPower(const std::shared_ptr<Node> &node) {
	return dynamic_cast<Power*>(node.get()) != nullptr;
}

class SquareRoot : public Power {
public:
	SquareRoot(const std::shared_ptr<Node> &base) : Power(base, newConstant(0.5f)) {}
};

inline std::shared_ptr<SquareRoot> newSquareRoot(const std::shared_ptr<Node> &argument) {
	return std::make_shared<SquareRoot>(argument);
}

class NaturalLogarithm : public Function, public std::enable_shared_from_this<NaturalLogarithm> {
public:
	NaturalLogarithm(const std::shared_ptr<Node> &argument) :
	Function(argument),
	fArgument(argument) {}

	std::shared_ptr<Node> deriveFunction(const std::shared_ptr<Node> &argument) override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;

	std::shared_ptr<Node> fArgument;
};

inline std::shared_ptr<NaturalLogarithm> newNaturalLogarithm(const std::shared_ptr<Node> &argument) {
	return std::make_shared<NaturalLogarithm>(argument);
}

class Cosine : public Function, public std::enable_shared_from_this<Cosine> {
public:
	Cosine(const std::shared_ptr<Node> &argument) :
	Function(argument) {}

	std::shared_ptr<Node> deriveFunction(const std::shared_ptr<Node> &argument) override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};

inline std::shared_ptr<Cosine> newCosine(const std::shared_ptr<Node> &argument) {
	return std::make_shared<Cosine>(argument);
}

class Sine : public Function, public std::enable_shared_from_this<Sine> {
public:
	Sine(const std::shared_ptr<Node> &argument) :
	Function(argument) {}

	std::shared_ptr<Node> deriveFunction(const std::shared_ptr<Node> &argument) override;
	float evaluate(float x) override;
	std::shared_ptr<Node> simplify() override;
	std::ostream &out(std::ostream &stream) const override;
};

inline std::shared_ptr<Sine> newSine(const std::shared_ptr<Node> &argument) {
	return std::make_shared<Sine>(argument);
}