# CS3D3 Project 2018

# Routing Algorithm Implementation (Bellman Ford)

I did this project during my third year of Computer Engineering at Trinity College Dublin. I learnt socket programming in C very fast and proficiently due to this course in Computer Networks, and was very proud at that time to pull it off at coding level.<br>

## Goal
Design and deploy a working demonstration of routing algortihm (Bellman Ford).
The purpose of this project is to implement, validate and demonstrate a simple Distance Vector protocol,in C/C++, of the type that might be used by Peer to Peer, Adhoc, Wireless Sensor or Internet of Things communication devices. You should use UDP Sockets and the C/C++ programming language to build a distance vector routing protocol that implements the distributed Bellman-Ford algorithm.

![Image of routers](https://github.com/DarkThr0n3/markdown-demo/raw/master/routers.png)

## Implementation

Main code is written in file <strong>final_router_code.c</strong><br>
Basically there are 6 routers, so I designed to run the set up such as 6 C programs will run, each for each router, as if the code is running inside the router's terminal. But in reality there are no real routers involved but only a make file which runs the C code 6 instances in parallel, for each router.<br>
If a router has to communicate with other router (one router's C program with another router's C program), they do so by sending UDP packets to each other (through local host/computer where all the 6 program are running).<br>
<br>
So with this setup, 6 (virtual) routers are set up and running. They discover directly connected neighbours (I have manually set realationship among themselves in 6x6 matrix and then they know who are they directly connected to). Routers have no idea of other router's existence. They only learn about other routers through routing algorithm which is implemented (Bellman Ford Routing Algo), and hence they decide where to forward an incoming packet to which link, based on the algorithm, which aims to provide <strong> shortest path possible.</strong><br>
Even if we terminate one of the program (virtually it means, if we remove a router), other C programs (or routers)<strong> will notice it and change their forwarding table according to the structure :-)</strong> <br>

Code is well documented. I have used multi-threading in combiantion with other functions to send UDP packets among routers. 

## Achievement
1. Fastest student in class to implement and obtain highest marks for this project.
2. Code is able to handle router addition/deletion to find new path shortest path between 2 routers by applying bellman algorithm.
3. Good implementation to show use of multi-threading.
4. Good implementation to depict use of C sockets.


