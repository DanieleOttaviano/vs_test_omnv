import matplotlib.pyplot as plt
import numpy as np
import os
import sys

# Path to the jitter file
file_path = sys.argv[1] + 'timestamps.txt'

# Load the jitter data from the file
# Assuming each value is in a new line
with open(file_path, 'r') as file:
    jitter_data = np.array([float(line.strip()) for line in file])

# Plot the jitter data
plt.figure(figsize=(10, 6))

# Plot with thin line and markers at each point
plt.plot(jitter_data, linewidth=0.2, marker='x', markersize=6, linestyle='--')

# Setting the y-axis limit between 0 and 500
plt.ylim(0, 500)

# Setting the y-axis ticks every 50 units
plt.yticks(np.arange(0, 501, 50))

# Adding title and labels with increased font size
plt.title('Execution Time', fontsize=16)
plt.xlabel('Samples', fontsize=14)
plt.ylabel('Time [\u03BCs]', fontsize=14)

# Display grid
plt.grid(True)

# Show the plot
plt.show()

