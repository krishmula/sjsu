class Class
    def my_attr_accessor(*args)
        args.each do |props|
            self.class_eval ("def #{props}; @#{props}; end")
            self.class_eval ("def #{props}=(v); @#{props} = v; end")
        end
    end
end

class Musician
    my_attr_accessor :name, :instrument
end


m = Musician.new()
m.name = "John"
puts m.name