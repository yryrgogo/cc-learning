# cc-learning

This is a repository for learning to implement C Compiler.

The code is implemented with reference to 9cc.
But 9cc is inactive, so the code was implemented by the author while researching.
The code bases is no longer a 9cc transcript from the middle, so please don't refer this.

## Assembler

x86-64

- the x86-64 integer arithmetic instruction normally accepts only two registers, so the result is stored by overwriting the value of the first argument register

### function

- the first argument is placed in the RDI register and the second argument is placed in the RSI register
- the return value is put into RAX register

### instructions

#### mov

- copy the value of a register

#### call

- push the address of the next instruction of call to stack
- jump to the address given as the argument of call

#### ret

- pop one address from the stack top and jump to the address

## Reference

- [9cc](https://github.com/rui314/9cc)
- [chibicc](https://github.com/rui314/chibicc)
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
