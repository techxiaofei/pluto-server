#include "timer.h"

namespace pluto
{

Timer::Timer(uint32_t timerid, uint32_t repeat_sec, ev_timer* timer):timerid_(timerid),repeat_sec_(repeat_sec),ev_timer_(timer)
{
	
}

Timer::~Timer()
{
	if (ev_timer_)
	{
		free(ev_timer_);
		ev_timer_ = NULL;
	}
}

bool Timer::Repeat()
{
	if (repeat_sec_ > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

}