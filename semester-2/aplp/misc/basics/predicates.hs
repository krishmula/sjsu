-- > We want a list comprehension, that when number is less than 10, we return "BOOM!" and when it's greater than 10, we return "BANG!". If it's exactly 10, we want to skip it and not return anything. Write this list comprehension.

listComp xs = [if x < 10 then "BOOM!" else "BANG!" | x <- xs, odd x]