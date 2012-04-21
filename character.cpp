#include "character.h"

std::vector<Character> Character::character_list;

Sensor::Sensor() {
	sensor_vector = Vector(0,0,0,0);
	t_closest = -1;
	active = false;
}

Sensor::Sensor(double x1, double y1, double x2, double y2) {
	sensor_vector = Vector(x1, y1, x2, y2);
	t_closest = -1;
	active = false;
}

Sensor::Sensor(Point p, double length, double angle) {
	sensor_vector = Vector(p, length, angle);
	t_closest = -1;
	active = false;
}

void Sensor::set_anchor(Point p) {
	sensor_vector.set_anchor(p);
}

void Sensor::set_angle(double angle) {
	sensor_vector.set_angle(angle);
}

double Sensor::get_angle() {
	return sensor_vector.get_angle();
}

void Sensor::set_length(double length) {
	sensor_vector.set_length(angle);
}

double Sensor::get_length() {
	return sensor_vector.get_length();
}

double Sensor::get_length_x() {
	return sensor_vector.get_length_x();
}

double Sensor::get_length_y() {
	return sensor_vector.get_length_y();
}

bool Sensor::is_active() {
	return active;
}

double Sensor::get_t_closest() {
	return t_closest;
}

double Sensor::set_t_closest(double t) {
	t_closest = t;
}

void Sensor::reset_t_closest() {
	t_closest = -1;
}

void Sensor::activate() {
	active = true;
}

void Sensor::deactivate() {
	active = false;
}

double Sensor::get_detected_angle() {
	return detected_angle;
}

void Sensor::set_detected_angle(double angle) {
	detected_angle = angle;
}

bool Sensor::intersects(Vector other) {
	return sensor_vector.intersects(other);
}

Point Sensor::get_intersection_point(Vector other) {
	return sensor_vector.get_intersection_point(other);
}

double Sensor::get_intersection_t_value(Vector other) {
	return sensor_vector.get_intersection_t_value(other);
}



Character::Character() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	
	// Sonic-specific constants
	acc = 0.046875;
	dec = 0.5;
	frc = 0.046875;
	xmx = 6.0;
	rol = 1.03125;
	rfr = 0.5 * frc; // rfr is always 1/2 frc, even when modified with the Game Genie.
	rdc = 0.125;
	jmp = -6.5;
	jrl = -4.0;
	grv = 0.21875;
	air = 0.09375;
	amx = 16.0;
	drg = 0.96875;
	slp = 0.125;
	sld = 0.078125;
	slu = 0.3125;
	wac = 0.0234375;
	wdc = 0.25;
	wfc = 0.0234375;
	wrf = 0.01171875;
	wmx = 3.0;
	war = 0.046875;
	wgr = 0.0625;
	wjp = -3.5;
	wjr = -2.0;
	
	max_slide_angle = 30;
	max_switch_angle = 45;
	
	mode_angle = MODE_DOWN;
	gravity_angle = MODE_DOWN;
	
	state = STATE_STANDING;
	
	speed_x = 0;
	speed_y = 0;
	speed_ground = 0;
	speed_air = 0;
	angle = 0;
	
	sensor_ground_length = 17;
	sensor_ground_l = Sensor(0,0,sensor_ground_length,clamp(mode_angle + DIR_DOWN, 0, 360));
	sensor_ground_r = Sensor(0,0,sensor_ground_length,clamp(mode_angle + DIR_DOWN, 0, 360));
	sensor_ground_spacing = 9;
	
	sensor_ceiling_length = 17;
	sensor_ceiling_l = Sensor(0,0,sensor_ceiling_length,clamp(mode_angle + DIR_UP, 0, 360));
	sensor_ceiling_r = Sensor(0,0,sensor_ceiling_length,clamp(mode_angle + DIR_UP, 0, 360));
	sensor_ceiling_spacing = 9;
	
	sensor_wall_length = 10;
	sensor_wall_l = Sensor(0,0,sensor_wall_length,clamp(mode_angle + DIR_LEFT, 0, 360));
	sensor_wall_r = Sensor(0,0,sensor_wall_length,clamp(mode_angle + DIR_RIGHT, 0, 360));
	
	input_queue_length = 8;
	
	for(int i = 0; i < input_queue_length; i++) {
		input_queue.push_back(0);
	}
}

