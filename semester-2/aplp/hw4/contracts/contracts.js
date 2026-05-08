// NOTE: This library uses non-standard JS features (although widely supported).
// Specifically, it uses Function.name.

function any(v) {
    return true;
  }

  function isNumber(v) {
    return !Number.isNaN(v) && typeof v === 'number';
  }
  isNumber.expected = "number";

  function isBoolean(v) {
    return typeof v === 'boolean';
  }
  isBoolean.expected = "boolean";

  function isDefined(v) {
    return v !== null && v !== undefined;
  }
  isDefined.expected = "defined";

  function isString(v) {
    return typeof v === 'string' || v instanceof String;
  }
  isString.expected = "string";

  function isNegative(v) {
    return isNumber(v) && v < 0;
  }
  isNegative.expected = "negative number";

  function isPositive(v) {
    return isNumber(v) && v > 0;
  }
  isPositive.expected = "positive number";


  // Combinators:

  function and() {
    let args = Array.prototype.slice.call(arguments);
    let cont = function(v) {
      for (let i in args) {
        if (!args[i].call(this, v)) {
          return false;
        }
      }
      return true;
    }
    cont.expected = expect(args[0]);
    for (let i=1; i<args.length; i++) {
      cont.expected += " and " + expect(args[i]);
    }
    return cont;
  }

  function or() {
    let args = Array.prototype.slice.call(arguments);
    let cont = function(v) {
      for (let i in args) {
        if (args[i].call(this, v)) {
          return true;
        }
      }
      return false;
    };
    cont.expected = expect(args[0]);
    for (let i = 1; i < args.length; i++) {
      cont.expected += " or " + expect(args[i]);
    }
    return cont;
  }

  function not(c) {
    let cont = function(v) {
      return !c.call(this, v);
    };
    cont.expected = "not " + expect(c);
    return cont;
  }



  // Utility function that returns what a given contract expects.
  function expect(f) {
    // For any contract function f, return the "expected" property
    // if it is specified.  (This allows developers to specify what
    // the expected property should be in a more readable form.)
    if (f.expected) {
      return f.expected;
    }
    // If the function name is available, use that.
    if (f.name) {
      return f.name;
    }
    // In case an anonymous contract is specified.
    return "ANONYMOUS CONTRACT";
  }

  function formatReceived(value) {
    if (typeof value === 'string') {
      return value;
    }
    if (value === undefined) {
      return "undefined";
    }
    if (value === null) {
      return "null";
    }
    return String(value);
  }

  function enforceInvariant(thisArg) {
    if (thisArg === null || thisArg === undefined) {
      return;
    }
    if (typeof thisArg !== 'object' && typeof thisArg !== 'function') {
      return;
    }
    let proto = Object.getPrototypeOf(thisArg);
    if (proto && typeof proto.invariant === 'function') {
      if (!proto.invariant.call(thisArg, thisArg)) {
        throw new Error(
          "Contract violation. Expected invariant to hold. Blame -> Top-level code"
        );
      }
    }
  }

  function contract (preList, post, f) {
    let wrappedName = f.name || "anonymous";
    return new Proxy(f, {
      apply: function (target, thisArg, argumentsList) {
        let blameCaller = "Top-level code";
        for (let i = 0; i < preList.length; i++) {
          let arg = argumentsList[i];
          if (!preList[i].call(thisArg, arg)) {
            let expected = expect(preList[i]);
            let received = formatReceived(arg);
            throw new Error(
              "Contract violation in position " + i + ". Expected " + expected +
              " but received " + received + ".  Blame -> " + blameCaller
            );
          }
        }
        let result = Reflect.apply(target, thisArg, argumentsList);
        if (!post.call(thisArg, result)) {
          let expected = expect(post);
          let received = formatReceived(result);
          throw new Error(
            "Contract violation. Expected " + expected + " but returned " + received +
            ". Blame -> " + wrappedName
          );
        }
        enforceInvariant(thisArg);
        return result;
      }
    });
  }


  module.exports = {
    contract: contract,
    any: any,
    isBoolean: isBoolean,
    isDefined: isDefined,
    isNumber: isNumber,
    isPositive: isPositive,
    isNegative: isNegative,
    isInteger: Number.isInteger,
    isString: isString,
    and: and,
    or: or,
    not: not,
    negative: isNegative,
  };
