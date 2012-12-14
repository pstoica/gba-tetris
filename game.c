#include <stdlib.h>
#include <debugging.h>
#include <stdio.h>
#include "mylib.h"
#include "game.h"
#include "text.h"
#include "start_image.h"
#include "gameover_image.h"
#include "corgi_image.h"

/* Array containing tetromino pieces and their rotations */
const int PIECES[NUM_PIECES][NUM_ROTATIONS][NUM_BLOCKS][2] = {
	{
		/*
		 *	TETROMINO_I
		 *	0000
		 *	1111
		 *	0000
		 *	0000
		 */
		{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
		{{0, 2}, {1, 2}, {2, 2}, {3, 2}},
		{{2, 0}, {2, 1}, {2, 2}, {2, 3}},
		{{0, 1}, {1, 1}, {2, 1}, {3, 1}}
	},
	{
		/*
		 *	TETROMINO_L
		 *	0010
		 *	1110
		 *	0000
		 *	0000
		 */
		{{1, 0}, {1, 1}, {1, 2}, {0, 2}},
		{{0, 1}, {1, 1}, {2, 1}, {2, 2}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 0}},
		{{0, 0}, {0, 1}, {1, 1}, {2, 1}}
	},
	{
		/*
		 *	TETROMINO_J
		 *	1000
		 *	1110
		 *	0000
		 *	0000
		 */
		{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
		{{0, 1}, {0, 2}, {1, 1}, {2, 1}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 2}},
		{{0, 1}, {1, 1}, {2, 0}, {2, 1}}
	},
	{
		/*
		 *	TETROMINO_O
		 *	0110
		 *	0110
		 *	0000
		 *	0000
		 */
		{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
		{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
		{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
		{{0, 1}, {0, 2}, {1, 1}, {1, 2}}
	},
	{
		/*
		 *	TETROMINO_S
		 *	0110
		 *	1100
		 *	0000
		 *	0000
		 */
		{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
		{{0, 1}, {1, 1}, {1, 2}, {2, 2}},
		{{1, 1}, {1, 2}, {2, 0}, {2, 1}},
		{{0, 0}, {1, 0}, {1, 1}, {2, 1}},
	},
	{
		/*
		 *	TETROMINO_T
		 *	0100
		 *	1110
		 *	0000
		 *	0000
		 */
		{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
		{{0, 1}, {1, 1}, {1, 2}, {2, 1}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 1}},
		{{0, 1}, {1, 0}, {1, 1}, {2, 1}}
	},
	{
		/*
		 *	TETROMINO_Z
		 *	1100
		 *	0110
		 *	0000
		 *	0000
		 */
		{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
		{{0, 2}, {1, 1}, {1, 2}, {2, 1}},
		{{1, 0}, {1, 1}, {2, 1}, {2, 2}},
		{{0, 1}, {1, 0}, {1, 1}, {2, 0}}
	}
};

/* Array containing the cells */
u16 board[BOARD_HEIGHT][BOARD_WIDTH];

/* Score, lines cleared, current level, current frame */
static int score;
static int lines;
static int frame;
static int speed;
static char scoreString[41];
static char levelString[41];

/* Current state of the game: TITLE, NORMAL, GAMEOVER */
int state;

/* Next and current blocks */
TETROMINO nextBlock;
TETROMINO currentBlock;

/*
* Called once per cycle. May increase the frame and/or move the block down, depending on the game state.
*/
void refresh() {
	switch(state) {
	case TITLE:
		frame++;
		break;
	case NORMAL:
		if (frame == speed) {
			moveBlock(0, 1);
			frame = 0;
		} else {
			frame++;
		}
		break;
	case GAMEOVER:
		frame++;
		break;
	}
}

/*
* Draws the current block on the board.
*/
void drawBlock() {
	for (int i = 0; i < NUM_BLOCKS; i++) {
		for (int j = 0; j < NUM_BLOCKS; j++) {
			// if the cell isn't empty, draw it
			if (currentBlock.cells[i][j]) {
				drawRect3(START_ROW + PPI * (i + currentBlock.y), START_COL + PPI * (j + currentBlock.x), PPI, PPI, currentBlock.color);
			}
		}
	}
}

/*
* Draws the next block.
*/
void drawPreview() {
	for (int i = 0; i < NUM_BLOCKS; i++) {
		for (int j = 0; j < NUM_BLOCKS; j++) {
			// if the cell is filled, draw the block's color, otherwise draw the background color
			u16 color = (nextBlock.cells[i][j]) ? nextBlock.color : BGCOLOR;
			drawRect3(PREV_ROW + PPI * (i + nextBlock.y), PREV_COL + PPI * (j + nextBlock.x), PPI, PPI, color);
		}
	}
}

/*
* Function that takes in a potential block and determines if it collides with anything.
* Returns 1 if it collides, 0 otherwise.
*/
int collides(TETROMINO newBlock) {
	for (int r = 0; r < NUM_BLOCKS; r++) {
		for (int c = 0; c < NUM_BLOCKS; c++) {
			// only check non-empty cells
			if (newBlock.cells[r][c] != EMPTY_CELL) {
				// check with the board limits
				if ((newBlock.x + c < 0) || (newBlock.x + c >= BOARD_WIDTH) || (newBlock.y + r >= BOARD_HEIGHT)) {
					return 1;
				}

				// check with the rest of the board cells
				if (board[newBlock.y + r][newBlock.x + c] != EMPTY_CELL) {
					return 1;
				}
			}
		}
	}

	return 0; // no collision, return 0
}

/*
* Moves a block in the given direction (x, y), but checks to see if it collides first.
*/
void moveBlock(int dx, int dy) {
	TETROMINO newBlock = currentBlock;
	newBlock.x += dx;
	newBlock.y += dy;
	int collision = collides(newBlock);
	// if there's no collision, copy over the potential values
	if (!collision) {
		blockMoved(); // erase the old block
		currentBlock = newBlock;
		drawBlock(); // draw the new block
	} else if (collision && dy == 1) { // see if the block was going down
		// did the collision happen on the 1st or 2nd row?
		// copy the block contents onto the board
		for (int r = 0; r < currentBlock.size; r++) {
			for (int c = 0; c < currentBlock.size; c++) {
				if (currentBlock.cells[r][c]) {
					board[currentBlock.y + r][currentBlock.x + c] = currentBlock.color;
				}
			}
		}

		// look for filled rows
		int start = currentBlock.y; // start scanning from where the block was placed
		int end = start + currentBlock.size; // stop where the block ends
		currentBlock = nextBlock;
		// check if the new block collides, meaning the top is filled
		if (collides(currentBlock)) {
			endGame();
		} else {
			// otherwise, make a new block, draw the new blocks, and clear any rows
			drawBlock();
			nextBlock = makeBlock();
			drawPreview();

			frame = 0; // reset the frame to make sure the fall down rate is the same initially

			int filledRows = 0;
			int isFull;
			for (int r = start; r < end; r++) {
				isFull = 1;
				for (int c = 0; c < BOARD_WIDTH; c++) {
					if (board[r][c] == EMPTY_CELL) {
						isFull = 0;
						break;
					}
				}

				// if this row is full, move down everything above
				if (isFull) {
					for (int x = 0; x < BOARD_WIDTH; x++) {
						for (int y = r; y >= 0; y--) {
							u16 prev = board[y][x];
							board[y][x] = (y == 0) ? EMPTY_CELL : board[y - 1][x]; // replace the top row with empty cells
						
							if (prev != board[y][x]) {
								redraw(y, x);
							}
						}	
					}		
				
					filledRows++;
				}
			}

			// if we have at least one filled row, make the game faster
			if (filledRows > 0) {
				speed = (speed > 1) ? (speed - 1) : speed;
				drawRect3(120, 150, 140, 8, BGCOLOR);
				sprintf(levelString, "Level: %d", INIT_SPEED - speed + 1);
				drawString(120, 150, levelString, WHITE);
			}

			// add to the score and redraw it
			score += (INIT_SPEED - speed + 1) * (filledRows + 1);
			drawRect3(100, 150, 140, 8, BGCOLOR);
			sprintf(scoreString, "Score: %d", score);
			drawString(100, 150, scoreString, WHITE);
		}
	}
}

/*
* Rotates a block clockwise if 1, counterclockwise otherwise.
*/
void rotateBlock(int clockwise) {
	// newBlock is the potential block; copies the current x and y
	TETROMINO newBlock;
	newBlock.x = currentBlock.x;
	newBlock.y = currentBlock.y;
	
	// don't rotate if it's type O, that's pointless!
	if (currentBlock.type == TETROMINO_O) {
		return;
	} else {
		int newRotation;
		// figure out which element in the rotation array you need
		if (clockwise) {
			newRotation = (currentBlock.rotation + 1) % NUM_ROTATIONS;
		} else {
			newRotation = (currentBlock.rotation == 0) ? (NUM_ROTATIONS - 1) : (currentBlock.rotation - 1);
		}
		// set the newBlock to the appropriate rotation
		setBlock(currentBlock.type, &newBlock, newRotation);
	}

	// if the newBlock doesn't collide, copy its properties to currentBlock
	if (!collides(newBlock)) {
		blockMoved();
		currentBlock = newBlock;
		drawBlock();
	}
}

/*
* Erases the old block. Called before redrawing the block.
*/
void blockMoved() {
	for (int i = 0; i < NUM_BLOCKS; i++) {
		for (int j = 0; j < NUM_BLOCKS; j++) {
			if (currentBlock.cells[i][j] != EMPTY_CELL) {
				drawRect3(START_ROW + PPI * (i + currentBlock.y), START_COL + PPI * (j + currentBlock.x), PPI, PPI, BOARDCOLOR);
			}
		}
	}
}

/*
* Redraws the given block on the board. Used when a row is cleared.
*/
void redraw(int r, int c) {
	u16 color = (board[r][c] != EMPTY_CELL) ? board[r][c] : BOARDCOLOR;
	drawRect3(START_ROW + PPI * r, START_COL + PPI * c, PPI, PPI, color);
}

/*
* Changes the game state and draws the gameover image.
*/
void endGame() {
	state = GAMEOVER;
	drawImage3(0, 0, GAMEOVER_IMAGE_WIDTH, GAMEOVER_IMAGE_HEIGHT, gameover_image);
}

/*
* Starts the game by setting the state to normal and drawing the appropriate things.
*/
void startGame() {
	state = NORMAL;

	drawRect3(0, 0, 240, 160, BGCOLOR); // draw the background

	drawRect3(START_ROW, START_COL, PPI * BOARD_WIDTH, PPI * BOARD_HEIGHT, BOARDCOLOR); // draw the board

	// draw the corgi!
	drawImage3(30, 150, CORGI_IMAGE_WIDTH, CORGI_IMAGE_HEIGHT, corgi_image);

	// initial score text
	sprintf(scoreString, "Score: %d", 0);
	drawString(100, 150, scoreString, WHITE);

	// initial level (calculated from the speed) text
	sprintf(levelString, "Level: %d", 1);
	drawString(120, 150, levelString, WHITE);

	srand(frame); // seed the random function with the current frame

	// make a new current block
	currentBlock = makeBlock();
	drawBlock();

	// Make a new upcoming block
	nextBlock = makeBlock();
	drawPreview();

	frame = 0; // reset the frame
}

/*
* Resets game variables and draws the title screen.
*/
void reset() {
	state = TITLE;
	score = 0;
	lines = 0;
	speed = INIT_SPEED;
	int src = EMPTY_CELL;
	DMA[3].src = &src;
	DMA[3].dst = &board[0];
	DMA[3].cnt = (BOARD_WIDTH * BOARD_HEIGHT) | DMA_SOURCE_FIXED | DMA_16 | DMA_ON;
	drawImage3(0, 0, START_IMAGE_WIDTH, START_IMAGE_HEIGHT, start_image);
}

/* Tetromino Stuff */

/*
* Sets the given block to the requested type and rotation.
*/
void setBlock(int type, TETROMINO *block, int rotation) {
	// clear all the cells first
	DMA[3].src = EMPTY_CELL;
	DMA[3].dst = &block->cells[0][0];
	DMA[3].cnt = (NUM_BLOCKS * NUM_BLOCKS) | DMA_SOURCE_FIXED | DMA_32 | DMA_ON;

	block->size = NUM_BLOCKS - 1; // the size saves a tiny bit of time for collision detection
	block->rotation = rotation; // gets the appropriate rotation
	for (int i = 0; i < NUM_BLOCKS; i++) {
		// convert the coordinates in the rotation array to actual cells
		int r = PIECES[type][rotation][i][0];
		int c = PIECES[type][rotation][i][1];
		block->cells[r][c] = 1;
	}

	// set the appropriate color or size, if different
	switch (type) {
	case TETROMINO_I:
		block->color = CYAN;
		block->size = NUM_BLOCKS;
		break;
	case TETROMINO_O:
		block->color = YELLOW;
		break;
	case TETROMINO_T:
		block->color = PURPLE;
		break;
	case TETROMINO_S:
		block->color = GREEN;
		break;
	case TETROMINO_Z:
		block->color = RED;
        	break;
	case TETROMINO_J:
		block->color = BLUE;
		break;
	case TETROMINO_L:
		block->color = ORANGE;
		break;
	}

	block->type = type;
}

/*
* Generates a new block.
*/
TETROMINO makeBlock() {
	TETROMINO block;
	setBlock(rand() % NUM_PIECES, &block, 0); 
	block.x = (BOARD_WIDTH - block.size) / 2;
	block.y = 0;
	return block;
}
