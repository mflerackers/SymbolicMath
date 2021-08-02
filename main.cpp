
#include "symbolic.h"

int main() {
	auto x = NodeRef(new Variable());
	auto n = 2.0f * x - 2.0f * (x ^ 2);

	std::cout << n << "\n";
  	std::cout << n.evaluate(5) << "\n";
	auto dn = n.derive();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	std::cout << dn.evaluate(5) << "\n";

	std::cout << "<------>\n";

	auto m = new Cosine(new Product(new Constant(2), new Variable()));

	std::cout << *m << "\n";
  	std::cout << m->evaluate(M_PI) << "\n";
	auto dm = m->derive();
	std::cout << *dm << "\n";
	dm = dm->simplify();
	std::cout << *dm << "\n";
	dm = dm->simplify();
	std::cout << *dm << "\n";
	dm = dm->simplify();
	std::cout << *dm << "\n";
	dm = dm->simplify();
	std::cout << *dm << "\n";
	std::cout << dm->evaluate(M_PI) << "\n";

	std::cout << "<------>\n";

	auto p = x^3;
	std::cout << p << "\n";
  	std::cout << p.evaluate(3) << "\n";
	std::cout << p.derive() << "\n";
	std::cout << p.derive().simplify() << "\n";
	std::cout << p.derive().evaluate(3) << "\n";

	std::cout << "<------>\n";

	auto q = new Power(new Cosine(x.fRef), 2.0f);
	std::cout << *q << "\n";
	std::cout << *q->derive() << "\n";
	std::cout << *q->derive()->simplify() << "\n";
	std::cout << *q->derive()->simplify()->simplify() << "\n";
	std::cout << *q->derive()->simplify()->simplify()->simplify() << "\n";

	std::cout << "<------>\n";

	auto r = (2 * x) * (4 * x);
	std::cout << r << "\n";
	std::cout << r.simplify() << "\n";
	std::cout << r.simplify().simplify() << "\n";

	std::cout << "<------>\n";

	auto s = 1.0f / x;
	std::cout << s << "\n";
	std::cout << s.derive() << "\n";
	std::cout << s.derive().simplify() << "\n";
	std::cout << s.derive().simplify().simplify() << "\n";
}