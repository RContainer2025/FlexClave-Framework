#include <asm/flexclave_const.h>

void switch_minios(void);
void switch_os(void);
void __trigger_syscall(struct nsproxy *ns);
void __trigger_syscall_ret(struct nsproxy *ns);
extern long do_execve(struct filename *filename,
          const char __user *const __user *__argv,
          const char __user *const __user *__envp);

void minios_gpt_init(struct nsproxy *new_nsp);

// specific to function definition
void minios_rc_create_clice(struct nsproxy *new_nsp);
void minios_rc_destroy_clice(struct nsproxy *ns);
struct nsproxy *minios_rc_malloc_mm(void);
void minios_rc_set_pte(struct mm_struct *mm, unsigned long addr, pte_t pte);
void minios_rc_copy_page(void *kto, void *kfrom, unsigned int len);
pid_t minios_rc_task_clone(unsigned long clone_flags, unsigned long newsp, int * parent_tidptr, unsigned long tls, int * child_tidptr);
long minios_rc_task_execve(const char * filename, const char *const * argv, const char *const * envp);
void minios_rc_task_exit(int error_code);