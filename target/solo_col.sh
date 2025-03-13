#!/bin/bash

# Setup Board  Environment
./setup_board.sh

# Load Jailhouse
cd ~
./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -c

# Load the Oracle Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo-col.cell 
jailhouse cell load inmate-demo-APU-col jailhouse/inmates/demos/arm64/oracle-demo.bin
jailhouse cell start inmate-demo-APU-col

# Run the Marte2 Application
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg

