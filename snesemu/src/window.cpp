#include "../include/window.hpp"
#include "../include/emulator.hpp"
#include "../include/debug/disassembler.hpp"

Window::Window() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }

    window = SDL_CreateWindow("snesemu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 224);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    // Make the window resizable
    SDL_SetWindowResizable(this->window, SDL_TRUE);

    // Initialize SDL Renderer for ImGui
    ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
    ImGui_ImplSDLRenderer2_Init(this->renderer);
}

Window::~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
        {
            return true;
        }
    }

    return false;
}

void Window::renderDisassembly(Emulator* emulator)
{
    Cpu* cpu = emulator->getCpu(); // Get the CPU from the emulator
    Disassembler disassembler = emulator->getDisassembler();
    uint16_t pc = cpu->getPC();     // Get the current PC from the CPU
    uint16_t startAddress = pc - 10; // Start disassembling a few instructions before the PC for context

    ImGui::Begin("Disassembler");

    // Flags (highlighted if set)
    if (cpu->getFlagC())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "C");
    }
    else
    {
        ImGui::Text("C");
    }
    ImGui::SameLine();

    if (cpu->getFlagV())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "D");
    }
    else
    {
        ImGui::Text("D");
    }
    ImGui::SameLine();

    if (cpu->getFlagI())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "I");
    }
    else
    {
        ImGui::Text("I");
    }
    ImGui::SameLine();

    if (cpu->getFlagM())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "M");
    }
    else
    {
        ImGui::Text("M");
    }
    ImGui::SameLine();

    if (cpu->getFlagN())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "N");
    }
    else
    {
        ImGui::Text("N");
    }
    ImGui::SameLine();

    if (cpu->getFlagV())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "V");
    }
    else
    {
        ImGui::Text("V");
    }
    ImGui::SameLine();

    if (cpu->getFlagX())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "X");
    }
    else
    {
        ImGui::Text("X");
    }
    ImGui::SameLine();

    if (cpu->getFlagZ())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Z");
    }
    else
    {
        ImGui::Text("Z");
    }

    // Registers
    // A
    ImGui::Text("A: %06X", cpu->getA());
    ImGui::SameLine();

    // X
    ImGui::Text("X: %06X", cpu->getX());
    ImGui::SameLine();

    // Y
    ImGui::Text("Y: %06X", cpu->getY());

    // SP
    ImGui::Text("SP: %04X", cpu->getSP());

    // PC
    ImGui::Text("PC: %06X", cpu->getPC());
    ImGui::SameLine();

    // P
    ImGui::Text("P: %02X", cpu->getP());

    // Buttons
    if (ImGui::Button("Step"))
    {
        emulator->step();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        emulator->reset();
    }

    ImGui::SameLine();

    if (emulator->isPaused())
    {
        if (ImGui::Button("Resume"))
        {
            emulator->unpause();
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            emulator->pause();
        }
    }

    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::Columns(6, "disassembler_columns", false);
    ImGui::SetColumnWidth(0, 15.0f);
    ImGui::SetColumnWidth(1, 60.0f);

    uint16_t address = startAddress;
    for (int i = 0; i < 20; i++)
    {
        // Check if the address is a breakpoint
        // if so, draw a red circle
        if (emulator->isBreakpoint(BREAKPOINT_TYPE_ADDRESS, address))
        {
            ImGui::SetCursorPosX(5.0f);
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "O");
        }
        else
        {
            ImGui::Text(" ");
        }
        ImGui::NextColumn();

        Instruction instruction = disassembler.disassemble(address);
        if (address == pc)
        {
            // Highlight the current PC
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%06X", instruction.address);
        }
        else
        {
            ImGui::Text("%06X", instruction.address);
        }

        // Check for double click to set a breakpoint
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            // Toggle the breakpoint
            if (emulator->isBreakpoint(BREAKPOINT_TYPE_ADDRESS, address))
            {
                emulator->clearBreakpoint(BREAKPOINT_TYPE_ADDRESS, address);
            }
            else
            {
                emulator->setBreakpoint(BREAKPOINT_TYPE_ADDRESS, address);
            }
        }

        ImGui::NextColumn();

        // Opcode
        ImGui::Text("%02X", instruction.opcode);
        ImGui::NextColumn();

        // Op 1
        ImGui::Text(instruction.bytes[0] == ' ' ? " " : "%02X", instruction.bytes[0]);
        ImGui::NextColumn();

        // Op 2
        ImGui::Text(instruction.bytes[1] == ' ' ? " " : "%02X", instruction.bytes[1]);
        ImGui::NextColumn();

        // Mnemonic
        ImGui::Text("%s", instruction.mnemonic);
        ImGui::NextColumn();
        address += 1;
    }

    ImGui::EndChild();

    ImGui::End();
}

