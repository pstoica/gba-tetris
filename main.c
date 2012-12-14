// Patrick Stoica
#include <stdlib.h>
#include <debugging.h>
#include "mylib.h"
#include "game.h"

int main () {
	// set the REG_DISPCTL for use with mode 3 and the 2nd background
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	int isReset = 1;

	while(1) {
		__key_prev = __key_curr; // save the previous key hit
		__key_curr = ~BUTTONS & BUTTON_MASK; // get the current key hit
		waitForVblank();
		if (KEY_HIT(BUTTON_START)) {
			switch(state) {
			case TITLE: // start the game
				state = NORMAL;
				startGame();
				break;
			case GAMEOVER: // reset
				reset();
				break;
			}
		} else if (KEY_HIT(BUTTON_SELECT) || isReset) {
			reset(); // reset whenever select is hit or at the start
			isReset = 0;
		} else if (state == NORMAL) {
			if (KEY_HELD(BUTTON_UP)) {
				// TODO: hard drop
			} else if (KEY_HELD(BUTTON_DOWN)) {
				moveBlock(0, 1); // move down
			} else if (KEY_HIT(BUTTON_LEFT)) {
				moveBlock(-1, 0); // move left
			} else if (KEY_HIT(BUTTON_RIGHT)) {
				moveBlock(1, 0); // move right
			} else if (KEY_HIT(BUTTON_L)) {
				rotateBlock(0); // rotate counterclockwise
			} else if (KEY_HIT(BUTTON_R)) {
				rotateBlock(1); // rotate clockwise
			}
		}
		refresh(); // increase the frame count, etc.
	}
}
