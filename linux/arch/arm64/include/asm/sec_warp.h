#ifndef __SEC_WARP_H
#define __SEC_WARP_H

// #include <inttypes.h>
#include <linux/init.h>

extern const __NEWSPACE unsigned long SecureStackBase;

/* !!!!warning: x0-x7 are used to pass parameters, so it is impossible to use these registers carefully.(DO not override parameters!!!!) */

/*stack switch*/                              

#define SWITCH_TO_SECURE_STACK            \
  "mov x4, sp\n"            \
  "ldr x5, =SecureStackBase\n"              \
  "ldr x5, [x5]\n"          \
  /*"str x0, [x1, #-8]\n"          */    \
 /* "str x30, [x1, #-16]\n"       */  \
  "stp x30, x4, [x5, #-16]\n"      /*x30 x4 value store to (x5-16)address, x5=x5-16*/               \
  "sub x5, x5, #16\n"            \
  "mov sp, x5\n"
       

#define SWITCH_BACK_TO_NORMAL_STACK       \
  "ldr x0, =SecureStackBase\n"        \
  "ldr x0, [x0]\n"        \
  /*"ldr x30, [x0, #-16]\n"     */  \
  /*"ldr x1, [x0, #-8]\n"  */  \
  "ldp x30, x1, [x0, #-16]\n"    /*(x0-16)address value load to x30 x1*/           \
  "mov sp, x1\n"    


/*IRQ control*/

#define DISABLE_IRQ                                                     \
  "msr daifset, #2\n"                                                                    

#define ENABLE_IRQ                     \
  "msr daifclr, #2\n"       


/*FIQ control*/
#define DISABLE_FIQ     \
    "msr daifset, #1\n"

#define ENABLE_FIQ      \
    "msr daifclr, #1\n"


/*ASY control*/
#define DISABLE_ASY         \
    "msr daifset, #4\n"

#define ENABLE_ASY      \
    "msr daifclr, #4\n"


/*DBG control*/
#define DISABLE_DBG         \
    "msr daifset, #8\n"

#define ENABLE_DBG      \
    "msr daifclr, #8\n"


/*ALL EXP control*/
#define DISABLE_INTERRUPT       \
    DISABLE_IRQ         \
    DISABLE_FIQ     \
    DISABLE_ASY     \
    DISABLE_DBG

#define ENABLE_INTERRUPT        \
    ENABLE_DBG      \
    ENABLE_ASY      \
    ENABLE_FIQ      \
    ENABLE_IRQ      


/*ENTRY/EXIT gate*/
#define SECURE_ENTRY        \
    DISABLE_INTERRUPT       \
    SWITCH_TO_SECURE_STACK      

#define SECURE_EXIT     \
    SWITCH_BACK_TO_NORMAL_STACK     \
    ENABLE_INTERRUPT

#define SECURE_WRAPPER(RET, FUNC, ...) \
asm( \
  ".text\n" \
  ".globl " #FUNC "\n" \
  ".align 16,0x90\n" \
  ".type " #FUNC ",@function\n" \
  #FUNC ":\n" \
  ".cfi_startproc\n" \
  /* Do whatever's needed on entry to secure area */ \
  SECURE_ENTRY \
  /* Call real version of function */ \
  "bl " #FUNC "_secure\n" \
  /* Operation complete, go back to unsecure mode */ \
  SECURE_EXIT \
  "ret\n" \
  #FUNC "_end:\n" \
  ".size " #FUNC ", " #FUNC "_end - " #FUNC "\n" \
  ".cfi_endproc\n" \
); \
RET FUNC ##_secure(__VA_ARGS__); \
RET __attribute__((visibility("hidden"))) FUNC ##_secure(__VA_ARGS__)


#endif