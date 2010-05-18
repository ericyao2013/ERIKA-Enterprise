/** 
* @file tsl2561t.h
* @brief TSL2561T Light Sensor Driver
* @author Matteo Petracca
* @author Marco Ghibaudi 
* @date 2010-04-22
*
* Using the Erika Enterprise abstraction for the compiler.
*/

#ifndef __tsl2561t_compiler_ee_h__
#define __tsl2561t_compiler_ee_h__

#include <ee.h>
/** 
* @name Basic type representations.
* @{ */
#ifndef UINT8_T
#define UINT8_T
typedef EE_UINT8 uint8_t;	/**< Unsigned 8 bit intgerer. */
#endif

#ifndef UINT16_T
#define UINT16_T
typedef EE_UINT16 uint16_t;	/**< Unsigned 16 bit intgerer. */
#endif

#ifndef UINT32_T
#define UINT32_T
typedef EE_UINT32 uint32_t;	/**< Unsigned 32 bit intgerer. */
#endif

// TODO: Create EE_UINT64?
#ifndef UINT64_T
#define UINT64_T
typedef unsigned long long uint64_t;	/**< Unsigned 64 bit intgerer. */
#endif

#ifndef INT8_T
#define INT8_T
typedef EE_INT8 int8_t;		/**< Unsigned 8 bit intgerer. */
#endif

#ifndef INT16_T
#define INT16_T
typedef EE_INT16 int16_t;	/**< Unsigned 16 bit intgerer. */
#endif

// TODO: Create EE_INT64?
#ifndef INT64_T
#define INT64_T
typedef signed long long int64_t;	/**< Unsigned 64 bit intgerer. */
#endif
/**  @} */

#ifndef COMPILER_INLINE 
#define COMPILER_INLINE __INLINE__
#endif

#ifndef COMPILER_ISR
#define COMPILER_ISR(func) ISR2(func)
#endif

#endif /* Header Protection */