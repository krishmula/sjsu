{-
  Name: Krishna Mula
  Class: CS 252
  Assigment: HW2
  Date: 06 March 2026
  Description: <Describe the program and what it does>
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
  | And      -- && :: Bool -> Bool -> Bool
  | Not      -- !  :: Bool -> Bool -> Bool
  | Or       -- || :: Bool -> Bool -> Bool
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
applyOp Divide (IntVal i) (IntVal 0) = error "Can't divide by 0"
applyOp Divide (IntVal i) (IntVal j) = IntVal $ i `div` j
applyOp Gt (IntVal i) (IntVal j) = BoolVal $ i > j
applyOp Ge (IntVal i) (IntVal j) = BoolVal $ i >= j
applyOp Lt (IntVal i) (IntVal j) = BoolVal $ i < j
applyOp Le (IntVal i) (IntVal j) = BoolVal $ i <= j
applyOp And (BoolVal i) (BoolVal j) = BoolVal $ i && j
applyOp Or (BoolVal i) (BoolVal j) = BoolVal $ i || j
applyOp Not (BoolVal b) _ = BoolVal $ not b 
applyOp _ _ _ = error "Provide valid arguments"


-- Implement this function according to the specified semantics
evaluate :: Expression -> Store -> (Value, Store)
evaluate expr store =
  case expr of
    -- Values are already evaluated.
    Val v -> (v, store)

    -- Variable lookup.
    Var x ->
      case Map.lookup x store of
        Just v  -> (v, store)
        Nothing -> error $ "Unbound variable: " ++ show x

    -- Assignment: evaluate RHS, then update the store.
    Assign x e ->
      let (v, store') = evaluate e store
      in (v, Map.insert x v store')

    -- Sequencing: run e1 for its effects, then e2.
    Sequence e1 e2 ->
      let (_v1, store') = evaluate e1 store
      in evaluate e2 store'

    -- Conditionals.
    If eCond eThen eElse ->
      let (vCond, store') = evaluate eCond store
      in case vCond of
           BoolVal True  -> evaluate eThen store'
           BoolVal False -> evaluate eElse store'
           _             -> error "Condition in if must evaluate to a boolean"

    -- While loops.
    -- Big-step flavor: keep re-checking the guard; when it becomes false, the
    -- whole while-expression evaluates to false.
    While eCond body ->
      let (vCond, store') = evaluate eCond store
      in case vCond of
           BoolVal True ->
             let (_vBody, store'') = evaluate body store'
             in evaluate (While eCond body) store''
           BoolVal False -> (BoolVal False, store')
           _             -> error "Condition in while must evaluate to a boolean"

    -- Operations.
    Op op e1 e2 ->
      case op of
        -- Reasonable programmer behavior: short-circuiting boolean operators.
        And ->
          let (v1, store1) = evaluate e1 store
          in case v1 of
               BoolVal False -> (BoolVal False, store1)
               BoolVal True  ->
                 let (v2, store2) = evaluate e2 store1
                 in case v2 of
                      BoolVal b2 -> (BoolVal b2, store2)
                      _          -> error "Right operand of 'and' must be boolean"
               _ -> error "Left operand of 'and' must be boolean"

        Or ->
          let (v1, store1) = evaluate e1 store
          in case v1 of
               BoolVal True  -> (BoolVal True, store1)
               BoolVal False ->
                 let (v2, store2) = evaluate e2 store1
                 in case v2 of
                      BoolVal b2 -> (BoolVal b2, store2)
                      _          -> error "Right operand of 'or' must be boolean"
               _ -> error "Left operand of 'or' must be boolean"

        -- Unary not: we ignore the second expression (it is forced by the AST).
        Not ->
          let (v1, store1) = evaluate e1 store
          in case v1 of
               BoolVal b -> (BoolVal (not b), store1)
               _         -> error "Operand of 'not' must be boolean"

        -- Everything else behaves like a strict binary operator.
        _ ->
          let (v1, store1) = evaluate e1 store
              (v2, store2) = evaluate e2 store1
          in (applyOp op v1 v2, store2)


-- Evaluates a program with an initially empty state
run :: Expression -> (Value, Store)
run prog = evaluate prog Map.empty

-- The same as run, but only returns the Store
testProgram :: Expression -> Store
testProgram prog = snd $ run prog


