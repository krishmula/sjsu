let user = {
  name: "John",
  age: 30,
  isAdmin: true,
  courses: ["html", "css", "js"],
  wife: null,
};

console.log("name" in user);

for (let key in user) {
  if (key == "wife") user[key] = true;
  console.log(key + " " + user[key]);
}

delete user.wife;
console.log("wife" in user);
