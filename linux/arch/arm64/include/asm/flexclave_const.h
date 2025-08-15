#ifndef FLEXCLAVE_CONST_H
#define FLEXCLAVE_CONST_H

#include <asm/memory.h>

// #define NEWSPACE_VADDR		(0xffff000100000000)
// #define NEWSPACE_VADDR (KIMAGE_VADDR + 0x10000000)
// #define NEWSPACE_VADDR_OFFSET 0x10000000
#define NEWSPACE_VADDR_OFFSET 0x1000000 * 4

#define UDF_SEGMENT_LENGTH 0x200000
/* --------------------------------------------- */

#define GPT_CLAVE_COUNT_MAX 30


#define FC_SYSTEM_INITIALIZATION 1  /* Initialize the TCB components of the system */
#define FC_CREATE_CLAVE 2           /* Create a new FlexClave instance */
#define FC_DESTROY_CLAVE 3          /* Destroy an existing FlexClave instance*/
#define FC_ENTRY_CLAVE 4            /* Entry a FlexClave instance */   
#define FC_EXIT_CLAVE 5             /* Exit a FlexClave instance */
#define FC_MALLOC_MM 6              /* Allocate memory for a FlexClave instance */
#define FC_MODIFY_PAGE_PERM 7       /* Modify page access permission */
#define FC_COPY_PAGE 8              /* Copy page to a container */
#define FC_ACCESS_CONTEXT 9         /* Access to the current CPU context */
#define FC_SET_IOPTE 10             /* Update the I/O page table of IO device */
#define FC_IPC_IN 11                /* Handle ipc within a FlexClave instance */
#define FC_IPC_OUT 12               /* Handle ipc between FlexClave instances */
#define FC_SWITCH_TO_TCB 13         /* Switch contexts to the TCB from untrusted system software */
#define FC_SWITCH_TO_SYS 14         /* Switch contexts to untrusted system software from the TCB */

/* --------------------------------------------- */

// 决定是否开启所有的GPT HOOK
#define GPT_HOOK

// 宏设置gpt_docker_count的最大值，即最多支持多少个docker，当前为32 - 2
#define GPT_DOCKER_COUNT_MAX 30
// minios api definition
#define RC_CREATE_CLICE UL(0x1)
#define RC_DESTROY_CLICE UL(0x2)
#define RC_CREATE_CONTAINER UL(0x3)
#define RC_DESTROY_CONTAINER UL(0x4)
#define RC_MALLOC_MM UL(0x5)
#define RC_SET_PTE UL(0x6)
#define RC_COPY_PAGE UL(0x7)
#define RC_SET_VMA UL(0x8)
#define RC_SET_IOPTE UL(0x9)
#define RC_IPC_IN UL(0xa)
#define RC_IPC_OUT UL(0xb)
#define RC_TASK_CLONE UL(0xc)
#define RC_TASK_EXEC UL(0xd)
#define RC_TASK_EXIT UL(0xe)


#define GET_FAR_SYMBOL_ADDR(sym) \
({ \
    unsigned long __addr; \
    asm volatile ( \
        "ldr %0, =" #sym "\n" \
        : "=r" (__addr) \
    ); \
    __addr; \
})

#define __NEWSPACE_PTR(sym, type) \
    ((type *)(uintptr_t)(sym))

#endif /* FLEXCLAVE_CONST_H */