Character::~Character() {
}

bool Character::pressed_button(int button) {
	return (input_queue[0]&button && !(input_queue[1]&button));
}

bool Character::holding_button(int button) {
	return (input_queue[0]&button);
}

bool Character::released_button(int button) {
	return (!(input_queue[0]&button) && input_queue[1]&button);
}

void Character::loop() {
	calculate_movement();
	
	perform_ground_movement(speed_ground *  cos(angle * DEG_TO_RAD),
	                        speed_ground * -sin(angle * DEG_TO_RAD));
							
	update_sensors();
	
	perform_air_movement(speed_air * sin(angle * DEG_TO_RAD),
	                     speed_air * cos(angle * DEG_TO_RAD));
	
	update_sensors();
	
	// Switch angle modes if detected angle is too steep
	if(abs(get_ground_angle()) > max_switch_angle){
		mode_angle = clamp(mode_angle + 90 * sign(get_ground_angle()), 0, 360);
		update_sensors();
	}
	
	//animate();
}

void Character::calculate_movement() {
	// Try to roll
	if (pressed_button(BUTTON_DOWN)) {
		roll();
	}
	
	// Use correct constants
		update_constants();
	
	// If character is no longer on the ground, convert his momentum to the default gravity at the time.
	// If not, make him hug the ground.
	if(!flag_grounded) {
		convert_momentum(gravity_angle);
		angle = gravity_angle;
	} else {
		touch_ground();
	}
	
	// Add slope factor
	if(speed_ground != 0 || abs(get_ground_angle()) > max_slide_angle)
		speed_ground -= slope_factor * sin(get_ground_angle() * DEG_TO_RAD);
		
	update_sensors();
	
	// Horizontal motion
	if(holding_button(BUTTON_LEFT)) {
		if (speed_ground > 0) {
			if(speed_ground - decel < 0) {
				speed_ground = -decel;
			} else {
				speed_ground += -decel;
			}
		} else if (speed_ground > -max_speed && state != STATE_ROLLING) {
			speed_ground += -accel;
			if (speed_ground < -max_speed) speed_ground = -max_speed;
		}
	} else if(holding_button(BUTTON_RIGHT)) {
		if (speed_ground < 0) {
			if(speed_ground - decel > 0) {
				speed_ground = decel;
			} else {
				speed_ground += decel;
			}
		} else if (speed_ground < max_speed && state != STATE_ROLLING) {
			speed_ground += accel;
			if (speed_ground > max_speed) speed_ground = max_speed;
		}
	}

	// Friction
	if(state == STATE_ROLLING || (!holding_button(BUTTON_LEFT) && !holding_button(BUTTON_RIGHT))) {
		speed_ground -= min(abs(speed_ground), friction) * sign(speed_ground);
	}

	// Unroll
	if(state == STATE_ROLLING && speed_ground == 0) {
		state = holding_button(BUTTON_DOWN) ? STATE_CROUCHING : STATE_STANDING;
	}


	// Air drag
	if(speed_air < 0 && speed_air > -4) {
		if(abs(speed_ground) >= 0.125) speed_ground *= drg;
	}

	// Check if walked off a platform
	if(!sensor_ground_l.is_active() && !sensor_ground_r.is_active()) {
		flag_grounded = false;
	}

	// Vertical motion
	if(!flag_grounded) {
		speed_air += gravity;
		if(speed_air > amx) speed_air = amx;
	}

	// Variable jump height
	if(state == STATE_JUMPING && released_button(BUTTON_JUMP) && speed_air < -4) {
		state = STATE_FALLING;
		speed_air = -4;
	}
}

void Character::perform_ground_movement(double move_x, double move_y) {
	x += move_x;
	y += move_y;
}

