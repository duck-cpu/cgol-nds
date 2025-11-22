#include <stdio.h>
#include <nds.h>
#include <time.h>
#include <nds/arm9/console.h>

PrintConsole bot_console;
PrintConsole top_console;

// 32x48 grids
static int grid_prev[48][32] = {0}; // [y][x]
static int grid_curr[48][32] = {0};
static int grid_next[48][32] = {0};

int main(void)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(
        &bot_console,
        0,
        BgType_Text4bpp,
        BgSize_T_256x256,
        31,
        0,
        false,
        true);

    consoleInit(
        &top_console,
        0,
        BgType_Text4bpp,
        BgSize_T_256x256,
        30,
        1,
        true,
        true);

    // seed
    srand(time(NULL));

    struct life
    {
        int state; // 1 or 0
        int neighbors;
    };

    iprintf("PRESS START.");

    while (1)
    {
        swiWaitForVBlank();
        swiDelay(1000000);
        // read buttons
        scanKeys();
        int keys = keysDown();

        // start and reset
        if (keys & KEY_START)
        {
            consoleSelect(&bot_console);
            iprintf("\x1b[2J");
            iprintf("\x1b[H");

            consoleSelect(&top_console);
            iprintf("\x1b[2J");
            iprintf("\x1b[H");

            memset(grid_prev, 0, sizeof(grid_prev));
            memset(grid_curr, 0, sizeof(grid_curr));
            memset(grid_next, 0, sizeof(grid_next));

            // seed
            for (int y = 0; y < 48; y++)
                for (int x = 0; x < 32; x++)
                    grid_curr[y][x] = (rand() % 4 == 0);

            memcpy(grid_prev, grid_curr, sizeof(grid_prev));
        }

        struct life cell;

        // simulate
        // life cycle loop
        for (int y = 0; y < 48; y++)
        {
            for (int x = 0; x < 32; x++)
            {
                cell.neighbors = 0;
                cell.state = grid_curr[y][x];
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        if (dx == 0 && dy == 0)
                            continue;

                        int ny = y + dy;
                        int nx = x + dx;

                        if (ny >= 0 && ny < 48 && nx >= 0 && nx < 32 && grid_curr[ny][nx] == 1)
                            cell.neighbors++;
                    }
                }
                if (cell.state == 1)
                {
                    // alive
                    if (cell.neighbors == 2 || cell.neighbors == 3)
                        grid_next[y][x] = 1;
                    else
                        grid_next[y][x] = 0;
                }
                else
                {
                    // dead
                    if (cell.neighbors == 3)
                        grid_next[y][x] = 1;
                    else
                        grid_next[y][x] = 0;
                }
            }
        }
        // promote
        memcpy(grid_curr, grid_next, sizeof(grid_curr));

        // draw
        // draw loop
        for (int i = 0; i < 48; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                if (grid_curr[i][j] != grid_prev[i][j])
                {
                    int screen_row;

                    if (i < 24)
                    {
                        consoleSelect(&top_console);
                        screen_row = i;
                    }
                    else
                    {
                        consoleSelect(&bot_console);
                        screen_row = i - 24;
                    }
                    iprintf("\x1b[%d;%dH", screen_row + 1, j + 1);
                    if (grid_curr[i][j] == 1)
                        iprintf(".");
                    else
                        iprintf(" ");
                }
            }
        }
        // sync
        memcpy(grid_prev, grid_curr, sizeof(grid_prev));
    }
    return 0;
}
