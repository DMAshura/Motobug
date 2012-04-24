#ifndef _TIMER_H_
#define _TIMER_H_

class Timer {
private:
	unsigned int count_max;
	unsigned int count;
	bool loop;
	bool active;

public:
	Timer();
	Timer(int start_count);
	Timer(int start_count, bool loop);
	unsigned int get_count();
	void set_count(unsigned int count);
	unsigned int get_counter_max();
	void set_count_max(unsigned int count_max);
	unsigned int tick();
	bool is_active();
	void start();
	void stop();
};

#endif