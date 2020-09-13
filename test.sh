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

assert 0 "main() {return 0; }"
assert 42 "main() {return 42; }"
assert 21 "main() {return 5+20-4; }"
assert 42 "main() {return 10 + 40 - 8; }"
assert 47 "main() {return 5 + 6 * 7; }"
assert 15 "main() {return 5 * (9 - 6); }"
assert 4 "main() {return (3 + 5) / 2; }"
assert 10 "main() {return -10 + 20; }"
assert 0 "main() {return 42 < 42; }"
assert 1 "main() {return 42 <= 42; }"
assert 0 "main() {return 42 > 42; }"
assert 1 "main() {return 42 >= 42; }"
assert 1 "main() {return 42 == 42; }"
assert 0 "main() {return 42 != 42; }"
assert 14 "main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }"
assert 8 "main() { foo = 3; bar = 5; return foo + bar; }"
assert 8 "main() { foo = 3; bar = 5; return foo + bar; }"
assert 42 "main() { if (1) return 42; }"
assert 42 "main() { if (1) return 42; else return 43; }"
assert 43 "main() { if (0) return 42; else return 43; }"
assert 42 "main() { if (1) if (1) return 42; else return 43; }"
assert 11 "main() { foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo; }"
assert 0 "main() { foo = 3; while(foo > 0) foo = foo - 1; return foo; }"
assert 2 "main() { foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo;}"
assert 6 "main() { for (i = 0; i < 5; i = i + 1) i = i + 1; return i; }"
assert 2 "main() { a = 1; if(a == 1) { a = a + 1; return a; } }"
assert 3 "foo() {return 3;} main() { return 3; }"
assert 3 "foo() {return 3;} main() { return foo(); }"
assert 5 "add(a, b) {return a + b;} main() { return 5; }"
assert 5 "add(a, b) {return a + b;} main() { return add(2, 3); }"
assert 8 "fibo(n) { if (n == 1) return 1; else if (n == 0) return 1; else return fibo(n-1) + fibo(n-2); } main(){return fibo(5);}"

echo OK
