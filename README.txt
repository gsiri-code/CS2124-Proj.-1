George Sirichartchai
gs3802@nyu.edu

This is a completed C++ E20 simulator. The resources I used were the CS-UY 2214 — E20 Manual by Jeff Epstein,
the CS-UY 2214 — Project 1 pdf by Ratan Dey, and Jeff Epstein, and C++ documentation.

Setup:

Initially the parameters of the simulation are set to 16 bit binary values of 0. This is because every register
is initialized to 0, as well as the PC, and memory (which starts at address 0). After initialize my parameters
I plug them into my E20 simulation function called "sim", which simulates all of the logic of E20.
gi
Sim function:
    The sim function starts with a boolean to flag for the instruction halt. The majority of the function is in a
    while loop, which will only stop if halt is true. The beginning of the loop starts with setting a 16 bit binary
    to the current instruction, where 16 bit pc is cut down to its 13 bit value (using bitwise operation &), which is
    used to index the memory parameter. Once this current instruction is read, then the current instructions are
    broken down into the necessary parameters needed to carry out specific instructions. I use combinations of
    bit shifting or bitwise operations or both to cut down or extend (sign extend imm. 7 or zero extend imm. 13).
    Once I get these values, I use func and opcode to dictate how my function handles these other parameters. Depending
    on if the operation is a 3 register instruction or 2 register, some parameters could be left out, while other
    parameters are crucial to the functionality of that particular current instruction. I index my register parameter
    with the register index's I get from the current instruction. With instructions lw and sw, I made sure to read the
    memory with specifically 13 bits of the sum of the relevant parameters used to index the memory. To handle modification
    of the program counter I made a new 16 bit value called new_pc, which is defaulted to pc + 1 because a majority of
    these instructions are incrementing the program counter by 1. Otherwise in jumps, I set the new_pc to their relevant
    values, which would be set in sequence after the defaulted value, ensuring the new_pc is jumped instead of default
    incremented by 1. Once all of the instructions are handled, the halt boolean checks if the original program counter is
    equal to new_pc. If they are equal, this would mean that there was halt instruction that set the new_pc to the current
    pc value, indicating the E20 program wants to halt. Otherwise the original pc is set to the new_pc value, and register 0
    is set back to zero to ensure that it is not overwritten to be a value other than 0. Once all of these processes are
    handled and there was no flag for a halt, the program runs once again to the next instruction that the program counter
    dictates.