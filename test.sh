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
    exit 1
  fi
}

assert 5 "a=2;if(a==2)a=5;return a;"
assert 1 "3==3;"
assert 0 "0;"
assert 42 "42;"
assert 21 "5+    20-4;"
assert 14 "3 - 2 + 13;"
assert 47 "5+6*7;"
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10 + 20;'
assert 1 '3 == 3;'
assert 0 '2 == 3;'
assert 1 '2 != 3;'
assert 1 ' 2 <= 2;'
assert 0 ' 2 < 2;'
assert 2 'foo=2;'
assert 5 'foo=2;foo+3;'
assert 1 'index=0; index=index+1; return index;'
assert 16 'foo = 2; var = 3; s = foo * var;return 2*(s+2);'
assert 1 "a=2;if(a==2)a = 1;return a;"
assert 3 "a=2;if(a!=2)a = 1;else a=3;return a;"
assert 4 "i = 0; while(i < 4) i=i+1; return i;"
assert 5 "hoge = 2; for(i=0; i< 3; i=i+1)hoge = hoge + i; return hoge;"
assert 2 "hoge = 2; for(;i!=12;)i=12;return hoge;"
assert 3 "{a = 2; a = 3;} return a;"
assert 14 "a=2;b=4;for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return b;"
assert 21 "a=2;b=4;for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return a;"
echo "ALL OK"