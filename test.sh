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

assert 2 "main(){a=2;return a;}"
assert 5 "main(){a=2;if(a==2)a=5;return a;}"
assert 1 "main(){3==3;}"
assert 0 "main(){0;}"
assert 42 "main(){42;}"
assert 21 "main(){5+    20-4;}"
assert 14 "main(){3 - 2 + 13;}"
assert 47 "main(){5+6*7;}"
assert 15 'main(){5*(9-6);}'
assert 4 'main(){(3+5)/2;}'
assert 10 'main(){-10 + 20;}'
assert 1 'main(){3 == 3;}'
assert 0 'main(){2 == 3;}'
assert 1 'main(){2 != 3;}'
assert 1 'main(){ 2 <= 2;}'
assert 0 'main(){ 2 < 2;}'
assert 2 'main(){foo=2;}'
assert 5 'main(){foo=2;foo+3;}'
assert 1 'main(){index=0; index=index+1; return index;}'
assert 16 'main(){foo = 2; var = 3; s = foo * var;return 2*(s+2);}'
assert 1 "main(){a=2;if(a==2)a = 1;return a;}"
assert 3 "main(){a=2;if(a!=2)a = 1;else a=3;return a;}"
assert 4 "main(){i = 0; while(i < 4) i=i+1; return i;}"
assert 5 "main(){hoge = 2; for(i=0; i< 3; i=i+1)hoge = hoge + i; return hoge;}"
assert 2 "main(){hoge = 2; for(;i!=12;)i=12;return hoge;}"
assert 3 "main(){{a = 2; a = 3;} return a;}"
assert 14 "main(){a=2;b=4;for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return b;}"
assert 21 "main(){a=2;b=4;for(i = 0; i< 5; i=i+1){a = a+1;b=b+2;}a = a+b;return a;}"
#最初の関数が実行される。定義しただけではその後の関数は呼ばれない。
assert 2 "main(){a=2;return a;} notmain(){b=4;return b;}"
assert 4 "main(){a=2;return notmain();} notmain(){b=4;return b;}"
assert 1 "main(){a=2;a=func() - a;return a;}func(){return 3;}"
assert 6 "main(){a=2;b = notmain();return a+b;} notmain(){c=4;return c;}"
#main関数以外の関数定義が先
assert 1 "func(){return 3;}main(){return func()-2;}"
assert 6 "main(){a = 3; b = nonmain(1,2); return a+b;} nonmain(x, y){return x+y;}"
assert 2 "main(){a = 1; b = func(1); return a+b;} func(x){c=2;return x;}"
assert 2 "main(){a=2;b=&a;return *b;}"
echo "ALL OK"