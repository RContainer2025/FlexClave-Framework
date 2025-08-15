#include "minikvm_op.h"
#include "minikvm_gpt_cmds.h"

void __NEWSPACE switch_to_minikvm()
{
    // int ret=-1;
    // ret = gpt_set_gptbrel3(0);
    gpt_set_gptbrel3(0);
}

void __NEWSPACE switch_to_host()
{
    // int ret=-1;
    // ret = gpt_set_gptbrel3(1);
    gpt_set_gptbrel3(1);
}

static void __NEWSPACE backup_kvm_context(struct kvm_vcpu *vcpu)
{
    int current_gpt_index;
    // backup the current gpt index
    current_gpt_index = vcpu->kvm->gpt_index;
    vcpu_backup[current_gpt_index] = vcpu;
}


static __NEWSPACE unsigned long user_addr_to_phy_addr(unsigned long address)
{
    pgd_t *pgd;
    p4d_t *p4d; // MODIFIED: Added p4d layer
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct mm_struct *mm = current->mm;

    pgd = pgd_offset(mm, address);
    if (pgd_none(*pgd))
        return 0;

    // MODIFIED: Added p4d lookup process to adapt to 5-level page table
    p4d = p4d_offset(pgd, address);
    if (p4d_none(*p4d))
        return 0;

    pud = pud_offset(p4d, address); // MODIFIED: pud_offset parameter changed from pgd to p4d
    if (pud_none(*pud))
        return 0;

    pmd = pmd_offset(pud, address);
    if (pmd_none(*pmd))
        return 0;

    pte = pte_offset_kernel(pmd, address);
    if (!pte_valid(*pte))
        return 0;

    return (pte_pfn(*pte) << PAGE_SHIFT) | (address & (PAGE_SIZE - 1));

}

// to fix: due to qemu using vmalloc, we cannnot get the hpa directly from virt_to_phys
static __NEWSPACE void get_current_mem_hpa(unsigned long *hpa_start, unsigned long *hpa_end)
{
    struct vm_area_struct *vma;
    // ADDED: VMA iterator
    struct vma_iterator vmi;
    // unsigned long vm_start, vm_end;
    unsigned long vm_start = 0, vm_end = 0, min_hpa, max_hpa; // MODIFIED: Initialize vm_start and vm_end
    int page_offset;
    
    /* Acquire read lock on mm_struct */
    // MODIFIED: Use mmap_read_lock instead of down_read(&current->mm->mmap_sem)
    mmap_read_lock(current->mm);
    
    // MODIFIED: Use new VMA iteration method instead of old linked list traversal
    vma_iter_init(&vmi, current->mm, 0);
    for_each_vma(vmi, vma) {
        // printk(KERN_INFO "VMA: start=0x%lx, end=0x%lx, flags=0x%lx\n",
        //        vma->vm_start, vma->vm_end, vma->vm_flags);
        // // If vma's vm_file is not NULL, print the file name
        // if (vma->vm_file) {
        //     printk(KERN_INFO "VMA: start=0x%lx, end=0x%lx, flags=0x%lx, file=%s\n",
        //            vma->vm_start, vma->vm_end, vma->vm_flags,
        //            vma->vm_file->f_path.dentry->d_name.name);
        // }

        if (vma->vm_file) {
            // if (vma->vm_file->f_path.dentry->d_name.name == "guest_ram")
            if (!strncmp(vma->vm_file->f_path.dentry->d_name.name, "guest_ram", 0x9))
            {
                printk(KERN_INFO "Found guest ram vma: start=0x%lx, end=0x%lx\n",
                       vma->vm_start, vma->vm_end);
                vm_start = vma->vm_start;
                // vm_end = vma->vm_end;
                vm_end = vma->vm_end - PAGE_SIZE; // Subtract one page to avoid out-of-bounds
                break; // Exit loop after finding target vma to avoid duplicate processing
            }
        }

    }
    // MODIFIED: Use mmap_read_unlock instead of up_read(&current->mm->mmap_sem)
    mmap_read_unlock(current->mm);

    // // Get the physical address corresponding to vm_start
    // if (vm_start) {
    //     printk(KERN_INFO "Physical address of vm_start: 0x%llx\n", virt_to_phys((void *)vm_start));
    // } else {
    //     printk(KERN_ERR "Failed to find guest ram vma\n");
    // }

    // // Get the physical address corresponding to vm_end
    // if (vm_end) {
    //     printk(KERN_INFO "Physical address of vm_end: 0x%llx\n", virt_to_phys((void *)vm_end));
    // } else {
    //     printk(KERN_ERR "Failed to find guest ram vma\n");
    // }


    // // According to observations, the physical addresses between vm_start and vm_end are continuous on this 512M/4G development board

    // if (vm_start && vm_end) {
    //     *hpa_start = virt_to_phys((void *)vm_start);
    //     *hpa_end = virt_to_phys((void *)vm_end);
    // } else {
    //     printk(KERN_ERR "Failed to find guest ram vma\n");
    // }


    // // BUG_ON(*hpa_end <= *hpa_start);
    // if (*hpa_end <= *hpa_start) 
    // {
    //     printk(KERN_ERR "Physical address range is invalid\n");
    //     *hpa_end = 0xdead000;
    //     *hpa_start = 0x0;
    //     return;
    // }
    
    // If no VMA is found, return directly to avoid subsequent errors
    if (!vm_start || !vm_end) {
        printk(KERN_ERR "Failed to find 'guest_ram' VMA.\n");
        *hpa_start = 0;
        *hpa_end = 0;
        return;
    }

    // if ((vm_end - vm_start) != (*hpa_end - *hpa_start)) {
    //     printk(KERN_ERR "VM Physical address range is not continuous\n");
    //     // handle the error: try to find the continuous physical address range again by min-max method
    //     min_hpa = *hpa_start;
    //     max_hpa = *hpa_end;
    //     for (page_offset = 0; page_offset < (vm_end - vm_start); page_offset += PAGE_SIZE) {
    //         phys_addr_t pa = virt_to_phys((void *)(vm_start + page_offset));
    //         if (pa < min_hpa)
    //             min_hpa = pa;
    //         if (pa > max_hpa)
    //             max_hpa = pa;
    //     }
    //     *hpa_start = min_hpa;
    //     *hpa_end = max_hpa;
    //     if ((vm_end - vm_start) != (*hpa_end - *hpa_start)) {
    //         printk(KERN_ERR "Failed to find continuous physical address range after min-max retry\n");
    //     }
    // }

    // fixed below
    *hpa_start = user_addr_to_phy_addr(vm_start);
    *hpa_end = user_addr_to_phy_addr(vm_end);
    min_hpa = *hpa_start;
    max_hpa = *hpa_end;
    for (page_offset = 0; page_offset < (vm_end - vm_start); page_offset += PAGE_SIZE) {
        phys_addr_t pa = user_addr_to_phy_addr(vm_start + page_offset);
        if (pa < min_hpa)
            min_hpa = pa;
        if (pa > max_hpa)
            max_hpa = pa;
    }
    *hpa_start = min_hpa;
    *hpa_end = max_hpa;
    if ((vm_end - vm_start) != (*hpa_end - *hpa_start)) {
        printk(KERN_ERR "Failed to find continuous physical address range after min-max retry\n");
        // cannot find a memory&&time save solution to fix
        *hpa_end = *hpa_start + (vm_end - vm_start);
    }

}


