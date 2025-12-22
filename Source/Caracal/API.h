
#ifndef CARACAL_API_H
#define CARACAL_API_H

#ifdef CARACAL_STATIC_DEFINE
#  define CARACAL_API
#  define CARACAL_NO_EXPORT
#else
#  ifndef CARACAL_API
#    ifdef Caracal_EXPORTS
        /* We are building this library */
#      define CARACAL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define CARACAL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef CARACAL_NO_EXPORT
#    define CARACAL_NO_EXPORT 
#  endif
#endif

#ifndef CARACAL_DEPRECATED
#  define CARACAL_DEPRECATED __declspec(deprecated)
#endif

#ifndef CARACAL_DEPRECATED_EXPORT
#  define CARACAL_DEPRECATED_EXPORT CARACAL_API CARACAL_DEPRECATED
#endif

#ifndef CARACAL_DEPRECATED_NO_EXPORT
#  define CARACAL_DEPRECATED_NO_EXPORT CARACAL_NO_EXPORT CARACAL_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CARACAL_NO_DEPRECATED
#    define CARACAL_NO_DEPRECATED
#  endif
#endif

#endif /* CARACAL_API_H */
