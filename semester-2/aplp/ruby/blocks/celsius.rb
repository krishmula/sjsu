def conversion_chart(from_units, to_units, values)
    puts "#{from_units}\t#{to_units}"
    left_line = right_line = ""
    from_units.length.times { left_line += '-' }
    to_units.length.times { right_line += '-' }
    puts "#{left_line}\t#{right_line}"
    for val in values
      converted = yield val
      puts "#{val}\t#{converted}"
    end
    puts
  end


celsius_temps = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]

conversion_chart("C", "F", celsius_temps) do |cel|
  cel * 9 / 5 + 32
end