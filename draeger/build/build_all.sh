#!/bin/bash

usage() {
    echo "usage:"
    echo "  $0      # build all"
    echo "  $0 -up1 # build only uP1"
    echo "  $0 -up2 # build only uP2"
}

###############################################################################
### main ######################################################################
###############################################################################

set -o errexit ; set -o nounset

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BASE_DIR=$( cd ${CURRENT_DIR}/../../.. >/dev/null 2>&1 && pwd )
SRC_DIR=${BASE_DIR}/u-boot
TOOLS_DIR=${SRC_DIR}/draeger/build
BLD_DIR=${BUILD_DIR:=${BASE_DIR}/bld}
BIN_DIR=${BLD_DIR}/bin

PROC1=1
PROC2=1

# without parameter build all
while [ "x${1-}" != "x" ]; do
    case $1 in
        -up1 )          PROC2=0
                        ;;
        -up2 )          PROC1=0
                        ;;
        * )             echo parameter failure
                        usage
                        exit 1
                        ;;
    esac
    shift
done

mkdir -p ${BIN_DIR}/UBoot
(cd ${SRC_DIR}/Licenses; zip -q -r ${BIN_DIR}/UBoot/Licenses.zip *)

if [ "${PROC1}" == "1" ]; then
    echo "******* creating UP1 UBoot and Preloader ********"
    rm -vrf ${BIN_DIR}/UBoot/Up1/*
    ${TOOLS_DIR}/mkuboot.sh -up1
fi

if [ "${PROC2}" == "1" ]; then
    echo "******* creating UP2 test UBoot ********"
    rm -vrf ${BIN_DIR}/UBoot/Up2/*
    ${TOOLS_DIR}/mkuboot.sh -up2 -dev M48Test -fpga ${SRC_DIR}/draeger/M48Test
    mv -v ${BIN_DIR}/UBoot/Up2/M48Test/u-boot.img  ${BIN_DIR}/UBoot/Up2
    mv -v ${BIN_DIR}/UBoot/Up2/M48Test/preloader-mkpimage.bin ${BIN_DIR}/UBoot/Up2
    mv -v ${BIN_DIR}/UBoot/Up2/M48Test/u-boot-with-spl.sfp ${BIN_DIR}/UBoot/Up2
    rm -vrf ${BIN_DIR}/UBoot/Up2/M48Test

    cp -v ${SRC_DIR}/draeger/FPGA/m48_test/bin/*.img ${BIN_DIR}/UBoot/Up2
fi
