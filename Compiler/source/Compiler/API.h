
#ifndef COMPILER_API_H
#define COMPILER_API_H

#ifdef COMPILER_STATIC_DEFINE
#  define COMPILER_API
#  define COMPILER_NO_EXPORT
#else
#  ifndef COMPILER_API
#    ifdef Compiler_EXPORTS
        /* We are building this library */
#      define COMPILER_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define COMPILER_API __declspec(dllimport)
#    endif
#  endif

#  ifndef COMPILER_NO_EXPORT
#    define COMPILER_NO_EXPORT 
#  endif
#endif

#ifndef COMPILER_DEPRECATED
#  define COMPILER_DEPRECATED __declspec(deprecated)
#endif

#ifndef COMPILER_DEPRECATED_EXPORT
#  define COMPILER_DEPRECATED_EXPORT COMPILER_API COMPILER_DEPRECATED
#endif

#ifndef COMPILER_DEPRECATED_NO_EXPORT
#  define COMPILER_DEPRECATED_NO_EXPORT COMPILER_NO_EXPORT COMPILER_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef COMPILER_NO_DEPRECATED
#    define COMPILER_NO_DEPRECATED
#  endif
#endif

#endif /* COMPILER_API_H */
