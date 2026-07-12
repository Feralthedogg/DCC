#ifndef DCC_EXPORT_H
#define DCC_EXPORT_H

#if defined(_WIN32) && defined(DCC_SHARED)
#  if defined(DCC_BUILD)
#    define DCC_API __declspec(dllexport)
#  else
#    define DCC_API __declspec(dllimport)
#  endif
#elif defined(__GNUC__) || defined(__clang__)
#  define DCC_API __attribute__((visibility("default")))
#else
#  define DCC_API
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define DCC_DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(_MSC_VER)
#  define DCC_DEPRECATED(message) __declspec(deprecated(message))
#else
#  define DCC_DEPRECATED(message)
#endif

#endif
