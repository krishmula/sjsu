-- Practising Maybe/Either

-- Write a function that returns the predecessor of a number, but returns Nothing if the number is 0.
-- safePred 5 → Just 4
-- safePred 1 → Just 0
-- safePred 0 → Nothing

pred :: Integer -> Maybe Integer
pred 0 = Nothing
pred x = Just (x - 1)

-- Write a function that extracts the value from Maybe, but returns 0 if it is Nothing.
-- fromMaybeInt (Just 5) → 5
--fromMaybeInt Nothing → 0

fromMaybeInt :: Maybe Integer -> Integer
fromMaybeInt $ Just x =  x
fromMaybeInt Nothing = 0

-- Add two Maybe Int values.
-- Rules
-- Just x + Just y → Just (x+y)
-- Anything with Nothing → Nothing

addMaybe :: Maybe Integer -> Maybe Integer -> Maybe Integer
addMaybe (Just x) (Just y) = (Just (x + y))
addMaybe (Just x) Nothing = Nothing
addMaybe Nothing (Just y) = Nothing

-- Write a safe division function.
-- safeDiv 10 2 → Just 5
-- safeDiv 10 0 → Nothing

safeDiv :: Integer -> Integer -> Maybe Integer
safeDiv x 0 = Nothing
safeDiv x y = Just (x `div` y)

-- Write a function that applies a function to a Maybe value.
-- applyMaybe (+1) (Just 4) → Just 5
-- applyMaybe (+1) Nothing → Nothing

applyMaybe :: (a -> b) -> Maybe a -> Maybe b
applyMaybe f Nothing = Nothing
applyMaybe f (Just x) = Just (f x)