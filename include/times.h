#ifndef __times_h
#define __times_h

#define second_ms 1000
#define minute_ms 60 * second_ms
#define hour_ms 60 * minute_ms
#define day_ms 24 * hour_ms

#define minute_sec 60
#define hour_sec 60 * minute_sec
#define day_sec 24 * hour_sec

#define ms_us 1000
#define second_us 1000000
#define min_us 60 * second_us
#define hour_us 60 * min_us
#define day_us 24 * hour_us

#define millis_us(ms) (ms_us * ms)
#define seconds_us(second) (second_us * second)
#define minutes_us(minute) (min_us * minute)
#define hours_us(hour) (hour_us * hour)
#define days_us(day) (day_us * day)

#define seconds_ms(second) (second_ms * second)
#define minutes_ms(minute) (minute_ms * minute)
#define hours_ms(hour) (hour_ms * hour)
#define days_ms(day) (day_ms * day)

#define minutes_sec(minute) (minute_sec * minute)
#define hours_sec(hour) (hour_sec * hour)
#define days_sec(day) (day_sec * day)

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS us_seconds(SLEEP_TIME_SECONDS)
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg

#endif