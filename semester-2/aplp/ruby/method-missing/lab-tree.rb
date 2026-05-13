class Tree
    attr_accessor :value, :left, :right
    def initialize(value, left=nil, right=nil)
      @value = value
      @left = left
      @right = right
    end

    def each_node(&blk)
        blk.call(@value)
        @left.each_node(&blk) if @left
        @right.each_node(&blk) if @right
    end

    def method_missing(name, *args)
        # based on the name of the method, we need to traverse the tree to find the value
        name_string = name.to_s
        splits = name_string.split("_")
        node = self

        for split in splits
            if split == "left"
                node = node.left
            elsif split == "right"
                node = node.right
            else
                return nil
            end
        end

        return node.value
    end
  end
  
  my_tree = Tree.new(42,
                     Tree.new(3,
                              Tree.new(1,
                                       Tree.new(7,
                                                Tree.new(22),
                                                Tree.new(123)),
                                       Tree.new(32))),
                     Tree.new(99,
                              Tree.new(81)))
  
  my_tree.each_node do |v|
    puts v
  end
  
  arr = []
  my_tree.each_node do |v|
    arr.push v
  end
  p arr
  
  p "Getting nodes from tree"
  p my_tree.left_left
  p my_tree.right_left
  p my_tree.left_left_right
  p my_tree.left_left_left_right


#   Today's lab will explore using both method_missing and blocks.

#   Download tree.rb from the course website.  Add an each_node method
#   that takes a block and applies it to every node in the tree (in
#   whatever order you desire).  For this to work correctly, you will
#   most likely need to explicitly name the block.
  
#   Once you have that working, use method_missing to add support for
#   fetching the value on a given path through an arbitrary method name.
#   Some examples:
  
#   my_tree.left_left
  
#   my_tree.left_left_right
  
#   my_tree.right_left_right
  
#   my_tree.left_right_left_left_right_left_left_right_left_left_right
  
  
#   The scan method from the String class may be useful.