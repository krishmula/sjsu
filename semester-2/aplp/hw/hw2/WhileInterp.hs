{-
  Name: Krishna Mula
  Class: CS 252
  Assigment: HW2
  Date: 06 March 2026
  Description: WHILE interpreter implementing big-step semantics with a store.
-}


module WhileInterp (
  Expression(..),
  Binop(..),
  Value(..),
  testProgram,
  run
) where

import Data.Map (Map)
import qualified Data.Map as Map

-- We represent variables as strings.
type Variable = String

-- The store is an associative map from variables to values.
-- (The store roughly corresponds with the heap in a language like Java).
type Store = Map Variable Value

data Expression =
    Var Variable                            -- x
  | Val Value                               -- v
  | Assign Variable Expression              -- x := e
  | Sequence Expression Expression          -- e1; e2
  | Op Binop Expression Expression
  | If Expression Expression Expression     -- if e1 then e2 else e3
  | While Expression Expression             -- while (e1) e2
  deriving (Show)

data Binop =
    Plus     -- +  :: Int  -> Int  -> Int
  | Minus    -- -  :: Int  -> Int  -> Int
  | Times    -- *  :: Int  -> Int  -> Int
  | Divide   -- /  :: Int  -> Int  -> Int
  | Gt       -- >  :: Int -> Int -> Bool
  | Ge       -- >= :: Int -> Int -> Bool
  | Lt       -- <  :: Int -> Int -> Bool
  | Le       -- <= :: Int -> Int -> Bool
  | And      -- && :: Bool -> Bool -> Bool   (short-circuit in evaluate)
  | Not      -- !  :: Bool -> Bool           (unary; second arg ignored)
  | Or       -- || :: Bool -> Bool -> Bool   (short-circuit in evaluate)
  deriving (Show)

data Value =
    IntVal Int
  | BoolVal Bool
  deriving (Show)


-- This function will be useful for defining binary operations.
-- The first case is done for you.
-- Be sure to explicitly check for a divide by 0 and throw an error.
applyOp :: Binop -> Value -> Value -> Value
applyOp Plus (IntVal i) (IntVal j) = IntVal $ i + j
applyOp Minus (IntVal i) (IntVal j) = IntVal $ i - j
applyOp Times (IntVal i) (IntVal j) = IntVal $ i * j
applyOp Divide (IntVal _) (IntVal 0) = error "Can't divide by 0"
applyOp Divide (IntVal i) (IntVal j) = IntVal $ i `div` j
applyOp Gt (IntVal i) (IntVal j) = BoolVal $ i > j
applyOp Ge (IntVal i) (IntVal j) = BoolVal $ i >= j
applyOp Lt (IntVal i) (IntVal j) = BoolVal $ i < j
applyOp Le (IntVal i) (IntVal j) = BoolVal $ i <= j
-- NOTE: And/Or/Not are handled specially in evaluate to get short-circuit + unary not.
-- These are here mainly as a fallback (and to keep the signature complete).
applyOp And (BoolVal i) (BoolVal j) = BoolVal $ i && j
applyOp Or  (BoolVal i) (BoolVal j) = BoolVal $ i || j
applyOp Not (BoolVal b) _           = BoolVal $ not b
applyOp _ _ _ = error "Provide valid arguments"


-- Implement this function according to the specified semantics
evaluate :: Expression -> Store -> (Value, Store)

evaluate (Val v) s = (v, s)

evaluate (Var x) s =
  case Map.lookup x s of
    Just v  -> (v, s)
    Nothing -> error ("Unbound variable: " ++ x)

evaluate (Assign x e) s =
  let (v, s1) = evaluate e s
  in (v, Map.insert x v s1)

evaluate (Sequence e1 e2) s =
  let (_, s1) = evaluate e1 s
  in evaluate e2 s1

evaluate (If e1 e2 e3) s =
  let (v1, s1) = evaluate e1 s
  in case v1 of
       BoolVal True  -> evaluate e2 s1
       BoolVal False -> evaluate e3 s1
       _             -> error "Condition in If must evaluate to a boolean"

evaluate (While e1 e2) s =
  let (v1, s1) = evaluate e1 s
  in case v1 of
       BoolVal True  ->
         let (_, s2) = evaluate e2 s1
         in evaluate (While e1 e2) s2
       BoolVal False -> (BoolVal False, s1)
       _             -> error "Condition in While must evaluate to a boolean"

evaluate (Op And e1 e2) s =
  let (v1, s1) = evaluate e1 s
  in case v1 of
       BoolVal True  -> evaluate e2 s1
       BoolVal False -> (BoolVal False, s1)
       _             -> error "Operand for And must be boolean"

evaluate (Op Or e1 e2) s =
  let (v1, s1) = evaluate e1 s
  in case v1 of
       BoolVal True  -> (BoolVal True, s1)
       BoolVal False -> evaluate e2 s1
       _             -> error "Operand for Or must be boolean"

evaluate (Op Not e1 _) s =
  let (v1, s1) = evaluate e1 s
  in case v1 of
       BoolVal True  -> (BoolVal False, s1)
       BoolVal False -> (BoolVal True, s1)
       _             -> error "Operand for Not must be boolean"

evaluate (Op o e1 e2) s =
  let (v1, s1) = evaluate e1 s
      (v2, s') = evaluate e2 s1
  in (applyOp o v1 v2, s')


-- Evaluates a program with an initially empty state
run :: Expression -> (Value, Store)
run prog = evaluate prog Map.empty

-- The same as run, but only returns the Store
testProgram :: Expression -> Store
testProgram prog = snd $ run prog