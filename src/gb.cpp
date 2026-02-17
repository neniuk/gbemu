#include "gb.h"

#include <SDL2/SDL.h>
#include <array>
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

GB::GB()
    : stack(registers.SP, memory), idu(registers, memory), alu(registers, memory), bmi(registers, memory), ppu(registers, memory, screen),
      cpu(registers, memory, stack, idu, alu, bmi, ppu), timer(registers, memory, cpu.stopped), joypad(memory) {};

CartridgeInfo GB::read_cartridge_header() {
    std::vector<uint8_t> entry_point = this->memory.read_range(0x0100, 0x0103);
    std::vector<uint8_t> logo = this->memory.read_range(0x0104, 0x0133);

    std::vector<uint8_t> title_bytes = this->memory.read_range(0x0134, 0x0143);
    std::string title = std::string(title_bytes.begin(), title_bytes.end());

    // std::vector<uint8_t> manufacturer_code = this->memory.read_range(0x013F, 0x0142);
    // uint8_t cgb_flag = this->memory.read_byte(0x0143);

    uint8_t new_licensee_code = this->memory.read_byte(0x0144);
    uint8_t sgb_flag = this->memory.read_byte(0x0146);
    uint8_t cartridge_type = this->memory.read_byte(0x0147);
    uint8_t rom_size_code = this->memory.read_byte(0x0148);
    uint8_t ram_size_code = this->memory.read_byte(0x0149);
    uint8_t destination_code = this->memory.read_byte(0x014A);
    uint8_t old_licensee_code = this->memory.read_byte(0x014B);
    uint8_t mask_rom_version = this->memory.read_byte(0x014C);
    uint8_t header_checksum = this->memory.read_byte(0x014D);
    uint16_t global_checksum = this->memory.read_word(0x014E);

    std::string licensee;
    if (old_licensee_code == 0x33) {
        licensee = this->new_licensees.at(std::to_string(new_licensee_code));
    } else {
        licensee = this->old_licensees.at(old_licensee_code);
    }

    std::string destination = (destination_code == 0) ? "Japan (and possibly overseas)" : "Overseas only";

    static const std::array<int, 6> ram_kbs = {0, 2, 8, 32, 128, 64};
    int ram_kb = (ram_size_code >= 0 && ram_size_code < ram_kbs.size()) ? ram_kbs[ram_size_code] : 0;

    CartridgeInfo cartridge_info = {.title = title,
                                    .entry_point = entry_point,
                                    .logo = logo,
                                    .licensee = licensee,
                                    .supports_sgb = (sgb_flag == 0x03),
                                    .rom_size_kb = (rom_size_code >= 0 && rom_size_code <= 8) ? 32 * (1 << rom_size_code) : 0,
                                    .num_rom_banks = (rom_size_code >= 0 && rom_size_code <= 8) ? (1 << rom_size_code) : 0,
                                    .ram_size_kb = ram_kb,
                                    .cartridge_type = this->cartridge_types.at(cartridge_type),
                                    .destination = destination,
                                    .game_version = mask_rom_version,
                                    .header_checksum = header_checksum,
                                    .global_checksum = global_checksum};

    return cartridge_info;
}

