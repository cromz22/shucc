int assert(int expected, int actual, char *code);

int gvar;
int garr[3];
int increment() { gvar = gvar + 1; return 0; }
int foo() { return 3; }
int add(int a, int b) { return a + b; }
int fibo(int n) { if (n == 1) return 1; else if (n == 0) return 1; else return fibo(n-1) + fibo(n-2); }
char first(char *str) { return str[0]; }

int test0() { return 0; }
int test1() { return 42; }
int test2() { return 5+20-4; }
int test3() { return 10 + 40 - 8; }
int test4() { return 5 + 6 * 7; }
int test5() { return 5 * (9 - 6); }
int test6() { return (3 + 5) / 2; }
int test7() { return -10 + 20; }
int test8() { return 42 < 42; }
int test9() { return 42 <= 42; }
int test10() { return 42 > 42; }
int test11() { return 42 >= 42; }
int test12() { return 42 == 42; }
int test13() { return 42 != 42; }
int test14() { int a; return 14; }
int test15() { int a; a = 3; return 42; }
int test16() { int a; a = 3; int b; b = 5 * 6 - 8; return a + b / 2; }
int test17() { int foo; int bar; foo = 3; bar = 5; return foo + bar; }
int test18() { int foo; int bar; foo = 3; bar = 5; return foo + bar; }
int test19() { if (1) return 42; }
int test20() { if (1) return 42; else return 43; }
int test21() { if (0) return 42; else return 43; }
int test22() { if (1) if (1) return 42; else return 43; }
int test23() { int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo; }
int test24() { int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo; }
int test25() { int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo; }
int test26() { int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i; }
int test27() { int a; a = 1; if(a == 1) { a = a + 1; return a; } }
int test28() { return foo(); }
int test29() { return add(2, 3); }
int test30() { return fibo(5); }
int test31() { int x; x = 3; int *y; y = &x; return *y; }
int test32() { int *****x; return 5; }
int test33() { int x; int *y; y = &x; *y = 3; return x; }
int test34() { int a; int b; int c; a = 1; b = 2; c = 3; int *p; p = &c; int *q; q = p + 2; return *q; }
int test35() { int a; int b; a = 1; b = 2; int *p; p = &a; int *q; q = p - 1; return *q; }
int test36() { int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &c; int *q; q = p - 1; return *q; }
int test37() { int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &d + 2; return *p; }
int test38() { int x; return sizeof(x); }
int test39() { int *y; return sizeof(y); }
int test40() { return sizeof(1); }
int test41() { return sizeof(sizeof(1)); }
int test42() { int a[10]; return 42; }
int test43() { int a[3]; return sizeof(a); }
int test44() { int *a[3]; return sizeof(a); }
int test45() { int a[1]; *a = 3; return *a; }
int test46() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }
int test47() { int a[2]; *a = 1; return a[0]; }
int test48() { int a[2]; *a = 1; *(a + 1) = 2; return a[0] + a[1]; }
int test49() { int a[2]; *a = 1; *(a + 1) = 2; return 0[a] + 1[a]; }
int test50() { gvar = 42; return gvar; }
int test51() { garr[1] = 42; return garr[1]; }
int test52() { gvar = 0; while (gvar < 3) {increment();} return gvar; }
int test53() { char x; x = 1; return x; }
int test54() { char x; return sizeof(x); }
int test55() { int x[2][2]; return 0; }
int test56() { int x[2][3]; int *y; y=x[0]; y[0]=0; return x[0][0]; }
int test57() { int x[2][3]; int *y; y=x[0]; y[1]=1; return x[0][1]; }
int test58() { int x[2][3]; int *y; y=x[0]; y[2]=2; return x[0][2]; }
int test59() { int x[2][3]; int *y; y=x[1]; y[0]=3; return x[1][0]; }
int test60() { int x[2][3]; int *y; y=x[1]; y[1]=4; return x[1][1]; }
int test61() { int x[2][3]; int *y; y=x[1]; y[2]=5; return x[1][2]; }
int test62() { int a[2][3]; return sizeof(*a); }
int test63() { int x = 3; return x; }
int test64() { int x = foo(); return x; }
int test65() { char x = 3; return x; }
int test66() { int a[3] = {0, 1, 2}; return a[1]; }
int test67() { int a[3] = {2}; return a[2]; }
int test68() { int a[] = {0, 1, 2}; return a[1]; }
int test69() { char* foo; foo = "bar"; return 0; }

