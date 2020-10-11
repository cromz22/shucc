
int gvar;
int garr[3];
int increment() { gvar = gvar + 1; return 0; }
int foo() { return 3; }
int add(int a, int b) {return a + b;}
int fibo(int n) { if (n == 1) return 1; else if (n == 0) return 1; else return fibo(n-1) + fibo(n-2); }
char first(char *str) { return str[0]; }
int test0() { if (0) return 42; else return 43; }
int test1() { char x; return sizeof(x); }
int test2() { int *a[3]; return sizeof(a); }
int test3() { return (3 + 5) / 2; }
int test4() { return 5+20-4; }
int test5() { int x[2][3]; int *y; y=x[0]; y[0]=0; return x[0][0]; }
int test6() { int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &d + 2; return *p; }
int test7() { int x[2][3]; int *y; y=x[0]; y[2]=2; return x[0][2]; }
int test8() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }
int test9() { int *****x; return 5; }
int test10() { return 42 <= 42; }
int test11() { int x[2][3]; int *y; y=x[0]; y[1]=1; return x[0][1]; }
int test12() { return 42 >= 42; }
int test13() { int a; return 14; }
int test14() { if (1) if (1) return 42; else return 43; }
int test15() { int a[2]; *a = 1; *(a + 1) = 2; return 0[a] + 1[a]; }
int test16() { return 10 + 40 - 8; }
int test17() { return 42 != 42; }
int test18() { int x[2][2]; return 0; }
int test19() { int a[10]; return 42; }
int test20() { garr[1] = 42; return garr[1]; }
int test21() { int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i; }
int test22() { int x[2][3]; int *y; y=x[1]; y[1]=4; return x[1][1]; }
int test23() { gvar = 0; while (gvar < 3) {increment();} return gvar; }
int test24() { int a[1]; *a = 3; return *a; }
int test25() { int a[3]; return sizeof(a); }
int test26() { int x; x = 3; int *y; y = &x; return *y; }
int test27() { int a; a = 3; return 42; }
int test28() { return add(2, 3); }
int test29() { return 42; }
int test30() { int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo; }
int test31() { int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &c; int *q; q = p - 1; return *q; }
int test32() { int a; a = 1; if(a == 1) { a = a + 1; return a; } }
int test33() { return fibo(5); }
int test34() { return sizeof(1); }
int test35() { int x; return sizeof(x); }
int test36() { return 5 + 6 * 7; }
int test37() { return 42 > 42; }
int test38() { int a[2][3]; return sizeof(*a); }
int test39() { char x; x = 1; return x; }
int test40() { return sizeof(sizeof(1)); }
int test41() { int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo; }
int test42() { return 0; }
int test43() { if (1) return 42; }
int test44() { int x[2][3]; int *y; y=x[1]; y[0]=3; return x[1][0]; }
int test45() { int a; int b; int c; a = 1; b = 2; c = 3; int *p; p = &c; int *q; q = p + 2; return *q; }
int test46() { gvar = 42; return gvar; }
int test47() { return 42 < 42; }
int test48() { int a[2]; *a = 1; *(a + 1) = 2; return a[0] + a[1]; }
int test49() { return 5 * (9 - 6); }
int test50() { int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo; }
int test51() { int a; a = 3; int b; b = 5 * 6 - 8; return a + b / 2; }
int test52() { int foo; int bar; foo = 3; bar = 5; return foo + bar; }
int test53() { int *y; return sizeof(y); }
int test54() { int a; int b; a = 1; b = 2; int *p; p = &a; int *q; q = p - 1; return *q; }
int test55() { int a[2]; *a = 1; return a[0]; }
int test56() { return 42 == 42; }
int test57() { int x[2][3]; int *y; y=x[1]; y[2]=5; return x[1][2]; }
int test58() { return -10 + 20; }
int test59() { if (1) return 42; else return 43; }
int test60() { return foo(); }
int test61() { int x; int *y; y = &x; *y = 3; return x; }
int assert(int expected, int actual, char *code);

