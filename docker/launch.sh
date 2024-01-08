#!/bin/bash

docker run \
    -v `pwd`:/game \
    -ti \
    raylib/${1} \
    /bin/bash -c "source /usr/local/bin/entrypoint.sh && /bin/bash"
