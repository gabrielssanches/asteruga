#!/bin/bash

docker run \
    -v `pwd`:/game \
    -it \
    raylib/${1} \
    /bin/bash -c "source /usr/local/bin/entrypoint.sh && make V=1"
