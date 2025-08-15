#ifndef __MINIKVM_GPT_CMDS_H
#define __MINIKVM_GPT_CMDS_H


// #include <linux/arm-smccc.h>
#include <asm/gpt_smc.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/flexclave_const.h> // 用于包含一些全局常量
#include <linux/kvm_host.h>

static inline unsigned long __NEWSPACE gpt_get_gptbrel3(void)
{
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_GPT_GET_GPTBR_EL3,0,0,0,0,0,0,0,&res);

    return res.a0;
}


static inline unsigned long __NEWSPACE gpt_set_gptbrel3(unsigned int index)
{
	struct arm_smccc_res res;

	if (index > GPT_CLAVE_COUNT_MAX) {
		pr_err("index %d exceeds max allowed %d\n", index, GPT_CLAVE_COUNT_MAX);
		return -1;
	}

	arm_smccc_smc(SMC_GPT_SET_GPTBR_EL3,index,0,0,0,0,0,0,&res);

	return res.a0;
}


static inline unsigned long __NEWSPACE gpt_read_any_phy_addr(phys_addr_t phys)
{
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_GPT_READ_ANY_PHY_ADDR,phys,0,0,0,0,0,0,&res);

    return res.a0;
}

static inline unsigned long __NEWSPACE gpt_set_gpi(phys_addr_t phys,int pagecount,unsigned int gpi)
{
	struct arm_smccc_res res;

	arm_smccc_smc(SMC_GPT_SET_GPI,phys,pagecount,gpi,0,0,0,0,&res);

    return res.a0;
}

// cwz added for multi-gpt docker
// // 记录当前docker所使用的GPT数量，注意gpt_count是从0开始计数的，
// static int gpt_docker_count;
// 记录是否已经将最初的GPT切换为OS所在的GPT index 0，以免重复给gpt_docker_count赋初值0
static unsigned int gpt_kvm_init;
// 记录对应index下的GPT是否已经被使用，0表示未使用，1表示已经被使用，注意gpt_used[0]永远为1，因为0号GPT是OS所在的GPT
static int gpt_used[GPT_CLAVE_COUNT_MAX + 1] = {0};

// some kvm vcpu state backup
static struct kvm_vcpu *vcpu_backup[GPT_CLAVE_COUNT_MAX + 1] = {0};

// 函数调试输出gpt_used
static inline void print_gpt_used(void)
{
	char buf[200]; // 由于printk每次输出都带换行符，所以用buf来存储输出的内容，最后一次性输出
	int i;
	buf[0] = '\0';
	for(i = 0; i <= GPT_CLAVE_COUNT_MAX; i++)
	{
		// printk("%d ",gpt_used[i]);
		snprintf(buf, sizeof(buf) - 1, "%s%d ", buf, gpt_used[i]);
	}
	printk("gpt_used:%s\n",buf);
}

// 函数查找一个未被使用的GPT index，返回值为该GPT index，如果没有未被使用的GPT index，则返回-1
static inline unsigned int find_unused_gpt(void)
{
	int i;
	for(i = 2; i <= GPT_CLAVE_COUNT_MAX; i++)
	{
		if(gpt_used[i] == 0)
		{
			return i;
		}
	}
	return -1;
}

#endif