static __NEWSPACE void minikvm_gpt_init(struct kvm *kvm)
{
    int set_res;
    phys_addr_t newspace_start_phys;
    // unsigned long curgptbrel3; // debug use

    if (gpt_kvm_init) {
        printk(KERN_INFO "minikvm_gpt_init has already been called\n");
        return;
    }

    printk("--------------do init multiGPT GPT(0,1)--------------------\n");
    printk("minios GPT init\n");
    // curgptbrel3 = gpt_get_gptbrel3()<<12;

    set_res = gpt_set_gptbrel3(0);
    BUG_ON(set_res != 0);

    // kvm->gpt_index = 0;
    // Set gpt_used[0] to 1, indicating GPT 0 is used
    gpt_used[0] = 1;

    set_res = gpt_set_gptbrel3(1);
    // kvm->gpt_index = 1;
    BUG_ON(set_res != 0);
    // Set gpt_used[1] to 1, indicating GPT 1 is used
    gpt_used[1] = 1;
    gpt_kvm_init = 1;

    newspace_start_phys = virt_to_phys(__newspace_start);
    set_res = gpt_set_gpi(newspace_start_phys,((unsigned long)virt_to_phys(__newspace_end)-newspace_start_phys)/PAGE_SIZE,GPT_GPI_NO_ACCESS);
    BUG_ON(set_res != 0);

#ifdef CONFIG_FLEXCLAVE_CCA_SIM
    printk(KERN_INFO "CONFIG_FLEXCLAVE_CCA_SIM is enabled.\n");
#endif

    printk("--------------fini init multiGPT GPT(0,1)--------------------\n");

}

