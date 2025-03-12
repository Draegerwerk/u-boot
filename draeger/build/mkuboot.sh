#!/bin/bash

usage() {
    echo "usage:"
    echo "	$0 -up1"
    echo "	$0 -revocation"
    echo "	$0 -up2 [-spl] -dev DeviceId -fpga FPGA_Directory "
}

###############################################################################
### main ######################################################################
###############################################################################

set -o errexit ; set -o nounset

# find Git repo directory relative from the location of the called script
CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BASE_DIR=$( cd ${CURRENT_DIR}/../../.. >/dev/null 2>&1 && pwd )
SRC_DIR=${BASE_DIR}/u-boot
BLD_DIR=${BUILD_DIR:=${BASE_DIR}/bld}
BIN_DIR=${BLD_DIR}/UBoot
export ARCH=arm

case $(uname -o) in
    *Linux )    export CROSS_COMPILE=${CROSS_COMPILE-"arm-linux-gnueabi-"}
                ;;
    * )         echo "OS not supported: $(uname -o) "
                exit
                ;;
esac


PROC=""
CONFIG=""
DEV_ID=""
FPGA_DIR=""
NO_JOBS=""
SPL=0
INSTALL_DIR=""

while [ "x${1-}" != "x" ]; do

    case $1 in
        -up1 )          PROC=Up1
                        CONFIG=draeger_imx6m48_defconfig
                        INSTALL_DIR=${BLD_DIR}/bin/UBoot/${PROC}
                        export KBUILD_OUTPUT=${BIN_DIR}/${PROC}
                        ;;
        -revocation )	PROC=Up1
                        CONFIG=draeger_imx6m48_revoke_defconfig
                        INSTALL_DIR=${BLD_DIR}/bin/UBoot/Up1HabRevokeTest
                        export KBUILD_OUTPUT=${BIN_DIR}/Up1HabRevokeTest
                        ;;
        -up2 )          PROC=Up2
                        CONFIG=draeger_socfpga_m48_defconfig
                        INSTALL_DIR=${BLD_DIR}/bin/UBoot/${PROC}
                        ;;
        -spl )          SPL=1
                        ;;
        -dev )          DEV_ID=${2-""}
                        shift
                        ;;
        -fpga )         FPGA_DIR=${2-""}
                        shift
                        ;;
        -j )            NO_JOBS=${2-""}
                        shift
                        ;;
        * )             echo parameter failure
                        usage
                        exit 1
                        ;;
    esac
    shift
done

UBOOT_EXTRA_VERSION="-draeger_$(echo ${BUILD_TAG-'dev'} | cut -d'-' -f1 | cut -d'+' -f1)"

echo "BASE_DIR               = ${BASE_DIR}"
echo "ARCH                   = ${ARCH}"
echo "CROSS_COMPILE          = ${CROSS_COMPILE}"
echo "SRC_DIR                = ${SRC_DIR}"
echo "INSTALL_DIR            = ${INSTALL_DIR}"
echo "UBOOT_EXTRA_VERSION    = ${UBOOT_EXTRA_VERSION}"
echo "BUILD_TAG              = ${BUILD_TAG-""}"

if [ "${PROC}" == "" ]; then
    echo neither up1 nor up2 set
    usage;
fi

if [ "${PROC}" == "Up1" ]; then


    echo "KBUILD_OUTPUT          = ${KBUILD_OUTPUT}"

    if [ ! -f ${KBUILD_OUTPUT}/.config ]; then
        (cd ${SRC_DIR}; make ${CONFIG})
    fi
    (cd ${SRC_DIR}; make -j ${NO_JOBS} EXTRAVERSION=${UBOOT_EXTRA_VERSION} KCFLAGS=-Werror)

    cp ${KBUILD_OUTPUT}/SPL ${KBUILD_OUTPUT}/u-boot-unsigned.imx
    dd if=${KBUILD_OUTPUT}/u-boot-dtb.img of=${KBUILD_OUTPUT}/u-boot-unsigned.imx bs=512 seek=136

    echo Copying ${KBUILD_OUTPUT}/SPL to DIST dir ${INSTALL_DIR}
    mkdir -p ${INSTALL_DIR}
    cp ${KBUILD_OUTPUT}/SPL ${INSTALL_DIR}/SPLIMG
    cp ${KBUILD_OUTPUT}/u-boot-ivt.img ${INSTALL_DIR}/u-boot.img

    NXP_CODE_SIGNING_TOOL_PATH=${BASE_DIR}/thirdparty/nxp_code_signing_tool/cst/release/
    NXP_CODE_SIGNING_SCRIPT_PATH=${BASE_DIR}/thirdparty/nxp_code_signing_tool/scripts
    ${NXP_CODE_SIGNING_SCRIPT_PATH}/sign_development_key_preloader_and_bootloader.sh ${NXP_CODE_SIGNING_TOOL_PATH} ${INSTALL_DIR}/SPLIMG
    echo "-->  ${INSTALL_DIR}/u-boot.imx"

    ${KBUILD_OUTPUT}/tools/mkimage -T script -C none -n 'Update U-Boot SD' -d ${SRC_DIR}/draeger/scripts/UpdateUBootSD.scr ${INSTALL_DIR}/UpdateUBootSD.img
