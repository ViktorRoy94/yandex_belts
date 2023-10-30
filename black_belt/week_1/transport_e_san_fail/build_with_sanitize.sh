clang++-6.0 -std=c++17 -I ../../utils/ *.cpp -o transport_e -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls
ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-6.0/bin/llvm-symbolizer ./transport_e 
