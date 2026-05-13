let constants = { pi: 3.14 };

let math = new Proxy(constants, {
  set() {
    return false;
  },
  deleteProperty() {
    return false;
  },
});

math.pi = 3;
delete math.pi;

// constants.pi = 4.14;
console.log(math.pi);
