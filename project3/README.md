#README

// RUN FOR COMPUTE INTENSIVE(SYSBENCH)

Make

sudo rmmod kprobe

sysbench cpu --cpu-max-prime=50000 --time=30 run & echo $!

ps

sudo insmod kprobe.ko input_pid=26721

dmesg

dmesg



//Run an iperf udp server with a 1 MB socket buffer (default is 108 KB)

iperf -s &

ps

sudo rmmod kprobe

sudo insmod kprobe.ko input_pid=20224

iperf -c localhost -b 10m -l 10M -n 500240M -P 70

dmesg | grep Parse | grep 5149 > network_io.csv



//RUN for SORT(I/O and COMPUTE INTENSIVE)

shuf -i 0-36000000 -n 10000000000 > test.txt

sort test.txt > sort_test.txt &

sudo rmmod kprobe

sudo insmod kprobe.ko input_pid=20224
