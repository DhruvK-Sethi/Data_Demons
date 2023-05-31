import sqlite3

# Connect to the database
conn = sqlite3.connect('roads.db')
cur = conn.cursor()

# Query the database to get all the coordinates
cur.execute('SELECT id, coords FROM roads')
rows = cur.fetchall()

# Create a dictionary to store the coordinates by ID
coords_by_id = {}
for id, coords_json in rows:
    # Convert the JSON string to a list of coordinates
    coords = eval(coords_json)
    # Add the coordinates to the dictionary, grouped by ID
    if id not in coords_by_id:
        coords_by_id[id] = []
    for coord in coords:
        coords_by_id[id].append(coord)

# Create a list to store the results
results = []
# Loop through the coordinates by ID
# Loop through the coordinates by ID
for id, coords in coords_by_id.items():
    # Group the coordinates by latitude and longitude
    coords_by_lat_lon = {}
    for coord in coords:
        if isinstance(coord, tuple) and len(coord) == 2:
            lat, lon = coord
            if (lat, lon) not in coords_by_lat_lon:
                coords_by_lat_lon[(lat, lon)] = []
            coords_by_lat_lon[(lat, lon)].append(coord)
    # Add the coordinates to the results list in the required format
    for lat_lon, coords_list in coords_by_lat_lon.items():
        lat, lon = lat_lon
        for coord in coords_list:
            results.append((id, lat, lon))


# print(results)      
# Create a dictionary to store the coordinates by ID
coords_by_id = {}
for id, coords_json in rows:
    # Convert the JSON string to a list of coordinates
    coords = eval(coords_json)
    # Add the coordinates to the dictionary, grouped by ID
    if id not in coords_by_id:
        coords_by_id[id] = []
    for coord in coords:
        coords_by_id[id].append(coord)

print(coords_by_id)


# Save the results to a new table in the database
cur.execute('CREATE TABLE separated_coords (id INTEGER, latitude REAL, longitude REAL)')
cur.executemany('INSERT INTO separated_coords VALUES (?, ?, ?)', results)
conn.commit()

# Close the database connection
conn.close()
