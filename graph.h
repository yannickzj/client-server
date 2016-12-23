#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include "vertex_edge.h"
#include "stdlib.h"
#include <climits>

#define BUFFERSIZE 256
#define VERTEX_STR "#vertex"
#define EDGE_STR "#edge"
#define PATH_STR "#path"
#define END_STR "#end"
#define PREV_DEFAULT ""
#define HOUR_TO_SECOND 3600

using namespace std;
enum tripType { SHORTEST = 0, FASTEST = 1 };

struct Node // use in priority queue.
{
	long priority;
	string label;
	bool operator < (const Node& n) const {
		return this->priority > n.priority;
	}
};

class Graph {
private:
	string name;

	map<string, vector<string> > adjOutList;

	map<string, vector<string> > adjInList;

	map<string, Vertex> vertexMap;

	map<string, Edge> edgeMap;

	map<string, vector<string> > roadMap;

	static int numVertex;

	static int numEdge;

	static int numRoad;

	vector<string> split(string str, string pattern) {  //  split function

		string::size_type start = 0;
		string::size_type end;
		vector<string> result;
		str += pattern;
		string::size_type size = str.size();

		while (start < size) {
			end = str.find(pattern, start);
			if (end > start) {
				string substr = str.substr(start, end - start);
				result.push_back(substr);
			}
			start = end + pattern.size();
		}

		return result;
	}

	void retrieveVertex(string str) {
		//cout << str << endl;
		vector<string> strSplit = split(str, " ");
		string name = strSplit[0];
		int typeInt = atoi(strSplit[1].c_str());
		int x = atoi(strSplit[2].c_str());
		int y = atoi(strSplit[3].c_str());

		vertexType type;
		if (typeInt == 0 || typeInt == 1) {
			type = (vertexType)typeInt;
		}
		else {
			cout << "vertexType does not match in \"" << str << "\"!" << endl;
		}

		addVertex(name, type, x, y);
	}

	void retrieveEdge(string str) {
		//cout << str << endl;
		vector<string> strSplit = split(str, " ");
		string name = strSplit[0];
		string v1 = strSplit[1];
		string v2 = strSplit[2];
		int dirInt = atoi(strSplit[3].c_str());
		int speed = atoi(strSplit[4].c_str());
		int length = atoi(strSplit[5].c_str());
		int eventTypeInt = atoi(strSplit[6].c_str());

		dirType dir;
		if (dirInt == 0 || dirInt == 1 || dirInt == 2) {
			dir = (dirType)dirInt;
		}
		else {
			cout << "dirType does not match in \"" << str << "\"!" << endl;
		}

		eventType type;
		if (eventTypeInt == 0 || eventTypeInt == 1) {
			type = (eventType) eventTypeInt;
		}
		else {
			cout << "eventType does not match in \"" << str << "\"!" << endl;
		}

		addEdge(name, v1, v2, dir, speed, length, type);
	}

	void retrievePath(string str) {
		//cout << str << endl;
		vector<string> strSplit = split(str, " ");
		string name = strSplit[0];
		vector<string> edges;
		int len = strSplit.size();
		for (int i = 1; i < len; i++) {
			edges.push_back(strSplit[i]);
		}
		road(name, edges);
	}


public:
	Graph() {}  //constructor

	Graph(string _name) : name(_name) {}    //constructor

	~Graph() {} //destructor

	string getName() {
			return name;
	}

	Vertex* vertex(string name) {   //check the vertex whether is in map or not
		if (containsVertex(name)) {
			return &vertexMap.at(name);
		}
		else {
			cout << "The graph does not contain the vertex \"" << name << "\"!" << endl;
			return NULL;
		}
	}

	Edge* getEdge(string name) { ////check the edge whether is in map or not
		if (containsEdge(name)) {
			return &edgeMap.at(name);
		}
		else {
			cout << "The graph does not contain the edge \"" << name << "\"!" << endl;
			return NULL;
		}
	}

