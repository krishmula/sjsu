#! /usr/bin/ruby -w

#Represents a Rogerian psychiatrist
class Shrink

  #initializes 'memory' of Eliza.
  def initialize()
    @he="he"
    @she="she"
    @they="they"
  end

  #read a statement and convert it to a psychiatric response.
  def generateResponse(line)
    blather = line.strip
    # Filter leading discourse markers (Well, Perhaps, etc.)
    blather.gsub!(/^(well|perhaps|oh|so|look)[,.\s]+/i, '')
    blather = blather.downcase

    # Human touch: acknowledge greetings warmly before clinical transforms
    if blather =~ /^(hi|hello|hey)[\s,!\.]*$/
      return "HELLO. PLEASE TELL ME WHAT BROUGHT YOU HERE TODAY."
    end

    # Absolute language — ask for specificity (before pronoun swaps)
    if blather =~ /\b(always|never|constantly|every time|nothing ever)\b/
      return "CAN YOU BE MORE SPECIFIC?"
    end

    # "Are you …?" and close variants — before you/I substitution
    if (m = blather.match(/^\s*are you\s+(.+?)\??\s*$/))
      rest = m[1].strip
      return "IS IT IMPORTANT IF I AM #{rest.upcase}?"
    end
    if (m = blather.match(/^\s*do you think you are\s+(.+?)\??\s*$/))
      rest = m[1].strip
      return "IS IT IMPORTANT IF I AM #{rest.upcase}?"
    end
    if (m = blather.match(/^\s*could you be\s+(.+?)\??\s*$/))
      rest = m[1].strip
      return "IS IT IMPORTANT IF I AM #{rest.upcase}?"
    end
    if (m = blather.match(/^\s*aren'?t\s+you\s+(.+?)\??\s*$/))
      rest = m[1].strip
      return "IS IT IMPORTANT IF I AM #{rest.upcase}?"
    end

    #change 'you', 'your', etc. to uppercase 'I', 'MY'
    blather.gsub!(/\byour\b/,"MY")
    blather.gsub!(/\byou\b/,'I')

    #Replace 'my' with 'your', 'me' with 'you', 'I' with 'you', etc.
    blather.gsub!(/\bmy\b/,"your")
    blather.gsub!(/\bme\b/,"you")
    # Avoid "you am" / "you'm" / "you was" after plain I -> you
    blather.gsub!(/\bi am\b/, 'you are')
    blather.gsub!(/\bi'm\b/, "you're")
    blather.gsub!(/\bi was\b/, 'you were')
    blather.gsub!(/\bi've\b/, "you've")
    blather.gsub!(/\bi'll\b/, "you'll")
    blather.gsub!(/\bi\b/,'you')

    #Sub in past references, but only for the 1st occurrence or it looks weird
    blather.sub!(/\b(he|him)\b/, @he)
    blather.sub!(/\b(she|her)\b/, @she)
    blather.sub!(/\b(they|them)\b/, @they)

    #Get future references -- note that these do NOT change the immediate output
    hePat=/.*\b(your (father|brother|(ex-?)?(husband|boyfriend)))\b.*/
    shePat = /.*\b(your (mother|sister|(ex-?)?(wife|girlfriend)))\b.*/
    theyPat = /.*\b(your (friends|parents|siblings|children|kids|coworkers))\b.*/
    @he=blather.sub(hePat, '\1').chomp if blather =~ hePat 
    @she=blather.sub(shePat, '\1').chomp if blather =~ shePat
    @they=blather.sub(theyPat, '\1').chomp if blather =~ theyPat

    #Deal with name
    namePat=/.*\byour name is (\w+).*/
    @name=blather.sub(namePat,'\1')
    blather.sub!(namePat,'nice to meet you, \1.  How can I help you')

    #results are uppercased, for aesthetics.
    return blather.upcase + "?"
  end
end


#main -- reads from standard input unless -test is the first parameter.
if __FILE__ == $0
  eliza = Shrink.new()
  if ARGV[0] == "-test"
    [
      'My girlfriend never listens to me',
      "I think she might be deaf",
      "yes",
      "I am afraid of clowns",
      "Well, they just seem creepy",
      "Also, when I was a kid, a clown killed my dad",
      "Are you a clown in disguise?",
    ].each do |stmt|
      puts stmt
      puts eliza.generateResponse(stmt)
    end
  else
    while line = gets
      response = eliza.generateResponse line
      puts response
    end
  end
end
