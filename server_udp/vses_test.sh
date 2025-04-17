#!/bin/bash

# Check for valid first argument
if [ "$1" != "solo" ] && [ "$1" != "membomb" ]; then
    echo "First argument must be 'solo' or 'membomb'"
    exit 1
fi

# Initialize flags
COL_FLAG=0
BWREG_FLAG=0

# Repetitions
REPS=1

# Parse additional arguments
for arg in "$@"; do
    case $arg in
        col)
            COL_FLAG=1
            ;;
        bwreg)
            BWREG_FLAG=1
            ;;
    esac
done

echo "Running the test $1  (COLORING=$COL_FLAG) (BW REGULATION=$BWREG_FLAG)"

# Construct the filename based on the arguments
FILENAME="Results/cycle_times_$1"
if [ $COL_FLAG -eq 1 ]; then
    FILENAME="${FILENAME}_col"
fi
if [ $BWREG_FLAG -eq 1 ]; then
    FILENAME="${FILENAME}_bwreg"
fi
FILENAME="${FILENAME}.txt"

# Remove the file if it exists
if [ -f "$FILENAME" ]; then
    rm "$FILENAME"
fi

i=1
while [ $i -le ${REPS} ]; do
    echo "root" > /dev/ttyUSB.kria.1
    sleep 1
    echo "root" > /dev/ttyUSB.kria.1
    sleep 1

    # Check if the board is responsive
    echo "cd tests/marte2_test/" > /dev/ttyUSB.kria.1
    echo "./alive.sh" > /dev/ttyUSB.kria.1
    sleep 2
    RESPONSE=$(timeout 5 cat /dev/ttyUSB.kria.1 | grep "alive")
    if [ -z "$RESPONSE" ]; then
        echo "Board is not responsive, rebooting..."
        taco target kria reset
        sleep 60
        continue
    else
        echo "Board Alive!"
    fi

    # Start the test on the target
    TEST_CMD="./$1"
    if [ $COL_FLAG -eq 1 ]; then
        TEST_CMD="${TEST_CMD}_col"
    fi
    if [ $BWREG_FLAG -eq 1 ]; then
        TEST_CMD="${TEST_CMD}_bwreg"
    fi
    TEST_CMD="${TEST_CMD}.sh &"
    echo "$TEST_CMD" > /dev/ttyUSB.kria.1

    # Wait for the test setup
    sleep 15 # TO CHANGE -> 30

    echo "Start the Simulation. Rep: $i"

    # Start sending the UDP packets
    # sudo chrt -f 75 ./vses_udp
    timeout -s 2 2m sudo chrt -f 75 ./vses_udp
    if [[ $? -ne 0 ]]; then 
        echo "Timeout, rebooting..."
        taco target kria reset
        sleep 60
        continue
    fi

    # Save the cycle times
    if [ -f VS_Extremum_Track/cycle_times.txt ]; then
        cat VS_Extremum_Track/cycle_times.txt >> $FILENAME
    else
        cp VS_Extremum_Track/cycle_times.txt $FILENAME
    fi

    # Reboot the board (Stop mempol if enabled)
    if [ $BWREG_FLAG -eq 1 ]; then
        echo "membw_ctrl --platform kria_k26 stop" > /dev/ttyUSB.kria.1
    fi
    echo "jailhouse disable" > /dev/ttyUSB.kria.1
    echo "reboot" > /dev/ttyUSB.kria.1

    i=$(($i+1))
    # Wait before the next iteration if is less than REPS
    if [ $i -le ${REPS} ]; then
        sleep 30
    fi
done
