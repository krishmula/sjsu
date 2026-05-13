class Person
    attr_accessor :name, :age, :gender

    def initialize(name, age, gender)
        @name = name
        @age = age
        @gender = gender
    end

    def to_s
        "#{@name} is a #{@age}-year-old #{@gender}."
    end
end

personJohn = Person.new("John", 30, "male")
personJane = Person.new("Jane", 25, "female")

grace = Person.new("Grace", 28, "female")


class << personJohn
    def signature
        puts "John's signature"
    end
end

class << grace
    def say_goodnight
        puts "Goodnight, Grace"
    end
end


personJohn.signature
grace.say_goodnight
personJohn.say_goodnight