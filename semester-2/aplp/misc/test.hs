data Expression = 
    Var String
  | Val Int
  | Assign String Expression
  | Sequence Expression Expression
  | Op Binop Expression Expression
  | If Expression Expression Expression
  | While Expression Expression
  deriving (Show)

data Binop = Plus | Minus | Times | Divide | Gt | Ge | Lt | Le
    deriving (Show)

data Value = IntVal Int | BoolVal Bool
    deriving (Show)

type Variable = String

-- Example expression:
-- Op Plus (Var "abc") (Var "def")