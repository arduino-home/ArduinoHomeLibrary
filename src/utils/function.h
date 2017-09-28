#ifndef __ARDUINO_HOME_FUNCTION_H__
#define __ARDUINO_HOME_FUNCTION_H__

#include "type_traits.h"

// https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented

namespace ah {
  namespace utils {

    template <typename T>
    class function;

    template <typename R, typename... Args>
    class function<R(Args...)>
    {
      typedef R (*invoke_fn_t)(void*, Args&&...);
      typedef void* (*construct_fn_t)(void*);
      typedef void (*destroy_fn_t)(void*);

      template <typename Functor>
      static R invoke_fn(Functor* fn, Args&&... args) {
        return (*fn)(forward<Args>(args)...);
      }

      template <typename Functor>
      static Functor* construct_fn(Functor* src) {
        return new Functor(*src);
      }

      template <typename Functor>
      static void destroy_fn(Functor* f) {
        delete f;
      }

      // these pointers are storing behaviors
      invoke_fn_t invoke_f;
      construct_fn_t construct_f;
      destroy_fn_t destroy_f;

      // erase the type of any functor and store it into a char*
      // so the storage size should be obtained as well
      void* data_ptr;

    public:

      void reset() {
        if (data_ptr) {
          this->destroy_f(this->data_ptr);
          data_ptr = nullptr;
        }
      }

      function()
       : invoke_f(nullptr)
       , construct_f(nullptr)
       , destroy_f(nullptr)
       , data_ptr(nullptr) {
      }

      // construct from any functor type
      template <typename Functor>
      function(Functor f)
       // specialize functions and erase their type info by casting
       : invoke_f(reinterpret_cast<invoke_fn_t>(invoke_fn<Functor>))
       , construct_f(reinterpret_cast<construct_fn_t>(construct_fn<Functor>))
       , destroy_f(reinterpret_cast<destroy_fn_t>(destroy_fn<Functor>))
       , data_ptr(nullptr) {
        // copy the functor to internal storage
        this->data_ptr = this->construct_f(&f);
      }

      // copy constructor
      function(const function& rhs)
       : invoke_f(rhs.invoke_f)
       , construct_f(rhs.construct_f)
       , destroy_f(rhs.destroy_f)
       , data_ptr(nullptr) {

        if (this->invoke_f) {
          // when the source is not a null function, copy its internal functor
          this->data_ptr = this->construct_f(rhs.data_ptr);
        }
      }

      function& operator=(const function& rhs) {
        reset();

        this->invoke_f = rhs.invoke_f;
        this->construct_f = rhs.construct_f;
        this->destroy_f = rhs.destroy_f;

        if (this->invoke_f) {
          this->data_ptr = this->construct_f(rhs.data_ptr);
        }
      }

      ~function() {
        reset();
      }

      // other constructors, from nullptr, from function pointers

      R operator()(Args&&... args) const {
        return this->invoke_f(this->data_ptr, forward<Args>(args)...);
      }

      operator bool() const {
        return data_ptr;
      }
    };

  } // namespace utils
} // namespace ah

#endif // __ARDUINO_HOME_FUNCTION_H__
