#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

int timer_start(int index, int msec);
int timer_is_expired(int index);
int timer_expire(int index);
int timer_cancel(int index);

#endif
