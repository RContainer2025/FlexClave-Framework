// #ifndef __ASM_GPT_CMDS_H
// #define __ASM_GPT_CMDS_H


// #include <linux/arm-smccc.h>
// #include <asm/gpt_smc.h>



// static inline unsigned long gpt_get_gptbrel3(void)
// {
// 	struct arm_smccc_res res;

// 	arm_smccc_1_1_invoke(SMC_GPT_GET_GPTBR_EL3,&res);

//     return res.a0;
// }


// static inline unsigned long gpt_set_gptbrel3(unsigned int index)
// {
// 	struct arm_smccc_res res;

// 	arm_smccc_1_1_invoke(SMC_GPT_SET_GPTBR_EL3,index,&res);

// 	return res.a0;
// }


// static inline unsigned long gpt_read_any_phy_addr(phys_addr_t phys)
// {
// 	struct arm_smccc_res res;

// 	arm_smccc_1_1_invoke(SMC_GPT_READ_ANY_PHY_ADDR,phys,&res);

//     return res.a0;
// }

// static inline unsigned long gpt_set_gpi(phys_addr_t phys,int pagecount,unsigned int gpi)
// {
// 	struct arm_smccc_res res;

// 	arm_smccc_1_1_invoke(SMC_GPT_SET_GPI,phys,pagecount,gpi,&res);

//     return res.a0;
// }

// // cwz added for multi-gpt docker
// // 一个全局变量，记录当前docker所使用的GPT数量，注意gpt_count是从0开始计数的，
// static unsigned int gpt_docker_count;
// // 另一个全局变量，记录是否已经将最初的GPT切换为OS所在的GPT index 0，以免重复给gpt_docker_count赋初值0
// static unsigned int gpt_os_init = 0;
// // 宏设置gpt_docker_count的最大值，即最多支持多少个docker，当前为32 - 2
// #define GPT_DOCKER_COUNT_MAX 30
// // 设置一个全局变量数组，记录对应index下的GPT是否已经被使用，0表示未使用，1表示已经被使用，注意gpt_used[0]永远为1，因为0号GPT是OS所在的GPT
// static unsigned int gpt_used[GPT_DOCKER_COUNT_MAX + 1] = {0};

// // 函数调试输出gpt_used
// static inline void print_gpt_used(void)
// {
// 	int i;
// 	printk("gpt_used:");
// 	for(i = 0; i <= GPT_DOCKER_COUNT_MAX; i++)
// 	{
// 		printk("%d ",gpt_used[i]);
// 	}
// 	printk("\n");
// }

// // 函数查找一个未被使用的GPT index，返回值为该GPT index，如果没有未被使用的GPT index，则返回-1
// static inline unsigned int find_unused_gpt(void)
// {
// 	int i;
// 	for(i = 2; i <= GPT_DOCKER_COUNT_MAX; i++)
// 	{
// 		if(gpt_used[i] == 0)
// 		{
// 			return i;
// 		}
// 	}
// 	return -1;
// }

// #endif