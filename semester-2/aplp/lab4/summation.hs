import Data.List

summation :: [Integer] -> Integer
summation xs = foldl' (+) 0 xs
