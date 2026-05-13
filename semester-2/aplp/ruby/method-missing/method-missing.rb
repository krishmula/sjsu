class Person
    attr_accessor :name

    def initialize(name)
        @name = name
    end
end


alice = Person.new("Alice")
alice.make_introduction
