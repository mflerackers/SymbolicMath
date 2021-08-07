
#include "symbolic.h"

int main() {
	auto x = NodeRef(newVariable());
	auto n = 2.0f * x - 2.0f * (x ^ 2);

	std::cout << "expression " << n << "\n";
  	std::cout << "evaluation x=5 " << n.evaluate(5) << "\n";
	auto dn = n.derive();
	std::cout << "derivative " << dn << "\n";
	std::cout << "simplify\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	std::cout << "evaluation x=5 " << dn.evaluate(5) << "\n";

	std::cout << "<------>\n";

	auto m = NodeRef(newCosine(newProduct(newConstant(2), newVariable())));

	std::cout << "expression " << m << "\n";
  	std::cout << "evaluation x=pi " << m.evaluate(M_PI) << "\n";
	auto dm = m.derive();
	std::cout << "derivative " << dm << "\n";
	std::cout << "simplify\n";
	dm = dm.simplify();
	std::cout << dm << "\n";
	dm = dm.simplify();
	std::cout << dm << "\n";
	dm = dm.simplify();
	std::cout << dm << "\n";
	dm = dm.simplify();
	std::cout << dm << "\n";
	std::cout << "evaluation x=pi  " << dm.evaluate(M_PI) << "\n";

	std::cout << "<------>\n";

	auto p = x^3;
	std::cout << "expression " << p << "\n";
  	std::cout << "evaluation x=3 " << p.evaluate(3) << "\n";
	auto dp = p.derive();
	std::cout << "derivative " << dp << "\n";
	std::cout << "simplify\n";
	dp = dp.simplify();
	std::cout << dp << "\n";
	std::cout << "evaluation x=3 " << dp.evaluate(3) << "\n";

	std::cout << "<------>\n";

	auto q = NodeRef(newPower(newCosine(x.fRef), 2.0f));
	std::cout << "expression " << q << "\n";
	auto dq = q.derive();
	std::cout << "derivative " << dq << "\n";
	std::cout << "simplify\n";
	dq = dq.simplify();
	std::cout << dq << "\n";
	dq = dq.simplify();
	std::cout << dq << "\n";
	dq = dq.simplify();
	std::cout << dq << "\n";

	std::cout << "<------>\n";

	auto r = (2 * x) * (4 * x);
	std::cout << "expression " << r << "\n";
	std::cout << "simplify\n";
	r = r.simplify();
	std::cout << r << "\n";
	r = r.simplify();
	std::cout << r << "\n";

	std::cout << "<------>\n";

	auto s = 1.0f / x;
	std::cout << "expression " << s << "\n";
	auto ds = s.derive();
	std::cout << "derivative " << ds << "\n";
	std::cout << "simplify\n";
	ds = ds.simplify();
	std::cout << ds << "\n";
	ds = ds.simplify();
	std::cout << ds << "\n";
	ds = ds.simplify();
}