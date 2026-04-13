# Lab 13 Part 1 Design

## Goal
Fix `lab13/rabbit.js` using only the changes needed to satisfy JSLint for part 1 of the assignment.

## Scope
- Keep the file in JavaScript.
- Make only JSLint-driven corrections.
- Preserve the intended runtime behavior.
- Do not modernize or refactor beyond what JSLint requires.

## Planned Changes
- Declare the top-level `name` variable with `var` so it is no longer an undeclared global.
- Add the appropriate JSLint environment directive so `console` is allowed.
- Instantiate `Rabbit` with `new` so `this` refers to the new object instead of the global object.
- Move the variable declaration for `r` to the top-level declaration area to satisfy JSLint's declaration-order rule.

## Expected Behavior
- `console.log(r.name);` should print `"Python"`.
- `console.log(name);` should print `"Monty"`.

## Verification
- Re-run JSLint on `lab13/rabbit.js` and confirm the listed warnings are resolved.
- Run the file with Node and confirm the output is:
- `Python`
- `Monty`