	vector<string> getAdjOutVertex(string v) {//get outdegree of vertex v
		map<string, vector<string> >::iterator iter = adjOutList.find(v);
		vector<string> vertices;

		if (iter != adjOutList.end()) {
			int num = iter->second.size();
			for (int i = 0; i < num; i++) {
				vertices.push_back(getV2(v, iter->second[i]));
			}
		}
		return vertices;
	}

	vector<string> getAdjInVertex(string v) {//get indegree of vertex v
		map<string, vector<string> >::iterator iter = adjInList.find(v);
		vector<string> vertices;

		if (iter != adjInList.end()) {
			int num = iter->second.size();
			for (int i = 0; i < num; i++) {
				vertices.push_back(getV2(v, iter->second[i]));
			}
		}
		return vertices;
	}

	vector<string>* getRoad(string name) { ////check the road whether is in map or not
		if (containsRoad(name)) {
			return &roadMap.at(name);
		}
		else {
			cout << "The graph does not contain the road \"" << name << "\"!" << endl;
			exit(1);
		}
	}

	string getV2(string v1, string edge) {//get the other vertex of the edge
		Edge* p = getEdge(edge);
		if (p == NULL) {
			cout << "the graph does not contain the edge \"" << edge << "\"!" << endl;
			exit(1);
		}
		if (p->getV1() == v1) {
			return p->getV2();
		}
		else if (p->getV2() == v1) {
			return p->getV1();
		}
		else {
			cout << "the edge " << edge << " does not contain the vertex " << v1 << " !" << endl;
			exit(1);
		}
	}

	string getEdgeByVertex(string v1, string v2) {//get the edge according to the vertexes.
		map<string, vector<string> >::iterator iter = adjOutList.find(v1);
		string edge;
		if (iter != adjOutList.end()) {
			int num = iter->second.size();
			for (int i = 0; i < num; i++) {
				string e = iter->second[i];
				if (getV2(v1, e) == v2) {
					edge = e;
					return edge;
				}
			}
		}
		cout << "cannot find the edge connecting " << v1 << " and " << v2 << endl;
		exit(1);
	}

	int getEdgeLength(string label) {
		Edge* p = getEdge(label);
		if (p == NULL) {
			cout << "the graph does not contain the edge \"" << label << "\"!" << endl;
			exit(1);
		}
		return p->getLength();
	}

	int getEdgeSpeed(string label) {
		Edge* p = getEdge(label);
		if (p == NULL) {
			cout << "the graph does not contain the edge \"" << label << "\"!" << endl;
			exit(1);
		}
		return p->getSpeed();
	}

	eventType getEdgeEvent(string label) {
		Edge* p = getEdge(label);
		if (p == NULL) {
			cout << "the graph does not contain the edge \"" << label << "\"!" << endl;
			exit(1);
		}
		return p->getType();
	}

	void addVertex(string label, vertexType type, int x, int y)
	{
		Vertex newVertex(label, type, x, y);
		vertexMap.insert(make_pair(label, newVertex));
		vector <string> newList1;
		vector <string> newList2;
		adjOutList.insert(make_pair(label, newList1));
		adjInList.insert(make_pair(label, newList2));
		numVertex++;
	}

	void addEdge(string label, string v1, string v2, dirType dir, int speed, int length, eventType type)
	{
		if (!containsVertex(v1) || !containsVertex(v2)) {
			cout << "error: edge vertex does not exist!" << endl;
			return;
		}

		Edge newEdge(v1, v2, dir, speed, length, type);
		edgeMap.insert(make_pair(label, newEdge));

		if (dir == V1_TO_V2) {
			adjOutList[v1].push_back(label);
			adjInList[v2].push_back(label);
		}

		if (dir == V2_TO_V1) {
			adjOutList[v2].push_back(label);
			adjInList[v1].push_back(label);
		}

		if (dir == BI_DIRECTIONAL)
		{
			adjOutList[v1].push_back(label);
			adjOutList[v2].push_back(label);
			adjInList[v1].push_back(label);
			adjInList[v2].push_back(label);
		}
		numEdge++;
	}

	void road(string name, vector<string> edges) {
		roadMap.insert(make_pair(name, edges));
		numRoad++;
	}

