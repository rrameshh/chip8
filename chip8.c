#include <stdio.h>
#include <stdlib.h>
#include "/usr/local/include/SDL2/SDL.h"
#include "/usr/local/include/SDL2/SDL_render.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>

typedef struct chip8
{
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delaytimer;
    uint8_t soundtimer;
    // uint8_t keypad[16];
    uint8_t screen[64 * 32];
    uint16_t opcode;
    bool drawflag;
    uint8_t keys[16];
} chip8_t;

// bool drawflag;

#define KEY_0 SDLK_x
#define KEY_1 SDLK_1
#define KEY_2 SDLK_2
#define KEY_3 SDLK_3
#define KEY_4 SDLK_q
#define KEY_5 SDLK_w
#define KEY_6 SDLK_e
#define KEY_7 SDLK_a
#define KEY_8 SDLK_s
#define KEY_9 SDLK_d
#define KEY_A SDLK_z
#define KEY_B SDLK_c
#define KEY_C SDLK_4
#define KEY_D SDLK_r
#define KEY_E SDLK_f
#define KEY_F SDLK_v


// uint8_t keys[16];

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void setupSDL()
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Log("SDL initialized successfully.");
}

chip8_t loadGame(const char *filename, chip8_t chip8)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory to store the file content
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * fileSize);
    if (buffer == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Read the file into buffer
    size_t bytesRead = fread(buffer, sizeof(uint8_t), fileSize, file);
    if ((long)bytesRead != fileSize)
    {
        fprintf(stderr, "Error: Failed to read file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Load the game data into Chip-8 memory starting from address 0x200 (512)
    // Only copy up to fileSize bytes
    // for (uint8_t i = 0; i < fileSize; ++i)
    // {
    //     if (i + 0x200 < 4096) {
    //         chip8.memory[0x200 + i] = buffer[i];
    //     } else {
    //         fprintf(stderr, "Error: Game file size exceeds Chip-8 memory capacity.\n");
    //         break;
    //     }c
    // }

    // for (size_t i = 0; i < (size_t)fileSize; ++i)
    // {
    //     printf("%X", buffer[i]);

    // }
    // printf("\n");

    memcpy(&chip8.memory[512], buffer, bytesRead * sizeof(uint8_t));


    free(buffer);
    fclose(file);
    return chip8;
}


void print_debug_info(chip8_t chip8)
{
    printf("======= DEBUG INFO =======\n");

    // Print registers
    printf("Registers:\n");
    for (int i = 0; i < 16; ++i)
    {
        printf("V%X: %X\t", i, chip8.registers[i]);
        if ((i + 1) % 4 == 0)
            printf("\n");
    }
    printf("\n");

    // Print index register, program counter, stack pointer
    printf("Index Register (I): %X\n", chip8.index);
    printf("Program Counter (PC): %X\n", chip8.pc);
    printf("Stack Pointer (SP): %X\n", chip8.sp);

    // Print stack contents
    printf("Stack:\n");
    for (int i = 0; i < 16; ++i)
    {
        printf("%X ", chip8.stack[i]);
    }
    printf("\n");

    // Print delay timer and sound timer
    printf("Delay Timer: %X\n", chip8.delaytimer);
    printf("Sound Timer: %X\n", chip8.soundtimer);

    // Print current opcode
    printf("Current Opcode: %X\n", chip8.opcode);

    // Print keypad state
    printf("Keypad:\n");
    for (int i = 0; i < 16; ++i)
    {
        printf("%X ", chip8.keys[i]);
    }
    printf("\n");

    // Print draw flag state
    printf("Draw Flag: %s\n", chip8.drawflag ? "True" : "False");

    // Print last executed instruction

    // Optionally, you can print the instruction set or disassemble the last opcode executed

    printf("==========================\n");
}


chip8_t emulateCycle(chip8_t chip8)
{
    // printf("chip8.memory[chip8.pc]: %X", chip8.memory[chip8.pc]);

    // printf("chip8.memory[chip8.pc] << 8: %X", chip8.memory[chip8.pc] << 8);
    // printf("chip8.memory[chip8.pc+1]: %X", chip8.memory[chip8.pc+1]);
    chip8.opcode = (uint16_t)(chip8.memory[chip8.pc] << 8) |(uint16_t)(chip8.memory[chip8.pc+1]);
    printf("Fetched Opcode: 0x%X\n", chip8.opcode);
    uint16_t nnn = (chip8.opcode & 0x0FFF);
    uint16_t n = (chip8.opcode & 0x000F);
    uint16_t x = (chip8.opcode & 0x0F00) >> 8;
    uint16_t y = (chip8.opcode & 0x00F0) >> 4;
    uint16_t kk = (chip8.opcode & 0x00FF);
    
    // uint16_t first = chip8.opcode & 0xF000;
    chip8.pc+=2;
    

    switch(chip8.opcode & 0xF000) {
        case (0x0000): {
            if (kk == 0xE0) {
                memset(chip8.screen, 0, sizeof(chip8.screen));
                chip8.drawflag = true;
                break;
            }
            else if (kk == 0xEE)
            {
                chip8.sp--;
                chip8.pc = chip8.stack[chip8.sp];
                //chip8.sp--;
                break;
            }
            break;
        }
        case (0x1000): {
            chip8.pc = nnn;
            break;
        }
        case (0x2000): {
            //chip8.sp++;
            // chip8.stack[chip8.sp] = chip8.pc;
            // chip8.sp++;
            // chip8.pc = nnn;
            chip8.stack[chip8.sp++] = chip8.pc;
            chip8.pc = nnn;
            break;
        }
        case (0x3000): {
            if (chip8.registers[x] == kk)
                chip8.pc+=2;
            break;
        }
        case (0x4000): {
            if (chip8.registers[x] != kk)
                chip8.pc+=2;
            break;
        }
        case (0x5000): {
            if (chip8.registers[x] == chip8.registers[y])
                chip8.pc+=2;
            break;
        }
        case (0x6000): {
            chip8.registers[x] = kk;
            break;
        }
        case (0x7000): {
            chip8.registers[x] += kk;
            break;
        }
        case (0x8000): {
            switch(n) {
                case 0x0000: {
                    chip8.registers[x] = chip8.registers[y];
                    break;
                }
                case 0x0001: {
                    chip8.registers[x] = chip8.registers[x] | chip8.registers[y];
                    break;
                }
                case 0x0002: {
                    chip8.registers[x] = chip8.registers[x] & chip8.registers[y];
                    break;
                }
                case 0x0003: {
                    chip8.registers[x] = chip8.registers[x] ^ chip8.registers[y];
                    break;
                }
                case 0x0004: {
                    chip8.registers[x] += chip8.registers[y];
                    if (chip8.registers[x] > 0xFF || chip8.registers[x] > 255)
                        chip8.registers[0xF] = 1;
                    else   
                        chip8.registers[0xF] = 0;
                    break;


                }
                case 0x0005:{
                    int temp = chip8.registers[x];
                    chip8.registers[x] = chip8.registers[x] - chip8.registers[y];
                    if (temp > chip8.registers[y])
                        chip8.registers[0xF] = 1;
                    else
                        chip8.registers[0x0F] = 0;
                    
                    break;
                }
                case 0x0006: {  
                    chip8.registers[0xF] = (chip8.registers[x] & 0x01);
                    chip8.registers[x] /=2;
                    break;

                }
                case 0x0007: {
                    int temp = chip8.registers[x];
                    chip8.registers[x] = chip8.registers[y] - chip8.registers[x];
                    if (chip8.registers[y] > temp)
                        chip8.registers[0x0F] = 1;
                    else
                        chip8.registers[0x0F] = 0;
                    break;
                }
                case 0x000E: {
                    uint8_t temp = chip8.registers[x];
                    chip8.registers[x] *= 2;
                    if (((temp >> 7) & 0x01) == 0x01)
                        chip8.registers[0xF] = 1;
                    else   
                        chip8.registers[0xF] = 0;
                    break;
                }
            }
            break;
        }
        case (0x9000): {
            if (chip8.registers[x] != chip8.registers[y])
                chip8.pc+=2;
            break;
        }
        case (0xA000): {
            chip8.index = nnn;
            break;
        }
        case (0xB000): {
            chip8.pc = chip8.registers[0x0] + nnn;
            break;
        }
        case (0xC000): { 
            srand(time(NULL));
            int randbyte = rand() % 255;
            chip8.registers[x] = randbyte & kk;
            break;
        }
        case (0xD000): {
            uint16_t sprite;
            chip8.registers[15] = 0;

            // Loop through each row of the sprite
            for (unsigned int row = 0; row < n; row++)
            {
                sprite = chip8.memory[chip8.index + row];
                // Loop through each column of the sprite (8 pixels per row)
                for (unsigned int col = 0; col < 8; col++) {
                    // Check if the pixel in the sprite is set (1)
                    if ((sprite & (0x80 >> col)) != 0) {
                    // Calculate the actual coordinates on the screen, considering wrapping
                    int screenX = (chip8.registers[x] + col) % 64;
                    int screenY = (chip8.registers[y] + row) % 32;

                    // Check collision with existing pixel on the screen
                    if (chip8.screen[screenX + screenY * 64] == 1) {
                        chip8.registers[0xF] = 1;
                    }
                    // Toggle the pixel value (XOR operation to flip between 0 and 1)
                    chip8.screen[screenX + screenY * 64] ^= 1;
                }
                }
            }
            chip8.drawflag = true;
            if (chip8.drawflag){
                chip8.registers[0xF] = 1;
            }
            else   
                chip8.registers[0xF] = 0;

            // Print the screen state for debugging
            // for (int i = 0; i < 64 * 32; i++) {
            //     printf("%X", chip8.screen[i]);
            // }
            break;
        }

        case (0xE000): {
            switch(kk) {
                case 0x009E: {
                    if (chip8.keys[chip8.registers[x]] == 1) // is the shift correct??
                        chip8.pc+=2;
                    break;
                }
                case 0x00A1: {
                    if (chip8.keys[chip8.registers[x]] == 0) // is the shift correct??
                        chip8.pc+=2;
                    break;
                }
            }
            break;
        }
        case (0xF000): {
            switch(chip8.opcode & 0x00FF) {
                printf("made it inside f section");
                case 0x0065: {
                    for (int i = 0; i<x; i++) {
                        chip8.registers[i] = chip8.memory[chip8.index + i];
                    }
                    break;
                }   
                case 0x0007: {
                    chip8.registers[x] = chip8.delaytimer;
                    break;
                }
                case 0x000A: {
                    if (chip8.keys[0]) {
		                chip8.registers[x] = 0;
	                }
	                else if (chip8.keys[1])
	                {
		                chip8.registers[x] = 1;
	                }
	                else if (chip8.keys[2])
	                {
		                chip8.registers[x] = 2;
	                }
	                else if (chip8.keys[3])
	                {
		                chip8.registers[x] = 3;
	                }
	                else if (chip8.keys[4])
	                {
		                chip8.registers[x] = 4;
	                }
	                else if (chip8.keys[5])
	                {
		                chip8.registers[x] = 5;
	                }
	                else if (chip8.keys[6])
	                {
		                chip8.registers[x] = 6;
	                }
	                else if (chip8.keys[7])
	                {
		                chip8.registers[x] = 7;
	                }
	                else if (chip8.keys[8])
	                {
		                chip8.registers[x] = 8;
	                }
	                else if (chip8.keys[9])
	                {
		                chip8.registers[x] = 9;
	                }
	                else if (chip8.keys[10])
	                {
		                chip8.registers[x] = 10;
	                }
	                else if (chip8.keys[11])
	                {
		                chip8.registers[x] = 11;
	                }
	                else if (chip8.keys[12])
	                {
		                chip8.registers[x] = 12;
	                }
	                else if (chip8.keys[13])
	                {
		                chip8.registers[x] = 13;
	                }
	                else if (chip8.keys[14])
	                {
		                chip8.registers[x] = 14;
	                }
	                else if (chip8.keys[15])
	                {
		                chip8.registers[x] = 15;
	                }
	                else
	                {
		                chip8.pc -= 2;
	                }
                    break;
                }
            case 0x0015: {
                chip8.delaytimer = chip8.registers[x];
                break;
            }
            case 0x0018: {
                chip8.soundtimer = chip8.registers[x];
                break;
            }
            case 0x001E: {
                chip8.index = chip8.index + chip8.registers[x];
                break;
            }
            case 0x0029: {
                chip8.index = FONTSET_START_ADDRESS + (5 * chip8.registers[x]);
                break;
            }
            case 0x0033: {
                chip8.memory[chip8.index + 2] = chip8.registers[x]%10;
                chip8.memory[chip8.index + 1] = (chip8.registers[x]/10)%10;
                chip8.memory[chip8.index] = (chip8.registers[x]/100)%10;
                break;
            }
            case 0x0055: {
                for (int i = 0; i<x; i++) {
                    chip8.memory[chip8.index + i] = chip8.registers[i];
                }
                break;
            }
            
            default: {
                 printf("F section, Unknown opcode: 0x%X at PC: 0x%X\n", chip8.opcode, chip8.pc);
                exit(EXIT_FAILURE); // Or handle appropriately based on your emulator's design
            }


        }
        break;
    }
    default: {
        printf("Unknown opcode: 0x%X at PC: 0x%X\n", chip8.opcode, chip8.pc);
        exit(EXIT_FAILURE); // Or handle appropriately based on your emulator's design
    }
    //chip8.pc+=2;

    }
    if (chip8.delaytimer > 0)
        chip8.delaytimer --;
    if (chip8.soundtimer > 0)
    {
        if (chip8.soundtimer == 1)
            printf("beep");
        chip8.soundtimer--;
    }
    return chip8;
}

void drawGraphics(SDL_Renderer *renderer, chip8_t chip8)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen with black
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set draw color to white

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if (chip8.screen[(y * 64) + x] == 1)
            {
                SDL_Rect pixelRect = {x * 10, y * 10, 10, 10}; // Scale up by 10 for visibility
                SDL_RenderFillRect(renderer, &pixelRect); // Draw pixel
            }
        }
    }


    SDL_RenderPresent(renderer); // Present rendered frame

}



