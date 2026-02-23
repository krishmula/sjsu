divide :: Int -> Int -> Maybe Int
divide x 0 = Nothing
divide x y = Just x `div` y
