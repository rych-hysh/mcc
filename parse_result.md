```mermaid
flowchart TD
0([ND_ASSIGN])
1([ND_LVAL])
0-- left ---1
3([ND_NUMBER])
0-- right ---3
100([ND_IF])
101([ND_NE])
102([ND_LVAL])
101-- left ---102
205([ND_NUMBER])
101-- right ---205
100-- cond ---101
203([ND_ASSIGN])
204([ND_LVAL])
203-- left ---204
409([ND_NUMBER])
203-- right ---409
100-- then ---203
407([ND_ASSIGN])
408([ND_LVAL])
407-- left ---408
817([ND_NUMBER])
407-- right ---817
100-- else ---407
200([ND_RETURN])
201([ND_LVAL])
200-- left ---201
```
