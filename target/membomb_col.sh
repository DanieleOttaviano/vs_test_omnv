#!/bin/bash

# Setup Board  Environment
./setup_board.sh

# Load Jailhouse
cd ~
./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -c

# Load the FPGA VS-Classic Controller
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-VS-Classic.cell
jailhouse cell start inmate-demo-VS

# Load the Oracle Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo-col.cell 
jailhouse cell load inmate-demo-APU-col jailhouse/inmates/demos/arm64/oracle-demo.bin
jailhouse cell start inmate-demo-APU-col

# Load the Jailhouse Membomb Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-zcu104-bomb0-col.cell
jailhouse cell load col-mem-bomb-0 jailhouse/inmates/demos/arm64/mem-bomb.bin
jailhouse cell start col-mem-bomb-0
membomb -c 1 -v -s 2097152 -e

# Enable FPGA traffic generators
devmem 9000000 32 1
devmem 9800000 32 1

# Snoop the network traffic
tcpdump -G 10800 -w packets_membomb_col.pcap "udp port 44489 or udp port 44488" &

# Run the Marte2 Application
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg

