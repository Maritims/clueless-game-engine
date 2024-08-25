#include "CluelessEngine.h"
#include "CluelessSnakeGame.h"

typedef struct SnakeFood {
	wchar_t c;
	Color fgColor;
	Color bgColor;
	int points;
} SnakeFood;

typedef struct SnakeFoodInMap {
	SnakeFood snakeFood;
	int x;
	int y;
} SnakeFoodInMap;

typedef struct SnakePoint {
	float x;
	float y;
	struct SnakePoint* next;
} SnakePoint;

static SnakePoint* createSnakePoint(int x, int y) {
	SnakePoint* newSnakePoint = malloc(sizeof(SnakePoint));
	if (newSnakePoint == NULL) {
		console_error(L"Failed to allocate memory for newSnakePoint");
		return NULL;
	}

	newSnakePoint->x = x;
	newSnakePoint->y = y;
	newSnakePoint->next = NULL;
	return newSnakePoint;
}

SnakePoint** snakePtr;
SnakeFood* snakeFoods;
SnakeFoodInMap snakeFoodsInMap[3];
int lengthToGrow = 0;
int totalScore = 0;
int totalFoodsEaten = 0;
ENGINE_KEYCODE previousKeyCode = ENGINE_KEYCODE_LEFT; // Initial direction.
bool hasCrashed = false;

/*
* Add a point to the front of the snake.
*/
static void enqueue(int x, int y) {
	SnakePoint* new_point = createSnakePoint(x, y);
	new_point->next = *snakePtr;
	*snakePtr = new_point;
}

/*
* Remove the tail of the snake.
*/
static void dequeue() {
	if (*snakePtr == NULL) {
		return;
	}

	SnakePoint* temp = *snakePtr;
	if (temp->next == NULL) {
		free(temp);
		*snakePtr = NULL;
		return;
	}

	while (temp->next->next != NULL) {
		temp = temp->next;
	}

	free(temp->next);
	temp->next = NULL;
}

/*
* The move is legal as long as the snake doesn't crash with a wall or its own tail.
*/
static bool isLegalMove(Engine* engine, float x, float y) {
	if (x < 1 || x >= engine->screen.width - 1 || y < 1 || y >= engine->screen.height - 1) {
		return false;
	}

	SnakePoint* current = *snakePtr;
	while (current != NULL) {
		if (x == current->x && y == current->y) {
			return false;
		}
		current = current->next;
	}
	return true;
}

static void generateSnakeFoodInMap(Engine* engine, int i) {
	int x = rand() % engine->screen.width;
	int y = rand() % engine->screen.height;
	int j = rand() % 8;

	if (isLegalMove(engine, x, y)) {
		snakeFoodsInMap[i] = (SnakeFoodInMap){
			.snakeFood = {
				.c = snakeFoods[i].c,
				.bgColor = snakeFoods[i].bgColor,
				.fgColor = snakeFoods[i].fgColor,
				.points = snakeFoods[i].points
			},
			.x = x,
			.y = y
		};
	}
	else {
		generateSnakeFoodInMap(engine, i);
	}
}

static void onCreate(Engine* engine) {
	snakePtr = malloc(sizeof(SnakePoint*));
	if (snakePtr == NULL) {
		console_error(L"Failed to allocate memory for snake pointer");
		return;
	}

	*snakePtr = createSnakePoint(engine->screen.width / 2.0, engine->screen.height / 2.0);
	if (*snakePtr == NULL) {
		console_error(L"Failed to create initial snake point");
		return;
	}

	snakeFoods = calloc(9, sizeof(SnakeFood));
	if (snakeFoods == NULL) {
		console_error(L"Failed to allocate memory for snake foods");
		free(*snakePtr);
		free(snakePtr);
		return;
	}
	snakeFoods[0] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_BLACK,
		.bgColor = BG_BLACK,
		.points = 9
	};
	snakeFoods[1] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_RED,
		.bgColor = BG_RED,
		.points = 8
	};
	snakeFoods[2] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_PURPLE,
		.bgColor = BG_PURPLE,
		.points = 7
	};
	snakeFoods[3] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_YELLOW,
		.bgColor = BG_YELLOW,
		.points = 6
	};
	snakeFoods[4] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_GRAY,
		.bgColor = BG_GRAY,
		.points = 5
	};
	snakeFoods[5] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_LIGHT_RED,
		.bgColor = BG_LIGHT_RED,
		.points = 4
	};
	snakeFoods[6] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_LIGHT_PURPLE,
		.bgColor = BG_LIGHT_PURPLE,
		.points = 3
	};
	snakeFoods[7] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_LIGHT_YELLOW,
		.bgColor = BG_LIGHT_YELLOW,
		.points = 2
	};
	snakeFoods[8] = (SnakeFood){
		.c = PIXEL_TYPE_SOLID,
		.fgColor = FG_WHITE,
		.bgColor = BG_WHITE,
		.points = 1
	};

	for (int i = 0; i < 3; i++) {
		generateSnakeFoodInMap(engine, i);
	}
}

