To Compile the dtso:
dtc -@ -O dtb -o vs_classic.dtbo vs_classic.dtso

to decompile and check:
dtc -I dtb -O dts -o decompiled.dts vs_classic.dtbo