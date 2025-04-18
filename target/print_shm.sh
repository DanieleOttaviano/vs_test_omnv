#!/bin/bash

BASE_ADDR=0x46d00000

# Helper function to read a 64-bit double from memory
read_double() {
    local addr=$1
    value=$(devmem "$addr" 64)
    printf "%.6f" "$value"
}

# Helper function to read a 32-bit unsigned int
read_uint32() {
    local addr=$1
    value=$(devmem "$addr" 32)
    printf "0x%08X" "$value"
}

# Helper function to read a byte
read_byte() {
    local addr=$1
    value=$(devmem "$addr" 8)
    printf "0x%02X" "$value"
}

# Hex offset adder
hex_add() {
    printf "0x%X\n" $(( $1 + $2 ))
}

# Read array of doubles
read_double_array() {
    local start_addr=$1
    local count=$2
    local label=$3

    echo "$label:"
    for ((i=0; i<count; i++)); do
        offset=$((i * 8))
        addr=$(hex_add $start_addr $offset)
        printf "  [%02d] @ %s = %s\n" $i "$addr" "$(read_double $addr)"
    done
}

echo "Reading shared_page structure from $BASE_ADDR"

# pf_voltage[11] @ 0x000
read_double_array $BASE_ADDR 11 "pf_voltage"

# vs3 @ 0x058
read_double $(hex_add $BASE_ADDR 0x058) | awk '{ print "vs3 =", $0 }'

# pf_currents[11] @ 0x060
read_double_array $(hex_add $BASE_ADDR 0x060) 11 "pf_currents"

# ivs3 @ 0x0B8
read_double $(hex_add $BASE_ADDR 0x0B8) | awk '{ print "ivs3 =", $0 }'

# ip @ 0x0C0
read_double $(hex_add $BASE_ADDR 0x0C0) | awk '{ print "ip =", $0 }'

# zc @ 0x0C8
read_double $(hex_add $BASE_ADDR 0x0C8) | awk '{ print "zc =", $0 }'

# zdot @ 0x0D0
read_double $(hex_add $BASE_ADDR 0x0D0) | awk '{ print "zdot =", $0 }'

# gaps[29] @ 0x0D8
read_double_array $(hex_add $BASE_ADDR 0x0D8) 29 "gaps"

# Xpoints[2] @ 0x1C0
read_double_array $(hex_add $BASE_ADDR 0x1C0) 2 "Xpoints"

# Byte fields @ 0x1D0 - 0x1D3
for offset in 0x1D0 0x1D1 0x1D2 0x1D3; do
    label=""
    case $offset in
        0x1D0) label="oracle_decision" ;;
        0x1D1) label="input_counter" ;;
        0x1D2) label="marte_strategy" ;;
        0x1D3) label="output_counter" ;;
    esac
    val=$(read_byte $(hex_add $BASE_ADDR $((0x${offset#0x}))))
    echo "$label = $val"
done

# reserved @ 0x1D4
echo -n "reserved = "
read_uint32 $(hex_add $BASE_ADDR 0x1D4)
echo

# es_output @ 0x1D8
read_double $(hex_add $BASE_ADDR 0x1D8) | awk '{ print "es_output =", $0 }'

# classic_output @ 0x1E0
read_double $(hex_add $BASE_ADDR 0x1E0) | awk '{ print "classic_output =", $0 }'

# vs3_ref @ 0x1E8
read_double $(hex_add $BASE_ADDR 0x1E8) | awk '{ print "vs3_ref =", $0 }'

