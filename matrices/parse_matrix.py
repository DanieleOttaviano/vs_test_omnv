import os
import re
import csv

def process_matrix_file(filename):
    # Extract name, X, and Y from filename
    match = re.match(r'(\w+)_([0-9]+)_([0-9]+)\.txt$', filename)
    if not match:
        return None
    name, x_dim, y_dim = match.groups()
    x_dim, y_dim = int(x_dim), int(y_dim)

    # Read the matrix values with auto-detected delimiter
    with open(filename, 'r') as f:
        lines = [line.strip() for line in f if line.strip()]

    if len(lines) != y_dim:
        raise ValueError(f"Incorrect number of rows in {filename}: expected {y_dim}, got {len(lines)}")

    matrix = []
    for idx, line in enumerate(lines):
        # Auto-detect delimiters: comma, tab, or space
        if ',' in line:
            values = [val.strip() for val in line.split(',')]
        elif '\t' in line:
            values = [val.strip() for val in line.split('\t')]
        else:
            values = [val.strip() for val in line.split()]

        if len(values) != x_dim:
            raise ValueError(f"Incorrect number of columns in {filename} at line {idx + 1}: expected {x_dim}, got {len(values)}. Line content: '{line}'")
        
        matrix.append(values)

    # Generate C-style matrix assignment using name[X][Y]
    c_output = [f'// Matrix from {filename}']
    c_output.append(f'double {name}[{y_dim}][{x_dim}] = {{')

    for i in range(y_dim):
        formatted_row = ', '.join(matrix[i])
        c_output.append(f'    {{ {formatted_row} }}' + (',' if i < y_dim - 1 else '') )
    
    c_output.append('};')

    return '\n'.join(c_output)

def main():
    for filename in os.listdir('.'): 
        if filename.endswith('.txt'):
            try:
                c_code = process_matrix_file(filename)
                if c_code:
                    print(c_code + '\n')
            except ValueError as e:
                print(f"Error processing {filename}: {e}")

if __name__ == "__main__":
    main()

