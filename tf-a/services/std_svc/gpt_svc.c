#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include <arch_helpers.h>
#include <arch_features.h>
#include <common/runtime_svc.h>
#include <lib/el3_runtime/context_mgmt.h>
#include <lib/el3_runtime/pubsub.h>
#include <lib/gpt_rme/gpt_rme.h>

#include <lib/spinlock.h>
#include <lib/utils.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/common_def.h>
#include <plat/common/platform.h>
#include <platform_def.h>
#include <smccc_helpers.h>
#include <lib/extensions/sve.h>

#include <services/gpt_svc.h>



uint64_t gpt_handler(uint32_t smc_fid,
		uint64_t x1,
		uint64_t x2,
		uint64_t x3,
		uint64_t x4,
		void *cookie,
		void *handle,
		uint64_t flags)
{
	int ret;
	switch(smc_fid){
	
	case GPT_GET_GPTBR_EL3:
	
		u_register_t reg;
		reg=read_gptbr_el3();
		SMC_RET1(handle, reg);
	
	case GPT_SET_GPI:
		for(int i=0;i<x2;i++)
		{
			ret=gpt_set_gpi(x1+PAGE_SIZE_4KB*i,PAGE_SIZE_4KB,x3);
			if(ret!=0)	SMC_RET1(handle, ret);
		}
		SMC_RET1(handle, ret);	

	case GPT_READ_ANY_PHY_ADDR:

		unsigned long value=*(unsigned long *)x1;
		SMC_RET1(handle,value);
	
	case GPT_SET_GPTBR_EL3:

		ret=set_gpt(x1);
		SMC_RET1(handle,ret);

	default:

		SMC_RET1(handle, SMC_UNK);			
	}
	
}