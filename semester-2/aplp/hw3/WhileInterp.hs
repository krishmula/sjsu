{-
  Name: Krishna Mula
  Class: CS 252
  Assigment: HW3
  Date: 27 March 2026
  Description: Parser for the IMP language
-}

module WhileInterp
  ( Expression (..),
    Binop (..),
    Value (..),
    runFile,
    showParsedExp,
    run,
  )
where

import Control.Monad.Except
import Data.Map (Map)
import Data.Map qualified as Map
import Text.ParserCombinators.Parsec

-- We represent variables as strings.
type Variable = String

-- We also represent error messages as strings.
type ErrorMsg = String

-- The store is an associative map from variables to values.
-- (The store roughly corresponds with the heap in a language like Java).
type Store = Map Variable Value

data Expression
  = Var Variable -- x
  | Val Value -- v
  | Assign Variable Expression -- x := e
  | Sequence Expression Expression -- e1; e2
  | Op Binop Expression Expression
  | If Expression Expression Expression -- if e1 then e2 else e3 endif
  | While Expression Expression -- while e1 do e2 endwhile
  deriving (Show)

data Binop
  = Plus -- +  :: Int  -> Int  -> Int
  | Minus -- -  :: Int  -> Int  -> Int
  | Times
  | -- \*  :: Int  -> Int  -> Int
    Divide -- /  :: Int  -> Int  -> Int
  | Gt -- >  :: Int -> Int -> Bool
  | Ge -- >= :: Int -> Int -> Bool
  | Lt -- <  :: Int -> Int -> Bool
  | Le -- <= :: Int -> Int -> Bool
  deriving (Show)

data Value
  = IntVal Int
  | BoolVal Bool
  deriving (Show)

fileP :: GenParser Char st Expression
fileP = do
  prog <- exprP
  eof
  return prog

exprP = do
  e <- exprP'
  rest <- optionMaybe restSeqP
  return
    ( case rest of
        Nothing -> e
        Just e' -> Sequence e e'
    )

-- Expressions are divided into terms and expressions for the sake of
-- parsing.  Note that binary operators **DO NOT** follow the expected
-- presidence rules.
--

-- *** FOR 2pts EXTRA CREDIT (hard, no partial credit)***

