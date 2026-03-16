makeListOfAdders :: [Integer] -> [Integer -> Integer]
makeListOfAdders [] = []
makeListOfAdders (x:xs) = (\y -> y + x) : makeListOfAdders xs