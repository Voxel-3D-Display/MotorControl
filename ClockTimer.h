#ifndef CLOCKTIMER_H
#define CLOCKTIMER_H

class ClockTimer {
protected:
	uint32_t _prev_time;
	uint32_t _period;
public:
	ClockTimer(const uint32_t period) : _prev_time(0), _period(period) {}

	bool ready(uint32_t time = micros()){
		if(abs(time - _prev_time) >= _period){
			if(_prev_time + _period < time) _prev_time += _period;
			else _prev_time = time;
			return 1;
		} else {
			return 0;
		}
	}

	void set_period(const uint32_t period) { _period = period; }

	uint32_t get_prev_time(){ return _prev_time; }
};

class ClockTimerf : public ClockTimer {
public:
	ClockTimerf(const float frequency) : ClockTimer((uint32_t) (1000000 / frequency)) {}
	void set_frequency (const float frequency) { set_period((uint32_t) (1000000 / frequency)); }
};

#endif