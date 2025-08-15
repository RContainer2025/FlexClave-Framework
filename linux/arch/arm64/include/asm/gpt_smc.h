#ifndef __ASM_GPT_SMC_H
#define __ASM_GPT_SMC_H

#include <linux/arm-smccc.h>

#define SMC_GPT_CALL(func)				\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,		\
			   ARM_SMCCC_SMC_64,		\
			   ARM_SMCCC_OWNER_STANDARD,	\
			   (func))


/* GPT GPI definitions */
#define GPT_GPI_NO_ACCESS		UL(0x0)
#define GPT_GPI_SECURE			UL(0x8)
#define GPT_GPI_NS			UL(0x9)
#define GPT_GPI_ROOT			UL(0xA)
#define GPT_GPI_REALM			UL(0xB)
#define GPT_GPI_ANY			UL(0xF)
#define GPT_GPI_VAL_MASK		UL(0xF)



//smc_fid 0x401-0x4ff
#define SMC_GPT_GET_GPTBR_EL3 SMC_GPT_CALL(0x401)
#define SMC_GPT_SET_GPI SMC_GPT_CALL(0x402)
#define SMC_GPT_READ_ANY_PHY_ADDR SMC_GPT_CALL(0x403)
#define SMC_GPT_SET_GPTBR_EL3 SMC_GPT_CALL(0x404)



#endif