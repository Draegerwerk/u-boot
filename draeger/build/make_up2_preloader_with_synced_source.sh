#!/bin/bash

errormsg() {
echo " _____ ____  ____   ___  ____   "
echo "| ____|  _ \|  _ \ / _ \|  _ \  "
echo "|  _| | |_) | |_) | | | | |_) | "
echo "| |___|  _ <|  _ <| |_| |  _ <  "
echo "|_____|_| \_\_| \_\\\\___/|_| \_\ "
echo ""
}

###############################################################################
### main ######################################################################
###############################################################################

set -o errexit ; set -o nounset

CURDIR=$(dirname "${BASH_SOURCE[0]}")
BASEDIR="$( cd "${CURDIR}/../.." >/dev/null 2>&1 && pwd )"
BINDIR=bld/bin

if [ "x${1-}" == "x" -o "x${2-}" == "x" ]; then
    echo "usage: $0 <Devicetype> <FPGA dir> [with_buildid]"
    exit 1
fi

DEV_TYPE=${1}
FPGA_DIR=${2}
BUILD_TAG_ENV=""
DOCKERBASE=""
DOCKERMOUNT=""

DRAMA_CMD=""
case $(uname -o) in
    Cygwin )    #falltrough
                ;&
    Msys   )    DOCKERBASE=$(cygpath -w ${BASEDIR})
    			DRAMA_CMD=${BASEDIR}/plain/tools/drama.cmd
                ;;
    *Linux )    DRAMA_CMD=${BASEDIR}/plain/tools/drama.sh
                ;;
    * )         echo "drama not found $(uname -o) "
                exit
                ;;
esac

# check if global build number has been requested
if [ "x${3-}" != "x" ]; then

	echo call drama to retrieve build number
	BUILD_NUMBER_OPTS='--with-build-number --build-number-project M48 --build-number-product VSB --build-number-version 1.0'
	${DRAMA_CMD} ${BUILD_NUMBER_OPTS}

	GLOBAL_BUILD_NUMBER=""

	while IFS== read -r VAR VAL; do
	    case ${VAR} in
	        GLOBAL_BUILD_NUMBER )          GLOBAL_BUILD_NUMBER=${VAL}
	                                       ;;
	        * )                            ;;
	    esac
	done < ${BASEDIR}/binaries_2/makeenv_All.properties

	BUILD_TAG_ENV="--env BUILD_TAG=${GLOBAL_BUILD_NUMBER}"
fi

DOCKERBASE=${BASEDIR}
DOCKERMOUNT=${BASEDIR}

echo "p3                 :${3-}"
echo "OS                 :$(uname -o)"
echo "DOCKERBASE         :${DOCKERBASE}"
echo "BASEDIR            :${BASEDIR}"
echo "FPGA_DIR           :${FPGA_DIR}"
echo

# echo 
docker run --rm -v ${DOCKERBASE}:${DOCKERMOUNT} -w ${DOCKERMOUNT} ${BUILD_TAG_ENV} artifactory.draeger.com/draeger/platform/uboot:2.0.1 u-boot/draeger/build/mkuboot.sh -up2 -spl -dev ${DEV_TYPE} -fpga ${FPGA_DIR}
