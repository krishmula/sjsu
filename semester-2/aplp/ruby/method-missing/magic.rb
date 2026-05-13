class Magic
    def initialize
      @data = {}
    end
  
    # Use method_missing to support dynamic setters and getters.
    # If the method name ends with "!", set @data[key] = true.
    # If the method name ends with "?", return @data[key].
    # Otherwise, store the first argument in @data[key].
    #
    # Examples:
    # m = Magic.new
    # m.hungry("very")   # sets @data["hungry"] = "very"
    # m.hungry?          # returns "very"
    # m.happy!           # sets @data["happy"] = true
    # m.happy?           # returns true

    def get(key)
        return @data[key]
    end
  
    def method_missing(m, *args)
        method_name = m.to_s
        method_name_length = method_name.length
        if (method_name.end_with?("!"))
            new_method_name = method_name.slice(0, method_name_length - 1)
            return @data[new_method_name] = true
        elsif (method_name.end_with?("?"))
            new_method_name = method_name.slice(0, method_name_length - 1)
            return @data[new_method_name]
        else
            return @data[method_name] = args[0]
        end
    end
  end

  m = Magic.new
  m.hungry("very")
  puts m.get("hungry")

  puts m.hungry?

  puts m.happy!