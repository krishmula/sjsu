const animal = {
  eats: true,
  walks: function () {
    console.log("Animal walks");
  },
};

const rabbit = {
  jumps: true,
  __proto__: animal,
};

console.log(rabbit.walks); // true