void Character::perform_air_movement(double move_x, double move_y) {
	x += move_x;
	y += move_y;
	
	update_sensors();
	
	if(speed_air >= 0 &&
		(sensor_ground_l.active() && 2*sensor_ground_l.get_t_closest()*sensor_ground_length < height ||
		 sensor_ground_r.active() && 2*sensor_ground_r.get_t_closest()*sensor_ground_length < height))
	{
		touch_ground();
	}
}

void Character::update_constants() {
	if(flag_underwater) {
		accel = (flag_grounded ? wac : war);
		decel = wdc;
		friction = ((state == STATE_ROLLING) ? wrf : wfc);
		max_speed = wmx;
		gravity = wgr;
		jump_strength = wjp;
		jump_release = wjr;
	} else if((state == STATE_ROLLING) && flag_grounded) {
		decel = rdc;
		friction = rfr;
	} else {
		accel = (flag_grounded ? acc : air);
		decel = dec;
		friction = frc;
		max_speed = xmx;
		gravity = grv;
		jump_strength = jmp;
		jump_release = jrl;
	}
	slope_factor = (state == STATE_ROLLING) ? (sign(sin(get_ground_angle())) == sign(speed_ground)
		? sld : slu ) : slp;
}

void Character::update_sensors() {
	// Set anchor points of floor and ceiling sensors based on mode_angle
	switch(mode_angle) {
		case MODE_DOWN: {
			sensor_ground_l.set_anchor(Point{x-sensor_ground_spacing, y});
			sensor_ground_r.set_anchor(Point{x+sensor_ground_spacing, y});
			sensor_ceiling_l.set_anchor(Point{x-sensor_ceiling_spacing, y});
			sensor_ceiling_r.set_anchor(Point{x+sensor_ceiling_spacing, y});
			break; }
		case MODE_LEFT: {
			sensor_ground_l.set_anchor(Point{x, y-sensor_ground_spacing});
			sensor_ground_r.set_anchor(Point{x, y+sensor_ground_spacing});
			sensor_ceiling_l.set_anchor(Point{x, y-sensor_ceiling_spacing});
			sensor_ceiling_r.set_anchor(Point{x, y+sensor_ceiling_spacing});
			break; }
		case MODE_UP: {
			sensor_ground_l.set_anchor(Point{x+sensor_ground_spacing, y});
			sensor_ground_r.set_anchor(Point{x-sensor_ground_spacing, y});
			sensor_ceiling_l.set_anchor(Point{x+sensor_ceiling_spacing, y});
			sensor_ceiling_r.set_anchor(Point{x-sensor_ceiling_spacing, y});
			break; }
		case MODE_RIGHT: {
			sensor_ground_l.set_anchor(Point{x, y+sensor_ground_spacing});
			sensor_ground_r.set_anchor(Point{x, y-sensor_ground_spacing});
			sensor_ceiling_l.set_anchor(Point{x, y+sensor_ceiling_spacing});
			sensor_ceiling_r.set_anchor(Point{x, y-sensor_ceiling_spacing});
			break; }
	}
	
	// Set anchor points of wall sensors
	sensor_wall_l.set_anchor(Point{x, y});
	sensor_wall_r.set_anchor(Point{x, y});
	
	// Set sensor angles to go in the right directions relative to the mode_angle
	sensor_ground_l.set_angle(clamp(mode_angle + DIR_DOWN,0,360));
	sensor_ground_r.set_angle(clamp(mode_angle + DIR_DOWN,0,360));
	sensor_ceiling_l.set_angle(clamp(mode_angle + DIR_UP,0,360));
	sensor_ceiling_r.set_angle(clamp(mode_angle + DIR_UP,0,360));
	sensor_wall_l.set_angle(clamp(mode_angle + DIR_LEFT,0,360));
	sensor_wall_r.set_angle(clamp(mode_angle + DIR_RIGHT,0,360));
	
	// Set maximum t positions
	sensor_ground_l.set_t_closest(1);
	sensor_ground_r.set_t_closest(1);
	sensor_ceiling_l.set_t_closest(1);
	sensor_ceiling_r.set_t_closest(1);
	sensor_wall_l.set_t_closest(1);
	sensor_wall_r.set_t_closest(1);
	
	// Deactivate all sensors
	sensor_ground_l.deactivate();
	sensor_ground_r.deactivate();
	sensor_ceiling_l.deactivate();
	sensor_ceiling_r.deactivate();
	sensor_wall_l.deactivate();
	sensor_wall_r.deactivate();
	
	// Set default detected angle of all sensors relative to the mode_angle
	sensor_ground_l.set_detected_angle(clamp(mode_angle + MODE_DOWN,0,360));
	sensor_ground_r.set_detected_angle(clamp(mode_angle + MODE_DOWN,0,360));
	sensor_ceiling_l.set_detected_angle(clamp(mode_angle + MODE_UP,0,360));
	sensor_ceiling_r.set_detected_angle(clamp(mode_angle + MODE_UP,0,360));
	sensor_wall_l.set_detected_angle(clamp(mode_angle + MODE_LEFT,0,360));
	sensor_wall_r.set_detected_angle(clamp(mode_angle + MODE_RIGHT,0,360));
	
	// Check for the closest terrain angle by iterating through all terrain
	for(int i = 0; i < (int)Terrain::terrain_list.size(); i++) {
		for(int j = 0; j < Terrain::terrain_list[i]->vector_list.size(); j++) {
			double t_other;
			
			// Initialize data about the Vector being tested
			int type_other = Terrain::terrain_list[i]->vector_list[j].get_type();
			double angle_other = Terrain::terrain_list[i]->vector_list[j].get_angle();
			double inward_normal_other = Terrain::terrain_list[i]->vector_list[j].get_inward_normal();
			
			// Check left ground sensor
			t_other = sensor_ground_l.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_ground_l.get_angle() - inward_normal_other) > 0))) {
								
				sensor_ground_l.activate();
				if t_other < sensor_ground_l.get_t_closest() {
					sensor_ground_l.set_t_closest(t_other);
					sensor_ground_l.set_detected_angle(angle_other);
				}
			}
			
			// Check right ground sensor
			t_other = sensor_ground_r.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_ground_r.get_angle() - inward_normal_other) > 0))) {
								
				sensor_ground_r.activate();
				if t_other < sensor_ground_r.get_t_closest() {
					sensor_ground_r.set_t_closest(t_other);
					sensor_ground_r.set_detected_angle(angle_other);
				}
			}
			
			// Check left ceiling sensor
			t_other = sensor_ceiling_l.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_ceiling_l.get_angle() - inward_normal_other) > 0))) {
								
				sensor_ceiling_l.activate();
				if t_other < sensor_ceiling_l.get_t_closest() {
					sensor_ceiling_l.set_t_closest(t_other);
					sensor_ceiling_l.set_detected_angle(angle_other);
				}
			}
			
			// Check right ceiling sensor
			t_other = sensor_ceiling_r.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_ceiling_r.get_angle() - inward_normal_other) > 0))) {
								
				sensor_ceiling_r.activate();
				if t_other < sensor_ceiling_r.get_t_closest() {
					sensor_ceiling_r.set_t_closest(t_other);
					sensor_ceiling_r.set_detected_angle(angle_other);
				}
			}
			
			// Check left wall sensor
			t_other = sensor_wall_l.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_wall_l.get_angle() - inward_normal_other) > 0))) {
								
				sensor_wall_l.activate();
				if t_other < sensor_wall_l.get_t_closest() {
					sensor_wall_l.set_t_closest(t_other);
					sensor_wall_l.set_detected_angle(angle_other);
				}
			}
			
			// Check right wall sensor
			t_other = sensor_wall_r.get_intersection_t_value(Terrain::terrain_list[i]->vector_list[j]);
			if(t_other != -1 && (type_other == TYPE_SOLID || (type_other == TYPE_SEMISOLID &&
				cos(sensor_wall_r.get_angle() - inward_normal_other) > 0))) {
								
				sensor_wall_r.activate();
				if t_other < sensor_wall_r.get_t_closest() {
					sensor_wall_r.set_t_closest(t_other);
					sensor_wall_r.set_detected_angle(angle_other);
				}
			}
		}
	}	
	
	// If not touching any terrain, set to default t positions
	if(!(sensor_ground_l.is_active()))
		sensor_ground_l.set_t_closest(height/2 / sensor_ground_length);
	if(!(sensor_ground_r.is_active()))
		sensor_ground_r.set_t_closest(height/2 / sensor_ground_length);
	if(!(sensor_ceiling_l.is_active()))
		sensor_ceiling_l.set_t_closest(height/2 / sensor_ceiling_length);
	if(!(sensor_ceiling_r.is_active()))
		sensor_ceiling_r.set_t_closest(height/2 / sensor_ceiling_length);
}

