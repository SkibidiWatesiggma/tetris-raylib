#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 30

#define BOARD_X 190
#define BOARD_Y 60

#define MAX_USERNAME 10

#define SCORE_FILE "scores.txt"
#define HIGH_SCORE_FILE "highscore.txt"

typedef enum {
    MENU,
    USERNAME,
    PLAYING,
    GAME_OVER
} GameState;

typedef struct {
    int type;
    int rotation;
    int x;
    int y;
} Piece;

static int board[BOARD_HEIGHT][BOARD_WIDTH];

static const int shapes[7][4][4][4] = {
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
        {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}
    },
    {
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
    },
    {
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    },
    {
        {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
    },
    {
        {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
        {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
    },
    {
        {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    },
    {
        {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}
    }
};

static const unsigned char paletteTable[10][4] = {
    {0x0F,0x30,0x21,0x12},
    {0x0F,0x30,0x29,0x1A},
    {0x0F,0x30,0x24,0x14},
    {0x0F,0x30,0x2A,0x12},
    {0x0F,0x30,0x2B,0x15},
    {0x0F,0x30,0x22,0x2B},
    {0x0F,0x30,0x00,0x16},
    {0x0F,0x30,0x05,0x13},
    {0x0F,0x30,0x16,0x12},
    {0x0F,0x30,0x27,0x16}
};

static const Color nesColors[64] = {
    {84,84,84,255},{0,30,116,255},{8,16,144,255},
    {48,0,136,255},{68,0,100,255},{92,0,48,255},
    {84,4,0,255},{60,24,0,255},{32,42,0,255},
    {8,58,0,255},{0,64,0,255},{0,60,0,255},
    {0,50,60,255},{0,0,0,255},{0,0,0,255},{0,0,0,255},
    {152,150,152,255},{8,76,196,255},{48,50,236,255},
    {92,30,228,255},{136,20,176,255},{160,20,100,255},
    {152,34,32,255},{120,60,0,255},{84,90,0,255},
    {40,114,0,255},{8,124,0,255},{0,118,40,255},
    {0,102,120,255},{0,0,0,255},{0,0,0,255},{0,0,0,255},
    {236,238,236,255},{76,154,236,255},{120,124,236,255},
    {176,98,228,255},{228,84,180,255},{236,88,124,255},
    {236,106,100,255},{212,136,32,255},{160,170,0,255},
    {116,196,0,255},{76,208,32,255},{56,204,108,255},
    {56,180,204,255},{60,60,60,255},{0,0,0,255},{0,0,0,255},
    {236,238,236,255},{168,204,236,255},{188,188,236,255},
    {212,178,236,255},{236,174,212,255},{236,174,176,255},
    {236,180,168,255},{236,200,132,255},{204,210,120,255},
    {180,222,120,255},{152,226,144,255},{144,224,180,255},
    {144,204,212,255},{120,120,120,255},{0,0,0,255},{0,0,0,255}
};

static int ClampColor(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

static Color GetNESColor(unsigned char index)
{
    return nesColors[index & 0x3F];
}

static bool CanPlace(Piece p)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (!shapes[p.type][p.rotation][y][x])
                continue;

            int bx = p.x + x;
            int by = p.y + y;

            if (bx < 0 || bx >= BOARD_WIDTH)
                return false;

            if (by >= BOARD_HEIGHT)
                return false;

            if (by >= 0 && board[by][bx])
                return false;
        }
    }

    return true;
}

static void LockPiece(Piece p)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (!shapes[p.type][p.rotation][y][x])
                continue;

            int bx = p.x + x;
            int by = p.y + y;

            if (bx >= 0 && bx < BOARD_WIDTH &&
                by >= 0 && by < BOARD_HEIGHT)
            {
                board[by][bx] = p.type + 1;
            }
        }
    }
}

