// util.c

#include <stdio.h>
#include <sys/time.h> // Linux -- different for Windows, probably

#include "defs.h"
#include "util.h"

int get_time_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1000 + t.tv_usec/1000;
}