
#include "headers.h"

char ts[30] = {0}; //global string for the current time stamp, it will be updated whenever executed getCurrentTimeMsStr(ts)

int64_t getCurrentTimeMs(void) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int64_t now = (int64_t)tp.tv_sec * 1000LL + tp.tv_usec / 1000; //get current timestamp in milliseconds
    return now;
}

int64_t GetCurrentTimeUs(void) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    int64_t now = (int64_t)tp.tv_sec * 1000000LL + tp.tv_usec; //get current timestamp in microseconds
    return now;
}

void getCurrentTimeMsStr(char *str) {
    int64_t epochTimeMs = getCurrentTimeMs();
    time_t rawtime = epochTimeMs / 1000; // time to seconds
    struct tm ts;
    char buf[35];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &ts);
    sprintf(str, "%s.%03dZ", buf, (int)(epochTimeMs % 1000)); //Add milisecond time as well
}