class Record
    def initialize(fields)
      @fields = fields        # a hash of field_name => value
    end
  
    def method_missing(m, *args)
        method_name = m.to_s
        if (method_name.end_with?("="))
            @fields[method_name.chop] = args[0]
        else
            @fields[method_name]
        end
    end

    # def method_missing(m, *args)
    #     method_name = m.to_s
    #     puts method_name
    # end
end
  
  r = Record.new({'fname' => 'Rick',
                  'lname' => 'Grimes',
                  'profession' => 'Police Officer'})
  
  puts r.profession          # "Police Officer"  — getter via method_missing
  r.profession = 'Zombie hunter'   # setter via method_missing
  puts r.profession          # "Zombie hunter"