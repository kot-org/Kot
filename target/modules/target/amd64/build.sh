#!/bin/bash

SOURCES=../../../../sources
MODULES=$SOURCES/core/modules
MODULES_LIST=$(ls -d $MODULES/*/)
MODULES_BINARY_PATH=$(realpath "bin")


for module in $MODULES_LIST; do
  if [ -d "$module/target/amd64" ]; then
    (echo -e "\033[0;36m[core/modules]\033[0;37m\033[1;37m Building module : $(basename $module)\033[0;37m")
    (cd "$module/target/amd64" && make && cp -r "bin/." $MODULES_BINARY_PATH)
  fi
done
