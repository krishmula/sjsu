For this lab, we will use JavaScript to write a compiler and a VM for a subset of the Scheme language.

We will support:

println to print out values.
the mathematical operators '+', '-', and '*'
define, for setting variables.
if, for conditional expressions. [EXTRA CREDIT]
The only supported values are integers and booleans ('#t' and '#f').


Our compiler will create binary bytecode files.  For no particular good reason, we will use the Ethereum Virtual Machine (EVM) as a rough guide.  (Ethereum has a notion of 'gas', which you may ignore).

 

PART 1 -- Run the Compiler and Virtual Machine

First, compile 'test.scm' with the following command:

$ node compiler.js test1.scm

You should see the following output:

Compiling test1.scm...
Bytecode written to test1.byco.

To read the bytecode format, you will need to view hex. On OSX, you can use the xxd command to do this:

$ xxd test1.byco 
00000000: 6001 6002 0160 0301 0c                   `.`..`...
$

Linux users may need to use hexdump -C test1.byco. For Windows users, this link might be helpful: https://www.howtohaven.com/system/view-binary-file-on-windows.shtmlLinks to an external site..

Note that the bytes translate to the opcodes in op-codes.js.  Roughly, the program is:

PUSH1 1
PUSH1 2
ADD
PUSH1 3
ADD
PRINT
Next, run the VM on the bytecode:

$ node vm.js test1.byco 
Executing test1.byco...
6

(Nothing needs to be submitted for this part of the assignment).


PART 2 -- Add Opcodes to VM
A slightly more complex example is available in test2.byco.

You will need to add the MUL, SUB, and SWAP1 opcodes to op-codes.js. The skeleton of the MUL opcode with comments is available in the file. For SUB and SWAP1, review the details from https://ethervm.io/Links to an external site. and https://ethereum.org/en/developers/docs/evm/opcodesLinks to an external site..

 

PART 3 -- Update compiler to support '*' and '-'
Now that you have VM support for MUL, SUB, and SWAP1, you can compile programs with '*' and '-'. Use test2.scm for an example that will exercise these functions.

Modify the writeBytecode method in compiler.js to add in this support.

 

PART 4 -- Add support for variables
To support more complex programs, you will need to add support for the MSTORE and MLOAD opcodes to the VirtualMachine class.  We will simplify them so that they only load a byte.

The VirtualMachine class has a "memory" array.  Store data here, where the offset matches to the index of the array.

In scheme, the 'define' keyword is used to assign values to variables.  For example, (define x 3) sets the value of x to 3.  Update compiler.js to add in support for define.

Test out your solution against store.scm.

 

PART 5 -- Add support for conditional expressions
(5 POINTS EXTRA CREDIT, NO PARTIAL CREDIT)
Update compiler.js to support booleans and 'if' expressions. You will need support for the 'JUMP' and 'JUMPI' opcodes. 'JUMPDEST' is already implemented, and may prove useful.

If you decide to do the extra credit, you must demo your working solution to me by the last day of regular classes in the semester.