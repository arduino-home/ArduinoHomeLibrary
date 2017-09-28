#ifndef __ARDUINO_HOME_TYPE_TRAITS_H__
#define __ARDUINO_HOME_TYPE_TRAITS_H__

namespace ah {
  namespace utils {

    template<class T>
    struct remove_reference {
      typedef T type;
    };

    template<class T>
    struct remove_reference<T&> {
      typedef T type;
    };

    template<class T>
    struct remove_reference<T&&> {
      typedef T type;
    };

    template< class T >
    T&& forward(typename remove_reference<T>::type& t) noexcept {
      return static_cast<T&&>(t);
    }

    template< class T >
    T&& forward(typename remove_reference<T>::type&& t) noexcept {
      return static_cast<T&&>(t);
    }

  } // namespace utils
} // namespace ah


#endif // __ARDUINO_HOME_TYPE_TRAITS_H__
