export TARGET=arm_gcc-linux
export MARTe2_DIR=$(pwd)/MARTe2
export MARTe2_Components_DIR=$(pwd)/MARTe2-components
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_DIR/Build/arm_gcc-linux/Core
export MARTe2_TEST_ENVIRONMENT=Portable
export target=frodo:/home/dottavia/environment_builder/environment/kria/jailhouse/output/rootfs/kria/root/tests/marte2_test
