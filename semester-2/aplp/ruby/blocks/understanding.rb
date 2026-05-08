def with_prob(prob)
    yield if (prob > 0.2)
  end
  
with_prob 0.42 do
  puts "It worked!"
end

with_prob 0.1 do
  if eval("0.1 > 0.2")
    puts "It didn't work!"
  else
    puts "It worked!"
  end
end
  