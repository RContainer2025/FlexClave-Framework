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
# PARALLELISM - number of cores to build across
# LINUX_BUILD_ENABLED - Flag to enable building Linux
# LINUX_RME_BUILD_ENABLED - Flag to enable building ARM Trusted Firmware with
# 	support for RME
# LINUX_PATH - sub-directory containing Linux code
# LINUX_ARCH - Build architecture (arm64)
# LINUX_CONFIG_LIST - List of Linaro configs to use to build
# LINUX_CONFIG_DEFAULT - the default from the list (for tools)
# LINUX_{config} - array of linux config options, indexed by
# 	path - the path to the linux source
#	defconfig - a defconfig to build
#	config - the list of config fragments
# TARGET_BINS_PLATS - the platforms to create binaries for
# TARGET_{plat} - array of platform parameters, indexed by
#	fdts - the fdt pattern used by the platform
# UBOOT_UIMAGE_ADDRS - address at which to link UBOOT image
# UBOOT_MKIMAGE - path to uboot mkimage
# LINUX_ARCH - the arch
# UBOOT_BUILD_ENABLED - flag to indicate the need for uimages.
# LINUX_IMAGE_TYPE - Image or zImage (Image is the default if not specified)
# LINUX_GCC_PATH - Path where the compiler is installed
# VARIANT_LINUX_GNU - The compiler variant
# OUTPUT_DIR - Directory where build products are stored
#

do_prepare ()
{
	echo
	echo -e "${GREEN}Preparing Linux for $PLATFORM on [`date`]${NORMAL}"
	echo
	
	pushd $LINUX_PATH
	# Set Compiler
	export CROSS_COMPILE=$LINUX_CROSS_COMPILE
	export PATH=$LINUX_GCC_PATH:$PATH
	export ARCH=$LINUX_ARCH

	make defconfig
	#original
	./scripts/config --enable CONFIG_DMA_RESTRICTED_POOL
	./scripts/config --enable CONFIG_TUN
	./scripts/config --enable CONFIG_MACVLAN
	./scripts/config --enable CONFIG_MACVTAP
	./scripts/config --enable CONFIG_TAP
	./scripts/config --enable CONFIG_IPV6
	./scripts/config --enable CONFIG_BRIDGE
	./scripts/config --enable CONFIG_EFI_GENERIC_STUB_INITRD_CMDLINE_LOADER
	./scripts/config --enable CONFIG_SAMPLES
	./scripts/config --set-str CONFIG_CMDLINE "cpuidle.off=1"
	./scripts/config --enable CONFIG_CMDLINE_EXTEND
	./scripts/config --enable CONFIG_UDMABUF
	./scripts/config --enable CONFIG_CHECKPOINT_RESTORE

	#add after check-ZQH.config
	./scripts/config --enable CONFIG_BRIDGE_NETFILTER
	./scripts/config --enable CONFIG_NF_CONNTRACK
	./scripts/config --enable CONFIG_NF_LOG_SYSLOG
	./scripts/config --enable CONFIG_NF_NAT
	./scripts/config --enable CONFIG_NETFILTER_XTABLES
	./scripts/config --enable CONFIG_NETFILTER_XT_MARK
	./scripts/config --enable CONFIG_NETFILTER_XT_TARGET_CHECKSUM
	./scripts/config --enable CONFIG_NETFILTER_XT_TARGET_LOG
	./scripts/config --enable CONFIG_NETFILTER_XT_NAT
	./scripts/config --enable CONFIG_NETFILTER_XT_TARGET_MASQUERADE
	./scripts/config --enable CONFIG_NETFILTER_XT_MATCH_ADDRTYPE
	./scripts/config --enable CONFIG_NETFILTER_XT_MATCH_CONNTRACK
	./scripts/config --enable CONFIG_NETFILTER_XT_MATCH_IPVS
	./scripts/config --enable CONFIG_IP_VS
	./scripts/config --enable CONFIG_NF_DEFRAG_IPV4
	./scripts/config --enable CONFIG_NF_REJECT_IPV4
	./scripts/config --enable CONFIG_IP_NF_IPTABLES
	./scripts/config --enable CONFIG_IP_NF_FILTER
	./scripts/config --enable CONFIG_IP_NF_TARGET_REJECT
	./scripts/config --enable CONFIG_IP_NF_NAT
	./scripts/config --enable CONFIG_IP_NF_TARGET_MASQUERADE
	./scripts/config --enable CONFIG_IP_NF_MANGLE
	./scripts/config --enable CONFIG_NF_DEFRAG_IPV6
	./scripts/config --enable CONFIG_OVERLAY_FS
	./scripts/config --enable CONFIG_LIBCRC32C


	#add after docker-check-config
	./scripts/config --enable CONFIG_VETH
	./scripts/config --enable CONFIG_BLK_DEV_THROTTLING
	./scripts/config --enable CONFIG_CGROUP_NET_PRIO
	./scripts/config --enable CONFIG_CFS_BANDWIDTH
	./scripts/config --enable CONFIG_IP_NF_TARGET_REDIRECT
	./scripts/config --enable CONFIG_IP_VS_NFCT
	./scripts/config --enable CONFIG_IP_VS_PROTO_TCP
	./scripts/config --enable CONFIG_IP_VS_PROTO_UDP
	./scripts/config --enable CONFIG_IP_VS_RR
	./scripts/config --enable CONFIG_SECURITY_APPARMOR
	./scripts/config --enable CONFIG_EXT3_FS_POSIX_ACL
	./scripts/config --enable CONFIG_EXT3_FS_SECURITY
	./scripts/config --enable CONFIG_EXT4_FS_SECURITY
	./scripts/config --enable CONFIG_VXLAN
	./scripts/config --enable CONFIG_XFRM_USER
	./scripts/config --enable CONFIG_INET_ESP
	./scripts/config --enable CONFIG_NETFILTER_XT_MATCH_BPF
	./scripts/config --enable CONFIG_IPVLAN
	./scripts/config --enable CONFIG_DUMMY
	./scripts/config --enable CONFIG_NF_CONNTRACK_FTP
	./scripts/config --enable CONFIG_NF_CONNTRACK_TFTP
	
	# # try to disable KASLR
	# ./script/config --disable CONFIG_RANDOMIZE_BASE
	# ./script/config --disable CONFIG_RANDOMIZE_MODULE_REGION_FULL

	# try to fix modpost: "kallsyms_lookup_name" undefined
	./scripts/config --enable CONFIG_KPROBES

	make olddefconfig
	popd
}

do_build ()
{
	echo
	echo -e "${GREEN}Building Linux for $PLATFORM on [`date`]${NORMAL}"
	echo

	pushd $LINUX_PATH
	# Set Compiler
	export CROSS_COMPILE=$LINUX_CROSS_COMPILE
	export PATH=$LINUX_GCC_PATH:$PATH
	export ARCH=$LINUX_ARCH

	make -j$PARALLELISM $LINUX_IMAGE
	make -j$PARALLELISM modules
	popd
}

do_clean ()
{
	echo
	echo -e "${GREEN}Cleaning Linux for $PLATFORM on [`date`]${NORMAL}"
	echo

	pushd $LINUX_PATH
	make distclean
	popd

	# Remove the bins copied to output-products
	rm -rf $OUTPUT_PLATFORM_DIR/Image
}

do_package ()
{
	echo
	echo -e "${GREEN}Packing Linux for $PLATFORM on [`date`]${NORMAL}"
	echo

	cp ${LINUX_PATH}/arch/arm64/boot/Image $OUTPUT_PLATFORM_DIR/Image
}

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/framework.sh $@