# CMake generated Testfile for 
# Source directory: /home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/examples/applications
# Build directory: /home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/build/examples/applications
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(skew3_--size_1mib_random_--check "skew3" "--size" "1mib" "random" "--check")
set_tests_properties(skew3_--size_1mib_random_--check PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/examples/applications/CMakeLists.txt;23;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/examples/applications/CMakeLists.txt;0;")
add_test(skew3-lcp_--size_1mib_random_--check "skew3-lcp" "--size" "1mib" "random" "--check")
set_tests_properties(skew3-lcp_--size_1mib_random_--check PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/examples/applications/CMakeLists.txt;24;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/examples/applications/CMakeLists.txt;0;")
