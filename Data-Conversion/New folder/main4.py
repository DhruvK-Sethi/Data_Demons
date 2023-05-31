import json

with open('roads.json', 'r') as f:
    data = json.load(f)

lat_long_data = {}

for feature in data['features']:
    if feature['geometry']['type'] == 'LineString':
        road_id = feature['properties'].get('id')
        coordinates = feature['geometry']['coordinates']
        for lat, long in coordinates:
            if road_id not in lat_long_data:
                lat_long_data[road_id] = []
            lat_long_data[road_id].append((lat, long))
            
print(lat_long_data)
