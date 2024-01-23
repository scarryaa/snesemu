#include "../include/interrupt.hpp"
#include "../include/cpu.hpp"

uint8_t Interrupt::handle_interrupt(Interrupts type, Cpu* cpu, Memory* memory) {
    switch (type)
    {
    case Interrupts::COP:
        handle_cop(cpu, memory);
    case Interrupts::BRK:
        handle_brk(cpu, memory);
        break;
    case Interrupts::ABORT:
        handle_abort(cpu, memory);
        break;
    case Interrupts::NMI:
        handle_nmi(cpu, memory);
        break;
    case Interrupts::IRQ:
        handle_irq(cpu, memory);
        break;
    case Interrupts::RESET:
        handle_reset(cpu, memory);
        break;
    }

    // Interrupts take ~7 cycles to complete + !E
    return 7 + !cpu->getFlagE() ? 1 : 0;
}

void Interrupt::handle_cop(Cpu* cpu, Memory* memory) {
    // Push PC and P onto stack
    cpu->push((cpu->getPC() >> 8) & 0xFF);
    cpu->push(cpu->getPC() & 0xFF);

    // Push P
    cpu->push(cpu->getP() & 0xFF);

    // Enable interrupt disable
    cpu->setI(1);

    // Set PC to COP vector (emulated or native)
    if (cpu->getFlagE()) cpu->setPC(memory->read(Cpu::COP_VECTOR_EMU) |
        (memory->read(Cpu::COP_VECTOR_EMU + 1) << 8));
    else cpu->setPC(memory->read(Cpu::COP_VECTOR_NATIVE) 
        | (memory->read(Cpu::COP_VECTOR_NATIVE + 1) << 8));
}

void Interrupt::handle_brk(Cpu* cpu, Memory* memory) {
    cpu->push((cpu->getPC() >> 8) & 0xFF);
    cpu->push(cpu->getPC() & 0xFF);
    cpu->push(cpu->getP() | 0x10);
    cpu->setI(1);

    // Set PC to BRK vector (emulated or native)
    if (cpu->getFlagE()) cpu->setPC(memory->read(Cpu::BRK_VECTOR_EMU) |
        (memory->read(Cpu::BRK_VECTOR_EMU + 1) << 8));
    else cpu->setPC(memory->read(Cpu::BRK_VECTOR_NATIVE)
        | (memory->read(Cpu::BRK_VECTOR_NATIVE + 1) << 8));
}

void Interrupt::handle_abort(Cpu* cpu, Memory* memory) {
    cpu->push((cpu->getPC() >> 8) & 0xFF);
    cpu->push(cpu->getPC() & 0xFF);
    cpu->push(cpu->getP() & 0xFF);
    cpu->setI(1);

    if (cpu->getFlagE()) {
        cpu->setPC(memory->read(Cpu::ABORT_VECTOR_EMU) |
            (memory->read(Cpu::ABORT_VECTOR_EMU + 1) << 8));
    }
    else {
        cpu->setPC(memory->read(Cpu::ABORT_VECTOR_NATIVE) |
            (memory->read(Cpu::ABORT_VECTOR_NATIVE + 1) << 8));
    }
}

void Interrupt::handle_nmi(Cpu* cpu, Memory* memory) {
    cpu->push((cpu->getPC() >> 8) & 0xFF);  
    cpu->push(cpu->getPC() & 0xFF);
    cpu->push(cpu->getP() & 0xFF);
    cpu->setI(1);

    if (cpu->getFlagE()) {
        cpu->setPC(memory->read(Cpu::NMI_VECTOR_EMU) |
            (memory->read(Cpu::NMI_VECTOR_EMU + 1) << 8));
    }
    else {
        cpu->setPC(memory->read(Cpu::NMI_VECTOR_NATIVE) |
            (memory->read(Cpu::NMI_VECTOR_NATIVE + 1) << 8));
    }
}

void Interrupt::handle_irq(Cpu* cpu, Memory* memory) {
    cpu->push((cpu->getPC() >> 8) & 0xFF);
    cpu->push(cpu->getPC() & 0xFF);
    cpu->push(cpu->getP() & 0xFF);
    cpu->setI(1);

    if (cpu->getFlagE()) {
        cpu->setPC(memory->read(Cpu::IRQ_VECTOR_EMU) |
            (memory->read(Cpu::IRQ_VECTOR_EMU + 1) << 8));
    }
    else {
        cpu->setPC(memory->read(Cpu::IRQ_VECTOR_NATIVE) |
            (memory->read(Cpu::IRQ_VECTOR_NATIVE + 1) << 8));
    }
}

void Interrupt::handle_reset(Cpu* cpu, Memory* memory) {
    // Set PC to RESET vector (emulated only)
    cpu->setPC(memory->read(Cpu::RESET_VECTOR_EMU) | (memory->read(Cpu::RESET_VECTOR_EMU + 1) << 8));
}