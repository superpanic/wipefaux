#include "joypad.h"
#include "libetc.h"
#include <sys/types.h>
#include <stdbool.h>

static u_long padstate;

int JoyPadCheck(int p) {
	return padstate & p;
};

bool JoyLeftDown() {
	static bool hold_left = false;
	if( JoyPadCheck(PAD1_LEFT) ) {
		if( hold_left ) {
			return false;
		} else {
			hold_left = true;
			return true;
		}	
	} 
	hold_left = false;
	return false;
}

bool JoyRightDown() {
	static bool hold_right = false;
	if( JoyPadCheck(PAD1_RIGHT) ) {
		if( hold_right ) {
			return false;
		} else {
			hold_right = true;
			return true;
		}	
	} 
	hold_right = false;
	return false;
}

bool JoyDownDown() {
	static bool down_down = false;
	if( JoyPadCheck(PAD1_DOWN) ) {
		if( down_down ) {
			return false;
		} else {
			down_down = true;
			return true;
		}	
	} 
	down_down = false;
	return false;
}

void JoyPadInit(void) {
	PadInit(0);
};

void JoyPadReset(void) {
	padstate = 0;
};

void JoyPadUpdate(void) {
	u_long pad = PadRead(0);
	padstate = pad;
};