# How to use the build tools

It is recommended to use a Linux environment such as [Engineering Workplace](https://confluence.draeger.com/display/ENGWORKPLC) or [WSL2](https://confluence.draeger.com/display/M48/Setting+up+Ubuntu+in+Windows+%28WSL2%29+for+M48) for building all M48-related binaries.

All builds are done using a docker container, which itself is developed in its own [repository](https://stash.draeger.com/projects/DOCKER/repos/draeger-platform-uboot/browse).

## Build U-Boot for uP1

These bootloaders are to be used for uP1 in M48.1, M48.2, M48.3, and M48.4.

Bootloader and Preloader are built in one step.

1. Start Docker Container

    `$ cd <M48 project path>/u-boot/draeger/build`<br/>
    `$ ./startDockerContainer.sh`

2. Build uP1 Production U-Boot for all M48 based products

    `$ ./mkuboot.sh -up1`

3. Build uP1 Revocation U-Boot used to test the [High Assurance Boot (HAB)](https://confluence.draeger.com/pages/viewpage.action?pageId=190521033)

    `$ ./mkuboot.sh -revocation`

## Build U-Boot for uP2

Preloader and Bootloader are build in two separate build steps because the preloader depends on the product it is running on, while the Bootloader is the same for all products.

To build a U-Boot bootloader for the HPS of the Cyclone V, one needs to provide the generated header files coming from the Altera Quartus tools. The device name should be the device Id which is also used in the FPGA header for verification of the correct hardware it is running on.

1. Start Docker Container

    `$ cd <M48 project path>/u-boot/draeger/build`<br/>
    `$ ./startDockerContainer.sh`

2. Build uP2 Production U-Boot for M48.1, M48.3 and M48.4 based products

    `$ ./mkuboot.sh -up2 -dev <device name of FPGA> -fpga <path to generated FPGA sources>`

    Where for example for the test FPGA the device name is "M48Test" and path is "../../FPGA/Up2/M48Test/"

3. Build uP2 SPL Production U-Boot for M48.1, M48.3 and M48.4 based products

    `$ ./mkuboot.sh -up2 -spl -dev <device name of FPGA> -fpga <path to generated FPGA sources>`

## Build U-Boot - All

To build all targets except the uP1 Revocation U-Boot, and the uP2 U-Boot for the M48Test FPGA in one step.

`$ cd <M48 project path>/u-boot/draeger/build`<br/>
`$ ./startDockerContainer.sh -c ./build_all.sh`
