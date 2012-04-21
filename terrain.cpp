#include <SDL/SDL.h>
#include "terrain.h"

Terrain::Terrain(int x, int y, unsigned int width, unsigned int height){
	origin.x = x;
	origin.y = y;
	this->width = width;
	this->height = height;
	corner_array[CORNER_TOPLEFT]     = Point{x,      y};
	corner_array[CORNER_TOPRIGHT]    = Point{x+width,y};
	corner_array[CORNER_BOTTOMRIGHT] = Point{x+width,y+height};
	corner_array[CORNER_BOTTOMLEFT]  = Point{x,      y+height};
}

Vector* Terrain::add_vector(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	Vector i(x1, y1, x2, y2);
	vector_list.push_back(i);
	return &vector_list.back();
}

Vector* Terrain::add_vector(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int type) {
	Vector i(x1, y1, x2, y2, type);
	vector_list.push_back(i);
	return &vector_list.back();
}

Vector* Terrain::add_vector(Point p1, Point p2) {
	Vector i(p1.x, p1.y, p2.x, p2.y);
	vector_list.push_back(i);
	return &vector_list.back();
}

Vector* Terrain::add_vector(Point p1, Point p2, int type) {
	Vector i(p1.x, p1.y, p2.x, p2.y, type);
	vector_list.push_back(i);
	return &vector_list.back();
}

void Terrain::del_vector(unsigned int index) {
	vector_list.erase(vector_list.begin() + index);
}

void Terrain::move(point new_p) {
	origin = new_p;
	reset_corner_array();
}

void Terrain::reset_corner_array() {
	corner_array[CORNER_TOPLEFT]     = Point{x,      y};
	corner_array[CORNER_TOPRIGHT]    = Point{x+width,y};
	corner_array[CORNER_BOTTOMRIGHT] = Point{x+width,y+height};
	corner_array[CORNER_BOTTOMLEFT]  = Point{x,      y+height};
}

void Terrain::resize(unsigned int new_width, unsigned int new_height) {
	width = new_width;
	height = new_height;
}

Rectangle::Rectangle(int x, int y, unsigned int width, unsigned int height) :
Terrain(x, y, width, height) {
	add_vector(corner_array[CORNER_TOPLEFT],corner_array[CORNER_TOPRIGHT]);
	add_vector(corner_array[CORNER_TOPRIGHT],corner_array[CORNER_BOTTOMRIGHT]);
	add_vector(corner_array[CORNER_BOTTOMRIGHT],corner_array[CORNER_BOTTOMLEFT]);
	add_vector(corner_array[CORNER_BOTTOMLEFT],corner_array[CORNER_TOPLEFT]);
}

Rectangle::Rectangle(Point p, unsigned int width, unsigned int height) :
Terrain(p.x, p.y, width, height) {
	add_vector(corner_array[CORNER_TOPLEFT],corner_array[CORNER_TOPRIGHT]);
	add_vector(corner_array[CORNER_TOPRIGHT],corner_array[CORNER_BOTTOMRIGHT]);
	add_vector(corner_array[CORNER_BOTTOMRIGHT],corner_array[CORNER_BOTTOMLEFT]);
	add_vector(corner_array[CORNER_BOTTOMLEFT],corner_array[CORNER_TOPLEFT]);
}

Triangle::Triangle(int x, int y, unsigned int width, unsigned int height, CornerType type) :
Terrain(x, y, width, height) {
	// Starts with corner vertex, goes to next vertex clockwise
	add_vector(corner_array[type],corner_array[(type + 1)%4]);
	// Skips one vertex since it's a triangle
	add_vector(corner_array[(type + 1)%4],corner_array[(type + 3)%4]);
	// Returns to original vertex
	add_vector(corner_array[(type + 3)%4],corner_array[type]);
}

Triangle::Triangle(Point p, unsigned int width, unsigned int height, CornerType type) :
Terrain(p.x, p.y, width, height) {
	// Starts with corner vertex, goes to next vertex clockwise
	add_vector(corner_array[type],corner_array[(type + 1)%4]);
	// Skips one vertex since it's a triangle
	add_vector(corner_array[(type + 1)%4],corner_array[(type + 3)%4]);
	// Returns to original vertex
	add_vector(corner_array[(type + 3)%4],corner_array[type]);
}