void Window::renderCgramView(Emulator* emulator) {
    Ppu* ppu = emulator->getPpu();

    ImGui::Begin("CGRAM");

    const int bytesPerRow = 16;
    const int totalLines = 0x100 / bytesPerRow;

    ImGui::BeginChild("CgramScrolling", ImVec2(0, 0), false, 0);

    for (int line = 0; line < totalLines; ++line)
    {
        uint16_t addr = line * bytesPerRow;
        ImGui::Text("%06X: ", addr);
        for (int col = 0; col < bytesPerRow; ++col)
        {
            ImGui::SameLine();
            ImGui::Text("%02X ", ppu->readCGRAM(addr + col));
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void Window::renderBreakpoints(Emulator* emulator)
{
    ImGui::Begin("Breakpoints");

    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), false);

    auto breakpoints = emulator->getBreakpoints();
    for (auto it = breakpoints.begin(); it != breakpoints.end(); ++it)
    {
        ImGui::Text("%04X", *it);
    }

    ImGui::EndChild();

    ImGui::Button("Clear All");

    if (ImGui::IsItemClicked())
    {
        emulator->clearAllBreakpoints();
    }

    ImGui::SameLine();

    ImGui::Button("Add");

    // Spawn a new window if the button is clicked
    if (ImGui::IsItemClicked())
    {
        ImGui::OpenPopup("Add Breakpoint");
    }

    // Create the popup window
    if (ImGui::BeginPopup("Add Breakpoint"))
    {
        static char input[5] = "0000";
        ImGui::InputText("Address", input, 5);

        // Radio button for scanline breakpoint
        static bool scanline = false;
        ImGui::RadioButton("Scanline", scanline);

        static bool cycle = false;
        static bool frame = false;
        static bool instruction = false;
        static bool read = false;
        static bool write = false;
        static bool execute = false;

        if (ImGui::IsItemClicked())
        {
            scanline = true;

            // Clear the other radio buttons
            cycle = false;
            frame = false;
            instruction = false;
            read = false;
            write = false;
            execute = false;
        }

        // Radio button for cycle breakpoint
        ImGui::RadioButton("Cycle", cycle);

        if (ImGui::IsItemClicked())
        {
            cycle = true;

            // Clear the other radio buttons
            scanline = false;
            frame = false;
            instruction = false;
            read = false;
            write = false;
            execute = false;
        }

        // Radio button for frame breakpoint
        ImGui::RadioButton("Frame", frame);

        // Radio button for instruction breakpoint
        ImGui::RadioButton("Instruction", instruction);

        // Radio button for read breakpoint
        ImGui::RadioButton("Read", read);

        // Radio button for write breakpoint
        ImGui::RadioButton("Write", write);

        if (ImGui::IsItemClicked())
        {
            write = true;

            // Clear the other radio buttons
            scanline = false;
            cycle = false;
            frame = false;
            instruction = false;
            read = false;
            execute = false;
        }

        // Radio button for execute breakpoint
        ImGui::RadioButton("Execute", execute);

        if (ImGui::IsItemClicked())
        {
            execute = true;

            // Clear the other radio buttons
            scanline = false;
            cycle = false;
            frame = false;
            instruction = false;
            read = false;
            write = false;
        }

        // Add the breakpoint
        if (ImGui::Button("Add"))
        {
            // Convert the input to an integer
            uint16_t value = std::stoi(input, nullptr, 16);

            breakpoint_type_t type;

            if (cycle)
            {
                type = BREAKPOINT_TYPE_CYCLE;
            }
            else if (instruction)
            {
                type = BREAKPOINT_TYPE_EXECUTION;
            }
            else if (write)
            {
                type = BREAKPOINT_TYPE_WRITE;
            }
            else
            {
                type = BREAKPOINT_TYPE_ADDRESS;
            }

            emulator->setBreakpoint(type, value);

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void Window::renderVramView(Emulator* emulator) {
    Ppu* ppu = emulator->getPpu();

    ImGui::Begin("VRAM");

    const int bytesPerRow = 16;
    const int totalLines = 0x8000 / bytesPerRow;

    ImGui::BeginChild("VramScrolling", ImVec2(0, 0), false, 0);

    for (int line = 0; line < totalLines; ++line)
    {
        uint16_t addr = line * bytesPerRow;
        ImGui::Text("%06X: ", addr);
        for (int col = 0; col < bytesPerRow; ++col)
        {
            ImGui::SameLine();
            ImGui::Text("%02X ", ppu->readVRAM(addr + col));
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void Window::renderCpuMemoryView(Emulator* emulator)
{
    Memory* memory = emulator->getMemory();

    ImGui::Begin("CPU Memory View");

    const int bytesPerRow = 16;
    const int totalLines = 0x10000 / bytesPerRow;

    ImGui::BeginChild("MemoryScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (int line = 0; line < totalLines; ++line)
    {
        uint16_t addr = line * bytesPerRow;
        ImGui::Text("%06X: ", addr);
        for (int col = 0; col < bytesPerRow; ++col)
        {
            ImGui::SameLine();
            ImGui::Text("%02X ", memory->read(addr + col));
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void Window::render(Emulator* emulator) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame(this->window);
    ImGui::NewFrame();

    this->renderDisassembly(emulator);
    this->renderCpuMemoryView(emulator);
    this->renderBreakpoints(emulator);
    this->renderVramView(emulator);
    this->renderCgramView(emulator);
}

void Window::postRender(uint8_t* frameBuffer) {
    // // Delay to control frame rate (16 ms = 60 fps)
    // SDL_Delay(16);

    ImGui::Render();
    SDL_RenderClear(this->renderer);
    SDL_UpdateTexture(this->texture, nullptr, frameBuffer, 256 * sizeof(uint8_t) * 4);
    SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(this->renderer);
}