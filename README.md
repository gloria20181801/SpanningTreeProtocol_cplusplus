# SpanningTreeProtocol_c++
a C++ file to implement spanning tree protocol and the learning bridges algorithm
The input will be given in the following format
The name of the bridges will be Bk, where k is an integer. The lan segments will be named as the single character (usually the a single character upper letter case), trace_flag will control the prining of the trace or not. The lan segments connected to the particular bridge is given after the : and a space eg. B1: A B C D
sample input
~~~
1
5
B1: A G B
B2: G F
B3: B C
B4: C F E
B5: C D E
~~~
The corresponding output will be a spanning tree of lan segment and the bridges given in output as:
~~~
B1: A-DP  B-DP  G-DP
B2: G-RP  F-DP
B3: B-RP  C-DP
B4: C-NP  F-RP   E-DP  
B5: C-RP  D-DP  E-NP
~~~
In the above output the NP-> null port, DP->designated port, RP->root port

After this, the program will read a list of host IDs per LAN and a set of data transfer end points which will be specified as follows
~~~
A: H1 H2 H3
B: H4 H5
C: H6 H7 H8
D: H9 H10 
E: H11
F: H12 H13
G: H14
3
H9 H2
H4 H12
H3 H9
~~~

3: This means that 3 transfers will be specified. 
H9 H2  : means Host H9 is sending to Host H2
And so on

Then the program will print the forwarding table of each bridge after the particular transfer executed: 
~~~
B1: 
HOST ID | FORWARDING PORT
H9 | B 
B2:
HOST ID | FORWARDING PORT
H9 | G
B3:
HOST ID | FORWARDING PORT
H9 | C
B4:
HOST ID | FORWARDING PORT
H9 | F
B5:
HOST ID | FORWARDING PORT
H9 | D

B1: 
HOST ID | FORWARDING PORT
H9 | B 
H4 | B
B2:
HOST ID | FORWARDING PORT
H9 | G
H4 | G
B3:
HOST ID | FORWARDING PORT
H9 | C
H4 | B
B4:
HOST ID | FORWARDING PORT
H9 | F
H4 | F
B5:
HOST ID | FORWARDING PORT
H9 | D
H4 | C

B1: 
HOST ID | FORWARDING PORT
H9 | B 
H4 | B
H3 | A
B2:
HOST ID | FORWARDING PORT
H9 | G
H4 | G
B3:
HOST ID | FORWARDING PORT
H9 | C
H4 | B
H3 | B
B4:
HOST ID | FORWARDING PORT
H9 | F
H4 | F
B5:
HOST ID | FORWARDING PORT
H9 | D
H4 | C
H3 | C
~~~
