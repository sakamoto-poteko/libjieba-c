set -e

CC=`which $1-gcc`
CXX=`which $1-g++`

cat << EOF >toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER ${CC})
set(CMAKE_CXX_COMPILER ${CXX})
EOF