#!/usr/bin/env bash

# Copyright (c) 2023, ARM Limited and Contributors. All rights reserved.
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
# RMM_BUILD_TYPE - The type of build to perform
# RMM_LOG_LEVEL - RMM log level
# RMM_PATH - Directory containing the RMM source
# RMM_BUILD_DIR - Directory where RMM build products are stored
# RMM_GCC_PATH - Path where the compiler is installed
# VARIANT_ELF - The compiler variant
#

do_prepare ()
{
	echo
	echo -e "${GREEN}Preparing RMM for $PLATFORM on [`date`]${NORMAL}"
	echo
	
	# Set Compiler
	export CROSS_COMPILE=$VARIANT_ELF-
	export PATH=$RMM_GCC_PATH:$PATH

	cmake -DRMM_CONFIG=fvp_defcfg \
		-DCMAKE_BUILD_TYPE=${RMM_BUILD_TYPE} \
		-DLOG_LEVEL=${RMM_LOG_LEVEL} \
		-S ${RMM_PATH} \
		-B ${RMM_BUILD_DIR}
}

do_build ()
{
	echo
	echo -e "${GREEN}Building RMM for $PLATFORM on [`date`]${NORMAL}"
	echo

	# Set Compiler
	export CROSS_COMPILE=$VARIANT_ELF-
	export PATH=$RMM_GCC_PATH:$PATH

	cmake --build ${RMM_BUILD_DIR}
}

do_clean ()
{
	echo
	echo -e "${GREEN}Cleaning RMM for $PLATFORM on [`date`]${NORMAL}"
	echo

	rm -rf $RMM_BUILD_DIR
}

do_package ()
{
	echo
	echo -e "${GREEN}Packing RMM for $PLATFORM on [`date`]${NORMAL}"
	echo

# Dummy call, RMM is packed into TF-A FIP binary.
}


DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/framework.sh $@
