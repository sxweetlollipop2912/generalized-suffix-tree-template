cmake_minimum_required(VERSION 3.21)
project(my_suffix_tree)

set(CMAKE_CXX_STANDARD 17)

add_executable(my_suffix_tree main.cpp SuffixTree/Edge.h SuffixTree/Node.h SuffixTree/SuffixTree.h SuffixTree/KeyInternal.h)
