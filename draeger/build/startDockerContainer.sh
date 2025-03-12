#!/bin/bash
set -o errexit ; set -o nounset

COMMAND="/bin/bash"

USER=$(whoami)
INTERACTIVE=""
DISPLAY=host.docker.internal:0.0


WORKSPACE=$(pwd)
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    -i|--interactive)
        ;;
    -u|--user)
        USER="$2"
        shift
        ;;
    -d|--display)
        DISPLAY="$2"
        shift
        ;;
    -c|--command)
        shift
        COMMAND="$*"
        break
        ;;
    -w|--workdir)
        WORKSPACE="$2"
        shift
        ;;
    *)    # unknown option
        echo "Unknown option $1. Ignoring."
        ;;
  esac
  shift # past value
done

if tty -s; then
    tty=t
else
    tty=
fi

workdir=$(realpath ${WORKSPACE})

echo DISPLAY =${DISPLAY}
echo workdir =$workdir
echo User:   =$USER
echo COMMAND:=$COMMAND

image_tag='artifactory.draeger.com/draeger/platform/uboot:2.0.1'

# docker pull ${image_tag}
docker run -v /home/${USER}:/home/${USER} -e BUILD_TAG=${BUILD_TAG-} -w ${workdir} -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=${DISPLAY} --rm -i${tty} ${image_tag} ${COMMAND}
