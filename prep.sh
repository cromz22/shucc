#!/usr/bin/env bash

declare -a inputs
declare -a outputs

append() {
  inputs+=( "$1" )
  outputs+=( "$2" )
}

append "return 0;" "0"
append "return 42;" "42"
append "return 5+20-4;" "21"
append "return 10 + 40 - 8;" "42"
append "return 5 + 6 * 7;" "47"
append "return 5 * (9 - 6);" "15"
append "return (3 + 5) / 2;" "4"
append "return -10 + 20;" "10"
append "return 42 < 42;" "0"
append "return 42 <= 42;" "1"
append "return 42 > 42;" "0"
append "return 42 >= 42;" "1"
append "return 42 == 42;" "1"
append "return 42 != 42;" "0"
append "int a; return 14;" "14"
append "int a; a = 3; return 42;" "42"
append "int a; a = 3; int b; b = 5 * 6 - 8; return a + b / 2;" "14"
append "int foo; int bar; foo = 3; bar = 5; return foo + bar;" "8"
append "int foo; int bar; foo = 3; bar = 5; return foo + bar;" "8"
append "if (1) return 42;" "42"
append "if (1) return 42; else return 43;" "42"
append "if (0) return 42; else return 43;" "43"
append "if (1) if (1) return 42; else return 43;" "42"
append "int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo;" "11"
append "int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo;" "0"
append "int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo;" "2"
append "int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i;" "6"
append "int a; a = 1; if(a == 1) { a = a + 1; return a; }" "2"
append "return foo();" "3"
append "return add(2, 3);" "5"
append "return fibo(5);" "8"
append "int x; x = 3; int *y; y = &x; return *y;" "3"
append "int *****x; return 5;" "5"
append "int x; int *y; y = &x; *y = 3; return x;" "3"
append "int a; int b; int c; a = 1; b = 2; c = 3; int *p; p = &c; int *q; q = p + 2; return *q;" "1"
append "int a; int b; a = 1; b = 2; int *p; p = &a; int *q; q = p - 1; return *q;" "2"
append "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &c; int *q; q = p - 1; return *q;" "4"
append "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &d + 2; return *p;" "2"
append "int x; return sizeof(x);" "4"
append "int *y; return sizeof(y);" "8"
append "return sizeof(1);" "4"
append "return sizeof(sizeof(1));" "4"
append "int a[10]; return 42;" "42"
append "int a[3]; return sizeof(a);" "12"
append "int *a[3]; return sizeof(a);" "24"
append "int a[1]; *a = 3; return *a;" "3"
append "int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);" "3"
append "int a[2]; *a = 1; return a[0];" "1"
append "int a[2]; *a = 1; *(a + 1) = 2; return a[0] + a[1];" "3"
append "int a[2]; *a = 1; *(a + 1) = 2; return 0[a] + 1[a];" "3"
append "gvar = 42; return gvar;" "42"
append "garr[1] = 42; return garr[1];" "42"
append "gvar = 0; while (gvar < 3) {increment();} return gvar;" "3"
append "char x; x = 1; return x;" "1"
append "char x; return sizeof(x);" "1"
append "int x[2][2]; return 0;" "0"
append "int x[2][3]; int *y; y=x[0]; y[0]=0; return x[0][0];" "0"
append "int x[2][3]; int *y; y=x[0]; y[1]=1; return x[0][1];" "1"
append "int x[2][3]; int *y; y=x[0]; y[2]=2; return x[0][2];" "2"
append "int x[2][3]; int *y; y=x[1]; y[0]=3; return x[1][0];" "3"
append "int x[2][3]; int *y; y=x[1]; y[1]=4; return x[1][1];" "4"
append "int x[2][3]; int *y; y=x[1]; y[2]=5; return x[1][2];" "5"
append "int a[2][3]; return sizeof(*a);" "12"


helper=$(cat <<<"
int gvar;
int garr[3];
int increment() { gvar = gvar + 1; return 0; }
int foo() { return 3; }
int add(int a, int b) {return a + b;}
int fibo(int n) { if (n == 1) return 1; else if (n == 0) return 1; else return fibo(n-1) + fibo(n-2); }
char first(char *str) { return str[0]; }
")
echo "${helper}"

# i=0
# for prog in "${!tests[@]}"
# do
#   echo "int test${i}() { ${prog} }"
#   let i++
# done
echo "int assert(int expected, int actual, char *code);"

echo ""

for i in "${!inputs[@]}"
do
  echo "int test${i}() { ${inputs[i]} }"
done

echo ""

echo "int main() {"
for i in "${!inputs[@]}"
do
  echo "    assert(${outputs[i]}, test${i}(), \"${inputs[i]}\");"
done
echo "    return 0;"
echo "}"
