# Lab 14: Evaluation and Typing Rules

## Language Grammar

```
e ::= true
    | false
    | i          (integer literals)
    | s          (string literals)
    | succ e
    | pred e
    | iszero e
    | if e then e else e
    | concat e e
    | isemptystr e
    | strlen e

T ::= Bool | Int | String
```

## Values

Values are fully-evaluated expressions that cannot be reduced further:

```
v ::= true | false | i | s
```

---

## Part 1: Small-Step Evaluation Rules (Operational Semantics)

### Booleans and Integers — base cases (values, no rules needed)

---

### `succ`

```
        e → e'
  ─────────────────   [E-Succ]
  succ e → succ e'


  ─────────────────────────────   [E-SuccInt]
  succ i → i + 1
```

---

### `pred`

```
        e → e'
  ─────────────────   [E-Pred]
  pred e → pred e'


  ─────────────────────────────   [E-PredInt]
  pred i → i - 1
```

---

### `iszero`

```
          e → e'
  ─────────────────────   [E-IsZero]
  iszero e → iszero e'


  ─────────────────────────   [E-IsZeroTrue]
  iszero 0 → true


  ─────────────────────────   [E-IsZeroFalse]    (i ≠ 0)
  iszero i → false
```

---

### `if`

```
                        e → e'
  ─────────────────────────────────────────────────────   [E-If]
  if e then e1 else e2 → if e' then e1 else e2


  ─────────────────────────────────   [E-IfTrue]
  if true then e1 else e2 → e1


  ─────────────────────────────────   [E-IfFalse]
  if false then e1 else e2 → e2
```

---

### `concat`

```
           e1 → e1'
  ──────────────────────────────   [E-Concat1]
  concat e1 e2 → concat e1' e2


          e2 → e2'
  ──────────────────────────────   [E-Concat2]
  concat s e2 → concat s e2'


  ──────────────────────────────   [E-ConcatStr]
  concat s1 s2 → s1 ++ s2
```

(Where `s1 ++ s2` denotes string concatenation of literal values `s1` and `s2`.)

---

### `isemptystr`

```
              e → e'
  ─────────────────────────────────   [E-IsEmptyStr]
  isemptystr e → isemptystr e'


  ─────────────────────────────────   [E-IsEmptyStrTrue]
  isemptystr "" → true


  ─────────────────────────────────   [E-IsEmptyStrFalse]    (s ≠ "")
  isemptystr s → false
```

---

### `strlen`

```
          e → e'
  ───────────────────────────   [E-StrLen]
  strlen e → strlen e'


  ───────────────────────────   [E-StrLenVal]
  strlen s → |s|
```

(Where `|s|` is the integer length of string literal `s`.)

---

## Part 2: Typing Rules

### Literals

```
  ─────────────────────   [T-True]
  Γ ⊢ true : Bool


  ─────────────────────   [T-False]
  Γ ⊢ false : Bool


  ─────────────────────   [T-Int]    (i is an integer literal)
  Γ ⊢ i : Int


  ─────────────────────   [T-Str]    (s is a string literal)
  Γ ⊢ s : String
```

---

### `succ` and `pred`

```
       Γ ⊢ e : Int
  ─────────────────────   [T-Succ]
  Γ ⊢ succ e : Int


       Γ ⊢ e : Int
  ─────────────────────   [T-Pred]
  Γ ⊢ pred e : Int
```

---

### `iszero`

```
       Γ ⊢ e : Int
  ──────────────────────   [T-IsZero]
  Γ ⊢ iszero e : Bool
```

---

### `if`

```
  Γ ⊢ e : Bool    Γ ⊢ e1 : T    Γ ⊢ e2 : T
  ────────────────────────────────────────────   [T-If]
        Γ ⊢ if e then e1 else e2 : T
```

---

### `concat`

```
  Γ ⊢ e1 : String    Γ ⊢ e2 : String
  ──────────────────────────────────────   [T-Concat]
       Γ ⊢ concat e1 e2 : String
```

---

### `isemptystr`

```
       Γ ⊢ e : String
  ──────────────────────────   [T-IsEmptyStr]
  Γ ⊢ isemptystr e : Bool
```

---

### `strlen`

```
       Γ ⊢ e : String
  ──────────────────────────   [T-StrLen]
  Γ ⊢ strlen e : Int
```

---

## Progress and Preservation

### Progress

**Theorem (Progress):** If `∅ ⊢ e : T`, then either `e` is a value, or there exists `e'` such that `e → e'`.

**Proof sketch (by induction on the typing derivation):**

- **T-True, T-False, T-Int, T-Str:** `e` is a value. ✓

- **T-Succ:** `e = succ e1` where `∅ ⊢ e1 : Int`. By IH, `e1` is a value or steps. If `e1` is a value `i`, then `E-SuccInt` applies. If `e1 → e1'`, then `E-Succ` applies.

- **T-Pred:** Symmetric to T-Succ.

- **T-IsZero:** `e = iszero e1` where `∅ ⊢ e1 : Int`. By IH, `e1` is a value `i` (E-IsZeroTrue or E-IsZeroFalse apply), or `e1 → e1'` (E-IsZero applies).

- **T-If:** `e = if e1 then e2 else e3` where `∅ ⊢ e1 : Bool`. By IH, `e1` is a value (must be `true` or `false`, so E-IfTrue or E-IfFalse apply), or `e1 → e1'` (E-If applies).

- **T-Concat:** `e = concat e1 e2` where `∅ ⊢ e1 : String`, `∅ ⊢ e2 : String`. By IH, `e1` steps or is a string value `s1`; if `s1`, `e2` steps or is a string value `s2`; if both values, E-ConcatStr applies.

- **T-IsEmptyStr:** `e = isemptystr e1` where `∅ ⊢ e1 : String`. By IH, `e1` steps (E-IsEmptyStr) or is a string value `s` (E-IsEmptyStrTrue or E-IsEmptyStrFalse apply).

- **T-StrLen:** Symmetric to T-IsEmptyStr.

### Preservation

**Theorem (Preservation):** If `Γ ⊢ e : T` and `e → e'`, then `Γ ⊢ e' : T`.

**Proof sketch (by induction on the evaluation derivation):**

- **E-Succ / E-Pred / E-IsZero / E-IsEmptyStr / E-StrLen / E-If / E-Concat1 / E-Concat2:** The sub-expression steps; the IH gives the sub-expression's type is preserved, and the same typing rule re-applies.

- **E-SuccInt:** `succ i → i+1`. Both have type `Int`.

- **E-PredInt:** `pred i → i-1`. Both have type `Int`.

- **E-IsZeroTrue / E-IsZeroFalse:** `iszero 0 → true`, `iszero i → false`. Both have type `Bool`.

- **E-IfTrue / E-IfFalse:** By T-If, `e2 : T` and `e3 : T`, so the chosen branch has type `T`.

- **E-ConcatStr:** `concat s1 s2 → s1 ++ s2`. Both have type `String`.

- **E-IsEmptyStrTrue / E-IsEmptyStrFalse:** `isemptystr s → true/false`. Both have type `Bool`.

- **E-StrLenVal:** `strlen s → |s|`. Both have type `Int`.
