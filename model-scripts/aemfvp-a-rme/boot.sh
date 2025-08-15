#!/bin/bash

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

print_usage ()
{
	echo ""
	echo "Usage: ./boot.sh <boot_mode>"
	echo ""
	echo "Boots the stack to buildroot console on a specified platform."
	echo "The login details are 'root' for both user and password."
	echo ""
	echo ""
	echo "Mandatory argument <boot_mode> is one of the following:"
	echo "  shell - Boots the software stack and fvp for interactive usage."
	echo "  validate - Boots the software stack and fvp, and parses the log"
	echo "	     files to validate that four worlds were successfully booted."
	echo ""
}

# Defaults
platform=aemfvp-a-rme
out_dir="output/$platform"
BL1="$out_dir/bl1.bin"
FIP="$out_dir/fip.bin"
IMAGE="$out_dir/Image"
FS="$out_dir/host-fs.ext4"

if [ ! -e "$MODEL" ]; then
	MODEL=$(pwd)"/FVP/Base_RevC_AEMvA_pkg/models/Linux64_GCC-9.3/FVP_Base_RevC-2xAEMvA"
fi


function 4_world_boot_test {
	# Launch and wait boot to finish
	$cmd | tee $LOG_DIR/$modellog &
	until [ -f $LOG_DIR/$modellog ]; do sleep 0.1; done
	grep -q "Welcome to Buildroot" <( tail -f $LOG_DIR/$uart0log )

	# Get PID of launched FVP and terminate it.
	FVP_PID=$(pgrep FVP_Base_RevC-2)
	kill $FVP_PID
	sleep 1

	local secure_status=1
	local rmm_status=1
	local nonsecure_status=1

	# Parse secure world string
	grep -q "INFO: Hafnium initialisation completed" $LOG_DIR/$uart0log
	secure_status=$?
	[ $secure_status == 0 ] \
		&& echo "[INFO]: Secure world boot successful" \
		|| echo "[ERROR]: Secure world boot unsuccessful"

	# Parse realm world string
	grep -q "INFO.*RMM init end" $LOG_DIR/$uart0log
	rmm_status=$?
	[ $rmm_status == 0 ] \
		&& echo "[INFO]: Realm world boot successful" \
		|| echo "[ERROR]: Realm world boot unsuccessful"

	# Parse non-secure world string
	grep -q "Welcome to Buildroot" $LOG_DIR/$uart0log
	nonsecure_status=$?
	[ $nonsecure_status == 0 ] \
		&& echo "[INFO]: Non-Secure world boot successful" \
		|| echo "[ERROR]: Non-Secure world boot unsuccessful"

	# Error if any of the previous fails
	[[ $secure_status == 0 && $rmm_status == 0 && $nonsecure_status == 0 ]] \
		&& echo "[INFO]: Four world boot successful"; return 0 \
		|| echo "[ERROR]: Four world boot unsuccessful"; return 1
}

# Parse the boot_mode after the optional arguments
BOOT_MODE=${@:$OPTIND:1}

source ./model-scripts/$platform/run_model.sh

case $BOOT_MODE in
	"")
		echo "BOOT_MODE default"
		;&
	"shell")
		echo "BOOT FVP FOR INTERACTIVE USAGE"
		# # shell script debug use
		# echo $cmd
		# echo "-------------------------------"
		# bash -c "${cmd}" | tee $LOG_DIR/$modellog &
		bash -c "${cmd}"
		;;

	"validate")
		echo "VALIDATE FOUR WORLD BOOT"
		4_world_boot_test
		[ $? -eq 0 ] && exit 0 || exit 1
		;;
	*)
		echo ""
		echo "[ERROR]: unsupported or missing boot_mode"
		print_usage
		exit 1
		;;
esac