fi

if [ "${PROC}" == "Up2" ]; then
    if [ "${DEV_ID}" == "" ]; then
        echo Device id missing
        usage
        exit -1
    fi

    if [ "${FPGA_DIR}" == "" ]; then
        echo FPGA directory missing
        usage
        exit -1
    fi

    case $(uname -o) in
        Cygwin )    # falltrough
                    ;&
        Msys   )    FPGA_DIR=$(cygpath -u ${FPGA_DIR})
                    ;;
        *Linux )    ;;
        * )         echo "OS not found $(uname -o)"
                    exit
                    ;;
    esac

    KBUILD_OUTPUT=${BIN_DIR}/${PROC}/${DEV_ID}/bld
    SPLIMG=${KBUILD_OUTPUT}/spl/u-boot-spl.bin
    FPGA_BUILD_DIR=${BIN_DIR}/${PROC}/${DEV_ID}/src
    export KBUILD_OUTPUT

    echo "DEV_ID                 = ${DEV_ID}"
    echo "KBUILD_OUTPUT          = ${KBUILD_OUTPUT}"
    echo "SPLIMG                 = ${SPLIMG}"
	echo "FPGA_DIR               = ${FPGA_DIR}"

	echo "copy sources to Quartus 16 directory structure"
	mkdir -p ${FPGA_BUILD_DIR}/hps_isw_handoff/sdram
	mkdir -p ${FPGA_BUILD_DIR}/generated/sdram
	cp -v ${FPGA_DIR}/sdram/sdram_config.h      ${FPGA_BUILD_DIR}/generated/sdram
	cp -v ${FPGA_DIR}/sdram/sequencer_*         ${FPGA_BUILD_DIR}/hps_isw_handoff/sdram
	cp -v ${FPGA_DIR}/iocsr_config_cyclone5*    ${FPGA_BUILD_DIR}/generated
	cp -v ${FPGA_DIR}/pinmux_config_cyclone5.c  ${FPGA_BUILD_DIR}/generated
	cp -v ${FPGA_DIR}/pll_config.h              ${FPGA_BUILD_DIR}/generated
    
    # Make U-Boot happy:
    sed -i "s/CONFIG_HPS/CFG_HPS/g"             ${FPGA_BUILD_DIR}/generated/*.*
    sed -i "s/CONFIG_HPS/CFG_HPS/g"             ${FPGA_BUILD_DIR}/generated/sdram/*.*
    sed -i "s/CONFIG_HPS/CFG_HPS/g"             ${FPGA_BUILD_DIR}/hps_isw_handoff/sdram/*.*

    echo "generate sources ${SRC_DIR}/arch/arm/mach-socfpga/qts-filter.sh cyclone5 ${FPGA_BUILD_DIR} ${FPGA_BUILD_DIR} ${KBUILD_OUTPUT}/include/qts"
    mkdir -p ${KBUILD_OUTPUT}/include/qts
    ${SRC_DIR}/arch/arm/mach-socfpga/qts-filter.sh cyclone5 ${FPGA_BUILD_DIR} ${FPGA_BUILD_DIR} ${KBUILD_OUTPUT}/include/qts
    cp ${FPGA_DIR}/reset_config.h ${KBUILD_OUTPUT}/include/qts

    if [ ! -f ${KBUILD_OUTPUT}/.config ]; then
        (cd ${SRC_DIR}; make ${CONFIG})
    fi

    mkdir -p ${INSTALL_DIR}/${DEV_ID}
    echo "******* (cd ${SRC_DIR}; make -j EXTRAVERSION=${UBOOT_EXTRA_VERSION}) *******"
    (cd ${SRC_DIR}; make -j EXTRAVERSION=${UBOOT_EXTRA_VERSION})
    echo "******* copying images to ${INSTALL_DIR}/${DEV_ID} *******"
    echo "'4x ${KBUILD_OUTPUT}/spl/u-boot-spl.sfp' -> ${INSTALL_DIR}/${DEV_ID}/preloader-mkpimage.bin"
    cat ${KBUILD_OUTPUT}/spl/u-boot-spl.sfp ${KBUILD_OUTPUT}/spl/u-boot-spl.sfp ${KBUILD_OUTPUT}/spl/u-boot-spl.sfp ${KBUILD_OUTPUT}/spl/u-boot-spl.sfp > ${INSTALL_DIR}/${DEV_ID}/preloader-mkpimage.bin
    if [ "${SPL}" == "0" ]; then
        cp -v ${KBUILD_OUTPUT}/u-boot.img ${INSTALL_DIR}/${DEV_ID}/u-boot.img
        cp -v ${KBUILD_OUTPUT}/u-boot-with-spl.sfp ${INSTALL_DIR}/${DEV_ID}
        # cp -v ${KBUILD_OUTPUT}/examples/standalone/*.bin ${INSTALL_DIR}/${DEV_ID}
    fi
fi
