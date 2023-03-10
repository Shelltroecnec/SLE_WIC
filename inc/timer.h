#ifndef TIMER_H
#define TIMER_H

extern char ts[30]; 

void getCurrentTimeMsStr(char *str);
int64_t getCurrentTimeMs(void);
int64_t GetCurrentTimeUs(void);

#endif