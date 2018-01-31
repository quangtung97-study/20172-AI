WORKING_DIR=$(pwd)
CXX=g++-5

git clone https://github.com/google/googletest
cd googletest
git checkout release-1.8.0
GTEST_DIR=googletest
GMOCK_DIR=googlemock

$CXX -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GTEST_DIR}/src/gtest-all.cc
$CXX -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
$CXX -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    -pthread -c ${GMOCK_DIR}/src/gmock_main.cc
ar -rv libgmock_main.a gtest-all.o gmock-all.o gmock_main.o

cd $WORKING_DIR
mkdir include 
mkdir -p lib/test

cp -r googletest/googlemock/include/gmock include/ 
cp -r googletest/googletest/include/gtest include/
cp googletest/libgmock_main.a lib/test
