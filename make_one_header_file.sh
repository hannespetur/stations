#!/usr/bin/bash
echo "#pragma once" > include/stations.hpp
echo "#include <assert.h>" >> include/stations.hpp
echo "#include <atomic>" >> include/stations.hpp
echo "#include <iostream>" >> include/stations.hpp
echo "#include <iterator>" >> include/stations.hpp
echo "#include <memory>" >> include/stations.hpp
echo "#include <thread>" >> include/stations.hpp
echo "#include <vector>" >> include/stations.hpp

grep -v "^#" include/stations/worker_queue.hpp >> include/stations.hpp

find include/stations -name "*.hpp" -not -empty ! -name "worker_queue.hpp" -exec grep -v "^#" {} \; >> include/stations.hpp
