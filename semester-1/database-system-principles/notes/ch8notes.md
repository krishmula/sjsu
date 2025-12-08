# CH-8: Views and Indexes

## Views

- These are just like tables (relations), but they aren't actually stored on disk. Instead, they are queries that have been given a name. When these are used in queries, the query engine executes the query related to the view while executing the entire query.
- For example, say, we have a view ParamountMovies, which is a SELECT statement from the Movies table to only include movies produced by Paramount Pictures. When I use ParamountMovies in an actual query (SELECT title, year from ParamountMovies), the query engine runs the query for ParamountMovies when this query is executed.

### Querying Views

- Like in the last example, we can query from Views too, just like we do from Tables/Relations.

### Renaming Attributes in Views

- Sometimes, we can change the names of the attributes/columns in the Views to names of our liking, for readability, conciseness, etc.
- So, we could write: CREATE VIEW MovieProd(movieTitle, prodName) AS SELECT title, name FROM Movies, MoviesExec WHERE producerC# = certC#;
- In the above example, we rename title, and name from the original tables, as movieTitle, and prodName.

## Modifying Views

- Some very specific views can be updated. These are called **updateable views**. They can propagate changes to the base table.
- The condition for updateable views is that the rows of the view should map to the rows of the base table.
- Views can be dropped using the DROP VIEW command.
- There are some constraints when you write some updateable queries on **updateable views**.

## Instead-Of Triggers on Views

- Triggers in SQL are actions that are executed when a certain event occurs. It is logic that fires _before_ or _after_ an event without manual invocation.
- We can use INSTEAD OF in place of BEFORE or AFTER. So, when the event occurs, instead of the action occurring, the INSTEAD OF action is the only thing that is executed.
- It's in the name: INSTEAD OF, not BEFORE or AFTER.

## Indexes in SQL

-
