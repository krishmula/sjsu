'use strict';

let opcodes = {
  0x01: { mnemonic: 'ADD', evaluate: (vm) => {
    let v1 = vm.stack.pop();
    let v2 = vm.stack.pop();
    vm.stack.push(v1+v2);
  }},
  0x02: { mnemonic: 'MUL', evaluate: (vm) => {
    let v1 = vm.stack.pop();
    let v2 = vm.stack.pop();
    vm.stack.push(v1 * v2);
  }},
  0x03: { mnemonic: 'SUB', evaluate: (vm) => {
    let b = vm.stack.pop();
    let a = vm.stack.pop();
    vm.stack.push(a - b);
  }},
  0x51: { mnemonic: 'MLOAD', evaluate: (vm) => {
    let off = vm.stack.pop();
    let v = vm.memory[off] | 0;
    vm.stack.push(v);
  }},
  0x52: { mnemonic: 'MSTORE', evaluate: (vm) => {
    // Stack (bottom ... value, offset), offset on top: match push value, push offset.
    let off = vm.stack.pop();
    let v = vm.stack.pop();
    vm.memory[off] = v;
  }},
  0x56: { mnemonic: 'JUMP', evaluate: (vm) => {
    let dest = vm.stack.pop();
    // Main loop will increment pc after this, so target one byte before destination.
    vm.pc = dest - 1;
  }},
  0x57: { mnemonic: 'JUMPI', evaluate: (vm) => {
    // Geth/EVM: first pop = destination, second = condition; jump if condition is non-zero.
    let dest = vm.stack.pop();
    let cond = vm.stack.pop();
    if (cond !== 0) {
      vm.pc = dest - 1;
    }
  }},
  0x5B: { mnemonic: 'JUMPDEST', evaluate: (vm) => {
  }},
  0x60: { mnemonic: 'PUSH1', evaluate: (vm) => {
    vm.pc++;
    let v = vm.bytecode.readUInt8(vm.pc);
    vm.stack.push(v);
  }},
  0x90: { mnemonic: 'SWAP1', evaluate: (vm) => {
    let a = vm.stack.pop();
    let b = vm.stack.pop();
    vm.stack.push(a);
    vm.stack.push(b);
  }},
  0x0c: { mnemonic: 'PRINT', evaluate: (vm) => {
    console.log(vm.stack.pop());
  }},
};

exports.opcodes = opcodes;
