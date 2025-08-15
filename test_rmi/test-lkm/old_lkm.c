/*lkm.c*/
#include <linux/module.h>    
#include <linux/kernel.h>   
#include <linux/init.h>        
#include <asm/rmi_cmds.h> 
#include <asm/gpt_cmds.h>


MODULE_LICENSE("GPL");

static int lkm_init(void) 
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
    // //*(int *)virt=100;


    // //(3)Read any phy address 8 bytes 64bits
    // unsigned long value;
    // value=gpt_read_any_phy_addr((phys_addr_t)(gptbrel3+0x10));
    // printk("addr:%08lx value:%08lx\n",gptbrel3,value);



    // //(4)set the GPTBR_EL3
    // int set_res=gpt_set_gptbrel3(0);
    // printk("set gpt:%d\n",set_res);
    // printk("gpt_get_GPTBR_EL3\n");
    // unsigned long curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("GPTBR_EL3:0x%08lx\n",curgptbrel3);


    // //(5)change the GPI
    // printk("set page GPT_GPI_REALM\n");
    // printk("phys:0x%08lx\n",(unsigned long)phys);
    // ret=gpt_set_gpi(phys,8,GPT_GPI_REALM);
    // printk("res:0x%08lx\n",(unsigned long)ret);
    // printk("phys:0x%08lx\n",(unsigned long)phys);


    // //(6)set the GPTBR_EL3 again
    // set_res=gpt_set_gptbrel3(1);
    // printk("set gpt:%d\n",set_res);
    // printk("gpt_get_GPTBR_EL3\n");
    // curgptbrel3 = gpt_get_gptbrel3()<<12;
    // printk("GPTBR_EL3:0x%08lx\n",curgptbrel3);

    return 0;    
}
 
static void lkm_exit(void)
{
    printk("test_lkm:module removed\n");
}
 
module_init(lkm_init);
module_exit(lkm_exit);