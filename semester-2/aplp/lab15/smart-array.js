"use strict";

// Matches paterns like '3-10'
const RANGE_PAT = /^(\d+)-(\d+)$/;

// Matches negative index values
const FROM_END_PAT = /^-(\d+)$/;

const NUM_PAT = /^-?\d+$/;

function SmartArray(...args) {
  return new Proxy(args, {
    get: function(target, prop) {
      if (typeof prop === "string" && prop.match(RANGE_PAT)) {
        // Return a subarray of the elements in the specified range,
        // INCLUDING the specified end index.
        let start = parseInt(prop.replace(RANGE_PAT, "$1"));
        let end = parseInt(prop.replace(RANGE_PAT, "$2")) + 1;
        return target.slice(start, end);
      } else if (typeof prop === "string" && prop.match(FROM_END_PAT)) {
        let k = parseInt(prop.replace(FROM_END_PAT, "$1"), 10);
        let idx = target.length - k;
        if (idx < 0) {
          throw new RangeError("Index out of range: " + prop);
        }
        return target[idx];
      } else if (typeof prop === "number" && !Number.isInteger(prop)) {
        throw new TypeError("Index must be an integer");
      } else {
        // Do the usual array thing -- get the value at the specified index.
        return Reflect.get(...arguments);
      }
    },
    set: function(target, prop, newVal) {
      if (typeof prop === "string" && prop.match(RANGE_PAT)) {
        throw new TypeError("Cannot assign to a range index");
      }
      if (typeof prop === "string" && prop.match(FROM_END_PAT)) {
        let k = parseInt(prop.replace(FROM_END_PAT, "$1"), 10);
        let idx = target.length - k;
        if (idx < 0) {
          throw new RangeError("Index out of range: " + prop);
        }
        return Reflect.set(target, String(idx), newVal);
      }
      if (typeof prop === "number") {
        if (!Number.isInteger(prop)) {
          throw new TypeError("Property name must be an integer");
        }
        let idx = prop < 0 ? target.length + prop : prop;
        if (idx < 0) {
          throw new RangeError("Index out of range");
        }
        return Reflect.set(target, String(idx), newVal);
      }
      if (typeof prop === "string" && prop.match(NUM_PAT)) {
        return Reflect.set(target, prop, newVal);
      }
      throw new TypeError("Property name must be an integer");
    },
    deleteProperty: function(target, prop) {
      if (typeof prop === "string" && prop.match(RANGE_PAT)) {
        throw new TypeError("Cannot delete a range index");
      }
      if (typeof prop === "string" && prop.match(FROM_END_PAT)) {
        let k = parseInt(prop.replace(FROM_END_PAT, "$1"), 10);
        let idx = target.length - k;
        if (idx < 0) {
          throw new RangeError("Index out of range: " + prop);
        }
        return Reflect.deleteProperty(target, String(idx));
      }
      if (typeof prop === "number") {
        if (!Number.isInteger(prop)) {
          throw new TypeError("Property name must be an integer");
        }
        let idx = prop < 0 ? target.length + prop : prop;
        if (idx < 0) {
          throw new RangeError("Index out of range");
        }
        return Reflect.deleteProperty(target, String(idx));
      }
      if (typeof prop === "string" && prop.match(NUM_PAT)) {
        return Reflect.deleteProperty(target, prop);
      }
      throw new TypeError("Property name must be an integer");
    },
  });
}

let arr = SmartArray('a', 'b', 'c', 'd', 'e', 'f');

console.log(arr[0]); // a
console.log(arr[4]); // e
console.log(arr['hello']); // undefined

console.log(arr['2-4']); // [c,d,E]
console.log(arr['3-5']); // [d,E,f]

console.log(arr[-1]); // f
console.log(arr[-3]); // d

try {
  console.log(arr[-99]);
} catch (e) {
  console.log("Exception correctly thrown.");
}

arr[1] = 'B';
console.log(arr[1]); // B

arr[-2] = 'E';
console.log(arr[4]); // E

try {
  arr['2-4'] = 'hello';
} catch (e) {
  console.log("Exception correctly thrown.");
}

try {
  arr[3*"hello"] = 'hello';
} catch (e) {
  console.log("Exception correctly thrown.");
}

console.log(arr);