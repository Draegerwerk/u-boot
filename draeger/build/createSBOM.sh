#!/bin/bash

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BASE_DIR=$( cd ${CURRENT_DIR}/../../.. >/dev/null 2>&1 && pwd )
SRC_DIR=${BASE_DIR}/u-boot
BLD_DIR=${BUILD_DIR:=${BASE_DIR}/bld}
BIN_DIR=${BLD_DIR}/bin

PACKAGE_gcc_YAML_src="${CURRENT_DIR}/package_gcc.yml"
PACKAGE_gcc_YAML_dst="${BIN_DIR}/UBoot/package_gcc.yml"
PACKAGE_uboot_YAML_src="${CURRENT_DIR}/package_uboot.yml"
PACKAGE_uboot_YAML_dst="${BIN_DIR}/UBoot/package_uboot.yml"

# copy package.yml
ls -lR ${BIN_DIR}
cp "${PACKAGE_gcc_YAML_src}" "${PACKAGE_gcc_YAML_dst}"
cp "${PACKAGE_uboot_YAML_src}" "${PACKAGE_uboot_YAML_dst}"

