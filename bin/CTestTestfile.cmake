# CMake generated Testfile for 
# Source directory: /Users/nbenavi/Developer/CS348B/pbrt-v3
# Build directory: /Users/nbenavi/Developer/CS348B/pbrt-v3/bin
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pbrt_unit_test "pbrt_test")
subdirs("src/ext/openexr")
subdirs("src/ext/glog")
