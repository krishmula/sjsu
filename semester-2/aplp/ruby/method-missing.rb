class Person
    attr_accessor :name

    def initialize(name)
        @name = name
    end

    def method_missing(method)
        puts "Didn't understand #{method}"
    end
end

personGracie = Person.new("Gracie")

personGracie.foo