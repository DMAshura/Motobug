Timer::Timer() {
	count = 0;
	count_max = 0;
	loop = false;
	active = false;
}

Timer::Timer(int start_count) {
	count = start_count;
	count_max = start_count;
	loop = false;
	active = false;
}

Timer::Timer(int start_count, bool loop) {
	count = start_count;
	count_max = start_count;
	this->loop = loop;
	active = false;
}

unsigned int Timer::get_count() {
	return count;
}

void Timer::set_count(unsigned int count) {
	this->count = count;
}

unsigned int Timer::get_count_max() {
	return count_max;
}

void Timer::set_count_max(unsigned int count_max) {
	this->count_max = count_max
}

unsigned int Timer::tick() {
	if (count > 0) {
		count--;
	}
	if (count == 0) {
		stop();
	}
	return count;
}

bool is_active() {
	return active;
}

void start() {
	active = true;
}

void stop() {
	active = false;
}