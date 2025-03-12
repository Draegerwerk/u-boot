#!/bin/bash

###############################################################################
### main ######################################################################
###############################################################################

set -o errexit ; set -o nounset

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BASE_DIR=$( cd ${CURRENT_DIR}/../../.. >/dev/null 2>&1 && pwd )
SRC_DIR=${BASE_DIR}/u-boot
BIN_DIR=${BASE_DIR}/bld/bin

echo "CURRENT_DIR       :${CURRENT_DIR}"
echo "BASE_DIR          :${BASE_DIR}"

BUILD_NUMBER_OPTS='--disable-plugin drama-local-tools --with-build-number --build-number-project M48 --build-number-product U-Boot --build-number-version 1.0'
DRAMA_CMD=${BASE_DIR}/plain/tools/drama.sh

# call drama to retrieve build number
${DRAMA_CMD} ${BUILD_NUMBER_OPTS}

GLOBAL_BUILD_NUMBER=""
m48_vsb_commit=""
m48_boot_commit=""
plain_commit=""
tools_commit=""
BRANCH_NAME=""

while IFS== read -r VAR VAL; do
    case ${VAR} in
        GLOBAL_BUILD_NUMBER )          GLOBAL_BUILD_NUMBER=${VAL}
                                       ;;
        m48_vsb_commit      )          m48_vsb_commit=${VAL}
                                       ;;
        m48_boot_commit     )          m48_boot_commit=${VAL}
                                       ;;
        plain_commit        )          plain_commit=${VAL}
                                       ;;
        tools_commit        )          tools_commit=${VAL}
                                       ;;
        m48_vsb_branch      )          BRANCH_NAME=${VAL}
                                       ;;
        * )                            ;;
    esac
done < ${BASE_DIR}/binaries_2/makeenv_All.properties

rm -rf  ${BIN_DIR}/UBoot

VERSIONFILE=${BIN_DIR}/UBoot/source_version_info.txt
mkdir -p $(dirname "${VERSIONFILE}")
echo "******* creating ${VERSIONFILE} ********"
echo "GLOBAL_BUILD_NUMBER:${GLOBAL_BUILD_NUMBER}" > ${VERSIONFILE}
echo "m48_vsb_commit     :${m48_vsb_commit}"     >> ${VERSIONFILE}
echo "m48_boot_commit    :${m48_boot_commit}"    >> ${VERSIONFILE}
echo "plain_commit       :${plain_commit}"       >> ${VERSIONFILE}
echo "tools_commit       :${tools_commit}"       >> ${VERSIONFILE}
cat ${VERSIONFILE}

BUILDNUMBERFILE=${BIN_DIR}/build_number.txt
mkdir -p $(dirname "${BUILDNUMBERFILE}")
echo "******* creating ${BUILDNUMBERFILE} ********"
echo "BRANCH_NAME=${BRANCH_NAME}"                  > ${BUILDNUMBERFILE}
echo "GLOBAL_BUILD_NUMBER=${GLOBAL_BUILD_NUMBER}" >> ${BUILDNUMBERFILE}
cat ${BUILDNUMBERFILE}

