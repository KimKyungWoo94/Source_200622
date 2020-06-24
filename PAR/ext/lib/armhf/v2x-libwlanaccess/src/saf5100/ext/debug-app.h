/**
 * @addtogroup cohda_debug_user Cohda's userspace debug library
 * @{
 *
 * @section cohda_debug_user_usage
 * Invoke like:
 * @code
 *     #define D_LOCAL D_ERR
 *     #include "linux/cohda/debug.h" // <linux/cohda/debug.h>
 * @endcode
 * At the end of your include file section
 *
 * @file
 * User Space Debug Support
 *
 *
 */

//------------------------------------------------------------------------------
// Copyright (c) 2017 Cohda Wireless Pty Ltd
//------------------------------------------------------------------------------

//#include "porting-file.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef INLINE
#undef INLINE
#endif
#if defined(_MSC_VER)
#define debug_app_MSVC 1
#define INLINE __inline
#define snprintf _snprintf
#else
#define INLINE inline
#endif


#if !defined(debug_app_MSVC) && !defined(__THREADX__)
#include <syslog.h>
#endif

#if 0
#include <sys/neutrino.h>
#include <sys/trace.h>
#endif

// Some handy levels to consider using with the d_*() functions
#define D_EMERG                 (0)
#define D_ALERT                 (1)
#define D_CRIT                  (2)
#define D_ERR                   (3)
#define D_WARN                  (4)
#define D_NOTICE                (5)
#define D_INFO                  (6)
#define D_DEBUG                 (7)
#define D_TEST                  (8)
#define D_VERBOSE               (9)
#define D_IRQ                   (127)
#define D_ALL                   (255)

/// Master debug switch; !0 enables, 0 disables
#ifndef D_MASTER
#define D_MASTER (!0)
#endif

/// Local (per-file) debug switch; \#define before \#including
#ifndef D_LOCAL
#define D_LOCAL D_WARN
#endif

/// Default to stderr (unbuffered)
#define D_LOGFILE stderr

#undef __d_printf
#undef d_fnstart
#undef d_fnend
#undef d_printf
#undef d_error
#undef d_dump

// Backend stuff

/// The debug level
extern int D_LEVEL;

/**
 * @brief Declare the debug level
 */
#define D_LEVEL_DECLARE() int D_LEVEL = D_LOCAL

/**
 * @brief Initialize the debug level
 */
#define D_LEVEL_INIT() D_LEVEL = __d_level()

// Maintain compatability with kernel style debug print mechanisms
#define D_SUBMODULE_DECLARE(_name) __D_SUBMODULE_##_name
#define D_SUBMODULE_DEFINE(_name)

/**
 * @brief Initialize the debug support functionality
 */
#define d_init(arg) \
  ({ \
    D_LEVEL_INIT(); \
  })

/**
 * @brief De-initialize the debug support functionality
 */
#define d_exit(arg) \
  ({ \
    ; \
  })

/**
 * @brief Generate a message header from a 'struct device'
 * @param head: buffer where to place the header
 * @param head_size: length of @param head
 * @param dev: pointer to device used to generate a header from. If NULL,
 *     an empty ("") header is generated.
 */
static INLINE void __d_dev_head(char *pHead,
                                size_t Size,
                                const void *pHdl)
{
  if (pHdl == NULL)
  {
    ;
  }
  else
  {
    (void)snprintf(pHead, Size, "[%p]:", pHdl);
  }
}

