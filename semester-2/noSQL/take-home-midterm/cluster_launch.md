# Cluster Setup Commands

I have extracted the IPs from your screenshot:
*   **nodeA**: Private `172.31.79.163`
*   **nodeB**: Private `172.31.72.1`
*   **nodeC**: Private `172.31.73.193`

---

## Step 1: Start Background Services
**Please run this ENTIRE block of code on ALL THREE nodes (nodeA, nodeB, and nodeC).** 
*Remember to take a screenshot of at least one of the nodes showing the `mkdir` execution to satisfy Task 4.*

```bash
# 1. Stop default services and create directories
sudo systemctl stop mongod
sudo systemctl disable mongod
mkdir -p ~/data/config ~/data/shard1 ~/data/shard2 ~/data/shard3

# 2. Launch Config Server process
mongod --configsvr --replSet configReplSet --port 27019 --dbpath ~/data/config --bind_ip 0.0.0.0 --fork --logpath ~/data/config.log

# 3. Launch the three Shard processes
mongod --shardsvr --replSet shard1 --port 27021 --dbpath ~/data/shard1 --bind_ip 0.0.0.0 --fork --logpath ~/data/shard1.log
mongod --shardsvr --replSet shard2 --port 27022 --dbpath ~/data/shard2 --bind_ip 0.0.0.0 --fork --logpath ~/data/shard2.log
mongod --shardsvr --replSet shard3 --port 27023 --dbpath ~/data/shard3 --bind_ip 0.0.0.0 --fork --logpath ~/data/shard3.log
```

---

## Step 2: Initialize Replica Sets
Once you have run Step 1 on **all three nodes**, copy and paste these commands **ONLY ON nodeA**:

```javascript
// Initialize Config Server Replica Set
mongosh --port 27019 --eval 'rs.initiate({_id: "configReplSet", configsvr: true, members: [{_id: 0, host: "172.31.79.163:27019"}, {_id: 1, host: "172.31.72.1:27019"}, {_id: 2, host: "172.31.73.193:27019"}]})'
sleep 3

// Initialize Shard 1 Replica Set
mongosh --port 27021 --eval 'rs.initiate({_id: "shard1", members: [{_id: 0, host: "172.31.79.163:27021"}, {_id: 1, host: "172.31.72.1:27021"}, {_id: 2, host: "172.31.73.193:27021"}]})'

// Initialize Shard 2 Replica Set
mongosh --port 27022 --eval 'rs.initiate({_id: "shard2", members: [{_id: 0, host: "172.31.79.163:27022"}, {_id: 1, host: "172.31.72.1:27022"}, {_id: 2, host: "172.31.73.193:27022"}]})'

// Initialize Shard 3 Replica Set
mongosh --port 27023 --eval 'rs.initiate({_id: "shard3", members: [{_id: 0, host: "172.31.79.163:27023"}, {_id: 1, host: "172.31.72.1:27023"}, {_id: 2, host: "172.31.73.193:27023"}]})'
```

---

## Step 3: Launch mongos (Connect to Config Servers)
This step connects the query router (`mongos`) to all three configure servers. Run this command on **any one** of your nodes (e.g., **nodeA**).
*Take a screenshot of you running this command, as this satisfies Task 7.*

```bash
mongos --configdb configReplSet/172.31.79.163:27019,172.31.72.1:27019,172.31.73.193:27019 --bind_ip 0.0.0.0 --port 27017 --fork --logpath ~/data/mongos.log
```

---

## Step 4: Add Shards to the Cluster (Tasks 8 & 9)
Now you need to tell the query router (`mongos`) about your three shards so it can distribute data across them.

Run this block by connecting to the `mongos` router (e.g., run this on **nodeA** after starting `mongos`):

```bash
# Connect to the mongos query router
mongosh --port 27017
```

Once inside the `mongosh` prompt, run the following commands.
*Make sure to take screenshots of the `sh.status()` outputs to satisfy Tasks 8 and 9.*

