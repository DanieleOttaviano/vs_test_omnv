#!/bin/bash

# usage source vses_test.sh [solo|membomb] [col] [bwreg]

# Check for valid first argument
if [ "$1" != "solo" ] && [ "$1" != "membomb" ]; then
    echo "First argument must be 'solo' or 'membomb'"
    exit 1
fi

sudo ls > /dev/null

# Board tty device
TTY_DEVICE="/dev/ttyUSB.kria_3.1"
DEVICE="kria_3"

# Initialize flags
CACHECOL="off"
BWREG="off"

# Repetitions
REPS=1

# Parse additional arguments
for arg in "$@"; do
    case $arg in
        col)
            CACHECOL="on"
            ;;
        bwreg)
            BWREG="on"
            ;;
    esac
done

echo "Running the test $1  (COLORING=$CACHECOL) (BW REGULATION=$BWREG)"

# Construct the filename based on the arguments
FILENAME="Results/cycle_times_$1_col_${CACHECOL}_bwreg_${BWREG}.txt"
PACKETSNAME="packets_$1_col_${CACHECOL}_bwreg_${BWREG}.pcap"
echo "Results will be saved in: $FILENAME"

# Remove the file if it exists
if [ -f "$FILENAME" ]; then
    rm "$FILENAME"
fi

i=1
while [ $i -le ${REPS} ]; do
    echo "root" > $TTY_DEVICE
    sleep 1
    echo "root" > $TTY_DEVICE
    sleep 1

    # Check if the board is responsive
    echo "cd tests/marte2_test/" > $TTY_DEVICE
    echo "./alive.sh" > $TTY_DEVICE
    sleep 2
    RESPONSE=$(timeout 5 cat $TTY_DEVICE | grep "alive")
    if [ -z "$RESPONSE" ]; then
        echo "Board is not responsive, rebooting..."
        taco target $DEVICE reset
        sleep 60
        continue
    else
        echo "Board Alive!"
    fi

    # Start the test on the target
    TEST_CMD="./interf_test.sh --interference $1 --cachecol $CACHECOL --bwreg $BWREG"
    echo "$TEST_CMD" > $TTY_DEVICE

    # Wait for the test setup
    sleep 10 # TO CHANGE -> 30

    echo "Start the Simulation. Rep: $i"

    # Start sending the UDP packets
    # sudo chrt -f 75 ./vses_udp
    timeout -s 2 2m sudo chrt -f 99 ./vses_udp
    if [[ $? -ne 0 ]]; then 
        echo "Timeout, rebooting..."
        taco target $DEVICE reset
        sleep 60
        continue
    fi

    # Stop MarteApp
    printf "\x03" > $TTY_DEVICE
    sleep 1
    printf "\x03" > $TTY_DEVICE
    sleep 1

    # stop tcpdump
    echo "./stop_tcp.sh" > $TTY_DEVICE

    # Save the cycle times
    if [ -f VS_Extremum_Track/cycle_times.txt ]; then
        cat VS_Extremum_Track/cycle_times.txt >> $FILENAME
    else
        cp VS_Extremum_Track/cycle_times.txt $FILENAME
    fi

    # Reboot the board (Stop mempol if enabled)
    if [[ $BWREG == "on" ]]; then
        echo "membw_ctrl --platform kria_k26 stop" > $TTY_DEVICE
    fi
    echo "jailhouse disable" > $TTY_DEVICE
    echo "reboot" > $TTY_DEVICE
    #echo "exit" > $TTY_DEVICE

    i=$(($i+1))
    # Wait before the next iteration if is less than REPS
    if [ $i -le ${REPS} ]; then
        sleep 30
    fi
done
