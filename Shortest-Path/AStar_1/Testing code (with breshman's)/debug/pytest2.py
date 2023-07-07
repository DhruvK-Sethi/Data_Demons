import plotly.graph_objects as go

# Define the matrix
matrix = [[1, 0, 1],
          [0, 1, 0],
          [1, 0, 1]]

# Create a heatmap figure
fig = go.Figure(data=go.Heatmap)

# Set the axis labels
fig.update_layout(xaxis_title='Columns', yaxis_title='Rows')

# Show the plot
fig.show()
