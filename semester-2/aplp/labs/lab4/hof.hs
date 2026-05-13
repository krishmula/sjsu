stringToNum :: [String] -> [Integer]
stringToNum [] = []

stringToNum [x] = map read [x]
