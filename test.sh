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
		echo "\033[31m[Failure]\033[m $input => expected $expected, but got $actual"
		exit 1
	fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 42 "10 + 40 - 8;"
assert 47 "5 + 6 * 7;"
assert 15 "5 * (9 - 6);"
assert 4 "(3 + 5) / 2;"
assert 10 "-10 + 20;"
assert 0 "42 < 42;"
assert 1 "42 <= 42;"
assert 0 "42 > 42;"
assert 1 "42 >= 42;"
assert 1 "42 == 42;"
assert 0 "42 != 42;"
assert 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"
assert 8 "foo = 3; bar = 5; foo + bar;"
assert 8 "foo = 3; bar = 5; return foo + bar;"
assert 42 "if (1) return 42;"
assert 42 "if (1) return 42; else return 43;"
assert 43 "if (0) return 42; else return 43;"
assert 42 "if (1) if (1) return 42; else return 43;"
assert 11 "foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo;"
assert 0 "foo = 3; while(foo > 0) foo = foo - 1; return foo;"
assert 2 "foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo;"

echo OK
