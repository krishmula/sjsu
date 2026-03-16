mydiv :: Integer -> Integer -> Maybe Integer
mydiv d n = 
  case d of
    0 -> Nothing
    _ -> Just (div n d)

