/*
CS-UY 2214
Adapted from Jeff Epstein
Starter code for E20 simulator
sim.cpp
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>
//#include <gay>
#include <bitset>

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1 << 13;
//size_t const static REG_SIZE = 1<<16;

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void load_machine_code(ifstream& f, uint16_t mem[]) {
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;
    string line;
    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedaddr++;
        mem[addr] = instr;
    }
}

/*
    Prints the current state of the simulator, including
    the current program counter, the current register values,
    and the first memquantity elements of memory.

    @param pc The final value of the program counter
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memquantity How many words of memory to dump
*/
void print_state(uint16_t pc, uint16_t regs[], const uint16_t memory[], const size_t memquantity) {
    cout << setfill(' ');
    cout << "Final state:" << endl;
    cout << "\tpc=" << setw(5) << pc << endl;

    for (size_t reg = 0; reg < NUM_REGS; reg++)
        cout << "\t$" << reg << "=" << setw(5) << regs[reg] << endl;

    cout << setfill('0');
    bool cr = false;
    for (size_t count = 0; count < memquantity; count++) {
        cout << hex << setw(4) << memory[count] << " ";
        cr = true;
        if (count % 8 == 7) {
            cout << endl;
            cr = false;
        }
    }
    if (cr)
        cout << endl;
}

void sim(uint16_t& pc, uint16_t regs[], uint16_t mem[]) {

    bool halt = false;
    while (!halt) {
        cout << "Begining Iteration --------------------------------------------" << endl;
        uint16_t curr_ins = mem[pc & 8191];

        //Params
        uint8_t opCode = curr_ins >> 13;
        //registers
        uint8_t rA = (curr_ins >> 10) & 7;
        uint8_t rB = (curr_ins >> 7) & 7;


        uint8_t rC = (curr_ins >> 4) & 7;
        uint8_t func = curr_ins & 15;

        uint16_t imm7 = curr_ins & 127;
        if (imm7 & 0x0040) {
            imm7 |= 0xFF80; // 0xFF80 is 11111111 10000000 in binary
        }
        uint16_t imm13 = curr_ins & 0x1FFF;
        uint16_t new_pc = pc + 1;

        cout << "\tpc: " << pc << endl;

//        cout << "full ins: " << bitset<16>(curr_ins) << endl;
//        cout << "\topCode: " << bitset<3>(opCode) << endl;
//        cout << "\timm13: " << bitset<16>(imm13) << endl;
//        cout << "\timm7: " << bitset<7>(imm7) << endl;
        cout << "\tRegisterA: " << (regs[rA]) << endl;
        cout << "\tRegisterB: " << (regs[rB]) << endl;
        cout << "\tRegisterC: " << (regs[rC]) << endl;



//        cout << "\trA: " << bitset<3>(rA) << endl;
//        cout << "\trB: " << bitset<3>(rB) << endl;
//        cout << "\trC: " << bitset<3>(rC) << endl;
//        cout << "\tfunc: " << bitset<4>(func) << endl;

        // three reg instructions (add, sub, or, and, slt, jr)
        if (opCode == 0b000) {
            if (func == 0b0000) {
                // add
                regs[rC] = regs[rA] + regs[rB];
            } else if (func == 0b0001) {
                // sub
                regs[rC] = regs[rA] - regs[rB];
            } else if (func == 2) {
                // or
                regs[rC] = regs[rA] | regs[rB];
            } else if (func == 3) {
                //and
                regs[rC] = regs[rA] & regs[rB];
            } else if (func == 4) {
                //slt
                regs[rC] = (regs[rA] < regs[rB]) ? 1 : 0;
            } else if (func == 5) {
                // jr
                new_pc = regs[rA];
            }
        } else {
            // Two reg instructions
            if (opCode == 1) {
                // addi
                regs[rB] = regs[rA] + imm7;
            } else if (opCode == 2) {
                // j
                new_pc = imm13;
            } else if (opCode == 3) {
                // jal
                regs[7] = pc + 1;
                new_pc = imm13;
            } else if (opCode == 4) {
                // lw
                if (rB != 0) regs[rB] = mem[regs[rA] + imm7];
            } else if (opCode == 5) {
                // sw
                mem[regs[rA] + imm7] = regs[rB];
            } else if (opCode == 6) {
                // jeq
                if (regs[rA] == regs[rB]) new_pc = pc+1+imm7;

            } else if (opCode == 7) {
                // slti
                regs[rB] = regs[rA] < imm7;
            }
        }
        //TODO: add a flag to check if pc coutner same, if same then halt, else update pc
        if (pc == new_pc){
            halt = true;
            cout << "HALT" <<endl;
        }else { pc = new_pc;}
        regs[0] = 0;
        cout << "After Iteration--------------------------------------------" << endl;
        cout << "\tpc: " << pc << endl;
        cout << "\tRegisterA: " << (regs[rA]) << endl;
        cout << "\tRegisterB: " << (regs[rB]) << endl;
        cout << "\tRegisterC: " << (regs[rC]) << endl;
    }
}


/**
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char* argv[]) {
    /*
        Parse the command-line arguments
    */
    char* filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0) {
            if (arg == "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] filename" << endl << endl;
        cerr << "Simulate E20 machine" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl << endl;
        cerr << "optional arguments:" << endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file " << filename << endl;
        return 1;
    }

    // TODO: your code here. Load f and parse using load_machine_code
    //initialized processor state

    uint16_t pc = 0;
    uint16_t regArr[NUM_REGS] = {0};
    uint16_t mem[MEM_SIZE] = {0};

    load_machine_code(f, mem);
    // TODO: your code here. Do simulation.
    sim(pc, regArr, mem);
    // TODO: your code here. print the final state of the simulator before ending, using print_state
    print_state(pc, regArr, mem, 128);

    return 0;
}
