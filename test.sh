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
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

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
assert 1 "a=2;if(a==2);"
echo OK