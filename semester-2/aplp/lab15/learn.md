# Lab 15: JavaScript Proxies and `SmartArray` — Study Guide

This document explains **from zero** what JavaScript **Proxies** are, how they relate to normal objects, and how this lab’s **`SmartArray`** uses them. Read it once top-to-bottom, then revisit sections while you read `smart-array.js` line by line.

---

## 1. Warm-up: objects and property access

In JavaScript, almost everything you interact with is an **object** (or behaves like one for certain operations). An **array** is a special kind of object whose keys are often numeric indices (`0`, `1`, `2`, …) and which has array-specific behavior (like `.length`).

When you write:

```js
const arr = ["a", "b", "c"];
arr[1];        // "b"
arr["1"];      // also "b" — object keys are strings internally
```

The engine **looks up a property** on `arr` whose name is the result of converting your bracket expression to a **property key** (this conversion is called **ToPropertyKey** in the language spec). For many values (numbers, strings), the key becomes a **string** like `"1"` or `"-1"`.

**Important idea:** `arr[-1]` does **not** use “Python-style” negative indexing in normal JavaScript. On a plain array, `arr[-1]` usually means “the property named `'-1'`,” which is **not** the same as the last element unless you put something there yourself.

This lab builds a **wrapper** that **intercepts** property access and **implements** negative indexing and ranges yourself.

---

## 2. What is a Proxy?

A **Proxy** is an object that **sits in front of** another object (the **target**). When code reads, writes, or deletes properties on the Proxy (or performs other operations), the Proxy can run your code first — these hooks are called **traps**.

Think of it as:

- **Target:** the “real” data (here, a real `Array` holding your elements).
- **Handler:** an object full of optional functions (traps) you provide.
- **Proxy:** the object you hand to the rest of the program. It **delegates** to the target when you choose to, or does something custom.

You create one with:

```js
const proxy = new Proxy(targetObject, handlerObject);
```

After that, many operations on `proxy` will invoke traps from `handlerObject` **if** you defined them. If you did not define a trap for an operation, the default is usually “do whatever would happen on the target directly” (exact rules are in the ECMAScript spec).

**Why Proxies exist:** they let libraries implement **virtual properties**, validation, logging, revocable references, reactive systems, etc., without changing the target object’s class.

---

## 3. The `Reflect` API (partner to `Proxy`)

**`Reflect`** is a built-in object with static methods like `Reflect.get`, `Reflect.set`, `Reflect.deleteProperty`, … Each method roughly corresponds to a **default** language operation.

In Proxy traps, you often want:

- Custom behavior **sometimes**
- “Just do the normal thing” **other times**

Calling `Reflect.get(target, prop, receiver)` (etc.) is the usual way to say: **perform the default `[[Get]]` operation on `target`**, optionally with a **receiver** (explained below).

In this lab’s `get` trap, the code ends with:

```js
return Reflect.get(...arguments);
```

Here, `arguments` is the trap’s full argument list `(target, prop, receiver)` (in strict modern style you might list those parameters explicitly). Spreading them into `Reflect.get` forwards **everything the engine passed in**, including `receiver`, so behavior stays aligned with the spec for edge cases (inheritance, `this` binding in getters, etc.).

---

## 4. Traps used in this lab

A **trap** is a function with a fixed name on the handler. The engine calls it with specific arguments and expects specific return types / behaviors.

### 4.1 `get(target, prop, receiver)`

Runs when something **reads** a property, e.g. `proxy[prop]` or `proxy.foo`.

- **`target`:** the underlying object (your real array).
- **`prop`:** the property key (often a **string** or **symbol**; sometimes a **number** for array indices in some paths).
- **`receiver`:** the object that was used as `this` for the `[[Get]]` operation (often the Proxy itself).

**Return value:** the value the read expression should produce.

This lab’s `get` trap:

1. If `prop` is a string matching a **range** pattern like `"2-4"`, return `target.slice(start, end)` (custom behavior).
2. Else if it matches **from-end** negative index syntax like `"-1"`, compute `idx = target.length - k`, error if `idx < 0`, else return `target[idx]`.
3. Else if `prop` is a **non-integer number** (e.g. `1.5`), throw (invalid index).
4. Else defer to **`Reflect.get`** so normal properties work (`0`, `"length"`, etc., depending on what you allow elsewhere).

