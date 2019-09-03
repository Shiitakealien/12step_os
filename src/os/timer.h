#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define TIMER_START_FLAG_CYCLE (1<<0)

int timer_start(int index, int msec, int flags);
int timer_is_expired(int index);
int timer_expire(int index);
int timer_cancel(int index);
int timer_is_running(int index);
int timer_gettime(int index);

#endif
