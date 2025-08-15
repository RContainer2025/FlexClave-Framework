# FVP with docker

## 一、Configuration
### BR configuration
- Remove the `exit 0` on line 101 of the `build-script/build.sh` script (to enable one-click full compilation mode).

- Before modifying the BR configuration, it is recommended to first execute `./build-script/build.sh -c all all`. If the execution fails, install `ake`, `gcc`, `gtk+-2.0`, `gmodule-2.0`, `libglade-2.0`, `flex`, `bison`, `ssl`, and `elf`

- Copy the files in the `FVP-docker/rootfs/` folder to `build-scripts/configs/aemfvp-a-rme/buildroot/rootfs_overlay/etc/`

- Copy the `FVP-docker/2020.05.config` file to `build-scripts/configs/aemfvp-a-rme/buildroot/`, replacing the original `2020.05.config`

- Execute `./build-scripts/build.sh -c br all`  

### Kernel paramater configuration

- Copy the `FVP-docker/build-linux.sh` file to `build-scripts/`, replacing the original `build-linux.sh`

- Execute `./build-scripts/build.sh -c kernel all`

## 二、Test docker in FVP
- Run `docker info` and enable `experimental` to `true`.

- Execute `./cpgON.sh`

- Execute `dockerd&` , and verify whether the Docker daemon is started

- Execute `docker load < arm-hello-world.tar` , to load the test image "hello-world"

- Execute `docker run hello-world` 

## 三、docker with arm64 image
- docker image pull --platform=arm64 imagename:tag

- docker save -o imagename.tar IMAGEID 

- docker load < imagename.tar
