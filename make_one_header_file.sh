#!/usr/bin/bash
INCLUDE_FILE=single_include/stations.hpp

echo "#pragma once

/** THIS IS AN AUTO-GENERATED FILE. IF YOU WOULD LIKE TO MAKE CHANGES TO STATIONS, PLEASE CHANGE THE ORIGINAL SOURCE FILES. */
" > $INCLUDE_FILE

# Internal files
find include/stations/internal -maxdepth 1 -name "*.hpp" -not -empty -exec grep -v "^#pragma" {} \; | grep -v "^#include<stations" >> $INCLUDE_FILE

# External files
grep -v "^#pragma " include/stations/worker_queue.hpp >> $INCLUDE_FILE
find include/stations -maxdepth 1 -name "*.hpp" -not -empty ! -name "worker_queue.hpp" -exec grep -v "^#pragma" {} \; | grep -v "^#include<stations" >> $INCLUDE_FILE