### 4.2 `set(target, prop, value, receiver)`

Runs on assignment: `proxy[prop] = value`.

- Must return a **boolean** in strict terms: **`true`** if the assignment succeeded or is a “successful no-op” in allowed cases, **`false`** in some failure modes (in sloppy mode, `false` can cause a throw in assignment; in strict mode, failed sets often throw). In practice, returning **`Reflect.set(...)`’s boolean** is the safe pattern.

This lab’s `set` trap enforces **integer-only** index writes for string/number keys (with special cases below), maps **`-k` strings** to indices from the end, and **rejects range keys** like `"2-4"` for assignment.

### 4.3 `deleteProperty(target, prop)`

Runs on `delete proxy[prop]`.

- Must return **boolean** indicating whether the property was deleted.

This lab adds the **optional** `deleteProperty` trap so **negative string keys** and validation mirror `get` / `set`.

---

## 5. How `SmartArray` is wired

```js
function SmartArray(...args) {
  return new Proxy(args, { /* handler */ });
}
```

- **`...args`** collects all arguments into a real **Array** instance** (the **target**).
- The function **returns the Proxy**, not the raw array. Variable `arr` in the demo **is** the Proxy.
- Reads/writes go to the Proxy first; traps run; they read/write **`target`** (the inner array) when appropriate.

So when you do `arr[-1]`:

1. The engine asks the Proxy’s **`get`** trap for property `"-1"` (string).
2. Your trap recognizes `FROM_END_PAT`, maps to `target.length - 1`, returns that element.

When you do `arr[1] = 'B'`:

1. The engine invokes **`set`** with `prop` often as the **number** `1` (engines may pass number or string; good handlers consider both).
2. Your trap validates and calls **`Reflect.set`** on the target with the resolved key.

---

## 6. Property keys: why you see `string`, `number`, and `symbol`

Object property keys are either **strings**, **symbols**, or (for array indices) certain **integer-like** keys with special array rules.

In Proxies:

- `obj[0]` might pass `prop` as **`0`** (number) or **`"0"`** (string) depending on path and engine. Robust code handles **both** for numeric access.
- `obj[Symbol.iterator]` passes a **symbol**.
- `obj['2-4']` passes the **string** `"2-4"`.

That is why the lab uses checks like `typeof prop === "string"` before `.match(...)`: **symbols do not have `.match`**, and calling it would throw.

---

## 7. Regular expressions in this lab (what each one means)

```js
const RANGE_PAT = /^(\d+)-(\d+)$/;
const FROM_END_PAT = /^-(\d+)$/;
const NUM_PAT = /^-?\d+$/;
```

### `RANGE_PAT`

- Matches strings like `"2-4"`: **digits**, **hyphen**, **digits**, whole string (`^` … `$`).
- **Capture groups** `$1` and `$2` are used to parse `start` and `end`.
- **Slice end is exclusive** in JavaScript, so the lab uses `end + 1` after parsing so the range is **inclusive** on the right, as the comments say.

### `FROM_END_PAT`

- Matches `"-1"`, `"-2"`, …: a minus sign followed by **one or more digits**.
- The magnitude `k` means “**k**th slot from the end”: index `length - k`.
- Example: length `6`, `"-1"` → `k = 1` → index `5` (last element).

### `NUM_PAT`

- Matches optional `-` then digits: **integer decimal notation** as a string, e.g. `"0"`, `"42"`, `"-3"`.
- **Note:** A string like `"-3"` is matched by **`FROM_END_PAT` first** in this lab’s `if` chain, so it is treated as **from-end** syntax, not as “the integer −3” for normal array indexing. That matches the lab’s intended `"-" + digits` meaning “from end.”
- Strings like `"NaN"` or `"2-4"` do **not** match `NUM_PAT`, which is useful for rejecting invalid “numeric” writes.

---

## 8. Walk-through: negative indexing (get)

For `FROM_END_PAT`:

