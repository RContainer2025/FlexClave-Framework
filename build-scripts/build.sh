#!/usr/bin/env bash

# Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
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

set -e

# find the script directory...
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

__print_usage_exit() {
	echo " ./build-scripts/build.sh -c <component> <cmd>"
	echo "         -c <component>:    default=all | rmm | atf | kernel | br"
	echo "         <cmd>:             all | prepare | build | package | clean"
	echo "                            # all = clean & prepare & build & package"
	exit 1
}

__do_single_cmd() {
	export CMD=$1
	echo "***********************************"
	section_descriptor="$CMD for $build on aemfvp-a-rme[aemfvp-a-rme][buildroot]"
	echo "Execute $section_descriptor"
	${DIR}/$build $@
	
	if [ "$?" -ne 0 ]; then
		echo -e "${BOLD}${RED}Command failed: $section_descriptor${NORMAL}"
		exit 1
	fi

	echo "Execute $section_descriptor done."
	echo "-----------------------------------"
}

__do_build_loop() {
	for build in $CHOSEN_BUILD_SCRIPTS ; do
		__do_single_cmd clean
		__do_single_cmd prepare
		__do_single_cmd build
	done
	for build in $CHOSEN_BUILD_SCRIPTS ; do
		__do_single_cmd package
	done
}

__do_build()
{
	# Now to execute each component build in turn
	for build in $CHOSEN_BUILD_SCRIPTS; do
		__do_single_cmd $1
	done

	if [ "$CMD" = "clean" ]; then
		echo -e "${GREEN}Finishing clean by removing and $PLATDIR${NORMAL}"
		rm -rf $PLATDIR
	fi
}



# Parse the command line parameters -c <component>
while getopts "c:" opt; do
	case $opt in
		c)
			component=$OPTARG
			#echo "component=$component"
			;;
	esac
done

case $component in
	"")
		;&
	"all")
		echo "CHOSEN_BUILD_SCRIPTS = all"
		#exit 0
		CHOSEN_BUILD_SCRIPTS="\
			build-rmm.sh \
			build-tf-a.sh \
			build-linux.sh \
			build-buildroot.sh \
			"
		;;
	"rmm")
		echo "CHOSEN_BUILD_SCRIPTS = rmm"
		CHOSEN_BUILD_SCRIPTS="\
			build-rmm.sh \
			"
		;;
	"atf")
		echo "CHOSEN_BUILD_SCRIPTS = atf"
		CHOSEN_BUILD_SCRIPTS="\
			build-tf-a.sh \
			"
		;;
	"kernel")
		echo "CHOSEN_BUILD_SCRIPTS = kernel"
		CHOSEN_BUILD_SCRIPTS="\
			build-linux.sh \
			"
		;;
	"br")
		echo "CHOSEN_BUILD_SCRIPTS = br"
		CHOSEN_BUILD_SCRIPTS="\
			build-buildroot.sh \
			"
		;;
	*)
		echo "[ERROR] unsupported build obj $component."
		__print_usage_exit
		;;
esac


# Parse the command line parameters
OPT_CMD=${@:$OPTIND:1}

# Ensure a build command is specified
if [ -z "$OPT_CMD" ] ; then
	echo "[ERROR] no build command."
	__print_usage_exit
fi

# Ensure that the build command is supported
if [ "$OPT_CMD" != "all" -a \
		"$OPT_CMD" != "prepare" -a \
		"$OPT_CMD" != "build" -a \
		"$OPT_CMD" != "package" -a \
		"$OPT_CMD" != "clean" ] ; then
	echo "[ERROR] unsupported build command $OPT_CMD."
	__print_usage_exit
fi


if [ "$OPT_CMD" = "all" ] ; then
	__do_build_loop
else
	__do_build $OPT_CMD
fi

