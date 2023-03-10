#ifndef TIMER_EVENT__
#define TIMER_EVENT__

#include <stdlib.h>

typedef enum
{
    TIMER_SINGLE_SHOT = 0,
    TIMER_PERIODIC
} t_timer;

typedef void (*time_handler)(size_t timer_id, void * user_data);

int     initialize();
size_t  start_timer(unsigned int interval, time_handler handler, t_timer type, void * user_data);
void    stop_timer(size_t timer_id);
void    finalize();


extern char ts[30]; 

void getCurrentTimeMsStr(char *str);

#endif