class TreeNode
  attr_accessor :val, :left, :right
  def initialize(val, left=nil, right=nil)
    @val = val
    @left = left
    @right = right
  end

  # Applies the block to the current value, then
  # recursively calls each_node on left (if non-nil),
  # then recursively calls each_node on right (if non-nil).
  def each_node &blk
    blk.call(@val)
    @left.each_node(&blk) if @left
    @right.each_node(&blk) if @right
  end
end

t = TreeNode.new(1,
      TreeNode.new(2),
      TreeNode.new(3))

sum = 0
t.each_node { |v| sum += v }
puts sum   # What does this print?