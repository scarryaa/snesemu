#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP

#include <stdint.h>
#include "memory.hpp"

class Cpu;

enum Interrupts {
	COP,
	BRK,
	ABORT,
	NMI,
	IRQ,
	RESET,
	NONE
};

class Interrupt {
public: 
	static uint8_t handle_interrupt(Interrupts type, Cpu* cpu, Memory* memory);
private:
	static void handle_cop(Cpu* cpu, Memory* memory);
	static void handle_brk(Cpu* cpu, Memory* memory);
	static void handle_abort(Cpu* cpu, Memory* memory);
	static void handle_nmi(Cpu* cpu, Memory* memory);
	static void handle_irq(Cpu* cpu, Memory* memory);
	static void handle_reset(Cpu* cpu, Memory* memory);
};

#endif