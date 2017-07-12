#ifndef __ARDUINO_HOME_UTILS_H__
#define __ARDUINO_HOME_UTILS_H__

// TODO: disable debug ?
#define Debug Serial

template<class T>
inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

static const char * endl PROGMEM = "\n";

#endif // __ARDUINO_HOME_UTILS_H__
