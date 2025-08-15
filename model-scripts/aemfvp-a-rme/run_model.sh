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

# Check for env MODEL
if [ ! -e "$MODEL" ]; then
	echo "ERROR: you should set variable MODEL to point to a valid FVP model binary."
	echo "Current MODEL value is \"$MODEL\""
	exit 1
fi

# # Scan for free local ports, starting at specified value
# function get_free_port {
# 	local port=$1
# 	local used=$(netstat -lat | tail -n+3 | awk '{print $4}' | cut -d ':' -f2)
# 	while : ; do
# 		if ( ! echo ${used} | grep -q ${port} ) then
# 			echo ${port}
# 			return
# 		fi
# 		port=$(expr ${port} + 1)
# 		[[ ${port} -lt 65536 ]] || port=1
# 		[[ ${port} != ${1} ]] || return
# 	done
# }

function get_free_port {
    local port=$1
    local timeout=1  # 设置超时时间(秒)
    
    # 使用ss命令替代netstat，更高效
    while : ; do
        # 检查端口是否被占用
        if ! (ss -tuln | grep -q ":${port}[[:space:]]"); then
            # 额外验证端口是否真正可用(尝试绑定)
            if timeout ${timeout} bash -c "</dev/tcp/localhost/${port}" 2>/dev/null; then
                port=$((port + 1))
                [[ ${port} -lt 65536 ]] || port=1
                [[ ${port} != ${1} ]] || return 1
                continue
            fi
            echo ${port}
            return 0
        fi
        port=$((port + 1))
        [[ ${port} -lt 65536 ]] || port=1
        [[ ${port} != ${1} ]] || return 1
    done
}

# Create log files
LOG_DIR=logs/$platform
mkdir -p ./$LOG_DIR
datestamp=`date +%s%N`

UART0_LOG=uart0-${datestamp}.log
touch $LOG_DIR/$UART0_LOG
uart0log=uart0.log
rm -f $LOG_DIR/$uart0log
ln -s $UART0_LOG $LOG_DIR/$uart0log

UART1_LOG=uart1-${datestamp}.log
touch $LOG_DIR/$UART1_LOG
uart1log=uart1.log
rm -f $LOG_DIR/$uart1log
ln -s $UART1_LOG $LOG_DIR/$uart1log

UART2_LOG=uart2-${datestamp}.log
touch $LOG_DIR/$UART2_LOG
uart2log=uart2.log
rm -f $LOG_DIR/$uart2log
ln -s $UART0_LOG $LOG_DIR/$uart2log

UART3_LOG=uart3-${datestamp}.log
touch $LOG_DIR/$UART3_LOG
uart3log=uart3.log
rm -f $LOG_DIR/$uart3log
ln -s $UART3_LOG $LOG_DIR/$uart3log

MODEL_LOG=model-${datestamp}.log
touch $LOG_DIR/$MODEL_LOG
modellog=model.log
rm -f $LOG_DIR/$modellog
ln -s $MODEL_LOG $LOG_DIR/$modellog

# Check for free SSH ports
SSH_PORT=$(get_free_port 8022)
# Check for free httpd port
HTTPD_PORT=$(get_free_port 8080)
# Check for free memcached port
MEM_PORT=$(get_free_port 11211)
# Check for free iperf3 server port
IPERF3_PORT=$(get_free_port 5201)
# Check for free Mysql server port 
MYSQL_PORT=$(get_free_port 3308)

