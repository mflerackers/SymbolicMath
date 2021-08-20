
#include "symbolic.h"

void simplificationTests() {
	auto x = variable();
	
	auto n = 2.0f * x + 3.0f * x;
	std::cout << "expression " << n << "\n";
  	std::cout << "simplify\n";
	n = n.simplifyStep();
	std::cout << n << "\n";

	std::cout << "<------>\n";

	auto o = (2.0f * x) * (3.0f * x);
	std::cout << "expression " << o << "\n";
  	std::cout << "simplify\n";
	o = o.simplifyStep();
	std::cout << o << "\n";
	o = o.simplifyStep();
	std::cout << o << "\n";

	std::cout << "<------>\n";

	auto p = (x ^ 2) + (x ^ 2);
	std::cout << "expression " << p << "\n";
  	std::cout << "simplify\n";
	p = p.simplifyStep();
	std::cout << p << "\n";

	std::cout << "<------>\n";

	auto q = (x ^ 2) * (x ^ 2);
	std::cout << "expression " << q << "\n";
  	std::cout << "simplify\n";
	q = q.simplifyStep();
	std::cout << q << "\n";
	q = q.simplifyStep();
	std::cout << q << "\n";
	q = q.simplifyStep();
	std::cout << q << "\n";

	std::cout << "<------>\n";
}

void vectorTests() {
	auto x = variable();

	auto v1 = vec2(x+x, constant(2.0f));
	std::cout << "simplify " << v1 << "\n";
	v1 = v1.simplify();
	std::cout << v1 << "\n";
	auto dv1 = v1.derive().simplify();
	std::cout << "derivative " << dv1 << "\n";

	auto v2 = vec2(x, constant(4.0f));
	auto v = v1 + v2;
	std::cout << "sum " << v << "\n";
	std::cout << "simplify " << v.simplify() << "\n";

	v = 2.0f * v1;
	std::cout << "product " << v << "\n";
	std::cout << "simplify " << v.simplify() << "\n";

	v = x * v1;
	std::cout << "product " << v << "\n";
	std::cout << "simplify " << v.simplify() << "\n";
	auto dv = v.derive().simplify();
	std::cout << "derivative " << dv << "\n";

	v = dot(v1, v2);
	std::cout << "dot " << v << "\n";
	std::cout << "simplify " << v.simplify() << "\n";
}

int main() {
	auto x = variable();
	auto n = 2.0f * x - 2.0f * (x ^ 2);

	std::cout << "expression " << n << "\n";
  	std::cout << "evaluation x=5 " << n.evaluate(5) << "\n";
	auto dn = n.derive();
	std::cout << "derivative " << dn << "\n";
	std::cout << "simplify\n";
	dn = dn.simplify();
	std::cout << dn << "\n";
	std::cout << "evaluation x=5 " << dn.evaluate(5) << "\n";

	std::cout << "<------>\n";

	auto m = cos(2 * x);

	std::cout << "expression " << m << "\n";
  	std::cout << "evaluation x=pi " << m.evaluate(M_PI) << "\n";
	auto dm = m.derive();
	std::cout << "derivative " << dm << "\n";
	std::cout << "simplify\n";
	dm = dm.simplify();
	std::cout << dm << "\n";
	std::cout << "evaluation x=pi  " << dm.evaluate(M_PI) << "\n";

	std::cout << "<------>\n";

	auto p = x ^ 3;
	std::cout << "expression " << p << "\n";
  	std::cout << "evaluation x=3 " << p.evaluate(3) << "\n";
	auto dp = p.derive();
	std::cout << "derivative " << dp << "\n";
	std::cout << "simplify\n";
	dp = dp.simplify();
	std::cout << dp << "\n";
	std::cout << "evaluation x=3 " << dp.evaluate(3) << "\n";

	std::cout << "<------>\n";

	auto q = cos(x) ^ 2;
	std::cout << "expression " << q << "\n";
	auto dq = q.derive();
	std::cout << "derivative " << dq << "\n";
	std::cout << "simplify\n";
	dq = dq.simplify();
	std::cout << dq << "\n";

	std::cout << "<------>\n";

	auto r = (2 * x) * (4 * x);
	std::cout << "expression " << r << "\n";
	std::cout << "simplify\n";
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

	std::cout << "<------>\n";

	auto t = sqrt(x);
	std::cout << "expression " << t << "\n";
	auto dt = t.derive();
	std::cout << "derivative " << dt << "\n";
	std::cout << "simplify\n";
	dt = dt.simplify();
	std::cout << dt << "\n";

	std::cout << "<------>\n";

	auto u = ln(x);
	std::cout << "expression " << u << "\n";
	auto du = u.derive();
	std::cout << "derivative " << du << "\n";
	std::cout << "simplify\n";
	du = du.simplify();
	std::cout << du << "\n";

	std::cout << "<------>\n";

	auto v = 3 ^ x;
	std::cout << "expression " << v << "\n";
  	std::cout << "evaluation x=3 " << v.evaluate(3) << "\n";
	auto dv = v.derive();
	std::cout << "derivative " << dv << "\n";
	std::cout << "simplify\n";
	dv = dv.simplify();
	std::cout << dv << "\n";
	std::cout << "evaluation x=3 " << dv.evaluate(3) << "\n";

	std::cout << "<------>\n";

	auto w = x ^ x;
	std::cout << "expression " << w << "\n";
  	std::cout << "evaluation x=3 " << w.evaluate(3) << "\n";
	auto dw = w.derive();
	std::cout << "derivative " << dw << "\n";
	std::cout << "simplify\n";
	dw = dw.simplify();
	std::cout << dw << "\n";
	std::cout << "evaluation x=3 " << dw.evaluate(3) << "\n";

	std::cout << "<------>\n";

	simplificationTests();
	vectorTests();
}