1. The tasks given in this exam should be done in the AWS Learners' Lab, where you have $50credit. When an AWS instance is not being used, you should pause it so that you are not being charged unnecessarily.  
2. **What to submit**: Include all your answers in midterm.pdf and submit it.  
3. Each screenshot must include the name of the host from which it was taken.  
4. Troubleshooting is your responsibility.

---

### Problem Description

* **Required**: In AWS, set up a MongoDB cluster with **at least 3 nodes**, including a sharding system consisting of three shards, one mongos, and a replica set of three config servers. The config servers should be deployed in a replica set.  
* You may launch the replica set of config servers on nodes of your choice.  
* Each shard should be replicated in a replica set with three members. You may launch the members of a replica set on nodes of your choice.  
* Carefully read the following task description. I suggest you plan the entire process for completing the tasks before actual deployment and implementation.

### Tasks

For each task, describe the steps and procedures, and include screenshot(s) to demonstrate the task was completed. It is your responsibility to select suitable screenshots to show your work. You do not need to follow the exact order provided below. Ensure that your identity is visible in each screenshot, either in the form of the IP address of the Ubuntu container (e.g., a prompt like ubuntu@35.174.136.58 for terminal screenshots) or your AWS account information for screenshots taken in the AWS console.

1. (5 Points) Set up nodes in AWS. Determine the number of nodes based on your deployment plan. (There should be at least 3 nodes.)  
2. (5 points) Access these instances (nodes) through SSH  
3. (5 Points) Install MongoDB in each node (i.e. instance)  
4. (3 points) Create a directory to store database in each node  
5. (5 points) Specify Public and Private IP Addresses of AWS instances used in your solution.  
6. (6 points) Set up and launch three config servers in a replica set.  
7. (5 points) Connect mongos to each config server.  
8. (10 points) Set up and launch each of the three shards. Make sure to include the result of sh.status() before adding the shards, which will be done in the next task.  
9. (5 points) Add the shards and ensure you include the result of sh.status() after adding them.  
10. (5points) Enable the shards and explain the nature of the shard key (ascending, random, or location-based) as well as the sharding strategy (range-based or hash-based) used in your deployment.  
11. (10 points) Populate the cluster with data using a public dataset. Explain your collection and include the code to populate the data, along with the result of sh.status() after the data is populated. Specify the URL for the dataset. (Refer to Task 12 to choose an appropriate dataset for executing the given queries. You are allowed to clean and reduce the public dataset of your choice to populate a reasonable amount of data to be distributed across the shards. You may determine what constitutes a reasonable amount.) You are not permitted to use zips.json provided in the prior assignment. If the public dataset you choose is not large enough, you are allowed to synthesize additional data to scale it.  
12. (12 points) Generate the following queries for the populated data. For each query, show its execution time and also show which shard served the query.  
    1. A range query to find documents in a given range.  
    2. A query involving $elemMatch involving at least two conditions.  
    3. A query involving $in, $nin, or $all  
    4. A query involving aggregate()  
    5. A update  
    6. A delete  
13. (5 points) Show shards are replicated using rs.status().  
14. (5 points) List of hosts and describe what are deployed in each host. For example,  
    `node0:port#  config server PRIMARY`  
    `node1:port#  config server SECONDARY`

### Grading

| Criteria | Maximum Obtainable Scores |
| :---- | :---- |
| 14 Tasks | 86 points |
| The use of public data set | 4 points |
| Clarity and Organization of report | 10 points |
| Total | 100 points  |
