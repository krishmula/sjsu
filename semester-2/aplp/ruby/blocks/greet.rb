# Way 1: using yield

def greet
    puts "Before"
    if block_given?
        yield
    end
    puts "After"
end

greet  { puts "This is the block"}


# Way 2: using &blk

def greet(&blk) 
    puts "Before"
    blk.call
    puts "After"
end

greet { puts "This is the block"}