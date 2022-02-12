#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }
int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF

assert() {
    expected="$1"
    input="$2"

    ./mcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 24 "5 + 20 - 1;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10++20;'
assert 1 "1== 1;"
assert 0 " 2 == 3 ;"
assert 0 "1 != 1;"
assert 1 "1 < 2;"
assert 1 "1 <= 1;"
assert 1 "2 > 1;"
assert 1 "1 >= 1;"
assert 4 "a = 3 + 3; b = 5 - 1;"
assert 7 "z = 3 + 3;a = 2 - 1;2 + z - a ;"
assert 11 "ab = 3;ab = 3;cd = 5;df = 3; return ab + cd + df;"
assert 4 "return 4;"
assert 14  "a = 3;
b = 5 * 6 - 8;
return a + b / 2;"
assert 4 "a = 3;b = 4; if(a == b -1) return 4;"
assert 5 "a = 3;b = 4; if(a != b -1) return 4; else return 5;"
assert 6 "a = 3;b = 4; if(a != b -1) return 4; else if ( a == 3) if ( b == 4) return 6;"
assert 3 "i = 0; 
while( i != 3 ) i = i+ 1;"
assert 2 "i = 0;
while( i > 3) return 3;
return 2;"
assert 4 "i = 0; j = 0; for( i = 0; i <4; i = i + 1) j = j+1;
return j;
"
assert 3 "i = 0; for(;i < 3;)i = i + 1;"
assert 25 "j = 0; k = 0;for(i = 0; i < 5;i = i + 1 ) {
    j = 2 + j ;
    k = 3 + k ;
    }
    return j + k;
    "
assert 30 "j = 0; k = 0;for(i = 0; i < 5;i = i + 1 ) {
    j = 2 + j ;
    k = 3 + k ;
    }
    if ( i == 5){
        j = 2+ j;
        k = 3 + k;
    }
    return j + k;
    "

#assert 8 '{ return add(3, 5); }'
#assert 2 '{ return sub(5, 3); }'
#assert 21 '{ return add6(1,2,3,4,5,6); }'
#assert 66 '{ return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
#assert 136 '{ return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }'

echo OK