```js
let k = parseInt(prop.replace(FROM_END_PAT, "$1"), 10);
let idx = target.length - k;
if (idx < 0) throw new RangeError(/* ... */);
return target[idx];
```

- **`k`** is how far from the end (1 = last).
- **`idx`** must be `>= 0`. If you ask for `"-99"` on a short array, **`idx`** becomes negative → **throw** (as the lab requires).

---

## 9. Walk-through: integer-only `set`

Design goals from the lab:

1. **Only integer-like index keys** for the custom rules; nonsense like `arr[3 * "hello"] = ...` (`NaN` → often key `"NaN"`) must **throw**.
2. **Positive / zero** indices: normal positions.
3. **Negative-from-end strings** (`"-2"`): same mapping as `get`.
4. **Numeric prop** that is negative (rare in bracket syntax but possible): `idx = target.length + prop` (because `prop` is negative).
5. **Range assignment** must **throw** (cannot mean “assign to three indices at once” without extra rules).

The handler also forwards **`symbol`** keys with `Reflect.set` so internal or library symbol properties are not accidentally blocked by the integer-only logic.

---

## 10. Why `Reflect.set(target, String(idx), ...)` sometimes

Arrays treat **canonical numeric indices** specially. Using **`String(idx)`** for some paths keeps keys in a predictable form and avoids odd corner cases with negative **numeric** keys (which are **not** “array index” keys in the spec sense). The important part is: you compute the **real** index `idx` on the underlying storage array, then write there.

---

## 11. Order of `if` branches matters

In `get`:

- Check **range** before **from-end**, because something like `"2-4"` is not a from-end pattern.
- Check **from-end** before treating a string as a generic integer via `NUM_PAT`, because both could match negative-looking strings; the lab defines **`-` + digits** as **from-end**, not “signed integer index.”

---

## 12. Mental model vs Python / Ruby

| Feature | Plain JS array | This lab’s `SmartArray` |
|--------|----------------|-------------------------|
| `arr[-1]` | Property `"-1"`, not last elem | **Last element** (via Proxy) |
| `arr['2-4']` | Unusual property name | **Slice** of elements |
| Out-of-range negative | N/A (not special) | **`RangeError`** |

---

## 13. Optional reading: other files in this folder

- **`tracing.js`:** tiny demo of `get` / `set` logging; good to see traps firing.
- **`undoable.js`:** another Proxy pattern (undo/revert). Notice how **`get`** can return **functions** that close over internal state (`oldVals`), not just data from `target`.

---

## 14. Other Proxy traps (not required for this lab)

For completeness, the language defines many traps, including:

- `has` (`prop in proxy`)
- `ownKeys` / `getOwnPropertyDescriptor`
- `apply` / `construct` (when target is function / used with `new`)

You do not need them for `SmartArray`, but knowing they exist helps when you read MDN or the spec later.

---

## 15. Study checklist (when you review `smart-array.js`)

1. Trace **`arr['2-4']`**: which trap, which regex, why `slice`.
2. Trace **`arr[-1]`**: string `"-1"`, `k`, `idx`, return path.
3. Trace **`arr[-99]`**: why `idx < 0` throws.
4. Trace **`arr[1] = 'B'`**: `set` trap, which branch, what `Reflect.set` returns.
5. Trace **`arr['2-4'] = ...`**: which branch throws and why.
6. Explain why **`typeof prop === "string"`** guards exist before `.match`.
7. Explain **`Reflect.get(...arguments)`** vs hard-coding `(target, prop, receiver)`.
8. (If you added it) **`deleteProperty`**: same index math as `get` / `set`.

---

## 16. Official references

- MDN: [Proxy](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Proxy), [Reflect](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Reflect)
- ECMA-262: Proxy object internal methods (`[[Get]]`, `[[Set]]`, …) — for deep dives after you’re comfortable with MDN.

---

## 17. One-line summary

> **`SmartArray`** wraps a real array in a **`Proxy`** whose **`get` / `set` / `deleteProperty`** traps **parse special string keys** (ranges and from-end indices), **validate** integer rules for writes, and otherwise **delegate** to **`Reflect`** so normal array behavior stays intact.

When you can explain that sentence aloud without looking, you understand the lab.
