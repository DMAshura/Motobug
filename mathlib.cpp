#include "mathlib.h"

int sign(double x) {
	return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

int sign(int x) {
	return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

double clamp(double x, double minvalue, double maxvalue) {
	return x-((maxvalue-minvalue)*floor((x-minvalue)/(maxvalue-minvalue)));
}

{ // Vector class
Vector::Vector() {}

Vector::Vector(double x1, double y1, double x2, double y2) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	if (x1==x2 && y1==y2) {
		angle = 0;
	} else {
		angle = (double)(atan2((double)(y2-y1), (double)(x2-x1)) * RAD_TO_DEG);
	}
	type = TYPE_SOLID;
}

Vector::Vector(double x1, double y1, double x2, double y2, int type) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	if (x1==x2 && y1==y2) {
		angle = 0;
	} else {
		angle = (double)(atan2((double)(y2-y1), (double)(x2-x1)) * RAD_TO_DEG);
	}
	this->type = type;
}

Vector::Vector(Point p1, Point p2) {
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y1 = p2.y;
	if (x1==x2 && y1==y2) {
		angle = 0;
	} else {
		angle = (double)(atan2((double)(y2-y1), (double)(x2-x1)) * RAD_TO_DEG);
	}
	this->type = TYPE_SOLID;
}

Vector::Vector(Point p1, Point p2, int type) {
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y1 = p2.y;
	if (x1==x2 && y1==y2) {
		angle = 0;
	} else {
		angle = (double)(atan2((double)(y2-y1), (double)(x2-x1)) * RAD_TO_DEG);
	}
	this->type = type;
}

Vector::Vector(Point p1, double length, double angle) {
	x1 = p1.x;
	y1 = p1.y;
	x2 = p1.x + length * cos(angle * DEG_TO_RAD);
	y2 = p1.y + length * sin(angle * DEG_TO_RAD);
	if (x1==x2 && y1==y2) {
		this->angle = 0;
	} else {
		this->angle = angle;
	}
	type = TYPE_SOLID;
}

Vector::Vector(Point p1, double length, double angle, int type) {
	x1 = p1.x;
	y1 = p1.y;
	x2 = p1.x + length * cos(angle * DEG_TO_RAD);
	y2 = p1.y + length * sin(angle * DEG_TO_RAD);
	if (x1==x2 && y1==y2) {
		this->angle = 0;
	} else {
		this->angle = angle;
	}
	this->type = type;
}

void Vector::set_anchor(Point p) {
	double length = get_length();
	x1 = p.x;
	y1 = p.y;
	x2 = x1 + length * cos(angle * DEG_TO_RAD);
	y2 = y1 + length * sin(angle * DEG_TO_RAD);
}

double Vector::get_angle() {
	return angle;
}

void Vector::set_angle(double angle) {
	double length = get_length();
	x2 = x1 + length * cos(angle * DEG_TO_RAD);
	y2 = y1 + length * sin(angle * DEG_TO_RAD);
	if (x1==x2 && y1==y2) {
		this->angle = 0;
	} else {
		this->angle = angle;
	}
}
	

double Vector::get_inward_normal() {
	return clamp(angle+90, 0, 360);
}

int Vector::get_type() {
	return type;
}

double Vector::get_length() {
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

void Vector::set_length(double length) {
	x2 = x1 + length * cos(angle * DEG_TO_RAD);
	y2 = y1 + length * sin(angle * DEG_TO_RAD);
	if (x1==x2 && y1==y2) {
		angle = 0;
	}
}

double Vector::get_length_x() {
	return x2-x1;
}

double Vector::get_length_y() {
	return y2-y1;
}
	
void Vector::set_type(int type) {
	this->type = type;
}

void Vector::set_vector(double x1, double y1, double x2, double y2) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	if (x1==x2 && y1==y2) {
		angle = 0;
	} else {
		angle = (double)(atan2((double)(y2-y1), (double)(x2-x1)) * RAD_TO_DEG);
	}
}

bool Vector::intersects(Vector other) {
	return (this->get_intersection_t_value(other) != -1);
}

Point Vector::get_intersection_point(Vector other) {
	Point p;
	p.x = -1;
	p.y = -1;
	double t = this->get_intersection_t_value(other);
	if (t != -1) {
		p.x = this->x1 + t * (this->x2 - this->x1);
		p.y = this->y1 + t * (this->y2 - this->y1);
	}
	return p;
}

double Vector::get_intersection_t_value(Vector other) {
	float t_numer_self, t_numer_other, t_denom;
	t_numer_self = (other.x2 - other.x1)*(this->y1 - other.y1) -
		(other.y2 - other.y1)*(this->x1 - other.x1);
	t_numer_other = (this->x2 - this->x1)*(this->y1 - other.y1) -
		(this->y2 - this->y1)*(this->x1 - other.x1);
	t_denom = (other.y2 - other.y1)*(this->x2 - this->x1) -
		(other.x2 - other.x1)*(this->y2 - this->y1);

	if (t_denom == 0) {
		if (t_numer_self == 0 && t_numer_other == 0) {
			return -1; // Vectors coincide
		} else {
			return -1; // Vectors are parallel
		}
	}

	if ((t_numer_self/t_denom >= 0 && t_numer_self/t_denom <= 1) &&
	    (t_numer_other/t_denom >= 0 && t_numer_other/t_denom <= 1))
		return t_numer_self/t_denom; // Vectors intersect

	return -1; // Vectors do not intersect
}
}