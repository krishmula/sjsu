> import Data.List

Experiment with foldl, foldr, and foldl'

First, implement your own version of the foldl function,
defined as myFoldl

> myFoldl :: (a -> b -> a) -> a -> [b] -> a
> myFoldl func z [] = z
> myFoldl func z (x:xs) = myFoldl func (func z x) xs

func z x (z)

Next, define a function to reverse a list using foldl.

> myReverse :: [a] -> [a]
> myReverse xs = foldl (\z x -> x : z) [] xs


Now define your own version of foldr, named myFoldr

> myFoldr :: (a -> b -> b) -> b -> [a] -> b
> myFoldr func z [] = z
> myFoldr func z (x: xs) = func x (myFoldr func z xs)


Now try using foldl (the library version, not yours) to sum up the numbers of a large list.
Why is it so slow?

> summation :: [Integer] -> Integer
> summation xs = foldl (+) 0 xs

Because foldl is thunking up the result on each step, and the thunks start off small, and then
accumulate to become a really large thunk, and thus, the bottleneck.

Instead of foldl, try using foldl'.
Why is it faster?
(Read http://www.haskell.org/haskellwiki/Foldr_Foldl_Foldl%27 for some hints)

> import Data.List
> summation :: [Integer] -> Integer
> summation xs = foldl' (+) 0 xs

foldl' doesn't doesn't thunk at each step, and instead actually finishes calculating, instead of
doing it lazily. So, no bottleneck.


For an extra challenge, try to implement foldl in terms of foldr.
See http://www.haskell.org/haskellwiki/Foldl_as_foldr for details.


Next, using the map function, convert every item in a list to its absolute value

> listAbs :: [Integer] -> [Integer]
> listAbs xs = map abs xs

Finally, write a function that takes a list of Integers and returns the sum of
their absolute values.

> sumAbs :: [Integer] -> Integer
> sumAbs xs = foldl (\z x -> z + abs x) 0 xs


