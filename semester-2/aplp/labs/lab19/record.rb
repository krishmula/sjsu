=begin
Ruby provides a number of hooks that allow a developer
to change the behavior of objects.
The design is similar to JavaScript's Proxies (both are
metaobject protocols), but unlike proxies,
these hooks apply for all objects.

The most famous of these is method_missing,
based on Smalltalk's doesNotUnderstand:.
=end

# Consider Ruby on Rails.
# With Rails you refer to a record's fields by their names.
# We will (crudely) simulate that.
class Record
    def initialize fields
      @fields = fields
    end
  
    def method_missing m, *args
      meth_name = m.to_s
      if (meth_name.end_with?("=")) then
        @fields[meth_name.chop] = args[0]
      else
        @fields[meth_name]
      end
    end
  end
  
  r = Record.new ({ 'fname' => 'Rick', 'lname' => 'Grimes', 'profession' => 'Police Officer' })
  puts r.profession
  r.profession = 'Zombie hunter'
  puts r.profession
  
  # Ruby has const_missing as well, which works in a similar manner, except that it applies
  # to missing class constants
  