cmd_args=(
	-C bp.terminal_0.terminal_command=\"zellij action new-pane -n %title --floating -c -- telnet localhost %port\"
	-C bp.terminal_1.terminal_command=\"zellij action new-pane -n %title --floating -c -- telnet localhost %port\"
	-C bp.terminal_2.terminal_command=\"zellij action new-pane -n %title --floating -c -- telnet localhost %port\"
	-C bp.terminal_3.terminal_command=\"zellij action new-pane -n %title --floating -c -- telnet localhost %port\"
	-C bp.refcounter.non_arch_start_at_default=1
	-C bp.refcounter.use_real_time=0
	-C bp.ve_sysregs.exit_on_shutdown=1
	-C cache_state_modelled=0
	-C bp.dram_metadata.is_enabled=1
	-C bp.dram_size=4
	-C bp.secure_memory=1
	-C cluster0.NUM_CORES=4
	-C cluster0.PA_SIZE=48
	-C cluster0.ecv_support_level=2
	-C cluster0.gicv3.cpuintf-mmap-access-level=2
	-C cluster0.gicv3.without-DS-support=1
	-C cluster0.gicv4.mask-virtual-interrupt=1
	-C cluster0.has_arm_v8-6=1
	-C cluster0.has_amu=1
	-C cluster0.has_branch_target_exception=1
	-C cluster0.rme_support_level=2
	-C cluster0.has_rndr=1
	-C cluster0.has_v8_7_pmu_extension=2
	-C cluster0.max_32bit_el=0
	-C cluster0.stage12_tlb_size=1024
	-C cluster0.check_memory_attributes=0
	-C cluster0.memory_tagging_support_level=2
	-C cluster0.restriction_on_speculative_execution=2
	-C cluster0.restriction_on_speculative_execution_aarch32=2
	-C cluster1.NUM_CORES=4
	-C cluster1.PA_SIZE=48
	-C cluster1.ecv_support_level=2
	-C cluster1.gicv3.cpuintf-mmap-access-level=2
	-C cluster1.gicv3.without-DS-support=1
	-C cluster1.gicv4.mask-virtual-interrupt=1
	-C cluster1.has_arm_v8-6=1
	-C cluster1.has_amu=1
	-C cluster1.has_branch_target_exception=1
	-C cluster1.rme_support_level=2
	-C cluster1.has_rndr=1
	-C cluster1.has_v8_7_pmu_extension=2
	-C cluster1.max_32bit_el=0
	-C cluster1.stage12_tlb_size=1024
	-C cluster1.check_memory_attributes=0
	-C cluster1.memory_tagging_support_level=2
	-C cluster1.restriction_on_speculative_execution=2
	-C cluster1.restriction_on_speculative_execution_aarch32=2
	-C pci.pci_smmuv3.mmu.SMMU_AIDR=2
	-C pci.pci_smmuv3.mmu.SMMU_IDR0=0x0046123B
	-C pci.pci_smmuv3.mmu.SMMU_IDR1=0x00600002
	-C pci.pci_smmuv3.mmu.SMMU_IDR3=0x1714
	-C pci.pci_smmuv3.mmu.SMMU_IDR5=0xFFFF0475
	-C pci.pci_smmuv3.mmu.SMMU_S_IDR1=0xA0000002
	-C pci.pci_smmuv3.mmu.SMMU_S_IDR2=0
	-C pci.pci_smmuv3.mmu.SMMU_S_IDR3=0
	-C pci.pci_smmuv3.mmu.SMMU_ROOT_IDR0=3
	-C pci.pci_smmuv3.mmu.SMMU_ROOT_IIDR=0x43B
	-C pci.pci_smmuv3.mmu.root_register_page_offset=0x20000
	-C pctl.startup=0.0.0.0
	-C bp.smsc_91c111.enabled=1
	-C bp.has_rme=1
	-C bp.hostbridge.interfaceName=tap0
	-C bp.pl011_uart0.uart_enable=0
	-C bp.pl011_uart1.uart_enable=0
	-C bp.pl011_uart2.uart_enable=0
	-C bp.pl011_uart3.uart_enable=0
	-C bp.pl011_uart0.out_file=$LOG_DIR/$uart0log
	-C bp.pl011_uart1.out_file=$LOG_DIR/$uart1log
	-C bp.pl011_uart2.out_file=$LOG_DIR/$uart2log
	-C bp.pl011_uart3.out_file=$LOG_DIR/$uart3log
	-C bp.pl011_uart0.unbuffered_output=1
	-C bp.pl011_uart1.unbuffered_output=1
	-C bp.pl011_uart2.unbuffered_output=1
	-C bp.pl011_uart3.unbuffered_output=1
	-C bp.secureflashloader.fname=$BL1
	-C bp.flashloader0.fname=$FIP
	-C bp.hostbridge.userNetworking=1
	-C bp.hostbridge.userNetPorts=${SSH_PORT}=22,${HTTPD_PORT}=8080,${MEM_PORT}=11211,${IPERF3_PORT}=5201,${MYSQL_PORT}=3306
	--stat
	-R
	# --iris-connect tcpserver,allowRemote
)

cmd="${MODEL} ${cmd_args[@]}"



	if [[ ! -z "$FS" ]]; then
		cmd="$cmd -C bp.virtioblockdevice.image_path=$FS"
	fi

	if [[ ! -z "$IMAGE" ]]; then
		cmd="$cmd --data cluster0.cpu0=$IMAGE@0x84000000"
	fi
