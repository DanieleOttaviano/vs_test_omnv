from scapy.utils import RawPcapReader
from scapy.layers.l2 import Ether
from scapy.layers.inet import IP
import sys
import os


def process_pcap(pcap_path, target='192.168.1.60', max_samples=200000):
    """Process single pcap file and write a .txt file with RTTs next to it.

    Example: packets_solo_color_off_bwreg_off.pcap -> packets_solo_color_off_bwreg_off.txt
    """
    last_recv_ts = None
    counter = 0

    out_path = os.path.splitext(pcap_path)[0] + '.txt'
    with open(out_path, 'w') as out_file:
        for (pkt_data, pkt_metadata,) in RawPcapReader(pcap_path):
            if counter >= max_samples:
                break
            pkt_timestamp = (pkt_metadata.sec * 1_000_000) + pkt_metadata.usec
            ether_pkt = Ether(pkt_data)
            if IP not in ether_pkt:
                continue
            ip_pkt = ether_pkt[IP]
            if ip_pkt.dst == target:
                # host received a packet
                last_recv_ts = pkt_timestamp
            elif ip_pkt.src == target and last_recv_ts is not None:
                # host sent a reply; compute send - receive
                diff = pkt_timestamp - last_recv_ts
                out_file.write(f"{diff}\n")
                if diff > 200:
                    print(f"Large RTT detected: {diff} us at packet nr {counter*2} (file {pcap_path}).")
                counter += 1
                last_recv_ts = None  # consume this receive-reply pair


def process_all_in_dir(path='.'): 
    """Process all .pcap files in given directory (non-recursive).

    For each pcap file found, creates a .txt file with the same base name in the same directory.
    """
    files = [f for f in os.listdir(path) if f.lower().endswith('.pcap')]
    if not files:
        print(f'No .pcap files found in {os.path.abspath(path)}')
        return
    for fn in files:
        pcap_path = os.path.join(path, fn)
        print(f'Processing {pcap_path} -> {os.path.splitext(pcap_path)[0] + ".txt"}')
        try:
            process_pcap(pcap_path)
        except Exception as e:
            print(f'ERROR processing {pcap_path}: {e}')


if __name__ == '__main__':
    # Usage:
    #  - no args: process all .pcap in current directory
    #  - one arg that is a directory: process all .pcap in that directory
    #  - one arg that is a pcap file: process that file only
    if len(sys.argv) == 1:
        process_all_in_dir('.')
    elif len(sys.argv) == 2:
        arg = sys.argv[1]
        if os.path.isdir(arg):
            process_all_in_dir(arg)
        elif os.path.isfile(arg) and arg.lower().endswith('.pcap'):
            process_pcap(arg)
        else:
            print('Argument must be a directory or a .pcap file')
    else:
        print('Usage: python3 pcap_to_timestamp_computation_times.py [dir|file.pcap]')