-- Correct the precedence of the binary operators.
exprP' = do
  spaces
  t <- compP
  spaces
  rest <- optionMaybe (string ":=" >> spaces >> exprP')
  spaces
  return
    ( case rest of
        Nothing -> t
        Just t' ->
          ( case t of
              Var varName -> Assign varName t'
              _ -> error "Expected var"
          )
    )

makeOp s op = try $ do
  spaces
  string s
  spaces
  return (Op op)

compP = arithP `chainl1` choice [makeOp "<=" Le, makeOp "<" Lt, makeOp ">=" Ge, makeOp ">" Gt]

arithP = termP_mult `chainl1` choice [makeOp "+" Plus, makeOp "-" Minus]

termP_mult = termP `chainl1` choice [makeOp "*" Times, makeOp "/" Divide]

restSeqP = do
  char ';'
  exprP

-- unused, kept for safety
transOp s = error "unused"

restP = error "unused"

-- All terms can be distinguished by looking at the first character
termP =
  valP
    <|> ifP
    <|> whileP
    <|> parenP
    <|> varP
    <?> "value, variable, 'if', 'while', or '('"

valP = do
  v <- boolP <|> numberP
  return $ Val v

boolP = do
  bStr <- string "true" <|> string "false" <|> string "skip"
  return $ case bStr of
    "true" -> BoolVal True
    "false" -> BoolVal False
    "skip" -> BoolVal False -- Treating the command 'skip' as a synonym for false, for ease of parsing

numberP = do
  digits <- many1 digit
  return $ IntVal (read digits)

varP = do
  v <- many1 letter
  return $ Var v

ifP = do
  string "if"
  spaces
  cond <- exprP
  spaces
  string "then"
  spaces
  e1 <- exprP
  spaces
  string "else"
  spaces
  e2 <- exprP
  spaces
  string "endif"
  return $ If cond e1 e2

whileP = do
  string "while"
  spaces
  cond <- exprP
  spaces
  string "do"
  spaces
  body <- exprP
  spaces
  string "endwhile"
  return $ While cond body

-- An expression in parens, e.g. (9-5)*2
parenP = do
  char '('
  spaces
  e <- exprP
  spaces
  char ')'
  return e

-- This function will be useful for defining binary operations.
-- Unlike in the previous assignment, this function returns an "Either value".
-- The right side represents a successful computaton.
-- The left side is an error message indicating a problem with the program.
-- The first case is done for you.
applyOp :: Binop -> Value -> Value -> Either ErrorMsg Value
applyOp Plus (IntVal i) (IntVal j) = Right $ IntVal $ i + j
applyOp Minus (IntVal i) (IntVal j) = Right $ IntVal $ i - j
applyOp Times (IntVal i) (IntVal j) = Right $ IntVal $ i * j
applyOp Divide (IntVal i) (IntVal j) = if j == 0 then Left "Divide by zero" else Right $ IntVal (i `div` j)
applyOp Gt (IntVal i) (IntVal j) = Right $ BoolVal $ i > j
applyOp Ge (IntVal i) (IntVal j) = Right $ BoolVal $ i >= j
applyOp Lt (IntVal i) (IntVal j) = Right $ BoolVal $ i < j
applyOp Le (IntVal i) (IntVal j) = Right $ BoolVal $ i <= j
applyOp op v1 v2 = Left $ "Invalid operands for operator " ++ show op ++ ": " ++ show v1 ++ ", " ++ show v2

-- As with the applyOp method, the semantics for this function
-- should return Either values.  Left <error msg> indicates an error,
-- whereas Right <something> indicates a successful execution.
evaluate :: Expression -> Store -> Either ErrorMsg (Value, Store)
evaluate (Val v) s = Right (v, s)
evaluate (Var x) s = case Map.lookup x s of
  Just v -> Right (v, s)
  Nothing -> Left $ "Variable '" ++ x ++ "' is not defined"
evaluate (Assign x e) s = do
  (v, s') <- evaluate e s
  Right (v, Map.insert x v s')
evaluate (Sequence e1 e2) s = do
  (_, s1) <- evaluate e1 s
  evaluate e2 s1
evaluate (Op o e1 e2) s = do
  (v1, s1) <- evaluate e1 s
  (v2, s') <- evaluate e2 s1
  v <- applyOp o v1 v2
  return (v, s')
evaluate (If cond thn els) s = do
  (cVal, s1) <- evaluate cond s
  case cVal of
    BoolVal True -> evaluate thn s1
    BoolVal False -> evaluate els s1
    IntVal v -> Left $ "Non-boolean value '" ++ show v ++ "' used as a conditional"
evaluate (While cond body) s = do
  (cVal, s1) <- evaluate cond s
  case cVal of
    BoolVal True -> do
      (_, s2) <- evaluate body s1
      evaluate (While cond body) s2
    BoolVal False -> Right (BoolVal False, s1)
    IntVal v -> Left $ "Non-boolean value '" ++ show v ++ "' used as a conditional"

-- Evaluates a program with an initially empty state
run :: Expression -> Either ErrorMsg (Value, Store)
run prog = evaluate prog Map.empty

showParsedExp fileName = do
  p <- parseFromFile fileP fileName
  case p of
    Left parseErr -> print parseErr
    Right exp -> print exp

runFile fileName = do
  p <- parseFromFile fileP fileName
  case p of
    Left parseErr -> print parseErr
    Right exp ->
      case (run exp) of
        Left msg -> print msg
        Right (v, s) -> print $ show s