#ifdef DEBUG_PRINT_TIMES
#if defined(debug_app_MSVC) || defined(__THREADX__)
#define __d_printf(l, tag, dev, f, a ...)                                \
  do {                                                                  \
    void *__dev = (dev);                                                \
    if (d_test(l)) {                                                    \
      char __head[64] = "";                                             \
      struct timespec _Now;                                              \
      __d_dev_head(__head, sizeof(__head), __dev);                      \
      clock_gettime(CLOCK_REALTIME, &_Now);                              \
      fprintf(D_LOGFILE, "[%ld.%09lu] %s%s" tag ": " f, (long)_Now.tv_sec, _Now.tv_nsec, __head,    __FUNCTION__, ## a); \
/* \
      trace_logf(_NTO_TRACE_USERLAST, "[%ld.%09lu] %s%s" tag ": " f, (long)Now.tv_sec, Now.tv_nsec, __head,    __func__, ## a); \
      if (l<D_WARN) {                                                   \
        syslog(LOG_MAKEPRI(LOG_USER,l),   "%s%s" tag ": " f, __head,    __func__, ## a); \
      }                                                                 \
*/ \
    }                                                                   \
  } while (0 && (dev != 0))                                             \

#else
#include <time.h>

/**
 * @brief Log some message if debugging is enabled
 * @param l: intended debug level
 * @param tag: tag to prefix the message with
 * @param dev: 'struct device' associated to this message
 * @param f: printf-like format and arguments
 *
 * Note this is optimized out if it doesn't pass the compile-time
 * check; however, it is *always* compiled. This is useful to make
 * sure the printf-like formats and variables are always checked and
 * they don't get bit rot if you have all the debugging disabled.
 */
#define __d_printf(l, tag, dev, f, a...)                                \
  do {                                                                  \
    void *__dev = (dev);                                                \
    if (d_test(l)) {                                                    \
      char __head[64] = "";                                             \
      struct timespec _Now;                                              \
      __d_dev_head(__head, sizeof(__head), __dev);                      \
      clock_gettime(CLOCK_REALTIME, &_Now);                              \
      fprintf(D_LOGFILE, "[%ld.%09lu] %s%s" tag ": " f, (long)_Now.tv_sec, _Now.tv_nsec, __head,    __func__, ## a); \
/* \
      trace_logf(_NTO_TRACE_USERLAST, "[%ld.%09lu] %s%s" tag ": " f, (long)Now.tv_sec, Now.tv_nsec, __head,    __func__, ## a); \
      if (l<D_WARN) {                                                   \
        syslog(LOG_MAKEPRI(LOG_USER,l),   "%s%s" tag ": " f, __head,    __func__, ## a); \
      }                                                                 \
*/ \
    }                                                                   \
  } while (0 && (dev != 0))                                             \

#endif //debug_app_MSVC

#else
/**
 * @brief Log some message if debugging is enabled
 * @param l: intended debug level
 * @param tag: tag to prefix the message with
 * @param dev: 'struct device' associated to this message
 * @param f: printf-like format
 * @param a: printf-like arguments
 *
 * Note this is optimized out if it doesn't pass the compile-time
 * check; however, it is *always* compiled. This is useful to make
 * sure the printf-like formats and variables are always checked and
 * they don't get bit rot if you have all the debugging disabled.
 */

#if defined(debug_app_MSVC) || defined(__THREADX__)
#define __d_printf(l, tag, dev, f, a...)                                \
  do {                                                                  \
    void *__dev = (dev);                                                \
    if (d_test(l)) {                                                    \
      char __head[64] = "";                                             \
      __d_dev_head(__head, sizeof(__head), __dev);                      \
      fprintf(D_LOGFILE, "%s%s" tag ": " f, __head,    __FUNCTION__, ## a);    \
/* \
      trace_logf(_NTO_TRACE_USERLAST, "%s%s" tag ": " f, __head,    __func__, ## a); \
      if (l<D_WARN) {                                                   \
        syslog(LOG_MAKEPRI(LOG_USER,l),   "%s%s" tag ": " f, __head,    __func__, ## a); \
      }                                                                 \
*/ \
    }                                                                   \
  } while (0 && (dev != 0))                                             \

#else

#define __d_printf(l, tag, dev, f, a...)                                \
  do {                                                                  \
    void *__dev = (dev);                                                \
    if (d_test(l)) {                                                    \
      char __head[64] = "";                                             \
      __d_dev_head(__head, sizeof(__head), __dev);                      \
      fprintf(D_LOGFILE, "%s%s" tag ": " f, __head,    __func__, ## a);    \
/* \
      trace_logf(_NTO_TRACE_USERLAST, "%s%s" tag ": " f, __head,    __func__, ## a); \
      if (l<D_WARN) {                                                   \
        syslog(LOG_MAKEPRI(LOG_USER,l),   "%s%s" tag ": " f, __head,    __func__, ## a); \
      }                                                                 \
*/ \
    }                                                                   \
  } while (0 && (dev != 0))                                             \

#endif //debug_app_MSVC

#endif // DEBUG_PRINT_TIMES

#if defined(debug_app_MSVC) || defined(__THREADX__)
/**
 * @brief Trace a function entry
 */
#define d_fnstart(l, _dev, f, a ...) __d_printf(l, " FNSTART", _dev, f, ## a)

/**
 * @brief Trace a function exit/return
 */
#define d_fnend(l, _dev, f, a ...) __d_printf(l, " FNEND", _dev, f, ## a)

/**
 * @brief Trace information
 */
#define d_printf(l, _dev, f, a ...) __d_printf(l, "", _dev, f, ## a)

/**
 * @brief Trace an error
 */
#define d_error(l, _dev, f, a ...) __d_printf(l, " ERROR", _dev, f, ## a)

/**
 * @brief Output a 'log' (stderr/stdout/syslog) message
 * @todo
 */
#define d_log(l, _dev, f, a ...) __d_printf(l, "", _dev, f, ## a)

#else
/**
 * @brief Trace a function entry
 */
#define d_fnstart(l, _dev, f, a...) __d_printf(l, " FNSTART", _dev, f, ## a)

/**
 * @brief Trace a function exit/return
 */
#define d_fnend(l, _dev, f, a...) __d_printf(l, " FNEND", _dev, f, ## a)

/**
 * @brief Trace information
 */
#define d_printf(l, _dev, f, a...) __d_printf(l, "", _dev, f, ## a)

/**
 * @brief Trace an error
 */
#define d_error(l, _dev, f, a...) __d_printf(l, " ERROR", _dev, f, ## a)

/**
 * @brief Output a 'log' (stderr/stdout/syslog) message
 * @todo
 */
#define d_log(l, _dev, f, a...) __d_printf(l, "", _dev, f, ## a)

#endif //debug_app_MSVC

/**
 * @brief Test whether to trace or not (based on D_MASTER, D_LOCAL and d_level)
 */
#define d_test(l) (((int)D_MASTER != 0) && (D_LOCAL >= (l)) && (D_LEVEL >= (l)))


static INLINE void __d_dump(int l, void *_dev, const void *_ptr, size_t size)
{
  const unsigned char *ptr = (const unsigned char *)_ptr;
  char str[64];
  size_t cnt, itr;
  for (itr = cnt = 0; cnt < size; cnt++)
  {
    itr += snprintf(str + itr, sizeof(str) - itr, "%02x ", (unsigned)ptr[cnt]);
    if ((cnt > 0 && (cnt + 1) % 8 == 0) || (cnt == size - 1))
    {
      __d_printf(l, "", _dev, "%s\n", str);
      itr = 0;
    }
  }
}

/**
 * @brief Trace buffer data
 */
#define d_dump(l, dev, ptr, size)   \
  do {                              \
    if (d_test(l))                  \
      __d_dump(l, dev, ptr, size);  \
  } while (0)

/**
 * @brief Assertion check
 * @param arg: assertion to check
 */
#define d_assert(arg) assert(arg)


static INLINE int __d_level(void)
{
  char *pEnvString;

  // Get the controllable debug verbosity level from the environment
  pEnvString = getenv("D_LEVEL");
  if (pEnvString)
    return atoi(pEnvString);
  else
    return D_LOCAL;
}

/**
 * @}
 */
