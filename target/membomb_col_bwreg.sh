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

# Load the Jailhouse Mempol Cell for the bandwidth regulator
jailhouse cell create jailhouse/configs/arm64/zynqmp-kv260-RPU0-mempol.cell 
jailhouse cell load inmate-mempol-RPU0 -r r5.elf 0
jailhouse cell start inmate-mempol-RPU0
membw_ctrl --platform kria_k26 init
membw_ctrl --platform kria_k26 start 100 5 100 700 0

# QoS regulation 
jailhouse qos intfpdsmmutbu3:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # VS-Classic on FPGA
jailhouse qos intfpdsmmutbu4:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # Traffic Generator 1 on FPGA
jailhouse qos intfpdsmmutbu5:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # Traffic Generator 2 on FPGA
jailhouse qos rpu0:ar_b=1,aw_b=1,ar_r=10,aw_r=10                # RPU-0 on ZCU102
jailhouse qos rpu1:ar_b=1,aw_b=1,ar_r=10,aw_r=10                # RPU-1 on ZCU102

# Load the Jailhouse Membomb Cell
jailhouse cell create jailhouse/configs/arm64/zynqmp-zcu104-bomb0-col.cell
jailhouse cell load col-mem-bomb-0 jailhouse/inmates/demos/arm64/mem-bomb.bin
jailhouse cell start col-mem-bomb-0
membomb -c 1 -v -s 2097152 -e

# Enable FPGA traffic generators
devmem 9000000 32 1
devmem 9800000 32 1

# Snoop the network traffic
tcpdump -G 10800 -w packets_membomb_col_bwreg.pcap "udp port 44489 or udp port 44488" &

# Run the Marte2 Application
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg

