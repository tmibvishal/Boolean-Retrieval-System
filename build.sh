#!/bin/bash

compiler="g++"
[ -d temp ] || mkdir temp
[ -d bin ] || mkdir bin
$compiler -w -c include/porter.cpp -o obj/porter.o
$compiler -std=c++17 -o bin/invidx_cons obj/porter.o src/compression.cpp src/helper.cpp src/utils.cpp src/invidx_cons.cpp -Iinclude -Llib -lsnappy -lstdc++fs
$compiler -std=c++17 -o bin/boolsearch obj/porter.o src/compression.cpp src/helper.cpp src/utils.cpp src/boolsearch.cpp -Iinclude -Llib -lsnappy -lstdc++fs

# $compiler -std=c++17 -Wall -Wextra -pedantic -std=c++17 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fno-sanitize-recover -fstack-protector -o bin/invidx_cons obj/porter.o src/compression.cpp src/helper.cpp src/utils.cpp src/invidx_cons.cpp -Iinclude -Llib -lsnappy -lstdc++fs
# $compiler -std=c++17 -Wall -Wextra -pedantic -std=c++17 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fno-sanitize-recover -fstack-protector -o bin/boolsearch obj/porter.o src/compression.cpp src/helper.cpp src/utils.cpp src/boolsearch.cpp -Iinclude -Llib -lsnappy -lstdc++fs

# $compiler -Wall -Wextra -pedantic -std=c++17 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector src/invidx_cons.cpp -o bin/invidx_cons
# $compiler -Wall -Wextra -pedantic -std=c++17 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fno-sanitize-recover -fstack-protector obj/porter.o src/invidx_cons.cpp -o bin/invidx_cons
# $compiler -std=c++17 -o bin/invidx_cons obj/porter.o src/compression.cpp src/helper.cpp src/utils.cpp src/unit_test.cpp src/invidx_cons.cpp -Iinclude -Llib -lsnappy -lstdc++fs
