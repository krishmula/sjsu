{-
  Name: Krishna Mula
  Class: CS 252
  Assigment: HW1
  Date: 16 February 2026
  Description: BigNum implementation
-}

module BigNum (
  BigNum,
  bigAdd,
  bigSubtract,
  bigMultiply,
  bigEq,
  bigDec,
  bigPowerOf,
  prettyPrint,
  stringToBigNum,
) where

type Block = Int -- An Int from 0-999

type BigNum = [Block]

maxblock = 1000

bigAdd :: BigNum -> BigNum -> BigNum
bigAdd x y = bigAdd' x y 0

bigAdd' :: BigNum -> BigNum -> Block -> BigNum
bigAdd' [] [] 0 = []
bigAdd' [] [] carry = [carry]
bigAdd' (x:xs) [] carry = 
  let sum = x + carry
      block = sum  `mod` 1000
      nextCarry = sum `div` 1000
  in block : bigAdd' xs [] nextCarry
bigAdd' [] (y:ys) carry = 
  let sum = y + carry
      block = sum  `mod` 1000
      nextCarry = sum `div` 1000
  in block : bigAdd' [] ys nextCarry
bigAdd' (x:xs) (y:ys) carry = 
  let sum = x + y + carry
      block = sum `mod` 1000
      nextCarry = sum `div` 1000
  in block : bigAdd' xs ys nextCarry


bigSubtract :: BigNum -> BigNum -> BigNum
bigSubtract x y =
  if length x < length y
    then error "Negative numbers not supported"
    else reverse $ stripLeadingZeroes $ reverse result
      where result = bigSubtract' x y 0

stripLeadingZeroes :: BigNum -> BigNum
stripLeadingZeroes (0:[]) = [0]
stripLeadingZeroes (0:xs) = stripLeadingZeroes xs
stripLeadingZeroes xs = xs

-- Negative numbers are not supported, so you may throw an error in these cases
bigSubtract' :: BigNum -> BigNum -> Block -> BigNum
bigSubtract' [] [] 0 = []
bigSubtract' (x:xs) [] borrow = 
  let diff = x - borrow
      block = if diff < 0 then diff + 1000 else diff
      nextBorrow = if diff < 0 then 1 else 0
  in block : bigSubtract' xs [] nextBorrow
bigSubtract' [] (y:ys) borrow = error "Negative numbers are not supported right now"
bigSubtract' (x:xs) (y:ys) borrow = 
  let diff = x - y - borrow
      block = if diff < 0 then diff + 1000 else diff
      nextBorrow = if diff < 0 then 1 else 0
  in block : bigSubtract' xs ys nextBorrow

bigEq :: BigNum -> BigNum -> Bool
bigEq [] [] = True
bigEq [] _ = False
bigEq _ [] = False
bigEq (x:xs) (y:ys) = x == y && bigEq xs ys

bigDec :: BigNum -> BigNum
bigDec x = bigSubtract x [1]

-- Handle multiplication following the same approach you learned in grade
-- school, except dealing with blocks of 3 digits rather than single digits.
-- If you are having trouble finding a solution, write a helper method that
-- multiplies a BigNum by an Int.
bigMultiply :: BigNum -> BigNum -> BigNum
bigMultiply xs y = foldr (\x acc -> bigAdd (multiplyEach y x 0) (0 : acc)) [] xs

multiplyEach :: BigNum -> Block -> Block -> BigNum
multiplyEach [] _ 0 = []
multiplyEach [] _ carry = [carry]
multiplyEach (e:es) x carry = (prod `mod` maxblock) : multiplyEach es x (prod `div` maxblock)
  where prod = e * x + carry

bigPowerOf :: BigNum -> BigNum -> BigNum
bigPowerOf _ [0] = [1]
bigPowerOf x exp = bigMultiply x (bigPowerOf x (bigDec exp))

prettyPrint :: BigNum -> String
prettyPrint [] = ""
prettyPrint xs = show first ++ prettyPrint' rest
  where (first:rest) = reverse xs

prettyPrint' :: BigNum -> String
prettyPrint' [] = ""
prettyPrint' (x:xs) = prettyPrintBlock x ++ prettyPrint' xs

prettyPrintBlock :: Int -> String
prettyPrintBlock x | x < 10     = ",00" ++ show x
                   | x < 100    = ",0" ++ show x
                   | otherwise  = "," ++ show x

stringToBigNum :: String -> BigNum
stringToBigNum "0" = [0]
stringToBigNum s = stringToBigNum' $ reverse s

stringToBigNum' :: String -> BigNum
stringToBigNum' [] = []
stringToBigNum' s | length s <= 3 = read (reverse s) : []
stringToBigNum' (a:b:c:rest) = block : stringToBigNum' rest
  where block = read $ c:b:a:[]

sig = "9102llaf"

