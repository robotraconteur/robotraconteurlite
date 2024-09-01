#!/bin/bash

# Run template files

set -e

if [ ! -f "src/template/array_types.c.j2" ] ; then
    echo "Run this script from the root of the project"
    exit 1
fi

jinja2 src/template/array_types.c.j2 > src/array_types.c
jinja2 src/template/array.h.j2 > include/robotraconteurlite/array.h
jinja2 src/template/message_types.c.j2 > src/message_types.c

python3 -m pre_commit run --all
