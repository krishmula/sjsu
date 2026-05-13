getMax :: [Int] -> Maybe Int
getMax [] = Nothing
getMax [x] = Just (x)
getMax (x:xs) = 
   case getMax xs of
      Nothing -> Just x
      Just m -> Just (if x > m then x else m)



reciprocal :: (Eq a, Fractional a) => a -> Maybe a
reciprocal 0 = Nothing
reciprocal x = Just (1 / x)



rectangleArea :: Int -> Int -> Either String Int
rectangleArea x y 
   | x < 0 = Left "Width is not positive"
   | y < 0 = Left "Height is not positive"
   | otherwise = Right (x * y)



main :: IO ()
main = do
  print $ getMax []
  print $ getMax [99,12,37]
  print $ getMax [-99,-12,-37]
  print $ reciprocal 4
  print $ reciprocal 2
  print $ reciprocal 0
  print $ rectangleArea 5 10
  print $ rectangleArea (-5) 10
  print $ rectangleArea 5 (-10)
