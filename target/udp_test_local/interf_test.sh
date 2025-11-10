#!/bin/bash

# Usage:
# ./interf_test.sh --interference [solo|membomb] --cachecol [on|off] --bwreg [on|off]

# Default values
INTERF="solo"
CACHECOL="off"
BWREG="off"

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --interference) INTERF="$2"; shift ;;
        --cachecol) CACHECOL="$2"; shift ;;
        --bwreg) BWREG="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Cell configurations
CELL_APU="jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo.cell"
CELL_APU_COL="jailhouse/configs/arm64/zynqmp-kv260-APU-inmate-demo-col.cell"
CELL_BOMB0="jailhouse/configs/arm64/zynqmp-zcu104-bomb0-col.cell"
CELL_BOMB0_COL="jailhouse/configs/arm64/zynqmp-zcu104-bomb0-col.cell"
CELL_VS="jailhouse/configs/arm64/zynqmp-kv260-VS-Classic.cell"
CELL_MEMPOL_RPU="jailhouse/configs/arm64/zynqmp-kv260-RPU0-mempol.cell"

#Inmates names
INMATE_APU="inmate-demo-APU"
INMATE_APU_COL="inmate-demo-APU-col"
INMATE_BOMB0="col-mem-bomb-0"
INMATE_BOMB0_COL="col-mem-bomb-0-col"
INMATE_VS="inmate-demo-VS"
MEMPOL_RPU_INMATE="inmate-mempol-RPU0"

#Bin names
BIN_ORACLE_APU="jailhouse/inmates/demos/arm64/oracle-demo.bin"
BIN_MEMPOL_RPU="r5.elf"
BIN_BOMB0="jailhouse/inmates/demos/arm64/mem-bomb.bin"

# MARTe2 configuration
MARTe2_CFG="./Configuration/VSES-Control.cfg"

# Membomb configuration
BOMB_SIZE=2097152  # 2 MB

# TCPDUMP filering
TCPDUMP_FILTER="udp port 44489 or udp port 44488"
# Output file for tcpdump
TCPDUMP_OUTPUT="packets_${INTERF}_color_${CACHECOL}_bwreg_${BWREG}.pcap"

# Traffic Generator addresses
TG1_ADDR=9000000
TG2_ADDR=9800000


# Debug print config
echo "Configuration:"
echo "  Interference: $INTERF"
echo "  Cache: $CACHECOL"
echo "  Bandwidth Regulation: $BWREG"
echo ""


# Execute setup if needed
echo "Running setup..."
./setup_board.sh


# Load Jailhouse
if [[ "$CACHECOL" == "isolated" ]]; then
    echo "Loading Jailhouse with cache-coloring configuration..."
    cd ~
    ./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -c
    CELL_APU=$CELL_APU_COL
    CELL_BOMB0=$CELL_BOMB0_COL
    INMATE_APU= $INMATE_APU_COL
    INMATE_BOMB0= $INMATE_BOMB0_COL
else
    echo "Loading Jailhouse with default configuration..."
    cd ~
    ./scripts_jailhouse_kria/jailhouse_setup/jailhouse_start.sh -o
fi


# Load the FPGA VS-Classic Controller
echo "Loading FPGA VS-Classic Controller..."
jailhouse cell create $CELL_VS
jailhouse cell start $INMATE_VS

# Load the Oracle Cell
echo "Loading Oracle Cell..."
jailhouse cell create $CELL_APU
jailhouse cell load $INMATE_APU $BIN_ORACLE_APU
jailhouse cell start $INMATE_APU

# If bwreg is enabled, load the Mempol Cell
if [[ "$BWREG" == "on" ]]; then
    echo "Loading Mempol Cell for bandwidth regulation..."
    jailhouse cell create $CELL_MEMPOL_RPU
    jailhouse cell load $MEMPOL_RPU_INMATE -r $BIN_MEMPOL_RPU 0
    jailhouse cell start $MEMPOL_RPU_INMATE
    membw_ctrl --platform kria_k26 init
    membw_ctrl --platform kria_k26 start 100 5 100 700 0

    # QoS regulation
    echo "Setting up QoS regulation..."
    jailhouse qos intfpdsmmutbu3:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # VS-Classic on FPGA
    jailhouse qos intfpdsmmutbu4:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # Traffic Generator 1 on FPGA
    jailhouse qos intfpdsmmutbu5:ar_b=1,aw_b=1,ar_r=10,aw_r=10      # Traffic Generator 2 on FPGA
    jailhouse qos rpu0:ar_b=1,aw_b=1,ar_r=10,aw_r=10                # RPU-0 on ZCU102
    jailhouse qos rpu1:ar_b=1,aw_b=1,ar_r=10,aw_r=10                # RPU-1 on ZCU102
fi

# If membomb is enabled, load the Jailhouse Membomb Cell and the Traffic Generators
if [[ "$INTERF" == "membomb" ]]; then
    echo "Loading Jailhouse Membomb Cell..."
    jailhouse cell create $CELL_BOMB0
    jailhouse cell load $INMATE_BOMB0 $BIN_BOMB0
    jailhouse cell start $INMATE_BOMB0
    membomb -c 1 -v -s $BOMB_SIZE -e

    # Enable FPGA traffic generators
    echo "Enabling FPGA traffic generators..."
    devmem $TG1_ADDR 32 1
    devmem $TG2_ADDR 32 1
fi

# Snoop the network traffic
echo "Starting tcpdump to snoop network traffic..."
tcpdump -G 10800 -w $TCPDUMP_OUTPUT "$TCPDUMP_FILTER" &

# Run the Marte2 Application
echo "Running Marte2 Application..."
cd ~/tests/marte2_test
./MARTeApp.sh -l RealTimeLoader -s State1 -f $MARTe2_CFG