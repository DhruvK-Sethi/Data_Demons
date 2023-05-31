# import json
# import sqlite3

# # Open the JSON file
# with open('roads.json') as f:
#     data = json.load(f)

# # Connect to the database
# conn = sqlite3.connect('roads.db')
# c = conn.cursor()

# # Create the roads table
# c.execute('''CREATE TABLE roads (id INTEGER PRIMARY KEY, coords TEXT)''')

# # Initialize the road counter
# road_id = 1

# # Loop over the features in the JSON data
# for feature in data['features']:
#     # Extract the coordinates of the road
#     coords = feature['geometry']['coordinates']
    
#     # Convert the coordinates to a string
#     coords_str = json.dumps(coords)
    
#     # Insert the road into the database with a unique identifier
#     c.execute("INSERT INTO roads (id, coords) VALUES (?, ?)", (road_id, coords_str))
    
#     # Increment the road counter
#     road_id += 1

# # Commit the changes to the database
# conn.commit()

# # Close the database connection
# conn.close()

import json
import sqlite3

# Open the JSON file and parse it as a Python dictionary
with open('roads.json') as f:
    data = json.load(f)

# Connect to the SQLite database
conn = sqlite3.connect('roads.db')

# Create the roads table if it doesn't already exist
conn.execute('''
    CREATE TABLE IF NOT EXISTS roads (
        id INTEGER PRIMARY KEY,
        road_id INTEGER,
        latitude REAL,
        longitude REAL
    );
''')

# Initialize the road ID and row ID counters
road_id = 1
row_id = 1

# Iterate over each feature in the GeoJSON data
for feature in data['features']:
    # Extract the road's coordinates
    coords = feature['geometry']['coordinates']

    # Iterate over each coordinate and insert it into the database
    for coord in coords:
        lat, lon = coord
        conn.execute('''
            INSERT INTO roads (id, road_id, latitude, longitude)
            VALUES (?, ?, ?, ?);
        ''', (row_id, road_id, lat, lon))
        row_id += 1

    # Increment the road ID for the next road
    road_id += 1

# Commit the changes to the database and close the connection
conn.commit()
conn.close()