static int ClearLines(void)
{
    int cleared = 0;

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--)
    {
        bool full = true;

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (!board[y][x])
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            cleared++;

            for (int row = y; row > 0; row--)
            {
                for (int x = 0; x < BOARD_WIDTH; x++)
                    board[row][x] = board[row - 1][x];
            }

            for (int x = 0; x < BOARD_WIDTH; x++)
                board[0][x] = 0;

            y++;
        }
    }

    return cleared;
}

static int GetGravityFrames(int level)
{
    static const int gravity[30] = {
        48,43,38,33,28,
        23,18,13,8,6,
        5,5,5,4,4,
        4,3,3,3,2,
        2,2,2,2,2,
        2,2,2,2,1
    };

    if (level >= 29)
        return 1;

    return gravity[level];
}

static int GetLevelTransitionLines(int level)
{
    if (level <= 8)
        return (level + 1) * 10;

    if (level <= 15)
        return 100;

    if (level <= 25)
        return level + 94;

    return 200;
}

static uint8_t NextPieceType(uint8_t previous)
{
    int type;

    do
    {
        type = GetRandomValue(0, 6);
    }
    while (type == previous);

    return (uint8_t)type;
}

static Piece MakePiece(int type)
{
    Piece p;

    p.type = type;
    p.rotation = 0;
    p.x = 3;
    p.y = 0;

    return p;
}

static void ResetGame(
    Piece *current,
    Piece *next,
    uint8_t *previous,
    int *level,
    int *lines,
    uint64_t *score,
    double *gravityTimer,
    double *dasTimer,
    int *dasDirection,
    bool *dasActive
)
{
    memset(board, 0, sizeof(board));

    *level = 0;
    *lines = 0;
    *score = 0;

    *previous = 7;

    int first = NextPieceType(*previous);
    *previous = (uint8_t)first;

    int second = NextPieceType(*previous);
    *previous = (uint8_t)second;

    *current = MakePiece(first);
    *next = MakePiece(second);

    *gravityTimer = 0.0;
    *dasTimer = 0.0;
    *dasDirection = 0;
    *dasActive = false;
}

static void SpawnNext(
    Piece *current,
    Piece *next,
    uint8_t *previous
)
{
    *current = *next;

    int type = NextPieceType(*previous);

    *next = MakePiece(type);
    *previous = (uint8_t)type;
}

static bool RotatePiece(Piece *p)
{
    Piece test = *p;

    test.rotation =
        (test.rotation + 1) % 4;

    if (CanPlace(test))
    {
        *p = test;
        return true;
    }

    test.x--;

    if (CanPlace(test))
    {
        *p = test;
        return true;
    }

    test.x += 2;

    if (CanPlace(test))
    {
        *p = test;
        return true;
    }

    test.x--;

    if (CanPlace(test))
    {
        *p = test;
        return true;
    }

    return false;
}

static void AwardLineScore(
    uint64_t *score,
    int cleared,
    int level
)
{
    uint64_t base = 0;

    if (cleared == 1)
        base = 40;
    else if (cleared == 2)
        base = 100;
    else if (cleared == 3)
        base = 300;
    else if (cleared == 4)
        base = 1200;

    *score += base * (uint64_t)(level + 1);
}

static uint32_t LoadHighScore(void)
{
    FILE *f = fopen(HIGH_SCORE_FILE, "r");

    if (!f)
        return 0;

    uint32_t value = 0;

    fscanf(f, "%u", &value);

    fclose(f);

    return value;
}

static void SaveHighScore(uint64_t score)
{
    FILE *f = fopen(HIGH_SCORE_FILE, "w");

    if (!f)
        return;

    fprintf(
        f,
        "%llu\n",
        (unsigned long long)score
    );

    fclose(f);
}

static void SaveScore(
    const char *username,
    uint64_t score
)
{
    FILE *f = fopen(SCORE_FILE, "a");

    if (!f)
        return;

    fprintf(
        f,
        "%s, %llu\n",
        username,
        (unsigned long long)score
    );

    fclose(f);
}

