#!/bin/bash

docker run --rm -v /:/host -v `pwd`:/tool/resource -v /var/run/docker.sock:/var/run/docker.sock registry.veinmind.tech/veinmind/veinmind-runner $*