int main() {
    assert(0, test0(), "return 0;");
    assert(42, test1(), "return 42;");
    assert(21, test2(), "return 5+20-4;");
    assert(42, test3(), "return 10 + 40 - 8;");
    assert(47, test4(), "return 5 + 6 Makefile codegen.c codegen.o foo.txt main.c main.o parse.c parse.o prep.sh sema.c sema.o shucc shucc.h test test.sh tokenize.c tokenize.o utils.c utils.o 7;");
    assert(15, test5(), "return 5 Makefile codegen.c codegen.o foo.txt main.c main.o parse.c parse.o prep.sh sema.c sema.o shucc shucc.h test test.sh tokenize.c tokenize.o utils.c utils.o (9 - 6);");
    assert(4, test6(), "return (3 + 5) / 2;");
    assert(10, test7(), "return -10 + 20;");
    assert(0, test8(), "return 42 < 42;");
    assert(1, test9(), "return 42 <= 42;");
    assert(0, test10(), "return 42 > 42;");
    assert(1, test11(), "return 42 >= 42;");
    assert(1, test12(), "return 42 == 42;");
    assert(0, test13(), "return 42 != 42;");
    assert(14, test14(), "int a; return 14;");
    assert(42, test15(), "int a; a = 3; return 42;");
    assert(14, test16(), "int a; a = 3; int b; b = 5 Makefile codegen.c codegen.o foo.txt main.c main.o parse.c parse.o prep.sh sema.c sema.o shucc shucc.h test test.sh tokenize.c tokenize.o utils.c utils.o 6 - 8; return a + b / 2;");
    assert(8, test17(), "int foo; int bar; foo = 3; bar = 5; return foo + bar;");
    assert(8, test18(), "int foo; int bar; foo = 3; bar = 5; return foo + bar;");
    assert(42, test19(), "if (1) return 42;");
    assert(42, test20(), "if (1) return 42; else return 43;");
    assert(43, test21(), "if (0) return 42; else return 43;");
    assert(42, test22(), "if (1) if (1) return 42; else return 43;");
    assert(11, test23(), "int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo;");
    assert(0, test24(), "int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo;");
    assert(2, test25(), "int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo;");
    assert(6, test26(), "int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i;");
    assert(2, test27(), "int a; a = 1; if(a == 1) { a = a + 1; return a; }");
    assert(3, test28(), "return foo();");
    assert(5, test29(), "return add(2, 3);");
    assert(8, test30(), "return fibo(5);");
    assert(3, test31(), "int x; x = 3; int *y; y = &x; return *y;");
    assert(5, test32(), "int *****x; return 5;");
    assert(3, test33(), "int x; int *y; y = &x; *y = 3; return x;");
    assert(1, test34(), "int a; int b; int c; a = 1; b = 2; c = 3; int *p; p = &c; int *q; q = p + 2; return *q;");
    assert(2, test35(), "int a; int b; a = 1; b = 2; int *p; p = &a; int *q; q = p - 1; return *q;");
    assert(4, test36(), "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &c; int *q; q = p - 1; return *q;");
    assert(2, test37(), "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &d + 2; return *p;");
    assert(4, test38(), "int x; return sizeof(x);");
    assert(8, test39(), "int *y; return sizeof(y);");
    assert(4, test40(), "return sizeof(1);");
    assert(4, test41(), "return sizeof(sizeof(1));");
    assert(42, test42(), "int a[10]; return 42;");
    assert(12, test43(), "int a[3]; return sizeof(a);");
    assert(24, test44(), "int *a[3]; return sizeof(a);");
    assert(3, test45(), "int a[1]; *a = 3; return *a;");
    assert(3, test46(), "int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);");
    assert(1, test47(), "int a[2]; *a = 1; return a[0];");
    assert(3, test48(), "int a[2]; *a = 1; *(a + 1) = 2; return a[0] + a[1];");
    assert(3, test49(), "int a[2]; *a = 1; *(a + 1) = 2; return 0[a] + 1[a];");
    assert(42, test50(), "gvar = 42; return gvar;");
    assert(42, test51(), "garr[1] = 42; return garr[1];");
    assert(3, test52(), "gvar = 0; while (gvar < 3) {increment();} return gvar;");
    assert(1, test53(), "char x; x = 1; return x;");
    assert(1, test54(), "char x; return sizeof(x);");
    assert(0, test55(), "int x[2][2]; return 0;");
    assert(0, test56(), "int x[2][3]; int *y; y=x[0]; y[0]=0; return x[0][0];");
    assert(1, test57(), "int x[2][3]; int *y; y=x[0]; y[1]=1; return x[0][1];");
    assert(2, test58(), "int x[2][3]; int *y; y=x[0]; y[2]=2; return x[0][2];");
    assert(3, test59(), "int x[2][3]; int *y; y=x[1]; y[0]=3; return x[1][0];");
    assert(4, test60(), "int x[2][3]; int *y; y=x[1]; y[1]=4; return x[1][1];");
    assert(5, test61(), "int x[2][3]; int *y; y=x[1]; y[2]=5; return x[1][2];");
    assert(12, test62(), "int a[2][3]; return sizeof(*a);");
    assert(3, test63(), "int x = 3; return x;");
    assert(3, test64(), "int x = foo(); return x;");
    assert(3, test65(), "char x = 3; return x;");
    assert(1, test66(), "int a[3] = {0, 1, 2}; return a[1];");
    assert(0, test67(), "int a[3] = {2}; return a[2];");
    assert(1, test68(), "int a[] = {0, 1, 2}; return a[1];");
    assert(0, test69(), "char* foo; foo = \"bar\"; return 0;");
    return 0;
}