void __NEWSPACE minikvm_create_vm(struct kvm_vcpu *vcpu)
{
    struct kvm *kvm = vcpu->kvm;
    unsigned long mem_phy_start, mem_phy_end;
    unsigned long set_res;
    unsigned int cur_gpt_index;

    minikvm_gpt_init(kvm);

    // start vm with gpc check

    printk(KERN_INFO "create_vm::get into minikvm\n");
    set_res = gpt_set_gptbrel3(0);
    BUG_ON(set_res != 0);

    get_current_mem_hpa(&mem_phy_start, &mem_phy_end);
    printk("--------------start VM with memory range: 0x%lx - 0x%lx--------------------\n", mem_phy_start, mem_phy_end);
    printk(KERN_INFO "set them as GPT_GPI_NO_ACCESS to host gpt\n");
    set_res = gpt_set_gptbrel3(1);
    set_res = gpt_set_gpi(mem_phy_start, (mem_phy_end - mem_phy_start) / PAGE_SIZE, GPT_GPI_NO_ACCESS);
    BUG_ON(set_res != 0);
    set_res = gpt_set_gptbrel3(0);

    cur_gpt_index = find_unused_gpt();
    // BUG_ON(cur_gpt_index == -1);
    if (cur_gpt_index == -1)
    {
        printk(KERN_ERR "No available GPT index found\n");
        cur_gpt_index = 2; // Use a fixed index if no available one is found
    }
    gpt_used[cur_gpt_index] = 1;

    kvm->gpt_index = cur_gpt_index;
    kvm->mem_phy_start = mem_phy_start;
    kvm->mem_phy_end = mem_phy_end;

    printk(KERN_INFO "then switch back to host\n");
    set_res = gpt_set_gptbrel3(1);

}

void __NEWSPACE minikvm_destroy_vm(struct kvm_vcpu *vcpu)
{
    struct kvm *kvm = vcpu->kvm;
    unsigned long mem_phy_start, mem_phy_end;
    unsigned long set_res;
    unsigned int cur_gpt_index;

    printk(KERN_INFO "destroy_vm::get into minikvm\n");
    set_res = gpt_set_gptbrel3(0);
    BUG_ON(set_res != 0);

    mem_phy_start = kvm->mem_phy_start;
    mem_phy_end = kvm->mem_phy_end;
    cur_gpt_index = kvm->gpt_index;
    printk(KERN_INFO "destroy vm, free the gpt index %d\n", cur_gpt_index);
    gpt_used[cur_gpt_index] = 0;
    kvm->gpt_index = -1;
    printk(KERN_INFO "destroy_vm::set them as GPT_GPI_ANY to host gpt: %lx end: %lx\n",
           mem_phy_start, mem_phy_end);
    set_res = gpt_set_gptbrel3(1);
    set_res = gpt_set_gpi(mem_phy_start, (mem_phy_end - mem_phy_start) / PAGE_SIZE, GPT_GPI_ANY);
    BUG_ON(set_res != 0);
    set_res = gpt_set_gptbrel3(0);

    printk(KERN_INFO "then switch back to host\n");
    set_res = gpt_set_gptbrel3(1);

}

void __NEWSPACE minikvm_enter_vm(struct kvm_vcpu *vcpu)
{
    struct kvm *kvm = vcpu->kvm;
    int current_gpt_index;
    int set_res;
#ifdef CONFIG_FLEXCLAVE_CCA_SIM
    int i;
    for (i=0; i<9; i++)
    {
        switch_to_minikvm();
    }
#endif

    switch_to_minikvm();

    current_gpt_index = kvm->gpt_index;
    // printk(KERN_INFO "current gpt index is %d\n", current_gpt_index);
    if (current_gpt_index == 0) {
        printk(KERN_ERR "GPT index is not set, cannot enter VM\n");
        return;
    }

    if (current_gpt_index > GPT_CLAVE_COUNT_MAX)
    {
        printk(KERN_ERR "Invalid GPT index %d, max allowed is %d\n", current_gpt_index, GPT_CLAVE_COUNT_MAX);
        return;
    }

    set_res = gpt_set_gptbrel3(current_gpt_index);
    BUG_ON(set_res != 0);

}

void __NEWSPACE minikvm_exit_vm(struct kvm_vcpu *vcpu)
{
    struct kvm *kvm = vcpu->kvm;
    int current_gpt_index;
    // int set_res;
#ifdef CONFIG_FLEXCLAVE_CCA_SIM
    int i;
    for (i=0; i<9; i++)
    {
        switch_to_minikvm();
    }
#endif

    switch_to_minikvm();
    current_gpt_index = kvm->gpt_index;
    if (current_gpt_index < 0)
    {
        printk(KERN_ERR "GPT index is not set, cannot exit VM\n");
        return;
    }
    if (current_gpt_index > GPT_CLAVE_COUNT_MAX)
    {
        printk(KERN_ERR "Invalid GPT index %d, cannot exit VM\n", current_gpt_index);
        return;
    }

    backup_kvm_context(vcpu);
    switch_to_host();

}