	void edgeEvent(string label, eventType type)
	{
		Edge* e = getEdge(label);
		if (e == NULL) {
			cout << "the graph does not contain the edge \"" << label << "\"!" << endl;
			exit(1);
		}
		e->setEventType(type);
	}

	bool trip(string fromVertex, string toVertex, string label, tripType type = SHORTEST) { //if type=shortest find the shortest trip between v1 and v2, if type=fastest find the fastest trip between v1 and v2
		// declaration
		priority_queue<Node> queue;
		map<string, long> priorityMap;
		map<string, string> prevMap;
		vector<string> source;
		Node n;
		map<string, Vertex>::iterator iterVertex;
		map<string, long>::iterator iterPriority;
		map<string, string>::iterator iterPrev;

		//initialize all the vertex priority values
		for (iterVertex = vertexMap.begin(); iterVertex != vertexMap.end(); iterVertex++) {
			priorityMap.insert(make_pair(iterVertex->first, LONG_MAX));
			prevMap.insert(make_pair(iterVertex->first, PREV_DEFAULT));
		}

		//dijkstra algorithm
		iterPriority = priorityMap.find(fromVertex);
		iterPriority->second = 0;
		n.priority = 0;
		n.label =fromVertex;
		queue.push(n);

		while (!queue.empty()) {
			Node node = queue.top();
			string u = node.label;
			source.push_back(u);

			map<string, vector<string> >::iterator iter = adjOutList.find(u);
			vector<string> list = iter->second;
			int num = list.size();
			long priorityU = node.priority;

			for (int i = 0; i < num; i++) {
				int length = getEdgeLength(list[i]);
				int speed = getEdgeSpeed(list[i]);
				long priorityDif;
				if (type == SHORTEST) {
					priorityDif = length;
				}
				else if (type == FASTEST) {
					priorityDif = length * HOUR_TO_SECOND / speed;
				}
				else {
					cout << "tripType not defined!" << endl;
					exit(1);
				}

				string v = getV2(u, list[i]);
				iterPriority = priorityMap.find(v);
				long priorityV = iterPriority->second;
				if (priorityV > priorityU + priorityDif && getEdgeEvent(list[i]) != CLOSE) {
					iterPriority->second = priorityU + priorityDif;
					iterPrev = prevMap.find(v);
					iterPrev->second = u;
					n.priority = iterPriority->second;
					n.label = v;
					queue.push(n);
				}
			}

			queue.pop();
		}

		// trace the path from fromVertex to toVertex
		vector<string> edges;
		vector<string>::iterator iterEdge;
		string trace = toVertex;
		iterPrev = prevMap.find(trace);
		string prev = iterPrev->second;
		while (prev != PREV_DEFAULT) {
			iterPrev = prevMap.find(trace);
			string edge = getEdgeByVertex(prev, trace);

			iterEdge = edges.begin();
			edges.insert(iterEdge, edge);

			trace = prev;
			iterPrev = prevMap.find(trace);
			prev = iterPrev->second;
		}

		if (trace == fromVertex) {
			string suffix;
			if (type == SHORTEST) {
				//cout << "shortest trip from " << fromVertex << " to " << toVertex << " is found!" << endl;
				suffix = "_shortest";
			}
			else if (type == FASTEST) {
				//cout << "fastest trip from " << fromVertex << " to " << toVertex << " is found!" << endl;
				suffix = "_fastest";
			}
			road(label + suffix, edges);
			return true;
		}

		if (type == SHORTEST) {
			cout << "cannot find the shortest trip!" << endl;
		}
		else if (type == FASTEST) {
			cout << "cannot find the fastest trip!" << endl;
		}
		else {
			cout << "tripType not defined!" << endl;
			exit(1);
		}

		return false;
	}

	bool containsVertex(string label)   //check the map contains the specific vertex?
	{
		map<string, Vertex>::iterator it1;
		it1 = vertexMap.find(label);
		if (it1 != vertexMap.end())
		{
			return true;
		}
		return false;
	}

