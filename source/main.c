#include <stdio.h>
#include <nds.h>
#include <time.h>

int main(void)
{
    // seed
    srand(time(NULL));

    consoleDemoInit();

    // 32x24 grids
    int grid_prev[24][32] = {0}; // [y][x]
    int grid_curr[24][32] = {0};
    int grid_next[24][32] = {0};

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
            iprintf("\x1b[2J");
            iprintf("\x1b[H");

            memset(grid_prev, 0, sizeof(grid_prev));
            memset(grid_curr, 0, sizeof(grid_curr));
            memset(grid_next, 0, sizeof(grid_next));

            // seed
            for (int y = 0; y < 24; y++)
                for (int x = 0; x < 32; x++)
                    grid_curr[y][x] = (rand() % 4 == 0);

            memcpy(grid_prev, grid_curr, sizeof(grid_prev));
        }

        struct life cell;

        // simulate
        // life cycle loop
        for (int y = 0; y < 24; y++)
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

                        if (ny >= 0 && ny < 24 && nx >= 0 && nx < 32 && grid_curr[ny][nx] == 1)
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
        for (int i = 0; i < 24; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                if (grid_curr[i][j] != grid_prev[i][j])
                {
                    iprintf("\x1b[%d;%dH", i + 1, j + 1);
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
