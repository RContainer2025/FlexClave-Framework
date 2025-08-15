#!/usr/bin/env bash

# Copyright (c) 2015-2023, ARM Limited and Contributors. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of ARM nor the names of its contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

#
# This script uses the following environment variables from the variant
#
# VARIANT - build variant name
# TOP_DIR - workspace root directory
# CROSS_COMPILE - PATH to GCC including CROSS-COMPILE prefix
# TF_A_ARCH - aarch64 or aarch32
# TF_A_BUILD_ENABLED - Flag to enable building ARM Trusted Firmware
# TF_A_RME_BUILD_ENABLED - Flag to enable building ARM Trusted Firmware with
# 	support for RME
# TF_A_PATH - sub-directory containing ARM Trusted Firmware code
# TF_A_PLATS - List of platforms to be built (from available in tf-a/plat)
# TF_A_DEBUG_ENABLED - 1 = debug, 0 = release build
# TF_A_BUILD_TYPE - The type of build to perform
# TF_A_BUILD_FLAGS - Additional build flags to pass on the build command line
# TF_A_TBBR_BUILD_FLAGS - command line options to enable TBBR in ARM TF build
# OPTEE_BUILD_ENABLED - Flag to indicate if optee is enabled
# TBBR_{plat} - array of platform parameters, indexed by
# 	tbbr - flag to indicate if TBBR is enabled
# OUTPUT_DIR - Directory where build products are stored
# TF_A_GCC_PATH - Path where the compiler is installed
# VARIANT_ELF - The compiler variant

do_prepare ()
{
	echo
	echo -e "${GREEN}Preparing TF-A for $PLATFORM on [`date`]${NORMAL}"
	echo
}

do_build ()
{
	echo
	echo -e "${GREEN}Building TF-A for $PLATFORM on [`date`]${NORMAL}"
	echo

	pushd $TF_A_PATH
	# Set Compiler
	export CROSS_COMPILE=$VARIANT_ELF-
	export PATH=$TF_A_GCC_PATH:$PATH

	# Set debug flag, based on user choice
	local tfa_debug_flag
	[ "${TF_A_BUILD_TYPE}" == "debug" ] && tfa_debug_flag=1 || tfa_debug_flag=0

	# Build TF-A for normal boot flow (no tests on fip binary).
	BUILD_FLAGS="${TF_A_3W_FLAGS} ${TF_A_PRELOADED_KERNEL}"
	make -j$PARALLELISM \
		PLAT=$TF_A_PLATS \
		ARCH=$TF_A_ARCH \
		DEBUG=$tfa_debug_flag \
		${BUILD_FLAGS} \
		all fip
	popd
}

do_clean ()
{
	echo
	echo -e "${GREEN}Cleaning TF-A for $PLATFORM on [`date`]${NORMAL}"
	echo

	pushd $TF_A_PATH
	make realclean
	popd

	# Remove the bins copied to output-products
	rm -rf $OUTPUT_PLATFORM_DIR/bl1.bin
	rm -rf $OUTPUT_PLATFORM_DIR/fip.bin
	rm -rf $OUTPUT_PLATFORM_DIR/fip-std-tests.bin
}

do_package ()
{
	echo
	echo -e "${GREEN}Packaging TF-A for $PLATFORM on [`date`]${NORMAL}"
	echo


	cp ${TF_A_PATH}/build/${TF_A_PLATS}/${TF_A_BUILD_TYPE}/bl1.bin $OUTPUT_PLATFORM_DIR/bl1.bin
	cp ${TF_A_PATH}/build/${TF_A_PLATS}/${TF_A_BUILD_TYPE}/fip.bin $OUTPUT_PLATFORM_DIR/fip.bin
}

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/framework.sh $@
