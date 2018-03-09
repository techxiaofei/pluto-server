#ifndef __PLUTO_TIMER_H__
#define __PLUTO_TIMER_H__

#include <ev.h>  // for libev
#include "util.h"

namespace pluto
{

class Timer
{

public:
	Timer(uint32_t timerid, uint32_t repeat_sec, ev_timer* timer);
	~Timer();

	uint32_t TimerId() {return timerid_;}
	struct ev_timer* EV_Timer(){return ev_timer_;}
	//uint32_t Delay(){return delay_;}
	bool Repeat();

private:
	uint32_t timerid_;
	//uint32_t delay_;
	uint32_t repeat_sec_;  //if 0, no repeat
	struct ev_timer* ev_timer_;

};

}

#endif