```javascript
// 1. Run sh.status() BEFORE adding shards (Task 8 requirement)
sh.status()

// 2. Add Shard 1
sh.addShard("shard1/172.31.79.163:27021,172.31.72.1:27021,172.31.73.193:27021")

// 3. Add Shard 2
sh.addShard("shard2/172.31.79.163:27022,172.31.72.1:27022,172.31.73.193:27022")

// 4. Add Shard 3
sh.addShard("shard3/172.31.79.163:27023,172.31.72.1:27023,172.31.73.193:27023")

// 5. Run sh.status() AFTER adding shards (Task 9 requirement)
sh.status()
```

---

## Step 5: Enable Sharding (Task 10)
For this task, you need to enable sharding for a particular database and then shard a specific collection within it using a shard key. Assuming you will use a generic database called `midtermDB` and a collection called `dataset` for your public dataset in Task 11.

Run the following inside your `mongosh` session:

```javascript
// 1. Enable sharding on your new database
sh.enableSharding("midtermDB")

// 2. Create an index on the field you plan to use as the shard key (e.g., an '_id' field)
use midtermDB
db.dataset.createIndex({ _id: "hashed" })

// 3. Shard the collection using a Hashed Sharding Strategy
sh.shardCollection("midtermDB.dataset", { _id: "hashed" })

// 4. Verify the collection is sharded
sh.status()
```

### What to put in your report for Task 10:
*   **Screenshot**: Take a screenshot of you running `sh.enableSharding()` and `sh.shardCollection()`.
*   **Explanation**: In your report, write: *"I used a **hashed** sharding strategy on the `_id` field. The nature of the shard key is **random** because the hashing function converts the ascending/random `_id` values into random hashed values, which ensures an even distribution of data across all three shards and prevents any single shard from becoming a bottleneck."*

---

## Step 6: Populate the Cluster with Data (Task 11)

For this task, we will use the **MongoDB NY Restaurants** dataset. It contains around 25,000 documents and is complex enough to support all the queries we need for Task 12 (it has nested arrays, numbers, and strings).

Run this on your terminal (e.g., **nodeA**) from the regular bash prompt (NOT inside `mongosh`):

```bash
# 1. Download the public dataset
wget https://raw.githubusercontent.com/mongodb/docs-assets/primer-dataset/primer-dataset.json

# 2. Import it into the query router (port 27017 will route it to the shards)
mongoimport --host 127.0.0.1 --port 27017 --db midtermDB --collection dataset --file primer-dataset.json
```

Once imported, enter your `mongosh` session again (`mongosh --port 27017`) and check the shard status:

```javascript
sh.status()
```

### What to put in your report for Task 11:
*   **Screenshot**: The output of `sh.status()` AFTER the data has been populated to prove chunks are distributed. Include the `mongoimport` command output if possible.
*   **Explanation**: Write: *"I used the public MongoDB New York Restaurants dataset (URL: `https://raw.githubusercontent.com/mongodb/docs-assets/primer-dataset/primer-dataset.json`). The collection contains data about restaurant locations, cuisines, and an array of inspection grades. I used `mongoimport` to import the JSON file into the cluster, and the hashed `_id` shard key distributed the blocks across the three shards. The `sh.status()` screenshot proves the chunks have been created and distributed for the collection."*

---

## Step 7: Execute Queries (Task 12)

Connect to your `mongos` router using `mongosh --port 27017` and select your database:

```javascript
use midtermDB
```

For each of the following queries, `.explain("executionStats")` is used. This output will explicitly show the **Execution Time** (`executionTimeMillis`) and **Which Shards** served the query under the `shards` object in the output JSON.

### 12.1. Range Query
Finds restaurants within a specific range of internal Zipcodes.
```javascript
db.dataset.find({ "address.zipcode": { $gt: "10000", $lt: "10010" } }).explain("executionStats")
```

### 12.2. $elemMatch involving at least two conditions
Finds restaurants that have an inspection grade of 'A' AND a score greater than 10 in the SAME inspection event (array item).
```javascript
db.dataset.find({ grades: { $elemMatch: { grade: "A", score: { $gt: 10 } } } }).explain("executionStats")
```

