import matplotlib.pyplot as plt

def plot_scattered_graph(file_path, title):
    f = open(file_path)
    times = list()
    virutal_addresses = list()
    for line in f.readlines():
        # Linux as well as dmesg timestamp
        linux_time = int(line.split(",")[-1])
        times.append(linux_time)   
        
        # Virtual address
        address = line.split(",")[-2]
        virutal_addresses.append(int(address, 16))

    plt.scatter(times, virutal_addresses)
    plt.ylabel('Virtual Address (hex as int)')
    plt.xlabel('Linux Timestamp (int)')
    plt.title(title)
    plt.show()

plot_scattered_graph("compute.csv", "Sysbench(Compute Intensive)")
#plot_scattered_graph("network_io.csv", "Iperf(Network I/O intensive)")
# plot_scattered_graph("compute_io.csv", "Sort(Compute & I/O intensive)")