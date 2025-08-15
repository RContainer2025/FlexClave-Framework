#include <asm/sec_warp.h>
#include <asm/flexclave_const.h>
#include "minikvm_op.h"
#include <linux/kernel.h>

SECURE_WRAPPER(void, fc_create_clave, struct kvm_vcpu *vcpu) {
    minikvm_create_vm(vcpu);
}

SECURE_WRAPPER(void, fc_destroy_clave,struct kvm_vcpu *vcpu) {
    minikvm_destroy_vm(vcpu);
}

SECURE_WRAPPER(void, fc_entry_clave, struct kvm_vcpu *vcpu) {
    minikvm_enter_vm(vcpu);
}

SECURE_WRAPPER(void, fc_exit_clave, struct kvm_vcpu *vcpu) {
    minikvm_exit_vm(vcpu);
}

void minikvm_api_call(unsigned int op,...)
{
    // long ret = 0;
    // struct nsproxy *ns;
    //uint64_t start, end;
    va_list args;
    va_start(args, op);

    switch (op)
    {
        case FC_SYSTEM_INITIALIZATION:
            break;
        case FC_CREATE_CLAVE:
            fc_create_clave_secure(va_arg(args, struct kvm_vcpu *));
            break;
        case FC_DESTROY_CLAVE:
            fc_destroy_clave_secure(va_arg(args,struct kvm_vcpu *));
            break;
        case FC_ENTRY_CLAVE:
            fc_entry_clave_secure(va_arg(args,struct kvm_vcpu *));
            break;
        case FC_EXIT_CLAVE:
            fc_exit_clave_secure(va_arg(args,struct kvm_vcpu *));
            break;
        default:
            break;
    }

    va_end(args);
    // return ret;
}
// EXPORT_SYMBOL(minikvm_api_call);