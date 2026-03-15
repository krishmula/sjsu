> Use the Tree type from the slides: data Tree k = Empty | Node (Tree k) (Tree k) k. 
> Write a function depth :: Tree k -> Int that returns 0 for Empty 
> and 1 + max (depth left) (depth right) for a Node. This one requires recursive case matching.
