#ifndef __times_h
#define __times_h

#define us_mili_second 1e3
#define us_second 1e6
#define us_minute 60 * us_second
#define us_hour 60 * us_minute
#define us_day 24 * us_hour

#define ms_second 1e3
#define ms_minute 60 * ms_second
#define ms_hour 60 * ms_minute
#define ms_day 24 * ms_hour

#define s_minute 60
#define s_hour 60 * s_minutes
#define s_day 24 * s_hours

#define us_milis(ms) (us_mili_second * ms)
#define us_seconds(second) (us_second * second)
#define us_minutes(minute) (us_minute * minute)
#define us_hours(hour) (us_hour * hour)
#define us_days(day) (us_day * day)

#define ms_seconds(second) (ms_second * second)
#define ms_minutes(minute) (ms_minute * minute)
#define ms_hours(hour) (ms_hour * hour)
#define ms_days(day) (ms_day * day)

#define s_minutes(minute) (s_minute * minute)
#define s_hours(hour) (s_hour * hour)
#define s_days(day) (s_day * day)

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS us_seconds(SLEEP_TIME_SECONDS)
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg

#endif