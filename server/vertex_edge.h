#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_VERTEX_NAME "unnamed"
#define DEFAULT_VERTEX_TYPE POINT_OF_INTEREST
#define DEFAULT_X 0
#define DEFAULT_Y 0

#define DEFAULT_DIRECTION BI_DIRECTIONAL
#define DEFAULT_LENGTH 20
#define DEFAULT_SPEED 30
#define DEFAULT_EDGE_TYPE OPEN

using namespace std;

enum vertexType { POINT_OF_INTEREST = 0, INTERSECTION = 1 };    //vertex type
enum dirType { BI_DIRECTIONAL = 0, V1_TO_V2 = 1, V2_TO_V1 = 2 }; //direction
enum eventType { OPEN = 0, CLOSE = 1, HAZARD = 2 };  //edge event type

class Vertex
{

private:
	string name;

	vertexType type;

	int x;

	int y;

public:

	Vertex(string _name = DEFAULT_VERTEX_NAME, vertexType _type = DEFAULT_VERTEX_TYPE, int _x = DEFAULT_X, int _y = DEFAULT_Y) :name(_name), type(_type), x(_x), y(_y) {} //constructor

	~Vertex() {}    //destructor

	Vertex(const Vertex& p)  // copy constructor
	{
		*this = p;
	}

	Vertex & operator = (const Vertex& p)    //operator =
	{
		if (this == &p) return(*this);
		name = p.name;
		type = p.type;
		x = p.x;
		y = p.y;
		return (*this);
	}


	string getName()
	{
		return name;
	}

	vertexType getType()
	{
		return type;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	void setType(vertexType _type) {
		type = _type;
	}

	void print() {
		cout << "vertex: name = " << name << ", type = " << type << ", x = " << x << ", y = " << y << endl;
	}


};

class Edge
{
private:
	string v1;

	string v2;

	dirType dir;

	int speed;

	int length;

	eventType type;

public:
	Edge(string _v1, string _v2, dirType _dir = DEFAULT_DIRECTION, int _speed = DEFAULT_SPEED, int _length = DEFAULT_LENGTH, eventType _type = DEFAULT_EDGE_TYPE) :
		v1(_v1), v2(_v2), dir(_dir), speed(_speed), length(_length), type(_type) {} //constructor

	~Edge() {}  //destructor

	Edge(const Edge& p)  // copy constructor
	{
		*this = p;
	}

	Edge & operator = (const Edge& p)    //operator =
	{
		if (this == &p) return(*this);
		v1 = p.v1;
		v2 = p.v2;
		dir = p.dir;
		speed = p.speed;
		length = p.length;
		type = p.type;
		return (*this);
	}

	void setEventType(eventType _type)
	{
		type = _type;
	}

	void setSpeed(int _speed)
	{
		speed = _speed;
	}

	string getV1(void)
	{
		return v1;
	}

	string getV2(void)
	{
		return v2;
	}

	dirType getDirection(void)
	{
		return dir;
	}

	int getSpeed(void)
	{
		return speed;
	}

	int getLength(void)
	{
		return length;
	}

	eventType getType(void)
	{
		return type;
	}

	void print() {
		cout << "edge: v1 = " << v1 << ", v2 = " << v2 << ", dir = " << dir << ", speed = " << speed << ", length = " << length << ", eventType = " << type << endl;
	}
};

