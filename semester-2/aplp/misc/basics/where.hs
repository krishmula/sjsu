density :: (RealFloat a) => a -> a -> String
density mass volume
    | d > 1 = "You're light baby!"
    | d == 1 = "You're good!"
    | otherwise = "u ded"
    where d = mass / volume