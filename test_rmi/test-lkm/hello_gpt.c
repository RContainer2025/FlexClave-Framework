/*lkm.c*/
#include <linux/module.h>    
#include <linux/kernel.h>   
#include <linux/init.h>        
#include <asm/rmi_cmds.h> 
#include <asm/gpt_cmds.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");

static unsigned long **acquire(char *symbol)
{
    unsigned long (*kallsyms_lookup_name)(const char *name); 
    struct kprobe kp = { 
        .symbol_name = "kallsyms_lookup_name", 
    };
    if (register_kprobe(&kp) < 0) 
        return NULL; 
    kallsyms_lookup_name = (unsigned long (*)(const char *name))kp.addr; 
    unregister_kprobe(&kp);
    return (unsigned long **)kallsyms_lookup_name(symbol); 
}


// static void __NEWSPACE test1(void)
// {
//     printk("test1\n");
// }

static int __init lkm_init(void) 
{
    printk("test_lkm:module loaded\n");

    //(1)read GPTBR_EL3
    printk("gpt_get_GPTBR_EL3\n");
    unsigned long gptbrel3 = gpt_get_gptbrel3()<<12;
    printk("GPTBR_EL3:0x%08lx\n",gptbrel3);

    

    // //(2)set attribute
    // phys_addr_t phys;
    // void *virt;
    
    // virt=(void *)__get_free_pages(GFP_KERNEL,3);
    // phys=virt_to_phys(virt);

    // printk("set page GPI_ROOT\n");
    // int ret=gpt_set_gpi(phys,8,GPT_GPI_ROOT);
    // printk("res:%d",ret);
    // printk("phys:0x%08lx\n",(unsigned long)phys);
    //*(int *)virt=100;
    
    // use kallsyms_lookup_name to get the address of __newspace_start and __newspace_end
    static unsigned long **newspace_start;
    if(!(newspace_start = acquire("__newspace_start")))
    {
        printk("acquire newspace failed\n");
        return -1;
    }
    static unsigned long **newspace_end;
    if(!(newspace_end = acquire("__newspace_end")))
    {
        printk("acquire newspace failed\n");
        return -1;
    }
    // 打印出自定义的内核段newspace的虚拟地址
    printk("newspace_start:%px\n",newspace_start);
    printk("newspace_end:%px\n",newspace_end);
    // 打印出自定义的内核段newspace的物理地址
    phys_addr_t newspace_start_phys = (void *)virt_to_phys((void *)newspace_start);
    phys_addr_t newspace_end_phys = (void *)virt_to_phys((void *)newspace_end);
    printk("newspace_start_phys:0x%08lx\n",(unsigned long)newspace_start_phys);
    printk("newspace_end_phys:0x%08lx\n",(unsigned long)newspace_end_phys);

    // 获取 printcwz 函数的虚拟地址
    unsigned long **printcwz;
    if(!(printcwz = acquire("printcwz")))
    {
        printk("acquire printcwz failed\n");
        return -1;
    }
    // 打印printcwz函数的虚拟地址
    printk("printcwz:%px\n",printcwz);
    // 打印printcwz函数的物理地址
    phys_addr_t printcwz_phys = (void *)virt_to_phys((void *)printcwz);
    printk("printcwz_phys:0x%08lx\n",(unsigned long)printcwz_phys);

    // 强制转换printcwz为函数指针
    void (*printcwz_func)(void) = (void (*)(void))printcwz;

    // 第一次调用printcwz函数
    printcwz_func();
    // GPT_GPI_REALM 置位
    printk("set page GPI_REALM\n");
    int ret=gpt_set_gpi(newspace_start_phys,2,GPT_GPI_REALM); // 2表示页数
    printk("res:%d",ret);
    // 第二次调用printcwz函数
    printcwz_func();
    
    printk("test_lkm:module executed\n");
    return 0;    
}
 
static void __exit lkm_exit(void)
{
    printk("test_lkm:module removed\n");
}
 
module_init(lkm_init);
module_exit(lkm_exit);