int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    setupSDL();

    SDL_Window *window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
    if (window == NULL)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Log("Window created successfully.");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Log("Renderer created successfully.");

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    chip8_t chip8;
    chip8.pc = 0x200;
    chip8.index = 0;
    chip8.sp = 0;
    chip8.opcode = 0;
    for (int i = 0; i < 16; i++)
    {
        chip8.registers[i] = 0;
        chip8.stack[i] = 0;
        chip8.keys[i] = 0;
    }
    // for (size_t i = 0; i < ; ++i)
    // {
    //     chip8.memory[i] = fontset[i];
    // }
    uint8_t zeros[4096] = {0};
    memcpy(chip8.memory, zeros, sizeof(uint8_t) * 4096);
    memcpy(&chip8.memory[0x50], fontset, sizeof(fontset));

    chip8 = loadGame(argv[1], chip8);


   
    int running = 1;
    while (running)
    { 
        // running = handle_input(chip8);
        // if (running == 0) break;
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
                break;
            }

             
            else if (event.type == SDL_KEYDOWN) {
                char chr_down = SDL_GetKeyName(event.key.keysym.sym)[0];
                    // char chr = key[0];
                     switch(chr_down) {
                        case KEY_0: {
                            chip8.keys[0] = 1;
                            break;
                        }
                        case KEY_1: {
                            chip8.keys[1] = 1;
                            break;
                        }
                        case KEY_2: {
                            chip8.keys[2] = 1;
                            break;
                        }
                        case KEY_3: {
                            chip8.keys[3] = 1;
                            break;
                        }
                        case KEY_4: {
                            chip8.keys[4] = 1;
                            break;
                        }
                        case KEY_5: {
                            chip8.keys[5] = 1;
                            break;
                        }
                        case KEY_6: {
                            chip8.keys[6] = 1;
                            break;
                        }
                        case KEY_7: {
                            chip8.keys[7] = 1;
                            break;
                        }
                        case KEY_8: {
                            chip8.keys[8] = 1;
                            break;
                        }
                        case KEY_9: {
                            chip8.keys[9] = 1;
                            break;
                        }
                        case KEY_A: {
                            chip8.keys[10] = 1;
                            break;
                        }
                        case KEY_B: {
                            chip8.keys[11] = 1;
                            break;
                        }
                        case KEY_C: {
                            chip8.keys[12] = 1;
                            break;
                        } 
                        case KEY_E: {
                            chip8.keys[14] = 1;
                            break;
                        }
                        case KEY_D: {
                            chip8.keys[13] = 1;
                            break;
                        }
                        case KEY_F: {
                            chip8.keys[15] = 1;
                            break;
                        }
                        default: break;
  
                    }
                    break;
            }
            else if (event.type == SDL_KEYUP) {
                char chr_up = SDL_GetKeyName(event.key.keysym.sym)[0];
                    // if (strlen(key) > 1) break;
                    switch(chr_up) {
                        case KEY_0: {
                            chip8.keys[0] = 0;
                            break;
                        }
                        case KEY_1: {
                            chip8.keys[1] = 0;
                            break;
                        }
                        case KEY_2: {
                            chip8.keys[2] = 0;
                            break;
                        }
                        case KEY_3: {
                            chip8.keys[3] = 0;
                            break;
                        }
                        case KEY_4: {
                            chip8.keys[4] = 0;
                            break;
                        }
                        case KEY_5: {
                            chip8.keys[5] = 0;
                            break;
                        }
                        case KEY_6: {
                            chip8.keys[6] = 0;
                            break;
                        }
                        case KEY_7: {
                            chip8.keys[7] = 0;
                            break;
                        }
                        case KEY_8: {
                            chip8.keys[8] = 0;
                            break;
                        }
                        case KEY_9: {
                            chip8.keys[9] = 0;
                            break;
                        }
                        case KEY_A: {
                            chip8.keys[10] = 0;
                            break;
                        }
                        case KEY_B: {
                            chip8.keys[11] = 0;
                            break;
                        }
                        case KEY_C: {
                            chip8.keys[12] = 0;
                            break;
                        } 
                        case KEY_E: {
                            chip8.keys[14] = 0;
                            break;
                        }
                        case KEY_D: {
                            chip8.keys[13] = 0;
                            break;
                        }
                        case KEY_F: {
                            chip8.keys[15] = 0;
                            break;
                        }
                        default: break;
                    }
                    break;
            }
            break;  
        } 

        chip8 = emulateCycle(chip8);

        if (chip8.drawflag) {
            drawGraphics(renderer, chip8);
            chip8.drawflag = false;
        }

        printf("Keypad:\n");
        for (int i = 0; i < 16; ++i)
        {
            printf("%X ", chip8.keys[i]);
        }
        printf("\n");
        SDL_Delay(16);
        // printf("%X\n", chip8.opcode);

        // print_debug_info(chip8);
    }

    // for (size_t i = 0; i<4096; i++)
    // {
    //     if (i == 512) printf(" READ MEM: ");
    //     printf("%X ", chip8.memory[i]);
    // }

        

    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    return 0;
}
//}


