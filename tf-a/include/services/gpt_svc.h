#ifndef GPT_SVC_H
#define GPT_SVC_H


#include <lib/smccc.h>
#include <lib/utils_def.h>


/* STD calls FNUM Min/Max ranges */
#define GPT_FNUM_MIN_VALUE	U(0x400)
#define GPT_FNUM_MAX_VALUE	U(0x4ff)

/* Construct GPT fastcall std FID from offset */
#define SMC64_GPT_FID(_offset)					  \
	((SMC_TYPE_FAST << FUNCID_TYPE_SHIFT)			| \
	 (SMC_64 << FUNCID_CC_SHIFT)				| \
	 (OEN_STD_START << FUNCID_OEN_SHIFT)			| \
	 (((GPT_FNUM_MIN_VALUE + (_offset)) & FUNCID_NUM_MASK)	  \
	  << FUNCID_NUM_SHIFT))


#define is_gpt_fid(fid) __extension__ ({		\
	__typeof__(fid) _fid = (fid);			\
	((GET_SMC_NUM(_fid) >= GPT_FNUM_MIN_VALUE) &&	\
	 (GET_SMC_NUM(_fid) <= GPT_FNUM_MAX_VALUE) &&	\
	 (GET_SMC_TYPE(_fid) == SMC_TYPE_FAST)	   &&	\
	 (GET_SMC_CC(_fid) == SMC_64)              &&	\
	 (GET_SMC_OEN(_fid) == OEN_STD_START)      &&	\
	 ((_fid & 0x00FE0000) == 0U)); })

#define GPT_GET_GPTBR_EL3 SMC64_GPT_FID(U(0x1))
#define GPT_SET_GPI SMC64_GPT_FID(U(0x2))
#define GPT_READ_ANY_PHY_ADDR SMC64_GPT_FID(U(0x3))
#define GPT_SET_GPTBR_EL3 SMC64_GPT_FID(U(0x4))

uint64_t gpt_handler(uint32_t smc_fid,
		uint64_t x1,
		uint64_t x2,
		uint64_t x3,
		uint64_t x4,
		void *cookie,
		void *handle,
		uint64_t flags);


#endif