/**
 * call, apply, bind — how they differ
 *
 * `this` is not like a closure variable: it is chosen when the function RUNS,
 * based on *how* it is invoked (unless arrow functions, which ignore `this`).
 * call / apply / bind let you supply `this` explicitly.
 */

function greet(greeting, punctuation) {
  return `${greeting}, I'm ${this.name}${punctuation}`;
}

const alice = { name: "Alice" };
const bob = { name: "Bob" };

// --- call: run now. `this` = first argument; rest are normal args one-by-one ---
console.log("call:", greet.call(alice, "Hi", "!"));
// → Hi, I'm Alice!

// --- apply: run now. `this` = first argument; second is an ARRAY of args ---
console.log("apply:", greet.apply(bob, ["Hello", "?"]));
// → Hello, I'm Bob?

// --- bind: do NOT run yet. Returns a new function with `this` (and optional args) fixed ---
const greetAsAlice = greet.bind(alice);
console.log("bind (later):", greetAsAlice("Hey", "."));
// → Hey, I'm Alice.

// Partial application: bind `this` AND the first argument
const sayHiToBob = greet.bind(bob, "Hi");
console.log("bind (partial):", sayHiToBob("!!"));
// → Hi, I'm Bob!!

// Contrast: plain call — `this` is undefined in strict mode (or global in sloppy mode)
function showThis() {
  "use strict";
  console.log("plain call this:", this);
}
showThis(); // undefined