static void onUpdate(Engine* engine) {
	if (hasCrashed) {
		screen_fill(&engine->screen, FG_BLACK);
		screen_draw_string(&engine->screen, L"You crashed and now you're dead. Game over!", 32, FG_RED, BG_BLACK, engine->screen.width / 2.0f, engine->screen.height / 2.0f);
		return;
	}

	screen_fill(&engine->screen, FG_BLUE);
	screen_draw_box(&engine->screen, PIXEL_TYPE_SOLID, FG_LIGHT_RED, BG_LIGHT_RED, (Point) { 0, 0 }, (Point) { engine->screen.width - 1, 0 }, (Point) { 0, engine->screen.height - 1 }, (Point) { engine->screen.width - 1, engine->screen.height - 1 });
	wchar_t score[256];
	int written = swprintf(score, 64, L"Current score: %d - Total foods eaten: %d", totalScore, totalFoodsEaten);
	screen_draw_string(&engine->screen, score, written, FG_WHITE, BG_LIGHT_RED, 1, 0);

	// Eat food
	for (int i = 0; i < 3; i++) {
		if (snakeFoodsInMap[i].x == (*snakePtr)->x && snakeFoodsInMap[i].y == (*snakePtr)->y) {
			lengthToGrow += snakeFoodsInMap[i].snakeFood.points;
			totalScore += snakeFoodsInMap[i].snakeFood.points;
			totalFoodsEaten += 1;
			generateSnakeFoodInMap(engine, i);
		}
	}

	// Move snake
	float x = (*snakePtr)->x;
	float y = (*snakePtr)->y;


	switch (previousKeyCode) {
	case ENGINE_KEYCODE_DOWN:
		y += 1;
		break;
	case ENGINE_KEYCODE_UP:
		y -= 1;
		break;
	case ENGINE_KEYCODE_LEFT:
		x -= 1;
		break;
	case ENGINE_KEYCODE_RIGHT:
		x += 1;
		break;
	}

	if (engine->keyboard.keys[ENGINE_KEYCODE_DOWN].isHeld) {
		previousKeyCode = ENGINE_KEYCODE_DOWN;
	}
	else if (engine->keyboard.keys[ENGINE_KEYCODE_UP].isHeld) {
		previousKeyCode = ENGINE_KEYCODE_UP;
	}
	else if (engine->keyboard.keys[ENGINE_KEYCODE_LEFT].isHeld) {
		previousKeyCode = ENGINE_KEYCODE_LEFT;
	}
	else if (engine->keyboard.keys[ENGINE_KEYCODE_RIGHT].isHeld) {
		previousKeyCode = ENGINE_KEYCODE_RIGHT;
	}

	hasCrashed = !isLegalMove(engine, x, y);
	SnakePoint* newHead = createSnakePoint(x, y);
	newHead->next = *snakePtr;
	*snakePtr = newHead;

	if (lengthToGrow) {
		lengthToGrow--;
	}
	else {
		dequeue();
	}
	
	// Draw snake
	SnakePoint* currentSnakePoint = *snakePtr;
	while (currentSnakePoint != NULL) {
		screen_draw(&engine->screen, PIXEL_TYPE_SOLID, FG_LIGHT_GREEN, FG_LIGHT_GREEN, currentSnakePoint->x, currentSnakePoint->y);
		currentSnakePoint = currentSnakePoint->next;
	}

	if (hasCrashed) {
		Sleep(50);
		return;
	}

	// Draw food
	for (int i = 0; i < 3; i++) {
		screen_draw(&engine->screen, snakeFoodsInMap[i].snakeFood.c, snakeFoodsInMap[i].snakeFood.fgColor, snakeFoodsInMap[i].snakeFood.bgColor, snakeFoodsInMap[i].x, snakeFoodsInMap[i].y);
	}

	Sleep(50);
}

void playSnakeGame() {
	Engine* engine = engine_construct(L"Hello, Snake!", 120, 60, 16, 16, onCreate, onUpdate);
	engine_start(engine);
}