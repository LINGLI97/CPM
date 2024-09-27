# CMake generated Testfile for 
# Source directory: /home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io
# Build directory: /home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/build/tests/io
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_io_. "test_io" ".")
set_tests_properties(test_io_. PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;17;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_syscall_./testdisk1 "test_cancel" "syscall" "./testdisk1")
set_tests_properties(test_cancel_syscall_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;19;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_fileperblock_syscall_./testdisk1 "test_cancel" "fileperblock_syscall" "./testdisk1")
set_tests_properties(test_cancel_fileperblock_syscall_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;21;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_mmap_./testdisk1 "test_cancel" "mmap" "./testdisk1")
set_tests_properties(test_cancel_mmap_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;23;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_fileperblock_mmap_./testdisk1 "test_cancel" "fileperblock_mmap" "./testdisk1")
set_tests_properties(test_cancel_fileperblock_mmap_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;24;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_linuxaio_./testdisk1 "test_cancel" "linuxaio" "./testdisk1")
set_tests_properties(test_cancel_linuxaio_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;29;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_cancel_memory_./testdisk1 "test_cancel" "memory" "./testdisk1")
set_tests_properties(test_cancel_memory_./testdisk1 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;35;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_io_sizes_syscall_./testdisk1_1073741824 "test_io_sizes" "syscall" "./testdisk1" "1073741824")
set_tests_properties(test_io_sizes_syscall_./testdisk1_1073741824 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;39;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_io_sizes_mmap_./testdisk1_1073741824 "test_io_sizes" "mmap" "./testdisk1" "1073741824")
set_tests_properties(test_io_sizes_mmap_./testdisk1_1073741824 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;41;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_io_sizes_linuxaio_./testdisk1_1073741824 "test_io_sizes" "linuxaio" "./testdisk1" "1073741824")
set_tests_properties(test_io_sizes_linuxaio_./testdisk1_1073741824 PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;44;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
add_test(test_mmap "test_mmap")
set_tests_properties(test_mmap PROPERTIES  TIMEOUT "3600" _BACKTRACE_TRIPLES "/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/CMakeLists.txt;636;add_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;52;stxxl_test;/home/ling/OneDrive/PhD/CPM/CPMLing/stxxl/tests/io/CMakeLists.txt;0;")
