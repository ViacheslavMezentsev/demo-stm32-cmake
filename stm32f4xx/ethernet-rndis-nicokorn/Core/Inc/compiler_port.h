#ifndef COMPILER_PORT_H
#define COMPILER_PORT_H

#if defined ( __CC_ARM )   // Keil
  #define COMPILER_PACKED __packed
#elif defined ( __ICCARM__ ) // IAR
  #define COMPILER_PACKED __packed
#elif defined ( __GNUC__ )   // GCC
  #define COMPILER_PACKED __attribute__((packed))
#elif defined ( __TASKING__ )// TASKING
  #define COMPILER_PACKED __unaligned
#else
  #error "Compiler not supported."
#endif

#endif // COMPILER_PORT_H