with open("hw4.md", "r") as f:
    text = f.read()

text = text.replace("**Result:**\n```text\n zip\n-------\n 95139", "**Result:**\n\n```text\n zip\n-------\n 95139")

with open("hw4.md", "w") as f:
    f.write(text)
