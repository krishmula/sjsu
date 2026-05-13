# x = 10
# [1, 2, 3].each do |i|
#     puts i + x
# end


y = 27

def apply(x)
    yield(x * 2)
end

apply(5) do |n|
    puts n + y
end

# --- More closure examples (uncomment to run) ---

# 1) The block mutates an outer local — same binding as outside the block.
sum = 0
[1, 2, 3, 4].each { |n| sum += n }
puts sum   # => 10

# 2) "Configuration" the method never passes — only `word` is the block param.
min_len = 4
p ["hi", "hello", "bye"].select { |word| word.length >= min_len }  # => ["hello"]

# 3) After the loop, the block you kept still sees the final `count`
# (classic "why closures matter" when storing callbacks).
callbacks = []
count = 0
3.times do
  callbacks << proc { count += 1; puts "called, count is #{count}" }
end
callbacks[0].call  # count becomes 1
callbacks[0].call  # same proc, count becomes 2