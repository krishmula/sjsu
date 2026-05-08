import csv
import json

input_file = "zips.json"
output_file = "zips_wrangled.csv"

with open(input_file, "r") as infile, open(output_file, "w", newline="") as outfile:
    writer = csv.writer(outfile)
    writer.writerow(["city", "loc", "pop", "state", "zip"])

    for line in infile:
        try:
            data = json.loads(line)
            loc_str = f"[{data['loc'][0]},{data['loc'][1]}]"
            writer.writerow(
                [data["city"], loc_str, data["pop"], data["state"], data["_id"]]
            )
        except json.JSONDecodeError:
            continue
