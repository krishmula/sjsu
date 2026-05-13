def apply(x) 
    yield(x * 2)
end

apply(5) do |n| 
    puts n * 2
end


# passing arguments to the block
