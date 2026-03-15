addOne :: [Int] -> [Int]
addOne [] = []
addOne x = map (+1) x

getSquares :: [Int] -> [Int]
getSquares [] = []
getSquares x = map (\x -> x * x) x