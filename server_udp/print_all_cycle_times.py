#!/usr/bin/env python3
"""Simplified aggregator: match Results/cycle_times_<suffix>.txt with
tcpdump_results/packets_<suffix>.txt (or fallback to other tcpdump locations).

This version assumes the suffix after the '*' is the same between
packets_* and cycle_times_* files.
"""
import os
import statistics
from typing import List, Optional


def read_floats(path: str) -> List[float]:
    vals = []
    if not path or not os.path.isfile(path):
        return vals
    with open(path, 'r') as f:
        for line in f:
            s = line.strip()
            if not s:
                continue
            try:
                vals.append(float(s))
            except ValueError:
                continue
    return vals


def summarize(data: List[float]) -> Optional[dict]:
    if not data:
        return None
    mean = statistics.mean(data)
    median = statistics.median(data)
    stdev = statistics.pstdev(data)
    return {
        'count': len(data),
        'mean': mean,
        'median': median,
        'stdev': stdev,
        'min': min(data),
        'max': max(data),
    }


def main():
    base = os.path.abspath(os.path.dirname(__file__))
    results_dir = os.path.join(base, 'Results')
    tcp_dir = os.path.join(base, 'tcpdump_results')

    if not os.path.isdir(results_dir):
        print('Results directory not found:', results_dir)
        return

    files = [f for f in os.listdir(results_dir) if f.startswith('cycle_times_') and f.endswith('.txt')]
    if not files:
        print('No cycle_times_*.txt files found in', results_dir)
        return

    print('\nCycle time summaries (matching suffix between cycle_times_* and packets_*)')

    for fn in sorted(files):
        suffix = fn[len('cycle_times_'):-4]
        res_path = os.path.join(results_dir, fn)
        res_vals = read_floats(res_path)
        res_sum = summarize(res_vals)

        # Prefer a precomputed tcpdump timestamps file named packets_<suffix>.txt in tcp_dir
        tcp_txt = os.path.join(tcp_dir, f'packets_{suffix}.txt')
        tcp_pcap = os.path.join(tcp_dir, f'packets_{suffix}.pcap')
        tcp_folder_ts = os.path.join(tcp_dir, suffix, 'timestamps.txt')
        tcp_global = os.path.join(tcp_dir, 'timestamps.txt')

        tcp_path = None
        if os.path.isfile(tcp_txt):
            tcp_path = tcp_txt
        elif os.path.isfile(tcp_pcap):
            tcp_path = tcp_pcap
        elif os.path.isfile(tcp_folder_ts):
            tcp_path = tcp_folder_ts
        elif os.path.isfile(tcp_global):
            tcp_path = tcp_global

        print('\n' + '=' * 60)
        print(f'Suffix: {suffix}')
        print(f'  Results file: {os.path.relpath(res_path, base)}')
        if res_sum:
            print('    Results: count={count}, mean={mean:.3f}, median={median:.3f}, stdev={stdev:.3f}, min={min:.3f}, max={max:.3f}'.format(**res_sum))
        else:
            print('    Results: no numeric data parsed')

        if tcp_path:
            if tcp_path.endswith('.pcap'):
                print(f'  Tcpdump pcap found (no timestamps.txt): {os.path.relpath(tcp_path, base)}')
                print('    -> run tcpdump_results/pcap_to_timestamp_computation_times.py to generate packets_<suffix>.txt')
            else:
                tcp_vals = read_floats(tcp_path)
                tcp_sum = summarize(tcp_vals)
                print(f'  Tcpdump file: {os.path.relpath(tcp_path, base)}')
                if tcp_sum:
                    print('    Tcpdump: count={count}, mean={mean:.3f}, median={median:.3f}, stdev={stdev:.3f}, min={min:.3f}, max={max:.3f}'.format(**tcp_sum))
                else:
                    print('    Tcpdump: no numeric data parsed')
        else:
            print('  Tcpdump file: NOT FOUND for suffix (tried packets_{suffix}.txt/.pcap, <suffix>/timestamps.txt, global timestamps.txt)'.replace('{suffix}', suffix))

        if res_sum and tcp_path and not tcp_path.endswith('.pcap'):
            tcp_vals = read_floats(tcp_path)
            tcp_sum = summarize(tcp_vals)
            if tcp_sum:
                diff = res_sum['mean'] - tcp_sum['mean']
                print(f'    Mean difference (Results - Tcpdump) = {diff:.3f} ({diff/res_sum["mean"]*100:.2f}% relative to Results mean)')


if __name__ == '__main__':
    main()
