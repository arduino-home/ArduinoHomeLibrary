#ifndef __ARDUINO_HOME_UTILS_H__
#define __ARDUINO_HOME_UTILS_H__

template<class T>
inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

static const char * const endl PROGMEM = "\n";

#define AH_DEBUG_INIT() do { Serial.begin(115200); AH_DEBUG(endl << "Starting" << endl); } while (0)
#define AH_DEBUG(msg) (Serial << msg)
#define AH_PANIC(msg) do { AH_DEBUG(msg); AH_DEBUG(endl); panic(); } while (0)
#define AH_ASSERT(cond, msg) do { if(!(cond)) { AH_PANIC(msg); } } while (0)

#endif // __ARDUINO_HOME_UTILS_H__