void GB::boot(std::vector<uint8_t> &rom_buf) {
    this->memory.load_rom(rom_buf);

    CartridgeInfo cartridge_info = this->read_cartridge_header();

    std::cout << "Cartridge Title: " << cartridge_info.title << '\n'
              << "Licensee: " << cartridge_info.licensee << '\n'
              << "Supports SGB: " << (cartridge_info.supports_sgb ? "Yes" : "No") << '\n'
              << "Cartridge Type: " << cartridge_info.cartridge_type << '\n'
              << "ROM Size (KB): " << cartridge_info.rom_size_kb << '\n'
              << "ROM Banks: " << cartridge_info.num_rom_banks << '\n'
              << "RAM Size (KB): " << cartridge_info.ram_size_kb << '\n'
              << "Destination: " << cartridge_info.destination << '\n'
              << "Game Version: " << static_cast<int>(cartridge_info.game_version) << '\n'
              << "Header Checksum: " << static_cast<int>(cartridge_info.header_checksum) << '\n'
              << "Global Checksum: " << cartridge_info.global_checksum << '\n';

    if (cartridge_info.rom_size_kb > 32 || cartridge_info.ram_size_kb > 0) {
        throw std::runtime_error("Unsupported cartridge type or RAM size");
    }

    // Start SDL window
    SDL_Init(SDL_INIT_EVERYTHING);

    // // Setup audio
    // bool is_playing = false;
    // SDL_AudioSpec want, have;
    // SDL_zero(want);
    // want.freq = 44100;
    // want.format = AUDIO_S16SYS;
    // want.channels = 1;
    // want.samples = 1024;
    // want.callback = play;
    // want.userdata = &is_playing;

    // SDL_AudioDeviceID audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

    // if (audio == 0) printf("[WARNING] main > Failed to open audio device: %s\n", SDL_GetError());

    // if (audio != 0) {
    //     SDL_PauseAudioDevice(audio, 0);
    //     if (debug_flag) {
    //         printf("[DEBUG] main > Audio device with: freq=%d, format=%d, channels=%d, samples=%d\n", have.freq,
    //                have.format, have.channels, have.samples);
    //     }
    // }

    // Setup window
    // clang-format off
    SDL_Window *window = SDL_CreateWindow(
        GB_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, 
        GB_SCREEN_WIDTH * GB_SCREEN_SCALE,
        GB_SCREEN_HEIGHT * GB_SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );
    // clang-format on

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture *texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);

    SDL_RenderSetLogicalSize(renderer, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    this->screen.set_renderer(renderer);
    this->screen.set_texture(texture);

    // if (debug_flag) {
    //     printf("[DEBUG] main > Window and renderer initialized\n");
    // }

    this->screen.draw_logo(cartridge_info.logo);
    this->screen.present();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    this->screen.clear();
    this->screen.present();

    this->registers.PC = 0x0100; // Entrypoint
    this->run();
};

void GB::run() {
    while (true) {

        // Joypad
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
            joypad.handle_event(event);
        }
        joypad.tick();

        const uint32_t t_states = this->cpu.step();
        int dots_advanced = t_states;

        // Tick hardware
        this->ppu.tick(dots_advanced);   // LCD control, STAT, LY, LYC, SCX, SCY, WX, WY, BGP, OBP0, OBP1, rendering, DMA
        this->timer.tick(dots_advanced); // DIV, TIMA, TMA, TAC
        // this->serial.tick(dots_advanced);
        // this->joypad.tick(dots_advanced);

        this->cpu.service_interrupts();

        if (this->ppu.consume_frame_ready()) {
            this->screen.present();
        }
    }
}

