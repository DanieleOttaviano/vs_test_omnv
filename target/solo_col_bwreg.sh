#!/bin/bash

# Setup Board  Environment
./setup_board.sh

# Load Jailhouse
cd ~
./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -c

# Load the Jailhouse Mempol Cell for the bandwidth regulator
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-RPU0-mempol.cell 
jailhouse cell load inmate-mempol-RPU0 -r r5.elf 0
jailhouse cell start inmate-mempol-RPU0
membw_ctrl --platform kria_k26 init
membw_ctrl --platform kria_k26 start 5 5 5 900 0

# Load the Oracle Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo-col.cell 
jailhouse cell load inmate-demo-APU-col jailhouse/inmates/demos/arm64/oracle-demo.bin
jailhouse cell start inmate-demo-APU-col


# Run the Marte2 Application
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg

