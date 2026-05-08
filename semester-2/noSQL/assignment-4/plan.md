# Assignment 4 (Cassandra) - Execution Plan

This is the authoritative, step-by-step plan to complete the Cassandra Assignment (Assignment 4) based on the course specifications for CS157C.

If an autonomous agent follows this plan, they should execute the commands sequentially, verify outputs at each milestone, and save the transaction logs and execution outputs to assemble into `hw4.pdf` at the end.

## Step 1: Environment Setup
We need to start a single-node Cassandra cluster using Docker.

**Commands to run in terminal:**
```bash
# 1. Start the Cassandra container
docker run -itd --name cnode cassandra:3.11.6

# 2. Verify the container is running
docker ps -a | grep cnode

# Wait a minute or two for Cassandra to initialize.
# 3. Enter the container's bash shell
docker exec -it cnode bash
```

## Step 2: Acquire and Wrangle Data
The dataset `zips.json` is a new-line separated JSON file (JSONL), which needs to be transformed to match the table schema.
`_id` represents the `zip` code. `loc` is an array of `[longitude, latitude]`.

**Commands from host environment (not inside container, or inside a connected terminal):**
```bash
# 1. Navigate to assignment directory
cd /Users/krishna/sjsu/semester-2/noSQL/assignment-4/

# 2. Download the data
wget http://media.mongodb.org/zips.json

# 3. Wrangle the data using Python.
# We will create a script `wrangle.py` that converts JSONL to a CSV format matching the destination schema.
```

**Content of `wrangle.py`:**
```python
import json
import csv

input_file = 'zips.json'
output_file = 'zips_wrangled.csv'

with open(input_file, 'r') as infile, open(output_file, 'w', newline='') as outfile:
    # Scheme: city, loc, pop, state, zip
    writer = csv.writer(outfile)
    writer.writerow(['city', 'loc', 'pop', 'state', 'zip'])
    
    for line in infile:
        try:
            data = json.loads(line)
            # Cassandras List format in CSV expects JSON-like array notation or Pipe delimited depending on the loader, 
            # CQL COPY handles python list strings reasonably if quoted properly, but strict format is e.g., "[ -72.6, 42.1 ]"
            loc_str = f"[{data['loc'][0]},{data['loc'][1]}]"
            city = data['city']
            pop = data['pop']
            state = data['state']
            zip_code = data['_id']
            writer.writerow([city, loc_str, pop, state, zip_code])
        except json.JSONDecodeError:
            continue
```

**Commands to execute wrangling and copy to container:**
```bash
# Generate the CSV
python3 wrangle.py

# Copy the wrangled data into the docker container so CQL COPY can access it
docker cp zips_wrangled.csv cnode:/zips_wrangled.csv
```

## Step 3: Database Initialization & Data Loading
Agents must enter `cqlsh` and run the DDL commands.

**Commands to run in terminal:**
```bash
# Access CQL shell inside the container
docker exec -it cnode cqlsh
```

**CQL Commands (inside cqlsh):**
```sql
-- 1. Create Keyspace
CREATE KEYSPACE cs157c WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'};

-- 2. Use Keyspace
USE cs157c;

-- 3. Create Table
CREATE TABLE citylist (
    city varchar,
    loc list<float>,
    pop int,
    state varchar,
    zip varchar,
    PRIMARY KEY (state, city, zip)
);

-- 4. Import Data
COPY citylist (city, loc, pop, state, zip) FROM '/zips_wrangled.csv' WITH header = true;

-- 5. Capture Screenshot/Log of first 15 rows for the report (Requirement 4)
SELECT * FROM citylist LIMIT 15;
```

## Step 4: Method Evaluation (city = 'NEW YORK')
The requirement is to query `city = 'NEW YORK'` without specifying the `state` partition key. This requires comparing 3 methods. To capture execution time, we turn tracing on.

**CQL Commands:**
```sql
USE cs157c;
TRACING ON;

-- Method 1: ALLOW FILTERING (Requirement 5 - Method 1)
SELECT * FROM citylist WHERE city = 'NEW YORK' ALLOW FILTERING LIMIT 15;
-- RECORD THE "Execute CQL3 query" execution time at the bottom of the trace!

-- Method 2: Secondary Index (Requirement 5 - Method 2)
CREATE INDEX IF NOT EXISTS city_idx ON citylist (city);
-- Wait a few seconds for index to build
SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15;
-- RECORD THE execution time!

-- Method 3: Materialized View (Requirement 5 - Method 3)
-- We need the view primary key to have city as the partition key.
CREATE MATERIALIZED VIEW IF NOT EXISTS citylist_by_city AS
    SELECT * FROM citylist
    WHERE city IS NOT NULL AND state IS NOT NULL AND zip IS NOT NULL
    PRIMARY KEY (city, state, zip);
-- Wait for view to build
SELECT * FROM citylist_by_city WHERE city = 'NEW YORK' LIMIT 15;
-- RECORD THE execution time!

TRACING OFF;
```

*Agent Note: The agent must capture the timings emitted by tracing and log them for comparing the performance degradation of Allow Filtering vs Indexing vs Materialized Views.*

## Step 5: Advanced Population Queries
Find NEW YORK city zips with pop between 20k and 30k (exclusive).

**CQL Commands:**
```sql
TRACING ON;
-- For Method 3 (Materialized View), which is the most efficient method here:
SELECT zip FROM citylist_by_city WHERE city = 'NEW YORK' AND pop > 20000 AND pop < 30000 ALLOW FILTERING LIMIT 15;

-- Alternatively:
SELECT zip FROM citylist WHERE city = 'NEW YORK' AND pop > 20000 AND pop < 30000 ALLOW FILTERING LIMIT 15;
TRACING OFF;
```
*Note for report (Req 6): Explain which method works. The secondary index and the materialized view both work but need `ALLOW FILTERING` on the `pop` constraint unless `pop` is added as a clustering key in the Materialized View.*

## Step 6: Schema `citylist2` and Population
Define another table to query in ascending order of city and descending order of zip.

**CQL Commands:**
```sql
CREATE TABLE citylist2 (
    state varchar,
    city varchar,
    zip varchar,
    loc list<float>,
    pop int,
    PRIMARY KEY (state, city, zip)
) WITH CLUSTERING ORDER BY (city ASC, zip DESC);

-- Import the same data
COPY citylist2 (city, loc, pop, state, zip) FROM '/zips_wrangled.csv' WITH header = true;

-- Show table content (Req 8)
SELECT * FROM citylist2 LIMIT 15;
```

## Step 7: Final Filtering Queries
Find zips in California, and San Jose population filters.

**CQL Commands:**
```sql
-- Query 1: All zip codes of state California (Req 9)
SELECT zip FROM citylist2 WHERE state = 'CA' LIMIT 15;

-- Query 2: All zip codes of city San Jose where pop > 5000 and pop < 10000 (Req 10)
SELECT zip FROM citylist2 WHERE state = 'CA' AND city = 'SAN JOSE' AND pop > 5000 AND pop < 10000 LIMIT 15;
```

## Step 8: Document Generation
The final step is to compile all the outputs, queries, execution times, screenshots, and logs into a markdown file (`report.md`), which can then be converted to `hw4.pdf`. Ensure that all numbered requirements from the README.md are clearly labeled with their respective outputs.
