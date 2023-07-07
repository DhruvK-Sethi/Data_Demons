import numpy as np
import matplotlib.pyplot as plt

# Generate a random 2D matrix
matrix = [[1, 0, 1],
          [0, 1, 0],
          [1, 0, 1]]

# Create a heatmap
plt.imshow(matrix, cmap='hot')

# Add colorbar
plt.colorbar()

# Show the plot
plt.show()
