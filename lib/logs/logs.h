

#ifndef __LOGS_H
#define __LOGS_H
void _empty();
// #define DISABLE_DEBUG_LOG
#ifdef DISABLE_DEBUG_LOG
#define log_value(msg, data) _empty()
#define logln(msg) _empty()
#define log(msg) _empty()
#define log_flush() _empty()
#else
#define logln(msg) (Serial.println(msg))
#define log(msg) (Serial.print(msg))
#define log_flush() (Serial.flush())
#define log_value(msg, data) ((Serial.print((msg))), (logln((data))), (void)0)
#endif
#endif