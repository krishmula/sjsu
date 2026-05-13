# Calls the block n times, passing the current count (starting at 1) each time.
def repeat(n, &blk)
    for i in 1..n
        blk.call(i)
    end
end

repeat(4) { |i| puts i * 10 }
# What does this print?