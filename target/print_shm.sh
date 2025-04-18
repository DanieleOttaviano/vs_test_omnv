#!/bin/bash

BASE_ADDR=0x46d00000

# Convert a hex 64-bit number to IEEE 754 double using Python
hex_to_double() {
    local hexval=$1
    python3 -c "import struct; print(struct.unpack('<d', struct.pack('<Q', int('$hexval', 16)))[0])"
}

# Helper: Add hex offset
hex_add() {
    printf "0x%X\n" $(( $1 + $2 ))
}

# Read a 64-bit value from memory and decode as double
read_double() {
    local addr=$1
    raw=$(devmem "$addr" 64)
    hex=$(printf "%016x\n" "$raw")
    hex_to_double "$hex"
}

# Read a 32-bit value as uint32
read_uint32() {
    local addr=$1
    devmem "$addr" 32
}

# Read an 8-bit value
read_byte() {
    local addr=$1
    devmem "$addr" 8
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
        value=$(read_double $addr)
        printf "  [%02d] @ %s = %.6f\n" $i "$addr" "$value"
    done
}

echo "Reading shared_page structure from $BASE_ADDR"

# pf_voltage[11] @ 0x000
read_double_array $BASE_ADDR 11 "pf_voltage"

# vs3 @ 0x058
vs3_addr=$(hex_add $BASE_ADDR 0x058)
vs3_val=$(read_double $vs3_addr)
echo "vs3 @ $vs3_addr = $vs3_val"

# pf_currents[11] @ 0x060
read_double_array $(hex_add $BASE_ADDR 0x060) 11 "pf_currents"

# ivs3 @ 0x0B8
ivs3_addr=$(hex_add $BASE_ADDR 0x0B8)
ivs3_val=$(read_double $ivs3_addr)
echo "ivs3 @ $ivs3_addr = $ivs3_val"

# ip @ 0x0C0
ip_addr=$(hex_add $BASE_ADDR 0x0C0)
ip_val=$(read_double $ip_addr)
echo "ip @ $ip_addr = $ip_val"

# zc @ 0x0C8
zc_addr=$(hex_add $BASE_ADDR 0x0C8)
zc_val=$(read_double $zc_addr)
echo "zc @ $zc_addr = $zc_val"

# zdot @ 0x0D0
zdot_addr=$(hex_add $BASE_ADDR 0x0D0)
zdot_val=$(read_double $zdot_addr)
echo "zdot @ $zdot_addr = $zdot_val"

# gaps[29] @ 0x0D8
read_double_array $(hex_add $BASE_ADDR 0x0D8) 29 "gaps"

# Xpoints[2] @ 0x1C0
read_double_array $(hex_add $BASE_ADDR 0x1C0) 2 "Xpoints"

# Byte fields @ 0x1D0 - 0x1D3
for i in {0..3}; do
    addr=$(hex_add $BASE_ADDR $((0x1D0 + i)))
    val=$(read_byte $addr)
    case $i in
        0) label="oracle_decision" ;;
        1) label="input_counter" ;;
        2) label="marte_strategy" ;;
        3) label="output_counter" ;;
    esac
    echo "$label @ $addr = $val"
done

# reserved @ 0x1D4
reserved_addr=$(hex_add $BASE_ADDR 0x1D4)
reserved_val=$(read_uint32 $reserved_addr)
echo "reserved @ $reserved_addr = $reserved_val"

# es_output @ 0x1D8
es_output=$(read_double $(hex_add $BASE_ADDR 0x1D8))
echo "es_output = $es_output"

# classic_output @ 0x1E0
classic_output=$(read_double $(hex_add $BASE_ADDR 0x1E0))
echo "classic_output = $classic_output"

# vs3_ref @ 0x1E8
vs3_ref=$(read_double $(hex_add $BASE_ADDR 0x1E8))
echo "vs3_ref = $vs3_ref"

