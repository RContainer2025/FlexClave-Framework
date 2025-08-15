#include <linux/pid_namespace.h>
#include <linux/nsproxy.h>
#include "minios_op.h"
#include <asm/sec_warp.h>

SECURE_WRAPPER(void, rc_create_clice, struct nsproxy *ns) {
    // minios_rc_create_clice(ns);
    __NEWSPACE_CALL(minios_rc_create_clice)(ns);
}
SECURE_WRAPPER(void, rc_destroy_clice, struct nsproxy *ns) {
    minios_rc_destroy_clice(ns);
}
SECURE_WRAPPER(struct nsproxy *, rc_malloc_mm, void) {
    return minios_rc_malloc_mm();
}
SECURE_WRAPPER(void, rc_set_pte, struct mm_struct *mm, unsigned long addr, pte_t pte) {
    minios_rc_set_pte(mm, addr, pte);
}
SECURE_WRAPPER(void, rc_copy_page, struct task_struct *kto, struct mm_struct *kfrom, unsigned int len) {
    minios_rc_copy_page(kto, kfrom, len);
}
SECURE_WRAPPER(pid_t, rc_task_clone, unsigned long clone_flags, unsigned long newsp, int * parent_tidptr, unsigned long tls, int * child_tidptr) {
    return minios_rc_task_clone(clone_flags, newsp, parent_tidptr, tls, child_tidptr);
}
SECURE_WRAPPER(long, rc_task_execve, const char * filename, const char *const * argv, const char *const * envp) {
    return minios_rc_task_execve(filename, argv, envp);
}
SECURE_WRAPPER(void, rc_task_exit, int error_code) {
    minios_rc_task_exit(error_code);
}

#include <linux/stdarg.h>

long minios_api_call(unsigned int op,...)
{
    long ret;
    struct nsproxy *ns;
    va_list args;
    va_start(args, op);

    switch (op)
    {
    case RC_CREATE_CLICE:
        ns = va_arg(args, struct nsproxy *);
        minios_gpt_init(ns);
        switch_minios();
        rc_create_clice_secure(ns);
        break;
    case RC_DESTROY_CLICE:
        ns = va_arg(args, struct nsproxy *);
        switch_minios();
        rc_destroy_clice_secure(ns);
        break;
    case RC_CREATE_CONTAINER:
        break;
    case RC_DESTROY_CONTAINER:
        break;
    case RC_MALLOC_MM:
        switch_minios();
        rc_malloc_mm_secure();
        switch_os();
        break;
    case RC_SET_PTE:
        switch_minios();
        rc_set_pte_secure(va_arg(args, struct mm_struct *), va_arg(args, unsigned long), va_arg(args, pte_t));
        switch_os();
        break;
    case RC_COPY_PAGE:
        switch_minios();
        rc_copy_page_secure(va_arg(args, struct task_struct *), va_arg(args, struct mm_struct *), va_arg(args, unsigned int));
        switch_os();
        break;
    case RC_SET_VMA:
        break;
    case RC_SET_IOPTE:
        break;
    case RC_IPC_IN:
        break;
    case RC_IPC_OUT:
        break;
    case RC_TASK_CLONE:
        switch_minios();
        ret = rc_task_clone_secure(va_arg(args, unsigned long), va_arg(args, unsigned long), va_arg(args, int *), va_arg(args, unsigned long), va_arg(args, int *));
        switch_os();
        break;
    case RC_TASK_EXEC:
        switch_minios();
        ret = rc_task_execve_secure(va_arg(args, const char *), va_arg(args, const char *const *), va_arg(args, const char *const *));
        switch_os();
        break;
    case RC_TASK_EXIT:
        switch_minios();
        rc_task_exit_secure(va_arg(args, int));
        switch_os();
        break;
    default:
        break;
    }

    va_end(args);
    return ret;
}

void trigger_syscall(struct nsproxy *ns)
{
    __trigger_syscall(ns);
}

void trigger_syscall_ret(struct nsproxy *ns)
{
    __trigger_syscall_ret(ns);
}