#ifndef _TERRAIN_H_
#define _TERRAIN_H_ 1

#include <vector>
#include "mathlib.h"

enum CornerType { 
	CORNER_TOPLEFT = 0,
	CORNER_TOPRIGHT = 1,
	CORNER_BOTTOMRIGHT = 2,
	CORNER_BOTTOMLEFT = 3
};

class Terrain {
	public:
		Terrain(int x, int y, unsigned int width, unsigned int height);
		//Terrain(Point p, unsigned int width, unsigned int height);
		virtual ~Terrain();
		
		//Misc Functions
		virtual bool blit();
		
		//Geometry management functions
		Vector* add_vector(Point p1, Point p2);
		Vector* add_vector(Point p1, Point p2, int type);
		Vector* add_vector(unsigned, unsigned, unsigned, unsigned);
		Vector* add_vector(unsigned, unsigned, unsigned, unsigned, int);
		
		virtual void del_vector(unsigned int index);
		
		
		//point* add_vertex(unsigned int X, unsigned int Y);
		//virtual void del_vertex(unsigned int index);
		//virtual void mov_vertex(unsigned int index, unsigned int nx, unsigned int ny);

		virtual void resize(unsigned int new_width, unsigned int new_height);
		virtual void move(point new_p);
		void reset_corner_array();
	
		//std::vector<point> vertices;
		static std::vector<Vector> vector_list;
		
	protected:
		Point origin;
		unsigned int width, height;
		Point corner_array[4];
};

class Rectangle: protected Terrain {
	public:
		Rectangle(int x, int y, unsigned int width, unsigned int height);
		Rectangle(Point p, unsigned int width, unsigned int height);
		virtual ~Rectangle();
};

class Triangle: protected Terrain {
	public:		
		Triangle(int x, int y, unsigned int width, unsigned int height, CornerType type);
		Triangle(Point p, unsigned int width, unsigned int height, CornerType type)
		virtual ~Triangle();
	private:
		CornerType type;
};
#endif
