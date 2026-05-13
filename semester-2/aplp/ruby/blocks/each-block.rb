class LinkedList
    attr_accessor :val, :next_node
    def initialize(val, next_node=nil)
        @val = val
        @next_node = next_node
    end

    # accepts a block of code (&blk), applies it to the current value, and recursively calls each_node on the next (non-nil) item in the list.

    def each_node(&blk)
        blk.call(@val)
        @next_node.each_node(&blk) if @next_node
    end
end

# Converts an array to a linkedlist maintaining the order of the original array.

def make_list lst
    return nil if lst.empty?
    head = LinkedList.new(lst[0])
    current = head

   for i in 1..lst.length - 1
    current.next_node = LinkedList.new(lst[i])
    current = current.next_node if current.next_node
   end

   return head
end


ll = make_list [1, 2, 3, 4]
sum = 0
ll.each_node do |v|
    sum += v
end
puts sum #Prints 10

product = 1
ll.each_node do |v|
    product *= v
end
puts product #Prints 24