const std::unordered_map<uint8_t, std::string> GB::cartridge_types = {{0x00, "ROM ONLY"},
                                                                      {0x01, "MBC1"},
                                                                      {0x02, "MBC1+RAM"},
                                                                      {0x03, "MBC1+RAM+BATTERY"},
                                                                      {0x05, "MBC2"},
                                                                      {0x06, "MBC2+BATTERY"},
                                                                      {0x08, "ROM+RAM"},
                                                                      {0x09, "ROM+RAM+BATTERY"},
                                                                      {0x0B, "MMM01"},
                                                                      {0x0C, "MMM01+RAM"},
                                                                      {0x0D, "MMM01+RAM+BATTERY"},
                                                                      {0x0F, "MBC3+TIMER+BATTERY"},
                                                                      {0x10, "MBC3+TIMER+RAM+BATTERY"},
                                                                      {0x11, "MBC3"},
                                                                      {0x12, "MBC3+RAM"},
                                                                      {0x13, "MBC3+RAM+BATTERY"},
                                                                      {0x19, "MBC5"},
                                                                      {0x1A, "MBC5+RAM"},
                                                                      {0x1B, "MBC5+RAM+BATTERY"},
                                                                      {0x1C, "MBC5+RUMBLE"},
                                                                      {0x1D, "MBC5+RUMBLE+RAM"},
                                                                      {0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
                                                                      {0x20, "MBC6"},
                                                                      {0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
                                                                      {0xFC, "POCKET CAMERA"},
                                                                      {0xFD, "BANDAI TAMA5"},
                                                                      {0xFE, "HuC3"},
                                                                      {0xFF, "HuC1+RAM+BATTERY"}};

const std::unordered_map<uint8_t, std::string> GB::old_licensees = {{0x00, "None"},
                                                                    {0x01, "Nintendo"},
                                                                    {0x08, "Capcom"},
                                                                    {0x09, "HOT-B"},
                                                                    {0x0A, "Jaleco"},
                                                                    {0x0B, "Coconuts Japan"},
                                                                    {0x0C, "Elite Systems"},
                                                                    {0x13, "EA (Electronic Arts)"},
                                                                    {0x18, "Hudson Soft"},
                                                                    {0x19, "ITC Entertainment"},
                                                                    {0x1A, "Yanoman"},
                                                                    {0x1D, "Japan Clary"},
                                                                    {0x1F, "Virgin Games Ltd.3"},
                                                                    {0x24, "PCM Complete"},
                                                                    {0x25, "San-X"},
                                                                    {0x28, "Kemco"},
                                                                    {0x29, "SETA Corporation"},
                                                                    {0x30, "Infogrames5"},
                                                                    {0x31, "Nintendo"},
                                                                    {0x32, "Bandai"},
                                                                    {0x34, "Konami"},
                                                                    {0x35, "HectorSoft"},
                                                                    {0x38, "Capcom"},
                                                                    {0x39, "Banpresto"},
                                                                    {0x3C, "Entertainment Interactive (stub)"},
                                                                    {0x3E, "Gremlin"},
                                                                    {0x41, "Ubi Soft1"},
                                                                    {0x42, "Atlus"},
                                                                    {0x44, "Malibu Interactive"},
                                                                    {0x46, "Angel"},
                                                                    {0x47, "Spectrum HoloByte"},
                                                                    {0x49, "Irem"},
                                                                    {0x4A, "Virgin Games Ltd.3"},
                                                                    {0x4D, "Malibu Interactive"},
                                                                    {0x4F, "U.S. Gold"},
                                                                    {0x50, "Absolute"},
                                                                    {0x51, "Acclaim Entertainment"},
                                                                    {0x52, "Activision"},
                                                                    {0x53, "Sammy USA Corporation"},
                                                                    {0x54, "GameTek"},
                                                                    {0x55, "Park Place15"},
                                                                    {0x56, "LJN"},
                                                                    {0x57, "Matchbox"},
                                                                    {0x59, "Milton Bradley Company"},
                                                                    {0x5A, "Mindscape"},
                                                                    {0x5B, "Romstar"},
                                                                    {0x5C, "Naxat Soft16"},
                                                                    {0x5D, "Tradewest"},
                                                                    {0x60, "Titus Interactive"},
                                                                    {0x61, "Virgin Games Ltd.3"},
                                                                    {0x67, "Ocean Software"},
                                                                    {0x69, "EA (Electronic Arts)"},
                                                                    {0x6E, "Elite Systems"},
                                                                    {0x6F, "Electro Brain"},
                                                                    {0x70, "Infogrames5"},
                                                                    {0x71, "Interplay Entertainment"},
                                                                    {0x72, "Broderbund"},
                                                                    {0x73, "Sculptured Software6"},
                                                                    {0x75, "The Sales Curve Limited7"},
                                                                    {0x78, "THQ"},
                                                                    {0x79, "Accolade8"},
                                                                    {0x7A, "Triffix Entertainment"},
                                                                    {0x7C, "MicroProse"},
                                                                    {0x7F, "Kemco"},
                                                                    {0x80, "Misawa Entertainment"},
                                                                    {0x83, "LOZC G."},
                                                                    {0x86, "Tokuma Shoten"},
                                                                    {0x8B, "Bullet-Proof Software2"},
                                                                    {0x8C, "Vic Tokai Corp.17"},
                                                                    {0x8E, "Ape Inc.18"},
                                                                    {0x8F, "I'Max19"},
                                                                    {0x91, "Chunsoft Co.9"},
                                                                    {0x92, "Video System"},
                                                                    {0x93, "Tsubaraya Productions"},
                                                                    {0x95, "Varie"},
                                                                    {0x96, "Yonezawa10/S'Pal"},
                                                                    {0x97, "Kemco"},
                                                                    {0x99, "Arc"},
                                                                    {0x9A, "Nihon Bussan"},
                                                                    {0x9B, "Tecmo"},
                                                                    {0x9C, "Imagineer"},
                                                                    {0x9D, "Banpresto"},
                                                                    {0x9F, "Nova"},
                                                                    {0xA1, "Hori Electric"},
                                                                    {0xA2, "Bandai"},
                                                                    {0xA4, "Konami"},
                                                                    {0xA6, "Kawada"},
                                                                    {0xA7, "Takara"},
                                                                    {0xA9, "Technos Japan"},
                                                                    {0xAA, "Broderbund"},
                                                                    {0xAC, "Toei Animation"},
                                                                    {0xAD, "Toho"},
                                                                    {0xAF, "Namco"},
                                                                    {0xB0, "Acclaim Entertainment"},
                                                                    {0xB1, "ASCII Corporation or Nexsoft"},
                                                                    {0xB2, "Bandai"},
                                                                    {0xB4, "Square Enix"},
                                                                    {0xB6, "HAL Laboratory"},
                                                                    {0xB7, "SNK"},
                                                                    {0xB9, "Pony Canyon"},
                                                                    {0xBA, "Culture Brain"},
                                                                    {0xBB, "Sunsoft"},
                                                                    {0xBD, "Sony Imagesoft"},
                                                                    {0xBF, "Sammy Corporation"},
                                                                    {0xC0, "Taito"},
                                                                    {0xC2, "Kemco"},
                                                                    {0xC3, "Square"},
                                                                    {0xC4, "Tokuma Shoten"},
                                                                    {0xC5, "Data East"},
                                                                    {0xC6, "Tonkin House"},
                                                                    {0xC8, "Koei"},
                                                                    {0xC9, "UFL"},
                                                                    {0xCA, "Ultra Games"},
                                                                    {0xCB, "VAP, Inc."},
                                                                    {0xCC, "Use Corporation"},
                                                                    {0xCD, "Meldac"},
                                                                    {0xCE, "Pony Canyon"},
                                                                    {0xCF, "Angel"},
                                                                    {0xD0, "Taito"},
                                                                    {0xD1, "SOFEL (Software Engineering Lab)"},
                                                                    {0xD2, "Quest"},
                                                                    {0xD3, "Sigma Enterprises"},
                                                                    {0xD4, "ASK Kodansha Co."},
                                                                    {0xD6, "Naxat Soft16"},
                                                                    {0xD7, "Copya System"},
                                                                    {0xD9, "Banpresto"},
                                                                    {0xDA, "Tomy"},
                                                                    {0xDB, "LJN"},
                                                                    {0xDD, "Nippon Computer Systems"},
                                                                    {0xDE, "Human Ent."},
                                                                    {0xDF, "Altron"},
                                                                    {0xE0, "Jaleco"},
                                                                    {0xE1, "Towa Chiki"},
                                                                    {0xE2, "Yutaka"},
                                                                    {0xE3, "Varie"},
                                                                    {0xE5, "Epoch"},
                                                                    {0xE7, "Athena"},
                                                                    {0xE8, "Asmik Ace Entertainment"},
                                                                    {0xE9, "Natsume"},
                                                                    {0xEA, "King Records"},
                                                                    {0xEB, "Atlus"},
                                                                    {0xEC, "Epic/Sony Records"},
                                                                    {0xEE, "IGS"},
                                                                    {0xF0, "A Wave"},
                                                                    {0xF3, "Extreme Entertainment"},
                                                                    {0xFF, "LJN"}};

const std::unordered_map<std::string, std::string> GB::new_licensees = {{"00", "None"},
                                                                        {"01", "Nintendo Research & Development 1"},
                                                                        {"08", "Capcom"},
                                                                        {"13", "EA (Electronic Arts)"},
                                                                        {"18", "Hudson Soft"},
                                                                        {"19", "B-AI"},
                                                                        {"20", "KSS"},
                                                                        {"22", "Planning Office WADA"},
                                                                        {"24", "PCM Complete"},
                                                                        {"25", "San-X"},
                                                                        {"28", "Kemco"},
                                                                        {"29", "SETA Corporation"},
                                                                        {"30", "Viacom"},
                                                                        {"31", "Nintendo"},
                                                                        {"32", "Bandai"},
                                                                        {"33", "Ocean Software/Acclaim Entertainment"},
                                                                        {"34", "Konami"},
                                                                        {"35", "HectorSoft"},
                                                                        {"37", "Taito"},
                                                                        {"38", "Hudson Soft"},
                                                                        {"39", "Banpresto"},
                                                                        {"41", "Ubi Soft1"},
                                                                        {"42", "Atlus"},
                                                                        {"44", "Malibu Interactive"},
                                                                        {"46", "Angel"},
                                                                        {"47", "Bullet-Proof Software2"},
                                                                        {"49", "Irem"},
                                                                        {"50", "Absolute"},
                                                                        {"51", "Acclaim Entertainment"},
                                                                        {"52", "Activision"},
                                                                        {"53", "Sammy USA Corporation"},
                                                                        {"54", "Konami"},
                                                                        {"55", "Hi Tech Expressions"},
                                                                        {"56", "LJN"},
                                                                        {"57", "Matchbox"},
                                                                        {"58", "Mattel"},
                                                                        {"59", "Milton Bradley Company"},
                                                                        {"60", "Titus Interactive"},
                                                                        {"61", "Virgin Games Ltd.3"},
                                                                        {"64", "Lucasfilm Games4"},
                                                                        {"67", "Ocean Software"},
                                                                        {"69", "EA (Electronic Arts)"},
                                                                        {"70", "Infogrames5"},
                                                                        {"71", "Interplay Entertainment"},
                                                                        {"72", "Broderbund"},
                                                                        {"73", "Sculptured Software6"},
                                                                        {"75", "The Sales Curve Limited7"},
                                                                        {"78", "THQ"},
                                                                        {"79", "Accolade8"},
                                                                        {"80", "Misawa Entertainment"},
                                                                        {"83", "LOZC G."},
                                                                        {"86", "Tokuma Shoten"},
                                                                        {"87", "Tsukuda Original"},
                                                                        {"91", "Chunsoft Co.9"},
                                                                        {"92", "Video System"},
                                                                        {"93", "Ocean Software/Acclaim Entertainment"},
                                                                        {"95", "Varie"},
                                                                        {"96", "Yonezawa10/S'Pal"},
                                                                        {"97", "Kaneko"},
                                                                        {"99", "Pack-In-Video"},
                                                                        {"9H", "Bottom Up"},
                                                                        {"A4", "Konami (Yu-Gi-Oh!)"},
                                                                        {"BL", "MTO"},
                                                                        {"DK", "Kodansha"}};
