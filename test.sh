#!/bin/bash
assert(){
  expected="$1"
  input="$2"

  ./mcc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"
  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
    echo OK;
  else
    echo "$input => $expected expected, but got $actual"
    exit -1
  fi
}

assert 1 "int main(){int a; a = 1; return a;}"
assert 5 "int main(){int a; a=2;if(a==2)a=5;return a;}"
assert 1 "int main(){3==3;}"
assert 0 "int main(){0;}"
assert 42 "int main(){42;}"
assert 21 "int main(){5+    20-4;}"
assert 14 "int main(){3 - 2 + 13;}"
assert 47 "int main(){5+6*7;}"
assert 15 'int main(){5*(9-6);}'
assert 4 'int main(){(3+5)/2;}'
assert 10 'int main(){-10 + 20;}'
assert 1 'int main(){3 == 3;}'
assert 0 'int main(){2 == 3;}'
assert 1 'int main(){2 != 3;}'
assert 1 'int main(){ 2 <= 2;}'
assert 0 'int main(){ 2 < 2;}'
assert 2 'int main(){int foo; foo=2;}'
assert 5 'int main(){int foo; foo=2;foo+3;}'
assert 1 'int main(){int index; index=0; index=index+1; return index;}'
assert 16 'int main(){int foo; foo = 2; int var; var = 3;int s;  s = foo * var;return 2*(s+2);}'
assert 1 "int main(){int a; a=2;if(a==2)a = 1;return a;}"
assert 3 "int main(){int a; a=2;if(a!=2)a = 1;else a=3;return a;}"
assert 4 "int main(){int i; i = 0; while(i < 4) i=i+1; return i;}"
#TODO: for内での関数定義(定義はできるが初期化できないため現状不可)
assert 5 "int main(){int hoge; hoge = 2; int i ;for(i=0; i< 3; i=i+1)hoge = hoge + i; return hoge;}"
assert 2 "int main(){int hoge; int i; hoge = 2; for(;i!=12;)i=12;return hoge;}"
assert 3 "int main(){int a; {a = 2; a = 3;} return a;}"
assert 14 "int main(){int a; a=2;int b; b=4; int i; for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return b;}"
assert 21 "int main(){int a; a=2;int b; b=4;int i; for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return a;}"
#最初の関数が実行される。定義しただけではその後の関数は呼ばれない。
assert 2 "int main(){int a; a=2;return a;} int notmain(){int b; b=4;return b;}"
assert 4 "int main(){int a; a=2;return notmain();} int notmain(){int b; b=4;return b;}"
assert 1 "int main(){int a; a=2;a=func() - a;return a;} int func(){return 3;}"
assert 6 "int main(){int a; a=2;int b; b = notmain();return a+b;} int notmain(){int c; c=4;return c;}"
#main関数以外の関数定義が先
assert 1 "int func(){return 3;}int main(){return func()-2;}"
assert 6 "int main(){int a; a = 3; int b; b = nonmain(1,2); return a+b;} int nonmain(int x, int y){return x+y;}"
assert 2 "int main(){int a; int b; a = 1; b = func(1); return a+b;} int func(int x){int c; c=2;return x;}"
assert 2 "int main(){int a; a=2; int b; b=&a;return *b;}"
echo "ALL OK"