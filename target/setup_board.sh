#Remove RT-Throttling
echo -1 > /proc/sys/kernel/sched_rt_runtime_us

# Remove all irq to core 3
cd /proc/irq
for i in [0-9]*
do
	echo 7 >$i/smp_affinity 2>/dev/null
done

# Assign network interrupts to core 3 (eth0 -> IRQ 47)
echo 8 >47/smp_affinity