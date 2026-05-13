class Employee
    attr_accessor :name, :salary, :ssid

    def initialize(name, salary, ssid)
        @name = name
        @salary = salary
        @ssid = ssid
        @employees = Hash.new
        Employee.add self
    end

    def to_s
        "#{@name} has a salary of #{@salary} and an SSID of #{@ssid}."
    end

    def print_employees
        @employees.each do |name, emp|
            puts emp.to_s
        end
    end
    
    class << self
        def add(emp)
            puts "Adding #{emp} to the database."
            @employees[emp.name] = emp
        end
    end
end

alice = Employee.new("Alice", 100000, 1234567890)
bob = Employee.new("Bob", 90000, 1234567891)
charles = Employee.new("Charles", 80000, 1234567892)

Employee.print_employees