#!/bin/bash

BASE_ADDR=0x88000000

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

# Read a 64-bit unsigned integer
read_uint64() {
    local addr=$1
    devmem "$addr" 64
}

# Read a 32-bit value as uint32
read_uint32() {
    local addr=$1
    devmem "$addr" 32
}

# Read a 16-bit value
read_uint16() {
    local addr=$1
    devmem "$addr" 16
}

# Read an 8-bit value
read_byte() {
    local addr=$1
    devmem "$addr" 8
}

echo "Reading classic_vs_pl_conf_port from $BASE_ADDR"
echo "------------------------------------------------"

# 0x00 - enabled (uint16_t)
enabled=$(read_uint16 $(hex_add $BASE_ADDR 0x00))
echo "enabled @ $(hex_add $BASE_ADDR 0x00) = $enabled"

# 0x02 - counter_offset (uint16_t)
counter_offset=$(read_uint16 $(hex_add $BASE_ADDR 0x02))
echo "counter_offset @ $(hex_add $BASE_ADDR 0x02) = $counter_offset"

# 0x04 - ivs3_offset (uint16_t)
ivs3_offset=$(read_uint16 $(hex_add $BASE_ADDR 0x04))
echo "ivs3_offset @ $(hex_add $BASE_ADDR 0x04) = $ivs3_offset"

# 0x06 - zdot_offset (uint16_t)
zdot_offset=$(read_uint16 $(hex_add $BASE_ADDR 0x06))
echo "zdot_offset @ $(hex_add $BASE_ADDR 0x06) = $zdot_offset"

# 0x08 - marte_strategy_offset (uint16_t)
marte_strategy_offset=$(read_uint16 $(hex_add $BASE_ADDR 0x08))
echo "marte_strategy_offset @ $(hex_add $BASE_ADDR 0x08) = $marte_strategy_offset"

# 0x0A - safe_control_output_offset (uint16_t)
safe_control_output_offset=$(read_uint16 $(hex_add $BASE_ADDR 0x0A))
echo "safe_control_output_offset @ $(hex_add $BASE_ADDR 0x0A) = $safe_control_output_offset"

# 0x0C - res_0_0 (uint32_t)
res_0_0=$(read_uint32 $(hex_add $BASE_ADDR 0x0C))
echo "res_0_0 @ $(hex_add $BASE_ADDR 0x0C) = $res_0_0"

# 0x10 - shared_page_base (uint64_t)
shared_page_base=$(read_uint64 $(hex_add $BASE_ADDR 0x10))
printf "shared_page_base @ %s = 0x%016X\n" $(hex_add $BASE_ADDR 0x10) "$shared_page_base"

# 0x18 - Af_val (double)
Af_val=$(read_double $(hex_add $BASE_ADDR 0x18))
echo "Af_val @ $(hex_add $BASE_ADDR 0x18) = $Af_val"

# 0x20 - Bf_val (double)
Bf_val=$(read_double $(hex_add $BASE_ADDR 0x20))
echo "Bf_val @ $(hex_add $BASE_ADDR 0x20) = $Bf_val"

# 0x28 - Cf_val (double)
Cf_val=$(read_double $(hex_add $BASE_ADDR 0x28))
echo "Cf_val @ $(hex_add $BASE_ADDR 0x28) = $Cf_val"

# 0x30 - Df_val (double)
Df_val=$(read_double $(hex_add $BASE_ADDR 0x30))
echo "Df_val @ $(hex_add $BASE_ADDR 0x30) = $Df_val"

# 0x38 - gain_val (double)
gain_val=$(read_double $(hex_add $BASE_ADDR 0x38))
echo "gain_val @ $(hex_add $BASE_ADDR 0x38) = $gain_val"

# 0x40 - res_0_1[0xC0] (bytes)
echo "res_0_1 (0x40 - 0xFF) skipped (reserved 0xC0 bytes)"

# 0x100 - x_old (double)
x_old=$(read_double $(hex_add $BASE_ADDR 0x100))
echo "x_old @ $(hex_add $BASE_ADDR 0x100) = $x_old"

# 0x108 - res_0_2 (uint64_t)
res_0_2=$(read_uint64 $(hex_add $BASE_ADDR 0x108))
printf "res_0_2 @ %s = 0x%016X\n" $(hex_add $BASE_ADDR 0x108) "$res_0_2"

# 0x110 - inner_counter (uint64_t)
inner_counter=$(read_uint64 $(hex_add $BASE_ADDR 0x110))
printf "inner_counter @ %s = 0x%016X\n" $(hex_add $BASE_ADDR 0x110) "$inner_counter"

# 0x118 - res_0_3 (uint64_t)
res_0_3=$(read_uint64 $(hex_add $BASE_ADDR 0x118))
printf "res_0_3 @ %s = 0x%016X\n" $(hex_add $BASE_ADDR 0x118) "$res_0_3"

# 0x120 - res_val / raw_val union
res_val=$(read_double $(hex_add $BASE_ADDR 0x120))
raw_val=$(read_uint64 $(hex_add $BASE_ADDR 0x120))
echo "res_val @ $(hex_add $BASE_ADDR 0x120) = $res_val"
printf "raw_val (uint64_t) = 0x%016X\n" "$raw_val"

# 0x128 - res_0_4 (uint64_t)
res_0_4=$(read_uint64 $(hex_add $BASE_ADDR 0x128))
printf "res_0_4 @ %s = 0x%016X\n" $(hex_add $BASE_ADDR 0x128) "$res_0_4"

echo "------------------------------------------------"
echo "Done reading classic_vs_pl_conf_port."