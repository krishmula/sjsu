> data Tree v =
>     Empty
>   | Node v (Tree v) (Tree v)
>   deriving (Show)

The findT method shows how we may search through the tree to find a value.

> findT :: Ord v => v -> Tree v -> Maybe v
> findT _ Empty = Nothing
> findT v (Node val left right) =
>   if val == v then
>     Just val
>   else if v < val then
>     findT v left
>   else
>     findT v right

Your job is to add support for fmap to this tree, so that the call to fmap below works:

> instance Functor Tree where
>   fmap f Empty = Empty
>   fmap f (Node v l r) = Node (f v) (fmap f l) (fmap f r)

> main = print $ fmap (+1) (Node 3 (Node 1 Empty Empty) (Node 7 (Node 4 Empty Empty) Empty))