class Calculator
    attr_accessor :value

    def initialize
        @value
    end
    # Use method_missing to support calls like:
    # c.add_5        → returns 5
    # c.add_5_3      → returns 8
    # c.add_10_20_3  → returns 33
    # i.e. split the method name on "_",
    # drop the "add", convert the rest to integers, sum them.
    #
    # HINT: "add_5_3_2".scan(/\d+/) might be useful.
  
    def method_missing(m, *args)
        method_name = m.to_s

        splits = method_name.split("_")
        sum = 0
        for split in splits
            sum += split.to_i
        end
        return sum
    end
  end
  
  c = Calculator.new
  puts c.add_5
  puts c.add_5_3
  puts c.add_10_20_3