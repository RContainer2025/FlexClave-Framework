#include <linux/slab.h>
#include <linux/export.h>
#include <linux/nsproxy.h>
#include <linux/init_task.h>
#include <linux/mnt_namespace.h>
#include <linux/utsname.h>
#include <linux/pid_namespace.h>
#include <net/net_namespace.h>
#include <linux/ipc_namespace.h>
#include <linux/time_namespace.h>
#include <linux/fs_struct.h>
#include <linux/proc_fs.h>
#include <linux/proc_ns.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <linux/cgroup.h>
#include <linux/perf_event.h>
#include <linux/types.h>
// linux/fs/exec.c
#include <linux/fs.h>

#include "minios_gpt_cmds.h"
#include "minios_op.h"

void switch_minios()
{
    int ret;
    ret = gpt_set_gptbrel3(0);
    // printk("gpt_set_gptbrel3 ret: %d\n", ret);
}

void switch_os()
{
    int ret;
    ret = gpt_set_gptbrel3(1);
    // printk("gpt_set_gptbrel3 ret: %d\n", ret);
}

void __trigger_syscall(struct nsproxy *ns)
{
    int ret_res;
    switch_minios();
    ret_res = gpt_set_gptbrel3(1); // Switch to OS GPT for subsequent system calls
    // printk("trigger_syscall, set gpt:%d\n",ret_res);
}

void __trigger_syscall_ret(struct nsproxy *ns)
{
    int ret_res;
    switch_minios();
    ret_res = gpt_set_gptbrel3(ns->pid_ns_for_children->gpt_index); // Switch back to docker GPT
    // printk("trigger_syscall_ret, set gpt:%d\n",ret_res);
}

