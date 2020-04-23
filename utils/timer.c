// Anthony Genson  23/04/2020

#include <time.h>

#ifdef OSX
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include "timer.h"

Time wcTime() {
    struct timespec result;
    Time time;

    #ifdef OSX // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;

        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);

        result.tv_sec = mts.tv_sec;
        result.tv_nsec = mts.tv_nsec;
    #else
        clock_gettime(CLOCK_REALTIME, &result);
    #endif

    time.sec = (int) result.tv_sec;
    time.nano = (int) result.tv_nsec;

    return time;
}

double elapsedTime(Time start, Time stop) {
    return (stop.sec - start.sec) * 1000.0 + (double)(stop.nano - start.nano) / 1000000.0;
}
