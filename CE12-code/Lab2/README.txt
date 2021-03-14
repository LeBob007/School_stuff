------------------------
LAB 2: Intro to Logic Simulation
CMPE 012 Summer 2018

Hu, Robert
ryhu
-------------------------

Part B Truth Table:

in_0 in_1 in_2 | a0 a1 b0 b1 c0 c1 d0 d1 e0 e1 f0 f1 g0 g1
----------------------------------------------------------
0    0    0    | 1  1  1  1  1  1  1  1  1  1  1  1  0  0
0    0    1    | 0  0  1  1  1  1  0  0  0  0  0  0  0  0
0    1    0    | 1  1  1  1  0  0  1  1  1  1  0  0  1  1
0    1    1    | 1  1  1  1  1  1  1  1  0  0  0  0  1  1
1    0    1    | 1  1  0  0  1  1  1  1  0  0  1  1  1  1
1    0    0    | 0  0  1  1  1  1  0  0  0  0  1  1  1  1
1    1    0    | 1  1  0  0  1  1  1  1  1  1  1  1  1  1
1    1    1    | 1  1  1  1  1  1  0  0  0  0  0  0  0  0

Part C Truth Table:

in_0 in_1 in_2 | out_0 out_1 out_2 out_3 out_4 out_5 out_6 out_7
----------------------------------------------------------------
0    0    0    |   1     0     0     0     0     0     0     0
0    0    1    |   0     1     0     0     0     0     0     0
0    1    0    |   0     0     1     0     0     0     0     0
0    1    1    |   0     0     0     1     0     0     0     0
1    0    0    |   0     0     0     0     1     0     0     0
1    0    1    |   0     0     0     0     0     1     0     0
1    1    0    |   0     0     0     0     0     0     1     0
1    1    1    |   0     0     0     0     0     0     0     1

What did you learn in this lab?
From this lab, I learned how to use the basics of multimedia logic and how to take a logic expression from equation form to make a visual structure.

What worked well? Did you encounter any issues?
I had a bit of trouble trying to understand how the 7 segment LED worked, especially while using only 3 switches. Figuring out part B and part C were a lot simpler for me.

How would you redesign this lab to make it better?
Maybe add a portion to the lab that is only worth one point for creating a logic circuit based on a logic equation with an accompanying truth table at the start of the lab to help people get familiarized with the lab better.