### 12.3. $in, $nin, or $all
Finds restaurants where the cuisine is either Bakery, Italian, or Mexican.
```javascript
db.dataset.find({ cuisine: { $in: ["Bakery", "Italian", "Mexican"] } }).explain("executionStats")
```

### 12.4. Aggregate Query
Groups restaurants by borough and counts how many restaurants are in each borough.
```javascript
db.dataset.explain("executionStats").aggregate([
   { $group: { _id: "$borough", count: { $sum: 1 } } }
])
```

### 12.5. Update Query
Updates all restaurants in the "Staten Island" borough that serve "Bakery" food to flag them as reviewed.
```javascript
db.dataset.explain("executionStats").update({ cuisine: "Bakery", borough: "Staten Island" }, { $set: { "status": "Reviewed" } }, { multi: true })
```

### 12.6. Delete Query
Deletes all restaurants from the dataset that serve "Hotdogs".
```javascript
db.dataset.explain("executionStats").remove({ cuisine: "Hotdogs" })
```

### What to put in your report for Task 12:
For **each** of the 6 queries above, you need to provide:
1.  **A Screenshot** showing you ran the query and showing the `executionStats` output.
2.  **Execution Time**: Look for `executionTimeMillis` in the printed JSON and write down the number (e.g., "Execution time: 14 ms").
3.  **Shard Served**: Look for the `shards` object in the explain output. It will list `shard1`, `shard2`, or `shard3`. Explicitly write down which shards participated in serving the query (e.g., "This query was broadcasted and served by all three shards: shard1, shard2, and shard3.").

---

## Step 8: Show Shards are Replicated (Task 13)

To show that your shards are properly replicated, you need to connect directly to the underlying shard processes (NOT the query router) and run `rs.status()`.

**Check Shard 1:**
Run this from your normal bash prompt on nodeA:
```bash
mongosh --port 27021
```
Inside, run `rs.status()` and take a screenshot. Then type `exit`.

**Check Shard 2:**
```bash
mongosh --port 27022
```
Inside, run `rs.status()` and take a screenshot. Then type `exit`.

**Check Shard 3:**
```bash
mongosh --port 27023
```
Inside, run `rs.status()` and take a screenshot. Then type `exit`.

### What to put in your report for Task 13:
*   **Screenshots**: Provide the screenshots of `rs.status()` for all three shards. You should see one member listed as `PRIMARY` and two listed as `SECONDARY` for each shard.
*   **Explanation**: *"By connecting directly to ports 27021, 27022, and 27023, I verified the `rs.status()` for all three shards. Each shard is fully replicated across the three EC2 nodes, successfully electing one PRIMARY node and two SECONDARY nodes per replica set."*

---

## Step 9: Report the Internal Topology (Task 14)

You do not need to run any additional commands for this task! You just need to document the architecture we built.

### What to put in your report for Task 14:
Copy and paste this host list and description exactly into your report:

**Host List & Deployments**
*   **nodeA (`172.31.79.163`)**: 
    *   `port 27019`: Config Server Member
    *   `port 27021`: Shard 1 Member
    *   `port 27022`: Shard 2 Member
    *   `port 27023`: Shard 3 Member
    *   `port 27017`: `mongos` Query Router
*   **nodeB (`172.31.72.1`)**: 
    *   `port 27019`: Config Server Member
    *   `port 27021`: Shard 1 Member
    *   `port 27022`: Shard 2 Member
    *   `port 27023`: Shard 3 Member
*   **nodeC (`172.31.73.193`)**: 
    *   `port 27019`: Config Server Member
    *   `port 27021`: Shard 1 Member
    *   `port 27022`: Shard 2 Member
    *   `port 27023`: Shard 3 Member

*Note: For the config server set and all 3 shard sets, the Replica Set elections automatically evaluated the members distributed across these three EC2 instances and elected one PRIMARY and two SECONDARY members for each set.*
