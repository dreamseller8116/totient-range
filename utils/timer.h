#ifndef TIMER_H
#define TIMER_H

typedef struct {
    int sec;    // Seconds
    int nano;   // Nanoseconds
} Time;

/**
 * Retrieve current wallclock value
 * 
 * @return      Returns a Time struct for the current wallclock value
 */
Time wcTime();

/**
 * Calculate elapsed time
 * 
 * @param start     A Time struct for the start time
 * @param top       A Time struct for the stop time
 * 
 * @return          Returns the elapsed time
 */
double elapsedTime(Time start, Time stop);

#endif /* TIMER_H */
