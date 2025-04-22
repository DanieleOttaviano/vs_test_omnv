#!/bin/bash

# Setup Board  Environment
./setup_board.sh

# Load Jailhouse
cd ~
./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -o

# Load the FPGA VS-Classic Controller
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-VS-Classic.cell
jailhouse cell start inmate-demo-VS

# Load the Oracle Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo.cell 
jailhouse cell load inmate-demo-APU jailhouse/inmates/demos/arm64/oracle-demo.bin
jailhouse cell start inmate-demo-APU

# Snoop the network traffic
tcpdump -G 10800 -w packets_solo.pcap "udp port 44489 or udp port 44488" &

# Run the Marte2 Application
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg

