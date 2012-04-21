#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <vector>

#include "mathlib.h"

enum CharacterStates {
	STATE_STANDING,
	STATE_WAITING,
	STATE_WALKING,
	STATE_RUNNING,
	STATE_LOOKINGUP,
	STATE_CROUCHING,
	STATE_ROLLING,
	STATE_SPINDASHING,
	STATE_JUMPING,
	STATE_FALLING,
	STATE_HURT,
	STATE_DEAD
};

enum InputButtons {
	BUTTON_UP    = 1 << 0,
	BUTTON_DOWN  = 1 << 1,
	BUTTON_LEFT  = 1 << 2,
	BUTTON_RIGHT = 1 << 3,
	BUTTON_A     = 1 << 4,
	BUTTON_B     = 1 << 5,
	BUTTON_C     = 1 << 6,
	BUTTON_START = 1 << 7
};

class Sensor{
private:
	Vector sensor_vector;
	double t_closest;
	bool active;
	double detected_angle;
	
public:
	Sensor();
	Sensor(double x1, double y1, double x2, double y2);
	Sensor(Point p, double length, double angle);
	void set_anchor(Point p);
	void set_angle(double angle);
	double get_angle();
	void set_length(double length);
	double get_length();
	double get_length_x();
	double get_length_y();
	bool is_active();
	double get_t_closest();
	double set_t_closest(double t);
	void reset_t_closest();
	void activate();
	void deactivate();
	double get_detected_angle();
	void set_detected_angle(double angle);
	bool intersects(Vector other);
	Point get_intersection_point(Vector other);
	double get_intersection_t_value(Vector other);
};

class Character {
public:
	static vector<Character*> character_list;
	
public:
	Character();
	virtual ~Character();
	
public:
	double x;					// Actually represents the CENTER x
	double y;					// Actually represente the CENTER y
	int width;					// Accounts for rotation - if rotated 90º, his "width" is vertical
	int height;					// Accounts for rotation - if rotated 90º, his "height" is horizontal
	double speed_ground;		// Speed parallel to ground
	double speed_air;			// Speed perpendicular to ground
	double angle;				// Angle of momentum
	
	double accel;				// Current Acceleration
	double decel;				// Current Deceleration
	double friction;			// Current Friction
	double max_speed;			// Current Maximum X Speed
	double gravity;				// Current Gravity
	double jump_strength;		// Current Jump Strength
	double jump_release;		// Current Jump Release Strength
	double slope_factor;		// Current Slope Factor
	double mode_angle;			// Current Mode Angle
	double gravity_angle;		// Current Gravity Angle
	
	double max_slide_angle;		// Maximum angle at which a character must slide down a slope
	double min_switch_angle;	// Minimum angle at which character switches modes
	
	int state;					// Current state of character.
	
	// Genesis constants
	double acc;         		// Acceleration
	double dec;         		// Deceleration
	double frc;         		// Friction
	double xmx;         		// Maximum X Speed
	double rol;         		// Minimum Roll Speed
	double rfr;         		// Rolling Friction
	double rdc;         		// Rolling Deceleration
	double jmp;         		// Jump Strength
	double jrl;         		// Jump Release Strength
	double grv;         		// Gravity Strength
	double air;         		// Air Acceleration
	double amx;         		// Maximum Fall Speed
	double drg;         		// Air Drag
	double slp;         		// Slope Factor
	double slu;         		// Slope Factor when rolling up a hill
	double sld;         		// Slope Factor when rolling down a hill
	double wac;         		// Water Acceleration
	double wdc;         		// Water Deceleration
	double wfc;         		// Water Friction
	double wrf;         		// Water Rolling Friction
	double wmx;         		// Water Maximum X Speed
	double war;         		// Water Air Acceleration
	double wgr;         		// Water Gravity
	double wjp;         		// Water Jump Strength
	double wjr;         		// Water Jump Release Strength

protected:
	double speed_x;				// Speed in x direction
	double speed_y;				// Speed in y direction
	vector<int> input_queue;	// Keeps track of button status
	int input_queue_length;		// How many frames to keep track of button status for
	
public:
	// Flags
	bool flag_grounded;			// Is character on the ground?
	bool flag_underwater;		// Is character underwater?
	bool pressed_button(int button);
	bool holding_button(int button);
	bool released_button(int button);
	
	// Methods
	void loop();				// Actions to do on each game loop
	void calculate_movement();	// Figure out main actions like running, rolling, etc
	void perform_ground_movement(double move_x, double move_y);
								// Move along the ground.
	void perform_air_movement(double move_x, double move_y);
								// Move in the air.
	double get_ground_angle();	// Find the angle of the terrain relative to the mode_angle
	void update_constants();	// Assign correct constants for air, underwater, etc
	void update_sensors();		// Reset sensor positions and directions
	void convert_momentum(double new_angle);
								// For changing momentum back to normal after jumping off a slope
	void touch_ground();		// Cause character to hug the ground
	
	// Actions
	bool roll();
	bool jump();
	
public:
	// Sensors
	Sensor sensor_ground_l, sensor_ground_r;
	Sensor sensor_ceiling_l, sensor_ceiling_r;
	Sensor sensor_wall_l, sensor_wall_r;
		
	// Sensor variables
	int sensor_ground_length, sensor_ground_spacing;
	int sensor_ceiling_length, sensor_ceiling_spacing;
	int sensor_wall_length;
};

#endif