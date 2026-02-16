#include "joypad.h"
#include "libetc.h"
#include <sys/types.h>
#include <stdbool.h>

static u_long padstate;

int JoyPadCheck(int p) {
	return padstate & p;
};

bool JoyLeftDown() {
	static bool left_down = false;
	if( JoyPadCheck(PAD1_LEFT) ) {
		if( left_down ) {
			return false;
		} else {
			left_down = true;
			return true;
		}	
	} 
	left_down = false;
	return false;
}

bool JoyRightDown() {
	static bool right_down = false;
	if( JoyPadCheck(PAD1_RIGHT) ) {
		if( right_down ) {
			return false;
		} else {
			right_down = true;
			return true;
		}	
	} 
	right_down = false;
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