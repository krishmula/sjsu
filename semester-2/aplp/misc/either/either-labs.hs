-- Practising Either

-- Safe division with error message
-- safeDiv 10 2 → Right 5
-- safeDiv 10 0 → Left "division by zero"

safeDiv :: Integer -> Integer -> Either String Integer
safeDiv x 0 = Left "Shouldn't divide by 0"
safeDiv x y = Right (x `div` y)

-- Extract result or default
-- eitherToInt (Right 7) → 7
-- eitherToInt (Left "error") → 0

eitherToInt :: Either String Integer -> Integer
eitherToInt (Right x) = x
eitherToInt (Left x) = 0

-- Add two Either values
-- Right x + Right y → Right (x+y)
-- Any Left → propagate error

addEither :: Either String Integer -> Either String Integer -> Either String Integer
addEither (Right x) (Right y) = Right (x + y)
addEither (Left x) _ = Left x
addEither _ (Left y) = Left y

-- Map function over Either
-- Apply a function only if it is Right.
-- mapEither (+1) (Right 4) → Right 5
-- mapEither (+1) (Left "err") → Left "err"

mapEither :: (Integer -> Integer) -> Either String Integer -> Either String Integer
mapEither f (Right x) = Right (f x)
mapEither f _ = Left "err"

-- Convert Maybe → Either
-- Convert a Maybe value to Either.
-- maybeToEither "error" (Just 5) → Right 5
-- maybeToEither "error" Nothing → Left "error"

maybeToEither :: String -> Maybe Integer -> Either String Integer
maybeToEither x (Just y) = Right y
maybeToEither x Nothing = Left x