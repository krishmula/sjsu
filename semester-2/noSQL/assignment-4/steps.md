# Assignment 4: Cassandra Step-by-Step Guide

This guide will walk you through completing your Cassandra assignment step-by-step and getting full marks.

## Step 1: Set up Cassandra

Run these commands in your host terminal:

```bash
# Start a single-node Cassandra cluster in a Docker container
docker run -itd --name cnode cassandra

# Verify the container is running
docker ps -a
```

Wait a minute or two for Cassandra to initialize, then enter the container:

```bash
docker exec -it cnode bash
```

Inside the container, start `cqlsh`:

```bash
cqlsh
```

Create and use a keyspace for the assignment:

```sql
CREATE KEYSPACE hw4 WITH REPLICATION = { 'class' : 'SimpleStrategy', 'replication_factor' : 1 };
USE hw4;
```

## Step 2: Create `citylist` Table

Execute the following CQL command to create the table with the required schema:

```sql
CREATE TABLE citylist (
  city varchar,
  loc list<float>,
  pop int,
  state varchar,
  zip varchar,
  PRIMARY KEY (state, city, zip)
);
```

## Step 3: Data Wrangling and Population

Open a new terminal window on your host machine (don't exit `cqlsh`).

1. **Download the Data:**

   ```bash
   wget http://media.mongodb.org/zips.json
   ```

2. **Wrangle the Data (Python Script):**
   Create a file named `wrangle.py` in the same directory as `zips.json` with this code:

   ```python
   import json
   import csv

   with open('zips.json', 'r') as f, open('citylist.csv', 'w', newline='') as out:
       writer = csv.writer(out)
       # Write the header in an order we can easily use for the COPY command later
       writer.writerow(['state', 'city', 'zip', 'loc', 'pop'])
       for line in f:
           try:
               data = json.loads(line)
               # Format the list of floats for CQL syntax
               loc = f"[{data['loc'][0]}, {data['loc'][1]}]"
               writer.writerow([data['state'], data['city'], data['_id'], loc, data['pop']])
           except json.JSONDecodeError:
               pass
   ```

3. **Run the script:**

   ```bash
   python3 wrangle.py
   ```

   **Description of Data Wrangling Method (for your report):**
   The given JSON data contains objects where the location is a JSON array (`[lon, lat]`) and the zip code is stored under the key `_id`. The Cassandra schema requires `loc` to be of type `list<float>` and explicitly maps `zip`. To populate the table correctly, I wrote a Python script to read the JSON file line by line. The script extracts each object, maps the `_id` field to the `zip` column, and formats the `loc` array into a string that matches CQL's list syntax (e.g., `[-72.62, 42.07]`). Finally, the script writes this cleaned and reorganized data into a standard CSV file with headers, allowing it to be seamlessly imported into Cassandra using the `COPY` command.

4. **Copy the CSV into the Docker container:**

   ```bash
   docker cp citylist.csv cnode:/citylist.csv
   ```

5. **Populate the Table in `cqlsh`:**
   Go back to your `cqlsh` prompt and execute:
   ```sql
   COPY citylist (state, city, zip, loc, pop) FROM '/citylist.csv' WITH HEADER = TRUE;
   ```

## Step 4: Show the First 15 Results

```sql
SELECT * FROM citylist LIMIT 15;
```

_(Take a screenshot of the results for your report)_

## Step 5: Queries for 'NEW YORK' (Methods 1-3)

First, enable tracing to measure execution times:

```sql
TRACING ON;
```

**Method 1: Allow Filtering**

```sql
SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15 ALLOW FILTERING;
```

_(Take a screenshot of the output including the execution time at the bottom)_

**Method 2: Secondary Index**

```sql
CREATE INDEX city_idx ON citylist (city);
```

_(Wait a few seconds for the index to build)_

```sql
SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15;
```

_(Take a screenshot of the result and execution time)_

**Method 3: Materialized View**

_(Note: Materialized views are disabled by default in newer Cassandra versions. You must enable them first)_

To enable them, open a new terminal window on your Mac (do not exit your current `cqlsh`) and run these two commands:

```bash
docker exec cnode sed -i 's/materialized_views_enabled: false/materialized_views_enabled: true/g' /etc/cassandra/cassandra.yaml
docker restart cnode
```

Wait about 30-60 seconds for the Cassandra container to fully restart. Since the container restarted, your `cqlsh` session will be disconnected. Reconnect by running `cqlsh` again and make sure to select your keyspace:

```sql
USE hw4;
```

Now you can create the materialized view:

```sql
CREATE MATERIALIZED VIEW citylist_mv AS
  SELECT * FROM citylist
  WHERE city IS NOT NULL AND state IS NOT NULL AND zip IS NOT NULL
  PRIMARY KEY (city, state, zip);
```

_(Wait a few moments for the view to build)_

```sql
SELECT * FROM citylist_mv WHERE city = 'NEW YORK' LIMIT 15;
```

_(Take a screenshot of the result and execution time)_

**Analysis for your report:**

- **Method 1:** `ALLOW FILTERING` scans all partitions across the entire dataset, loading every single row to check the condition. This usually results in the longest execution time and is very inefficient for large datasets.
- **Method 2:** A Secondary Index creates a hidden index table. Cassandra first queries the index to find matching primary keys, and then fetches those rows from the base table. Execution time is faster than Method 1, but there is still overhead due to looking up non-contiguous data.
- **Method 3:** A Materialized View actively maintains a copy of the data reorganized using `city` as the partition key. Fetching `city = 'NEW YORK'` is an extremely fast exact partition key lookup, making it the most efficient of the three methods.

## Step 6: 'NEW YORK' with Population Filter

To find NEW YORK zip codes with a population between 20,000 and 30,000:

**Which methods work?**
Method 3 (Materialized View) combined with `ALLOW FILTERING` works perfectly because the Materialized View's partition key is `city`. Method 2 (Secondary Index) with `ALLOW FILTERING` also works. However, using the Materialized View is considered much better practice.

```sql
-- Using the Materialized View method
SELECT zip FROM citylist_mv
WHERE city = 'NEW YORK' AND pop > 20000 AND pop < 30000 LIMIT 15 ALLOW FILTERING;
```

_(Take a screenshot of the results)_

_(Don't forget to turn tracing off if the output is too cluttered)_

```sql
TRACING OFF;
```

## Step 7 & 8: Schema Declaration for `citylist2` and Populating it

Create the new table with clustering order specified:

```sql
CREATE TABLE citylist2 (
  city varchar,
  loc list<float>,
  pop int,
  state varchar,
  zip varchar,
  PRIMARY KEY (state, city, zip)
) WITH CLUSTERING ORDER BY (city ASC, zip DESC);
```

Populate the table using the same CSV file from earlier:

```sql
COPY citylist2 (state, city, zip, loc, pop) FROM '/citylist.csv' WITH HEADER = TRUE;
```

Show the table content:

```sql
SELECT * FROM citylist2 LIMIT 15;
```

_(Take a screenshot of the results)_

## Step 9: Zip Codes in California

```sql
SELECT zip FROM citylist2 WHERE state = 'CA' LIMIT 15;
```

_(Take a screenshot of the results)_

## Step 10: Zip Codes in San Jose with Population Filter

```sql
SELECT zip FROM citylist2
WHERE state = 'CA' AND city = 'SAN JOSE' AND pop > 5000 AND pop < 10000 LIMIT 15 ALLOW FILTERING;
```

_(Take a screenshot of the results)_

---

### Final Submission Checklist

Compile your screenshots, the data wrangling code/description, the exact CQL queries used, and your analysis of the execution times into `hw4.pdf` as per the outline in the `readme.md`.
