// https://gist.github.com/cmaglie/5883185

#ifndef __ARDUINO_HOME_STRING_STREAM_H__
#define __ARDUINO_HOME_STRING_STREAM_H__

#include <Stream.h>

namespace ah {
  namespace utils {

    class StringStream : public Stream
    {
    public:
      StringStream(String &s) : string(s), position(0) { }

      // Stream methods
      virtual int available() { return string.length() - position; }
      virtual int read() { return position < string.length() ? string[position++] : -1; }
      virtual int peek() { return position < string.length() ? string[position] : -1; }
      virtual void flush() { };
      // Print methods
      virtual size_t write(uint8_t c) { string += (char)c; };

    private:
      String &string;
      int length;
      int position;
    };

  } // namespace utils
} // namespace ah


#endif // __ARDUINO_HOME_STRING_STREAM_H__
