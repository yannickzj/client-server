#include <iostream>
#include "graph.h"
#include <queue>

using namespace std;


int main(int argc, char** argv) {
	
	Graph m1;
	m1.retrieve("./test cases/test1.txt");
	m1.vertex("ccjil");
	m1.edgeEvent("E26",CLOSE);
	m1.trip("CLV", "340", "path1", SHORTEST);
	m1.edgeEvent("E26",OPEN);
	m1.trip("CLV", "340", "path1", FASTEST);
	m1.addVertex("RCH",POINT_OF_INTEREST,4,5);
	m1.addEdge("additional","RCH","CLV",V1_TO_V2,50,2,OPEN);
	m1.vertex("CLN");
	m1.store("output1.txt");

	Graph m2;
	m2.retrieve("./test cases/test1.txt");
	m2.trip("CLV", "375", "path1", SHORTEST);
	m2.trip("CLV", "375", "path1", FASTEST);
	vector<string> *a = new vector<string> ;
	m2.road("test_road",*a);
	m2.store("output2.txt");

	return 0;
}
