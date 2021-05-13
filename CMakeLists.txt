cmake_minimum_required(VERSION 3.10)
project(bev-daat)

set(CMAKE_CXX_STANDARD 11)

add_executable(main.cpp header/Node.h cpp/Trie.cpp header/Trie.h cpp/EditVector.cpp header/EditVector.h cpp/utils.cpp header/utils.h cpp/ActiveNode.cpp header/ActiveNode.h cpp/State.cpp header/State.h cpp/Framework.cpp header/Framework.h cpp/Beva.cpp header/Beva.h header/C.h cpp/Experiment.cpp header/Experiment.h  header/Node.h header/ShortVector.h header/Directives.h cpp/StaticString.cpp header/StaticString.h header/crow_all.h)