	bool containsEdge(string label)//check the map contains the specific edge?
	{
		map<string, Edge>::iterator it1;
		it1 = edgeMap.find(label);
		if (it1 != edgeMap.end())
		{
			return true;
		}
		return false;
	}

	bool containsRoad(string label) {//check the map contains the specific road?
		map<string, vector<string> >::iterator it1;
		it1 = roadMap.find(label);
		if (it1 != roadMap.end())
		{
			return true;
		}
		return false;
	}

	void store(string filename) {   //output function
		ofstream outfile;
		outfile.open(filename.c_str());

		//store the vertex information
		outfile << "#vertex: name; vertex_type; x; y" << endl;
		map<string, Vertex>::iterator iterVertex;
		for (iterVertex = vertexMap.begin(); iterVertex != vertexMap.end(); iterVertex++) {
			outfile << iterVertex->first << " " << iterVertex->second.getType() << " " << iterVertex->second.getX() << " " << iterVertex->second.getY() << endl;
		}
		outfile << endl;

		//store the edge information
		outfile << "#edge: name; v1_name; v2_name; directional; speed; length; eventType" << endl;
		map<string, Edge>::iterator iterEdge;
		for (iterEdge = edgeMap.begin(); iterEdge != edgeMap.end(); iterEdge++) {
			outfile << iterEdge->first << " " << iterEdge->second.getV1() << " " << iterEdge->second.getV2() << " " << iterEdge->second.getDirection() << " " << iterEdge->second.getSpeed() << " " << iterEdge->second.getLength() << " " << iterEdge->second.getType() << endl;
		}
		outfile << endl;

		//store the path information
		outfile << "#path: name; edge1_name; edge2_name; ...; edgeN_name" << endl;
		map<string, vector<string> >::iterator iterRoad;
		for (iterRoad = roadMap.begin(); iterRoad != roadMap.end(); iterRoad++) {
			outfile << iterRoad->first;
			vector<string> edges = iterRoad->second;
			int num = edges.size();
			for (int i = 0; i < num; i++) {
				outfile << " " << edges[i];
			}
			outfile << endl;
		}
		outfile << endl;
		outfile << "#end" << endl;
		outfile.close();

	}

	void retrieve(string filename) {    //input function
		ifstream infile;
		infile.open(filename.c_str());

		if (!infile.good()) {
			cout << "cannot open the input file!" << endl;
		}

		char* buffer = new char[BUFFERSIZE];
		infile.getline(buffer, BUFFERSIZE);
		string s = buffer;

		while (!infile.eof() && s.find(END_STR) == string::npos) {

			if (s.find(VERTEX_STR) != string::npos) {
				infile.getline(buffer, BUFFERSIZE);
				s = buffer;

				while (s.find("#") == string::npos && !infile.eof()) {
					if (!s.empty()) {
						retrieveVertex(s);
					}
					infile.getline(buffer, BUFFERSIZE);
					s = buffer;
				}
				cout << endl;
			}

			if (s.find(EDGE_STR) != string::npos) {
				infile.getline(buffer, BUFFERSIZE);
				s = buffer;

				while (s.find("#") == string::npos && !infile.eof()) {
					if (!s.empty()) {
						retrieveEdge(s);
					}
					infile.getline(buffer, BUFFERSIZE);
					s = buffer;
				}
				cout << endl;
			}

			if (s.find(PATH_STR) != string::npos) {
				infile.getline(buffer, BUFFERSIZE);
				s = buffer;

				while (s.find("#") == string::npos && !infile.eof()) {
					if (!s.empty()) {
						retrievePath(s);
					}
					infile.getline(buffer, BUFFERSIZE);
					s = buffer;
				}
				cout << endl;
			}

			infile.getline(buffer, BUFFERSIZE);
			s = buffer;

		}

		delete buffer;

		infile.close();
	}

	static int getNumVertex() {
		return numVertex;
	}

	static int getNumEdge() {
		return numEdge;
	}

	static int getNumRoad() {
		return numRoad;
	}

};

int Graph::numEdge = 0;

int Graph::numVertex = 0;

int Graph::numRoad = 0;
