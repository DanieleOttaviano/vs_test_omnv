import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

def remove_outliers(data):
    q1 = np.percentile(data, 1)
    q3 = np.percentile(data, 99)
    iqr = q3 - q1
    lower_bound = q1 - 1.5 * iqr
    upper_bound = q3 + 1.5 * iqr
    return [x for x in data if lower_bound <= x <= upper_bound]

# Read the cycle times from the file
file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_solo.txt'
with open(file_path, 'r') as file:
    cycle_times_solo = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_membomb_col_bwreg.txt'
with open(file_path, 'r') as file:
    cycle_times_membomb_col_bwreg = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_membomb_bwreg.txt'
with open(file_path, 'r') as file:
    cycle_times_membomb_bwreg = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_membomb_col.txt'
with open(file_path, 'r') as file:
    cycle_times_membomb_col = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_membomb.txt'
with open(file_path, 'r') as file:
    cycle_times_membomb = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_solo_col.txt'
with open(file_path, 'r') as file:
    cycle_times_solo_col = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_solo_bwreg.txt'
with open(file_path, 'r') as file:
    cycle_times_solo_bwreg = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/Results/cycle_times_solo_col_bwreg.txt'
with open(file_path, 'r') as file:
    cycle_times_solo_col_bwreg = remove_outliers([float(line.strip()) for line in file])

file_path = '/home/daniele/Projects/Omnivisor/marte_test/cycle_times.txt'
with open(file_path, 'r') as file:
    cycle_times = remove_outliers([float(line.strip()) for line in file])



# Combine all cycle times into a single list and create corresponding labels
data = [cycle_times, cycle_times_solo]
labels= ['solo', 'solo_old']
#data = [cycle_times_solo, cycle_times_membomb, cycle_times_solo_col, cycle_times_membomb_col, cycle_times_solo_bwreg, cycle_times_membomb_bwreg, cycle_times_solo_col_bwreg, cycle_times_membomb_col_bwreg]
#labels = ['solo', 'membomb' ,'solo_col', 'membomb_col', 'solo_bwreg', 'membomb_bwreg', 'solo_col_bwreg', 'membomb_col_bwreg']

# Plot the box plots
plt.figure(figsize=(10, 6))
sns.violinplot(data=data)
plt.xticks(ticks=range(len(labels)), labels=labels)
plt.title('Round Trip Times [us]')
plt.xlabel('Configuration')
plt.ylabel('Time')
# plt.ylim(0, 1000000)  # Set y-axis range from 0 to 1,000,000
plt.grid(True)
plt.show()
