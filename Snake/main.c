#include <stdio.h>
#include <windows.h>
#include <time.h>

#define LENGTH_SIZE 40
#define WIDTH_SIZE 50
#define SECOND 1000
#define SECONDS_TO_FRAME 0.25

//------------------------------------------------------------------------------------------------------------
enum Direction
{
	D_Left,
	D_Right,
	D_Up,
	D_Down
} snake_direction = D_Right;

typedef struct
{
	char x;
	char y;
} Vector2;

HANDLE h_stdin, h_thread;
DWORD old_mode, new_mode, num_read;
INPUT_RECORD input[128];

void Init_Level(char *level, int snake_cell, int fruit_cell);
void Draw_Level(char *level);
void Snake_Check_Direction(int *step);
void Create_Console_Mode();

int Redraw_Snake_Position(char *level, int *snake_tail, int *snake_cells, int step);
int Generate_Fruit_Slot();
DWORD WINAPI Input_Check(LPVOID arg);

//------------------------------------------------------------------------------------------------------------
int main(void)
{
	int step_to_move = 0;
	int snake_tail = 1;
	Vector2 snake_pos = { .x = 5, .y = 5 };

	char level[LENGTH_SIZE][WIDTH_SIZE];
	int snake_cells[100]; // save all snake cell positions(including a head)

	snake_cells[0] = snake_pos.y * WIDTH_SIZE + snake_pos.x;
	step_to_move = snake_cells[0];

	srand((unsigned int)(time(NULL)));

	Create_Console_Mode();
	Init_Level(&level[0][0], snake_cells[0], Generate_Fruit_Slot());
	h_thread = CreateThread(NULL, 0, Input_Check, NULL, 0, NULL);

	while (1)
	{
		system("cls");

		Snake_Check_Direction(&step_to_move);
		if (Redraw_Snake_Position(&level[0][0], &snake_tail, snake_cells,step_to_move) == 1)
			return 0;

		Draw_Level(&level[0][0]);

		Sleep((DWORD)(SECONDS_TO_FRAME * SECOND));
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
void Init_Level(char *level, int snake_cell, int fruit_cell)
{
	int i = 0, j = 0;
	for (i = 0; i < LENGTH_SIZE; i++)
	{
		for (j = 0; j < WIDTH_SIZE; j++)
		{ 
			level[i * WIDTH_SIZE + j] = '/';

			if ((i * WIDTH_SIZE + j) == fruit_cell)
				level[i * WIDTH_SIZE + j] = '4';

			if (j == 0 || j == WIDTH_SIZE - 1)
				level[i * WIDTH_SIZE + j] = '|';

			if (i == 0 || i == LENGTH_SIZE - 1)
				level[i * WIDTH_SIZE + j] = '-';

			if ((i * WIDTH_SIZE + j) == snake_cell)
				level[i * WIDTH_SIZE + j] = '0';
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void Draw_Level(char *level)
{
	int i = 0, j = 0;
	for (i = 0; i < LENGTH_SIZE; i++)
	{
		for (j = 0; j < WIDTH_SIZE; j++)
		{
			putchar(level[i * WIDTH_SIZE + j]);
			if (j == WIDTH_SIZE - 1)
				puts("");
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void Snake_Check_Direction(int *step)
{
	int direction_to_cell = 0;

	switch (snake_direction)
	{
	case D_Right:
		direction_to_cell = 1;
		break;
	case D_Left:
		direction_to_cell = -1;
		break;
	case D_Up:
		direction_to_cell = -WIDTH_SIZE;
		break;
	case D_Down:
		direction_to_cell = WIDTH_SIZE;
		break;
	}

	(*step) += direction_to_cell;
}
//------------------------------------------------------------------------------------------------------------
int Redraw_Snake_Position(char* level, int* snake_tail, int *snake_cells ,int step)
{
	int i = 0;

	if (level[step] == '|' || level[step] == '-' || level[step] == '0')
		return 1;

	if (level[step] == '4')
	{
		(*snake_tail)++;
		level[Generate_Fruit_Slot()] = '4';
	}

	for (i = *snake_tail; i > 0; i--)
	{
		snake_cells[i] = snake_cells[i - 1];
		level[snake_cells[i]] = '0';
	}

	snake_cells[0] = step;
	level[snake_cells[0]] = '0';
	level[snake_cells[*snake_tail]] = '/';

	return 0;
}
//------------------------------------------------------------------------------------------------------------
DWORD WINAPI Input_Check(LPVOID arg)
{
	while (1)
	{
		if (!SetConsoleMode(h_stdin, new_mode))
			puts("Set mode ERROR!!!!!!!!!!!!!!!!!!!!!!");

		if (!ReadConsoleInput(h_stdin, input, 128, &num_read))
			puts("Read input ERROR!!!!!!!!!!!!!!!!!!!!!!");

		for (int i = 0; i < (int)num_read; i++)
		{
			if (input[i].EventType == KEY_EVENT)
			{
				switch (input[i].Event.KeyEvent.wVirtualKeyCode)
				{
				case VK_RIGHT:
					snake_direction = D_Right;
					break;
				case VK_LEFT:
					snake_direction = D_Left;
					break;
				case VK_UP:
					snake_direction = D_Up;
					break;
				case VK_DOWN:
					snake_direction = D_Down;
					break;
				}
			}
		}
		SetConsoleMode(h_stdin, old_mode);
	}
}
//------------------------------------------------------------------------------------------------------------
void Create_Console_Mode()
{
	h_stdin = GetStdHandle(STD_INPUT_HANDLE);
	if (h_stdin == INVALID_HANDLE_VALUE)
		puts("Get handle ERROR!!!!!!!!!!!!!!!!!!!!!!");

	if (!GetConsoleMode(h_stdin, &old_mode))
		puts("Get mode ERROR!!!!!!!!!!!!!!!!!!!!!!");

	new_mode = old_mode | ENABLE_INSERT_MODE;
}
//------------------------------------------------------------------------------------------------------------
int Generate_Fruit_Slot()
{
	int random = 0;
	while (random < (WIDTH_SIZE - 1) || (random > ((LENGTH_SIZE - 1) * WIDTH_SIZE)) || (random % WIDTH_SIZE) == 0 || (random % (WIDTH_SIZE + 1)) == 0)
	{
		random = rand() % (LENGTH_SIZE * WIDTH_SIZE);
	}
	return random;
}
//------------------------------------------------------------------------------------------------------------
