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

echo OK
