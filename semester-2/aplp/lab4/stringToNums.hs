stringToNums :: [String] -> [Integer]
stringToNums [] = []
stringToNums x = map read x
