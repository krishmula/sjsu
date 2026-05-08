class Employee
    attr_accessor :name, :salary, :ssid

    def initialize(name, salary, ssid)
        @name = name
        @salary = salary
        @ssid = ssid
        Employee.add self
    end

    def to_s
        "#{@name} has a salary of #{@salary} and an SSID of #{@ssid}."
    end

    class << self
        def add(emp)
            puts "Adding #{emp} to the database."
            @employees = Hash.new unless @employees
            @employees[emp.name] = emp
        end

        def get_emp_by_name(name)
            @employees[name]
        end
    end
end