int main() {
    assert(43, test0(), "if (0) return 42; else return 43;");
    assert(1, test1(), "char x; return sizeof(x);");
    assert(24, test2(), "int *a[3]; return sizeof(a);");
    assert(4, test3(), "return (3 + 5) / 2;");
    assert(21, test4(), "return 5+20-4;");
    assert(0, test5(), "int x[2][3]; int *y; y=x[0]; y[0]=0; return x[0][0];");
    assert(2, test6(), "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &d + 2; return *p;");
    assert(2, test7(), "int x[2][3]; int *y; y=x[0]; y[2]=2; return x[0][2];");
    assert(3, test8(), "int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);");
    assert(5, test9(), "int *****x; return 5;");
    assert(1, test10(), "return 42 <= 42;");
    assert(1, test11(), "int x[2][3]; int *y; y=x[0]; y[1]=1; return x[0][1];");
    assert(1, test12(), "return 42 >= 42;");
    assert(14, test13(), "int a; return 14;");
    assert(42, test14(), "if (1) if (1) return 42; else return 43;");
    assert(3, test15(), "int a[2]; *a = 1; *(a + 1) = 2; return 0[a] + 1[a];");
    assert(42, test16(), "return 10 + 40 - 8;");
    assert(0, test17(), "return 42 != 42;");
    assert(0, test18(), "int x[2][2]; return 0;");
    assert(42, test19(), "int a[10]; return 42;");
    assert(42, test20(), "garr[1] = 42; return garr[1];");
    assert(6, test21(), "int i; for (i = 0; i < 5; i = i + 1) i = i + 1; return i;");
    assert(4, test22(), "int x[2][3]; int *y; y=x[1]; y[1]=4; return x[1][1];");
    assert(3, test23(), "gvar = 0; while (gvar < 3) {increment();} return gvar;");
    assert(3, test24(), "int a[1]; *a = 3; return *a;");
    assert(12, test25(), "int a[3]; return sizeof(a);");
    assert(3, test26(), "int x; x = 3; int *y; y = &x; return *y;");
    assert(42, test27(), "int a; a = 3; return 42;");
    assert(5, test28(), "return add(2, 3);");
    assert(42, test29(), "return 42;");
    assert(0, test30(), "int foo; foo = 3; while(foo > 0) foo = foo - 1; return foo;");
    assert(4, test31(), "int a; int b; int c; int d; a = 1; b = 2; c = 3; d = 4; int *p; p = &c; int *q; q = p - 1; return *q;");
    assert(2, test32(), "int a; a = 1; if(a == 1) { a = a + 1; return a; }");
    assert(8, test33(), "return fibo(5);");
    assert(4, test34(), "return sizeof(1);");
    assert(4, test35(), "int x; return sizeof(x);");
    assert(47, test36(), "return 5 + 6 * 7;");
    assert(0, test37(), "return 42 > 42;");
    assert(12, test38(), "int a[2][3]; return sizeof(*a);");
    assert(1, test39(), "char x; x = 1; return x;");
    assert(4, test40(), "return sizeof(sizeof(1));");
    assert(2, test41(), "int foo; foo = 3; while(foo > 0) if (foo == 2) return foo; else foo = foo - 1; return foo;");
    assert(0, test42(), "return 0;");
    assert(42, test43(), "if (1) return 42;");
    assert(3, test44(), "int x[2][3]; int *y; y=x[1]; y[0]=3; return x[1][0];");
    assert(1, test45(), "int a; int b; int c; a = 1; b = 2; c = 3; int *p; p = &c; int *q; q = p + 2; return *q;");
    assert(42, test46(), "gvar = 42; return gvar;");
    assert(0, test47(), "return 42 < 42;");
    assert(3, test48(), "int a[2]; *a = 1; *(a + 1) = 2; return a[0] + a[1];");
    assert(15, test49(), "return 5 * (9 - 6);");
    assert(11, test50(), "int foo; int bar; int baz; foo = 3; bar = 5; baz = 8; if (foo == 1) return foo + bar; if (foo == 2) return bar + baz; else return baz + foo;");
    assert(14, test51(), "int a; a = 3; int b; b = 5 * 6 - 8; return a + b / 2;");
    assert(8, test52(), "int foo; int bar; foo = 3; bar = 5; return foo + bar;");
    assert(8, test53(), "int *y; return sizeof(y);");
    assert(2, test54(), "int a; int b; a = 1; b = 2; int *p; p = &a; int *q; q = p - 1; return *q;");
    assert(1, test55(), "int a[2]; *a = 1; return a[0];");
    assert(1, test56(), "return 42 == 42;");
    assert(5, test57(), "int x[2][3]; int *y; y=x[1]; y[2]=5; return x[1][2];");
    assert(10, test58(), "return -10 + 20;");
    assert(42, test59(), "if (1) return 42; else return 43;");
    assert(3, test60(), "return foo();");
    assert(3, test61(), "int x; int *y; y = &x; *y = 3; return x;");
    return 0;
}
