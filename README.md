# FLEXCLAVE-FRAMEWORK

Official ARM Tutorial: docs/aemfvp-a-rme/user-guide.rst · master · Arm Reference Solutions / arm-reference-solutions-docs · GitLab

FVP Version: 11.21.15

## Overview of the CCA Software Stack

| Component Name           | Repository URL <br> Version (Tag🏷️ / Commit 🆔)                | Execution Level         | Function Description                           | Retained? |
| ------------------------ | ------------------------------------------------------------ | ----------------------- | ---------------------------------------------- | --------- |
| Linux-CCA                | https://git.gitlab.arm.com/linux-arm/linux-cca.git   🏷️ cca-full/rfc-v1 | EL0/1/2   R-EL0/1       | Host OS and VM kernel with built-in RME driver | ✅         |
| Kvmtool-CCA              | https://git.gitlab.arm.com/linux-arm/kvmtool-cca.git   🏷️ cca/rfc-v1 | EL0                     | Lightweight QEMU-like tool                     | ✅         |
| Kvm-Unit-Tests-CCA       | https://git.gitlab.arm.com/linux-arm/kvm-unit-tests-cca.git   🏷️ cca/rfc-v1 | EL0/1                   | KVM unit testing tool                          | ❌         |
| Build-scripts            | https://git.gitlab.arm.com/arm-reference-solutions/build-scripts.git   🏷️ AEMFVP-A-RME-2023.03.17 | Development Environment | Build scripts                                  | ✅         |
| Model-scripts            | https://git.gitlab.arm.com/arm-reference-solutions/model-scripts.git   🏷️ AEMFVP-A-RME-2023.03.17 | Development Environment | FVP runtime scripts                            | ✅         |
| Trusted-Firmware-A       | https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git   🆔 3d2da6f5d3a931d97e0294f0a565b1810a55ab98 | EL3                     | EL3 level firmware                             | ✅         |
| Trusted-Firmware-A-Tests | https://git.trustedfirmware.org/TF-A/tf-a-tests.git   🆔 8d80d6501af8a903b9a828ee57a6f490fc89bc18 | ❓EL3                    | TFTF full-stack testing components             | ❌         |
| Hafnium                  | https://git.trustedfirmware.org/hafnium/hafnium.git   🆔 4df5520d166a2955566ce8826a5254f7f7ff5fdc | S-EL2                   | SPMC component                                 | ❌         |
| TF-RMM                   | https://git.trustedfirmware.org/TF-RMM/tf-rmm.git   🆔 61bdf4e8418f8c34def91a0ea6e4057f535e211a | R-EL2                   | RMM component                                  | ✅         |
| Buildroot                | https://github.com/buildroot/buildroot.git   🏷️ 2020.05       | EL0/1/2   R-EL0/1       | Lightweight FS for host and VMs                | ✅         |

You can check flexclave linux modification on [Container](linux/arch/arm64/mm) and [VM](linux/arch/arm64/kvm), ATF modification on [gpt_rme](tf-a/lib/gpt_rme).

## TODO 
**We are currently porting this project to QEMU-Linaro and continuously improving it.**
