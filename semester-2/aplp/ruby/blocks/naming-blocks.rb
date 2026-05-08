def with_prob(prob, &blk)
    blk.call if (prob > 0.2)
end

def half_the_time(&block)
    with_prob(0.5, &block)
end

half_the_time do
    puts "Half the time, bitches!"
end