void Character::touch_ground() {
	double t_ground_l = sensor_ground_l.get_t_closest();
	double t_ground_r = sensor_ground_r.get_t_closest();
	double detected_angle_ground_l = sensor_ground_l.get_detected_angle();
	double detected_angle_ground_r = sensor_ground_r.get_detected_angle();
	if(sensor_ground_l.is_active() && sensor_ground_r.is_active()) {
		if(t_ground_l < t_ground_r) {
			x += t_ground_l*sensor_ground_l.get_length_x() -
				sign(sensor_ground_l.get_length_x())*(double)width/2;
			y += t_ground_l*sensor_ground_l.get_length_y() -
				sign(sensor_ground_l.get_length_y())*(double)height/2;
			if(!flag_grounded)
				convert_momentum(detected_angle_ground_l);
			angle = detected_angle_ground_l;
		} else {
			x += t_ground_r*sensor_ground_r.get_length_x() -
				sign(sensor_ground_r.get_length_x())*(double)width/2;
			y += t_ground_r*sensor_ground_r.get_length_y() -
				sign(sensor_ground_r.get_length_y())*(double)height/2;
			if(!flag_grounded)
				convert_momentum(detected_angle_ground_r);
			angle = detected_angle_ground_r;
		}
	} else {
		if(sensor_ground_l.is_active()) {
			x += t_ground_l*sensor_ground_l.get_length_x() -
				sign(sensor_ground_l.get_length_x())*(double)width/2;
			y += t_ground_l*sensor_ground_l.get_length_y() -
				sign(sensor_ground_l.get_length_y())*(double)height/2;
			if(!flag_grounded)
				convert_momentum(detected_angle_ground_l);
			angle = detected_angle_ground_l;
		} else if(sensor_ground_r.is_active()) {
			x += t_ground_r*sensor_ground_r.get_length_x() -
				sign(sensor_ground_r.get_length_x())*(double)width/2;
			y += t_ground_r*sensor_ground_r.get_length_y() -
				sign(sensor_ground_r.get_length_y())*(double)height/2;
			if(!flag_grounded)
				convert_momentum(detected_angle_ground_r);
			angle = detected_angle_ground_r;
		}
	}
	flag_grounded = true;
	speed_air = 0;
}

void Character::convert_momentum(double new_angle) {
	double Gsp = speed_ground;
	double Asp = speed_air;
	double Cos = cos((angle - new_angle) * DEG_TO_RAD);
	double Sin = sin((angle - new_angle) * DEG_TO_RAD);
	speed_ground = Gsp * Cos - Asp * Sin;
	speed_air    = Gsp * Sin + Asp * Cos;
}

double Character::get_ground_angle() {
	double theta = (angle - mode_angle);
	return clamp(theta, -180, 180);
}

bool Character::roll() {
	if(!flag_grounded || state == STATE_ROLLING || abs(speed_ground) < rol) {
		return false;
	}
	
	state = STATE_ROLLING;

	return true;
}

bool Character::jump() {
	if(!flag_grounded)
		return false;

	flag_grounded = false;
	state = STATE_JUMPING;

	speed_air = jump_strength;

	return true;
}