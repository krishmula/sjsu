class Tree
    attr_accessor :value, :left, :right
    def initialize(value, left=nil, right=nil)
      @value = value
      @left = left
      @right = right
    end

    def each_node(&block)
      block.call(@value)
      @left.each_node(&block) if @left
      @right.each_node(&block) if @right
    end

    def method_missing(name, *args)
      s = name.to_s
      return super unless s.match?(/\A(?:left|right)(?:_(?:left|right))*\z/)

      node = self
      s.scan(/left|right/).each do |dir|
        node = (dir == 'left') ? node.left : node.right
        return nil if node.nil?
      end
      node.value
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
  