// #define GPT_ISOLATION
void minios_gpt_init(struct nsproxy *new_nsp)
{
    int set_res;
    unsigned long curgptbrel3;
    phys_addr_t newspace_start_phys;
    if (gpt_os_init == 1)
    {
        return;
    }
    // Divider line
    // printk("--------------do init multiGPT GPT(0,1)--------------------\n");
    // printk("minios GPT init\n");
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("before gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);
    // Switch GPTBR_EL3 and debug output
    set_res = gpt_set_gptbrel3(0);
    // Set gpt_index of nsproxy->pid_ns_for_children
    new_nsp->pid_ns_for_children->gpt_index = 0;
    // Set gpt_used[0] to 1, indicating GPT 0 is used
    gpt_used[0] = 1;
    // printk("set gpt:%d\n",set_res);
    // printk("gpt_get_GPTBR_EL3\n");
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("after gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);

    // printk("os GPT init\n");
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("before gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);
    // Switch GPTBR_EL3 and debug output
    set_res = gpt_set_gptbrel3(1);
    // Set gpt_index of nsproxy->pid_ns_for_children
    new_nsp->pid_ns_for_children->gpt_index = 1;
    // Set gpt_used[1] to 1, indicating GPT 1 is used
    gpt_used[1] = 1;
    // printk("set gpt:%d\n",set_res);
    // printk("gpt_get_GPTBR_EL3\n");
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("after gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);

    gpt_os_init = 1;
    // And at this point, initialize gpt_docker_count to 0
    gpt_docker_count = 0;
    // Divider line

#ifdef GPT_ISOLATION
    // Set the physical pages corresponding to newspace in the permission matrix of GPT(1) to GPT_GPI_NO_ACCESS
    newspace_start_phys = virt_to_phys(__newspace_start);
    // printk("newspace_start_phys:0x%08lx\n",(unsigned long)newspace_start_phys);
    
    set_res = gpt_set_gpi(newspace_start_phys,((unsigned long)virt_to_phys(__newspace_end)-newspace_start_phys)/PAGE_SIZE,GPT_GPI_NO_ACCESS);
    BUG_ON(set_res != 0);
#endif
    // printk("gpt_os_init: %d\n", gpt_os_init);

    // printk("--------------fini init multiGPT GPT(0,1)--------------------\n");
}

void minios_rc_create_clice(struct nsproxy *new_nsp)
{
    int set_res;
    unsigned long curgptbrel3;
    unsigned int cur_gpt_index;

    // printk("create_new_namespaces hook\n");
    // printk("new_nsp:0x%08lx\n",(long unsigned int)new_nsp);
    // printk("new_nsp->pid_ns_for_children:0x%08lx\n",(long unsigned int)new_nsp->pid_ns_for_children);

    // Divider line
    // printk("--------------do docker GPT switch----------------------\n");
    // Debug output
    // printk("before gpt_set_gptbrel3, gpt_docker_count:%d\n",gpt_docker_count);
    BUG_ON((gpt_docker_count + 1) > GPT_DOCKER_COUNT_MAX);

    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("before gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);
    // Switch GPTBR_EL3 and debug output
    // print_gpt_used();
    // Find an unused GPT index as the GPT index for this docker
    cur_gpt_index = find_unused_gpt();
    BUG_ON(cur_gpt_index == -1);
    // printk("cur_gpt_index:%d\n",cur_gpt_index);
    set_res = gpt_set_gptbrel3(cur_gpt_index);
    // Set gpt_used[cur_gpt_index] to 1, indicating cur_gpt_index GPT is used
    gpt_used[cur_gpt_index] = 1;
    // Set gpt_index of nsproxy->pid_ns_for_children
    new_nsp->pid_ns_for_children->gpt_index = cur_gpt_index;
    // printk("set gpt:%d\n",set_res);
    // printk("gpt_get_GPTBR_EL3\n");
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("after gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);
    gpt_docker_count++;
    // Divider line
    // printk("--------------fini docker GPT switch----------------------\n");
}

void minios_rc_destroy_clice(struct nsproxy *ns)
{
    int set_res;
    unsigned long curgptbrel3;
    // printk("free_nsproxy hook\n");
    // printk("gpt_os_init: %d\n", gpt_os_init);
    // printk("ns:0x%08lx\n",(long unsigned int)ns);
    // printk("ns->pid_ns_for_children:0x%08lx\n",(long unsigned int)ns->pid_ns_for_children);
    // printk("ns->pid_ns_for_children->gpt_index:%d\n",ns->pid_ns_for_children->gpt_index);
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("before gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);

    // Set gpt_used[ns->pid_ns_for_children->gpt_index] to 0, indicating ns->pid_ns_for_children->gpt_index GPT is released
    gpt_used[ns->pid_ns_for_children->gpt_index] = 0;
    // Set gpt_index of nsproxy->pid_ns_for_children
    ns->pid_ns_for_children->gpt_index = 0;
    // Decrement gpt_docker_count
    gpt_docker_count--;
    BUG_ON(gpt_docker_count < 0);
    // Debug output
    // printk("after gpt_set_gptbrel3, gpt_docker_count:%d\n",gpt_docker_count);
    // print_gpt_used();

    // Since theoretically after switching to OS GPT, security functions should not be called again, this step is placed at the end (the last debug statement also needs to be commented out)
    // Switch GPTBR_EL3 and debug output
    set_res = gpt_set_gptbrel3(1);
    BUG_ON(set_res != 0);
    curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("after gpt_set_gptbrel3, GPTBR_EL3:0x%08lx\n",curgptbrel3);
}

static struct kmem_cache *nsproxy_cachep;

struct nsproxy *minios_rc_malloc_mm(void)
{
	struct nsproxy *nsproxy;

	nsproxy = kmem_cache_alloc(nsproxy_cachep, GFP_KERNEL);
	if (nsproxy)
		atomic_set(&nsproxy->count, 1);
	return nsproxy;
}

// update PTE of a process/thread in container
void minios_rc_set_pte(struct mm_struct *mm, unsigned long addr, pte_t pte)
{
    // Actually calls the set_pte_at function in the Linux kernel
    set_pte_at(mm, addr, &pte, pte);
}

// copy page to a container
// It's actually the copy_page function under arm64 architecture, implemented with memcpy
void minios_rc_copy_page(void *kto, void *kfrom, unsigned int len)
{
    // printk("minios_rc_copy_page\n, from:0x%08lx, to:0x%08lx, len:%d\n",(unsigned long)kfrom,(unsigned long)kto,len);
    memcpy(kto, kfrom, len);
}

// run a new process in container
// __do_sys_clone(unsigned long clone_flags, unsigned long newsp, int * parent_tidptr, unsigned long tls, int * child_tidptr)
// SYSCALL_DEFINE5(clone, unsigned long, clone_flags, unsigned long, newsp,
// 		 int __user *, parent_tidptr,
// 		 unsigned long, tls,
// 		 int __user *, child_tidptr)
// {
// 	struct kernel_clone_args args = {
// 		.flags		= (lower_32_bits(clone_flags) & ~CSIGNAL),
// 		.pidfd		= parent_tidptr,
// 		.child_tid	= child_tidptr,
// 		.parent_tid	= parent_tidptr,
// 		.exit_signal	= (lower_32_bits(clone_flags) & CSIGNAL),
// 		.stack		= newsp,
// 		.tls		= tls,
// 	};

// 	return kernel_clone(&args);
// }
pid_t minios_rc_task_clone(unsigned long clone_flags, unsigned long newsp, int * parent_tidptr, unsigned long tls, int * child_tidptr)
{
    pid_t pid;
    struct kernel_clone_args args = {
        .flags		= (lower_32_bits(clone_flags) & ~CSIGNAL),
        .pidfd		= parent_tidptr,
        .child_tid	= child_tidptr,
        .parent_tid	= parent_tidptr,
        .exit_signal	= (lower_32_bits(clone_flags) & CSIGNAL),
        .stack		= newsp,
        .tls		= tls,
    };
    pid = kernel_clone(&args);
    return pid;
}

// run a new program in new address space in a container
// long __do_sys_execve(const char * filename, const char *const * argv, const char *const * envp)
// SYSCALL_DEFINE3(execve,
// 		const char __user *, filename,
// 		const char __user *const __user *, argv,
// 		const char __user *const __user *, envp)
// {
// 	return do_execve(getname(filename), argv, envp);
// }
long minios_rc_task_execve(const char * filename, const char *const * argv, const char *const * envp)
{
    return do_execve(getname(filename), argv, envp);
}

// exit a process in a container
// long __do_sys_exit(int error_code)
// void __noreturn do_exit(long code) {...}
void minios_rc_task_exit(int error_code)
{
	do_exit((error_code&0xff)<<8);
}