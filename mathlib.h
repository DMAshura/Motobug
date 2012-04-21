#ifndef _MATHLIB_H_
#define _MATHLIB_H_

#define PI 3.14159265358
#define RAD_TO_DEG 57.29577951308
#define DEG_TO_RAD 0.01745329251

#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

enum VectorTypes {
	TYPE_NONSOLID = 0,
	TYPE_SEMISOLID = 1,
	TYPE_SOLID = 2
};

enum Modes {
	MODE_DOWN = 0,
	MODE_LEFT = 90,
	MODE_UP = 180,
	MODE_RIGHT = 270
};

enum Directions {
	DIR_RIGHT = 0,
	DIR_DOWN = 90,
	DIR_LEFT = 180,
	DIR_UP = 270
};

// Useful mathematical functions

int sign(double x);
int sign(int x);
double clamp(double x, double minvalue, double maxvalue);

// Point struct

typedef struct point{
	float x;
	float y;
} Point;

// Vector class

class Vector {
public:
	double x1, y1; // Starting position of vector
	double x2, y2; // Ending point of vector

private:
	double angle; // Stores the angle at which the vector points
	int type; // Stores the solidity type of the vector (for terrain collisions)
	
public:
	Vector();
	Vector(double x1, double y1, double x2, double y2);
	Vector(double x1, double y1, double x2, double y2, int type);
	Vector(Point p1, Point p2);
	Vector(Point p1, Point p2, int type);
	Vector(Point p1, double length, double angle);
	Vector(Point p1, double length, double angle, int type);
	void set_anchor(Point p);
	double get_angle(); // 0 is straight forward, 90 is down, etc
	void set_angle(double angle);
	double get_inward_normal(); // Gives vector pointing inside a platform.
	int get_type();
	double get_length();
	void set_length(double length);
	double get_length_x();
	double get_length_y();
	void set_type(int type);
	void set_vector(double x1, double y1, double x2, double y2);
	bool intersects(Vector other);
	Point get_intersection_point(Vector other);
	double get_intersection_t_value(Vector other); // Gives value between 0 and 1 if there is a collision
	                                               // and -1 if there is no collision
};

#endif