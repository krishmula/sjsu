prodList :: [Integer] -> Integer
prodList [] = 1
prodList (x: xs) = x * prodList xs
