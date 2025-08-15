#include <asm/flexclave_const.h>

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/printk.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/kvm_host.h>
#include <asm-generic/sections.h>
#include <linux/mmap_lock.h>
// #include <linux/vma_iter.h>

void switch_to_minikvm(void);
void switch_to_host(void);
// void get_current_mem_pfn(void);
// void minikvm_gpt_init(void);
void minikvm_create_vm(struct kvm_vcpu *vcpu);
void minikvm_destroy_vm(struct kvm_vcpu *vcpu);
void minikvm_enter_vm(struct kvm_vcpu *vcpu);
void minikvm_exit_vm(struct kvm_vcpu *vcpu);