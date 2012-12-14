/* General Settings */

#define NUM_PIECES 7	// number of pieces
#define NUM_BLOCKS 4	// number of blocks in a piece
#define NUM_ROTATIONS 4 // number of rotations per piece
#define BOARD_WIDTH 10	// number of tiles wide
#define BOARD_HEIGHT 20	// number of tiles high
#define START_ROW 0
#define START_COL 60
#define PREV_ROW 20
#define PREV_COL 0
#define PPI 8 // "pixels per inch"
#define EMPTY_CELL 0
#define INIT_SPEED 30

/* Tetromino Piece Definitions */
#define	TETROMINO_I 0
#define	TETROMINO_L 1
#define	TETROMINO_J 2
#define	TETROMINO_O 3
#define	TETROMINO_S 4
#define	TETROMINO_T 5
#define	TETROMINO_Z 6

/* Game states */
#define	TITLE 0
#define	NORMAL 1
#define GAMEOVER 2

/* Struture representing a tetromino */
typedef struct {
	/**
	 * X goes left to right.
	 * Y goes top to bottom.
	 * The size of the array is 4x4, specified in the NUM_BLOCKS constant.
	 */	
	u16 color;
	int cells[NUM_BLOCKS][NUM_BLOCKS];
	int rotation;
	int x;
	int y;
	int size;
	int type;
} TETROMINO;

/* External variables */
extern int state;
extern const int PIECES[NUM_PIECES][NUM_ROTATIONS][NUM_BLOCKS][2];
extern u16 board[BOARD_HEIGHT][BOARD_WIDTH];
extern TETROMINO nextBlock;
extern TETROMINO currentBlock;

void refresh();
void reset();
void startGame();
void endGame();
void setBlock(int type, TETROMINO *block, int rotation);
void moveBlock(int dx, int dy);
void drawBlock();
void drawPreview();
void blockMoved();
void redraw(int r, int c);
TETROMINO makeBlock();
void rotateBlock(int clockwise);
int collides(TETROMINO newBlock);
