# Derivatives

derivative of cos(x) ^ 2

```c++
auto f = cos(x) ^ 2;
f.derive().simplify();
```

> (-2 * (cos(x) * sin(x)))

derivative of 3^x

```c++
auto f = 3 ^ x;
f.derive().simplify();
```

> ((3 ^ x) * ln(3))

# Integrals

integral of x

```c++
auto f = x;
f.derive().simplify();
```

> (0.5 * (x ^ 2))

integral of 1/x

```c++
auto f = 1/x;
f.derive().simplify();
```

> ln(x)

# Partial derivatives

derivative of x^2 + xy + y^2 in x

```c++
((x ^ 2) + x * y + (y ^ 2)).derive("x").simplify();
```

> (2x + y)

# Taylor series

approximation of cosine around 0 by a 4th degree polynomial

```c++
auto f = cos(x);
taylor(f, 0, 4);
```

> ((1 + (-0.5 * (x ^ 2))) + (0.0416667 * (x ^ 4)))

# Newton root finding

to solve x^2 = 2, look for a root of 2-x^2 around 1 using a maximum of 10 steps

```c++
auto f = 2 - (x ^ 2);
newton(f, 1, 10);
```

> 1.41421

which is equal to √2