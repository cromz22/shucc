#! /bin/bash
assert() {
	expected="$1"
	input="$2"

	./shucc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo -e "\033[32m[Success]\033[m $input => $actual"
	else
		echo -e "\033[31m[Failure]\033[m $input => expected $expected, but got $actual"
		exit 1
	fi
}

assert 0 "int main() { return 0; }"
assert 42 "int main() { return 42; }"
assert 21 "int main() { return 5+20-4; }"
assert 42 "int main() { return 10 + 40 - 8; }"
assert 47 "int main() { return 5 + 6 * 7; }"
assert 15 "int main() { return 5 * (9 - 6); }"
assert 4 "int main() { return (3 + 5) / 2; }"
assert 10 "int main() { return -10 + 20; }"
assert 0 "int main() { return 42 < 42; }"
assert 1 "int main() { return 42 <= 42; }"
assert 0 "int main() { return 42 > 42; }"
assert 1 "int main() { return 42 >= 42; }"
assert 1 "int main() { return 42 == 42; }"
assert 0 "int main() { return 42 != 42; }"
assert 14 "int main() { int a; return 14; }"
assert 14 "int main() { int a; a = 3; int b; b = 5 * 6 - 8; return a + b / 2; }"
assert 8 "int main() { int foo; int bar; foo = 3; bar = 5; return foo + bar; }"
assert 8 "int main() { int foo; int bar; foo = 3; bar = 5; return foo + bar; }"
assert 42 "int main() { if (1) return 42; }"
assert 42 "int main() { if (1) return 42; else return 43; }"
assert 43 "int main() { if (0) return 42; else return 43; }"
assert 42 "int main() { if (1) if (1) return 42; else return 43; }"
assert 11 "int main() { int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo; }"
assert 0 "int main() { int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo; }"
assert 2 "int main() { int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo; }"
assert 6 "int main() { int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i; }"
assert 2 "int main() { int a; a = 1; if(a == 1) { a = a + 1; return a; } }"
assert 3 "int foo() { return 3; } int main() { return 3; }"
assert 3 "int foo() { return 3; } int main() { return foo(); }"
assert 5 "int add(int a, int b) { return a + b; } int main() { return 5; }"
assert 5 "int add(int a, int b) { return a + b; } int main() { return add(2, 3); }"
assert 8 "int fibo(int n) { if (n == 1) return 1; else if (n == 0) return 1; else return fibo(n-1) + fibo(n-2); } int main(){ return fibo(5); }"
assert 3 "int main() { int x; x = 3; int y; y = &x; return *y; }"
assert 5 "int main() { int *****x; return 5; }"
assert 3 "int main() { int x; int *y; y = &x; *y = 3; return x; }"

echo OK
