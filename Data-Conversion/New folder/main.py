import json

with open('test_1.json') as f:
    data = json.load(f)

new_data = []

for feature in data:
    coords = feature['geometry']['coordinates']
    if feature['geometry']['type'] == 'LineString':
        for point in coords:
            longitude, latitude = point
            new_data.append([longitude, latitude])

with open('output.json', 'w') as f:
    json.dump(new_data, f)
