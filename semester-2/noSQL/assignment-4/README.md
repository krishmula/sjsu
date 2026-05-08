Assignment 4 (Cassandra)
Due Apr 30 by 11:59pm Points 100 Submitting a file upload Available Apr 13 at 12am - May 2 at 11:59pm
CS157C:NoSQL Database Systems
Programming Assignment 4
Instructor: Suneuy Kim
Cassandra set up in a single node (docker container) in your local machine
On the prompt of your laptop, run the following commands.

docker run -itd --name cnode cassandra
docker ps -a
docker exec -it cnode bash

On the prompt of the docker container, type cqlsh to access cqlsh.

root@8bfca549e5ab:/# cqlsh
Connected to Test Cluster at 127.0.0.1:9042.
[cqlsh 5.0.1 | Cassandra 3.11.6 | CQL spec 3.4.4 | Native protocol v4]
Use HELP for help.
cqlsh>
cqlsh>
In this assignment, we will exercise creating a table, populating the table through data wrangling, and manipulate data using CQL commands.

Write your report in hw4.pdf and submit it.

Create the following Cassandra table with a primary key consisting of state, city, and zip. The state serves as a partition key and the city and zip together serve as a clustering key.
create table citylist
(
city varchar,
loc List <float>,
pop int,
state varchar,
zip varchar,
primary key (state,city,zip)
) ;

Get a JSON data from http://media.mongodb.org/zips.json. You may use the wget command.
Populate the table with this data. Notice that the format of the given JSON data does not follow the given schema. Therefore, you need to wrangle the given data to populate the given table. This process may involves converting JSON data to CSV and cleaning and reorganizing them using regular expressions. It is a good practice to work out your logic with a small datasets before moving to large data set. Briefly describe how you wrangle the data.
Get the screenshot of the first 15 of the table content.
Write a CQL command that gets all rows where city is 'NEW YORK'. The CQL command should not come with the state = 'NY' predicate so that the query itself doesn't work without using one of the following methods.
Method 1: Writing a query with a predicate city = 'NEW YORK' only with ALLOW FILTERING
Method 2: Writing a query with a prediate city = 'NEW YORK' only with a secondary index built on the city column
Method 3: Using a materialized view of which partition key is city (You need to define a materialized view and select \* from it.)
For each method, show the first 15 results and show the execution time of the query. (It is your job to find the way to measure the execution time of a CQL query.)
Analyze the execution time and describe your rationale behind of your analysis.
Write a CQL command that gets all the zip codes of the NEW YORK city with a population greater than 20,000 and less than 30,000. Answer which method(s) of the previous question work(s) for this query? Also, show the first 15 results of the query.
Define another schema for citylist2 table in a way that data can be queried in the ascending order of city and descending order of zip code
Populate citylist2 with the given JSON data and get the table content.
Get all the zip codes of the state California. Show the first 15 results.
Get all the zip codes of the city San Jose of which population is greater than 5,000 and less than 10,000. Show the first 15 results.
Your report should be organized as shown below.

3. Describe your data wrangling method.
4. Screenshot of results
5. Method 1: Allow Filtering
   A CQL query to get the result
   Result
   Execution Time
   Method 2: Secondary Index
   CQL command to create an index and a CQL query to get the result.
   Result
   Execution Time
   Method 3: Materialized View
   CQL command to create a materialized view and a CQL query to get the result  
    Result
   Execution Time

Your analysis goes here.

6. Which method(s) work for this question?
   CQL query to get the result
   Result
7. Schema declaration
8. Show the table content.
9. CQL query to get the result
   Result
10. CQL query to get the result
    Result