static uint64_t LoadLargestScore(void)
{
    FILE *f = fopen(SCORE_FILE, "r");

    if (!f)
        return 0;

    char line[128];
    uint64_t best = 0;

    while (fgets(line, sizeof(line), f))
    {
        char name[32];
        unsigned long long value;

        if (sscanf(
                line,
                "%31[^,], %llu",
                name,
                &value
            ) == 2)
        {
            if ((uint64_t)value > best)
                best = (uint64_t)value;
        }
    }

    fclose(f);

    return best;
}

static Color GetPieceColor(int type, int level)
{
    uint8_t index = (uint8_t)level;

    for (;;)
    {
        uint8_t difference = (uint8_t)(index - 10);

        if ((int8_t)difference < 0)
            break;

        index = difference;
    }

    uint8_t offset = (uint8_t)(index << 2);

    static const uint8_t paletteROM[] = {
        0x0F, 0x30, 0x21, 0x12,
        0x0F, 0x30, 0x29, 0x1A,
        0x0F, 0x30, 0x24, 0x14,
        0x0F, 0x30, 0x2A, 0x12,
        0x0F, 0x30, 0x2B, 0x15,
        0x0F, 0x30, 0x22, 0x2B,
        0x0F, 0x30, 0x00, 0x16,
        0x0F, 0x30, 0x05, 0x13,
        0x0F, 0x30, 0x16, 0x12,
        0x0F, 0x30, 0x27, 0x16,
        0x60,
        0xE6, 0x49, 0xA5, 0x49, 0xC9, 0x14,
        0x30, 0x06, 0xA9, 0x20, 0x85, 0x49,
        0xE6, 0x89, 0xA5, 0x89, 0xC9, 0x14,
        0x30, 0x06, 0xA9, 0x20, 0x85, 0x89,
        0x60,
        0x00
    };

    if ((size_t)offset + 3 >= sizeof(paletteROM))
        offset = (uint8_t)(offset % 40);

    uint8_t color1 = paletteROM[offset + 2];
    uint8_t color2 = paletteROM[offset + 3];

    if (type == 0 || type == 1 || type == 2)
        return GetNESColor(color2);

    if (type == 3 || type == 6)
        return GetNESColor(color1);

    return GetNESColor(color2);
}

static void DrawBlock(
    int x,
    int y,
    int type,
    int level,
    int size,
    int originX,
    int originY
)
{
    Color main =
        GetPieceColor(type, level);

    int px = originX + x * size;
    int py = originY + y * size;

    DrawRectangle(
        px + 2,
        py + 2,
        size - 4,
        size - 4,
        main
    );

    Color light = {
        (unsigned char)ClampColor(main.r + 45),
        (unsigned char)ClampColor(main.g + 45),
        (unsigned char)ClampColor(main.b + 45),
        255
    };

    DrawRectangle(
        px + 5,
        py + 5,
        size - 10,
        4,
        light
    );
}

static void DrawBoard(int level)
{
    DrawRectangle(
        BOARD_X - 4,
        BOARD_Y - 4,
        BOARD_WIDTH * CELL_SIZE + 8,
        BOARD_HEIGHT * CELL_SIZE + 8,
        DARKGRAY
    );

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            DrawRectangleLines(
                BOARD_X + x * CELL_SIZE,
                BOARD_Y + y * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE,
                (Color){20,20,20,255}
            );

            if (board[y][x])
            {
                DrawBlock(
                    x,
                    y,
                    board[y][x] - 1,
                    level,
                    CELL_SIZE,
                    BOARD_X,
                    BOARD_Y
                );
            }
        }
    }
}

static void DrawPiece(
    Piece p,
    int level
)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (!shapes[p.type][p.rotation][y][x])
                continue;

            int bx = p.x + x;
            int by = p.y + y;

            if (bx < 0 ||
                bx >= BOARD_WIDTH ||
                by < 0 ||
                by >= BOARD_HEIGHT)
                continue;

            DrawBlock(
                bx,
                by,
                p.type,
                level,
                CELL_SIZE,
                BOARD_X,
                BOARD_Y
            );
        }
    }
}

static void DrawNextPiece(
    Piece next,
    int level
)
{
    int boxX = 555;
    int boxY = 90;
    int boxW = 190;
    int boxH = 155;

    DrawText(
        "NEXT",
        boxX,
        55,
        24,
        WHITE
    );

    DrawRectangle(
        boxX,
        boxY,
        boxW,
        boxH,
        (Color){15,15,15,255}
    );

    DrawRectangleLines(
        boxX,
        boxY,
        boxW,
        boxH,
        DARKGRAY
    );

    int size = 28;

    int originX =
        boxX + boxW / 2 - size * 2;

    int originY =
        boxY + boxH / 2 - size * 2;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (!shapes[next.type][0][y][x])
                continue;

            DrawBlock(
                x,
                y,
                next.type,
                level,
                size,
                originX,
                originY
            );
        }
    }
}

static void DrawCentered(
    const char *text,
    int y,
    int size,
    Color color
)
{
    int width =
        MeasureText(text, size);

    DrawText(
        text,
        (SCREEN_WIDTH - width) / 2,
        y,
        size,
        color
    );
}

static void FormatScore(
    uint64_t score,
    char *buffer,
    int size
)
{
    if (score <= 999999)
    {
        snprintf(
            buffer,
            size,
            "%06llu",
            (unsigned long long)score
        );

        return;
    }

    const char digits[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char temp[64];

    int pos = 0;

    while (score > 0 && pos < 63)
    {
        temp[pos++] =
            digits[score % 36];

        score /= 36;
    }

    if (pos == 0)
        temp[pos++] = '0';

    int out = 0;

    for (int i = pos - 1;
         i >= 0 && out < size - 1;
         i--)
    {
        buffer[out++] = temp[i];
    }

    buffer[out] = '\0';
}

int main(void)
{
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "NES Tetris"
    );

    SetTargetFPS(0);

    GameState state = MENU;

    char username[MAX_USERNAME + 1] = {0};
    int usernameLength = 0;

    uint64_t score = 0;

    uint32_t highScore =
        LoadHighScore();

    uint64_t fileHighScore =
        LoadLargestScore();

    if (fileHighScore > highScore)
    {
        highScore =
            fileHighScore > UINT32_MAX
                ? UINT32_MAX
                : (uint32_t)fileHighScore;
    }

    int level = 0;
    int lines = 0;

    Piece current = {0};
    Piece next = {0};

    uint8_t previousPiece = 7;

    double gravityTimer = 0.0;
    double dasTimer = 0.0;

    int dasDirection = 0;
    bool dasActive = false;

    bool saved = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (dt > 0.25f)
            dt = 0.25f;

        if (state == MENU)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                username[0] = '\0';
                usernameLength = 0;
                state = USERNAME;
            }
        }
        else if (state == USERNAME)
        {
            int key = GetCharPressed();

            while (key > 0)
            {
                if (usernameLength < MAX_USERNAME)
                {
                    if (key >= 'a' && key <= 'z')
                        key -= 32;

                    if (
                        (key >= 'A' && key <= 'Z') ||
                        (key >= '0' && key <= '9')
                    )
                    {
                        username[usernameLength++] =
                            (char)key;

                        username[usernameLength] =
                            '\0';
                    }
                }

                key = GetCharPressed();
            }

            if (
                IsKeyPressed(KEY_BACKSPACE) &&
                usernameLength > 0
            )
            {
                usernameLength--;
                username[usernameLength] = '\0';
            }

            if (
                IsKeyPressed(KEY_ENTER) &&
                usernameLength > 0
            )
            {
                ResetGame(
                    &current,
                    &next,
                    &previousPiece,
                    &level,
                    &lines,
                    &score,
                    &gravityTimer,
                    &dasTimer,
                    &dasDirection,
                    &dasActive
                );

                saved = false;
                state = PLAYING;
            }
        }
        else if (state == PLAYING)
        {
            if (IsKeyPressed(KEY_W))
                RotatePiece(&current);

            if (IsKeyPressed(KEY_SPACE))
            {
                int distance = 0;

                while (true)
                {
                    Piece test = current;
                    test.y++;

                    if (!CanPlace(test))
                        break;

                    current = test;
                    distance++;
                }

                score +=
                    (uint64_t)distance * 2;

                LockPiece(current);

                int cleared = ClearLines();

                if (cleared > 0)
                {
                    lines += cleared;

                    AwardLineScore(
                        &score,
                        cleared,
                        level
                    );

                    int threshold =
                        GetLevelTransitionLines(level);

                    while (
                        lines >= threshold &&
                        level < 255
                    )
                    {
                        level++;

                        threshold =
                            GetLevelTransitionLines(level);
                    }
                }

                SpawnNext(
                    &current,
                    &next,
                    &previousPiece
                );

                gravityTimer = 0.0;
                dasTimer = 0.0;
                dasActive = false;

                if (!CanPlace(current))
                    state = GAME_OVER;
            }

            bool left = IsKeyDown(KEY_A);
            bool right = IsKeyDown(KEY_D);

            int direction = 0;

            if (left && !right)
                direction = -1;
            else if (right && !left)
                direction = 1;

            if (direction == 0)
            {
                dasTimer = 0.0;
                dasActive = false;
                dasDirection = 0;
            }
            else
            {
                bool justPressed =
                    IsKeyPressed(KEY_A) ||
                    IsKeyPressed(KEY_D);

                if (justPressed)
                {
                    Piece test = current;
                    test.x += direction;

                    if (CanPlace(test))
                        current = test;

                    dasTimer = 0.0;
                    dasActive = true;
                    dasDirection = direction;
                }
                else if (
                    dasActive &&
                    direction == dasDirection
                )
                {
                    dasTimer += dt;

                    const double initialDelay = 0.20;
                    const double repeatDelay = 0.08;

                    if (dasTimer >= initialDelay)
                    {
                        double repeatTime =
                            dasTimer - initialDelay;

                        int moves =
                            (int)(repeatTime / repeatDelay);

                        if (moves > 0)
                        {
                            if (moves > 4)
                                moves = 4;

                            for (int i = 0; i < moves; i++)
                            {
                                Piece test = current;
                                test.x += direction;

                                if (!CanPlace(test))
                                    break;

                                current = test;
                            }

                            dasTimer =
                                initialDelay +
                                fmod(
                                    repeatTime,
                                    repeatDelay
                                );
                        }
                    }
                }
            }

            int gravityFrames =
                GetGravityFrames(level);

            double gravityDelay =
                gravityFrames / 60.0988;

            if (IsKeyDown(KEY_S))
                gravityDelay = 1.0 / 7.0;

            gravityTimer += dt;

            while (gravityTimer >= gravityDelay)
            {
                gravityTimer -= gravityDelay;

                Piece test = current;
                test.y++;

                if (CanPlace(test))
                {
                    current = test;

                    if (IsKeyDown(KEY_S))
                        score++;
                }
                else
                {
                    LockPiece(current);

                    int cleared = ClearLines();

                    if (cleared > 0)
                    {
                        lines += cleared;

                        AwardLineScore(
                            &score,
                            cleared,
                            level
                        );

                        int threshold =
                            GetLevelTransitionLines(level);

                        while (
                            lines >= threshold &&
                            level < 255
                        )
                        {
                            level++;

                            threshold =
                                GetLevelTransitionLines(level);
                        }
                    }

                    SpawnNext(
                        &current,
                        &next,
                        &previousPiece
                    );

                    gravityTimer = 0.0;

                    if (!CanPlace(current))
                        state = GAME_OVER;

                    break;
                }
            }

            if (score > highScore)
            {
                highScore =
                    score > UINT32_MAX
                        ? UINT32_MAX
                        : (uint32_t)score;

                SaveHighScore(score);
            }
        }
        else if (state == GAME_OVER)
        {
            if (!saved)
            {
                SaveScore(
                    username,
                    score
                );

                saved = true;
            }

            if (IsKeyPressed(KEY_ENTER))
                state = MENU;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        if (state == MENU)
        {
            DrawCentered(
                "TETRIS",
                110,
                80,
                WHITE
            );

            DrawCentered(
                "NES-STYLE CHAOS",
                205,
                24,
                GREEN
            );

            DrawCentered(
                "ENTER TO START",
                330,
                30,
                WHITE
            );

            DrawCentered(
                "A / D   MOVE",
                420,
                19,
                LIGHTGRAY
            );

            DrawCentered(
                "W       ROTATE",
                450,
                19,
                LIGHTGRAY
            );

            DrawCentered(
                "S       SOFT DROP",
                480,
                19,
                LIGHTGRAY
            );

            DrawCentered(
                "SPACE   HARD DROP",
                510,
                19,
                LIGHTGRAY
            );

            char hs[32];

            FormatScore(
                highScore,
                hs,
                sizeof(hs)
            );

            DrawCentered(
                TextFormat(
                    "HIGH SCORE  %s",
                    hs
                ),
                620,
                22,
                GREEN
            );
        }
        else if (state == USERNAME)
        {
            DrawCentered(
                "USERNAME",
                150,
                44,
                WHITE
            );

            DrawRectangle(
                SCREEN_WIDTH / 2 - 220,
                250,
                440,
                75,
                DARKGRAY
            );

            int width =
                MeasureText(
                    username,
                    34
                );

            DrawText(
                username,
                SCREEN_WIDTH / 2 - width / 2,
                268,
                34,
                GREEN
            );

            DrawCentered(
                TextFormat(
                    "%d / %d",
                    usernameLength,
                    MAX_USERNAME
                ),
                355,
                18,
                GRAY
            );

            DrawCentered(
                "A-Z AND 0-9 ONLY",
                410,
                20,
                WHITE
            );

            DrawCentered(
                "ENTER TO PLAY",
                450,
                22,
                GREEN
            );
        }
        else
        {
            DrawBoard(level);

            if (state == PLAYING)
                DrawPiece(current, level);

            DrawNextPiece(
                next,
                level
            );

            char scoreText[64];
            char highText[64];

            FormatScore(
                score,
                scoreText,
                sizeof(scoreText)
            );

            FormatScore(
                highScore,
                highText,
                sizeof(highText)
            );

            DrawText(
                "SCORE",
                555,
                270,
                22,
                WHITE
            );

            DrawText(
                scoreText,
                555,
                300,
                34,
                GREEN
            );

            DrawText(
                "HIGH",
                555,
                355,
                22,
                WHITE
            );

            DrawText(
                highText,
                555,
                385,
                34,
                GREEN
            );

            DrawText(
                TextFormat(
                    "LEVEL %d",
                    level
                ),
                555,
                440,
                25,
                WHITE
            );

            DrawText(
                TextFormat(
                    "LINES %d",
                    lines
                ),
                555,
                475,
                23,
                WHITE
            );

            DrawText(
                username,
                555,
                510,
                23,
                GREEN
            );

            DrawText(
                "A / D  MOVE",
                555,
                555,
                18,
                WHITE
            );

            DrawText(
                "W      ROTATE",
                555,
                580,
                18,
                WHITE
            );

            DrawText(
                "S      SOFT DROP",
                555,
                605,
                18,
                WHITE
            );

            DrawText(
                "SPACE  HARD DROP",
                555,
                630,
                18,
                WHITE
            );

            if (level >= 138)
            {
                DrawText(
                    "GLITCHED PALETTE",
                    555,
                    665,
                    15,
                    RED
                );
            }

            DrawText(
                TextFormat(
                    "FPS: %d",
                    GetFPS()
                ),
                12,
                12,
                20,
                GREEN
            );

            if (state == GAME_OVER)
            {
                DrawRectangle(
                    BOARD_X,
                    285,
                    BOARD_WIDTH * CELL_SIZE,
                    130,
                    (Color){0,0,0,235}
                );

                DrawCentered(
                    "GAME OVER",
                    305,
                    38,
                    GREEN
                );

                DrawCentered(
                    "ENTER FOR MENU",
                    355,
                    19,
                    WHITE
                );
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}