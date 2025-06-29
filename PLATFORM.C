/* test room for game named MALLOC */
/* its a prototype, no wonder why its trash */

#include <stdlib.h>
#include <dos.h>

unsigned char far *vram = (unsigned char far *)0xA0000000L;
unsigned char vidbuffer[320][200];

void setvidmode(int mode) {
	union REGS regs;
	regs.h.ah = 0;
	regs.h.al = mode;
	int86(0x10,&regs,&regs);
}

void setpixel(int x, int y, unsigned char color) {
	if (((x>(-1)) && (x<320)) && ((y>(-1)) && (y<200))) {
		vidbuffer[x][y] = color;
	}
}

void applybuffer() {
	int x,y;
	for (x=0;x<320;x++) {
		for (y=0;y<200;y++) {
			vram[y * 320 + x] = vidbuffer[x][y];
		}
	}
}

void fill(int pX, int pY, int sX, int sY, unsigned char color) {
	int lX,lY;
	int x,y;
	lX = pX+sX;
	lY = pY+sY;
	x=pX;
	while (x<lX) {
		y=pY;
		while (y<lY) {
			setpixel(x,y,color);
			y++;
		}
		x++;
	}
}

void game_displaymalloc() {
	fill(155,95,10,10,0x13);
	fill(158,98,4,4,0x35);
	fill(155,99,10,2,0x35);
	fill(159,95,2,10,0x35);
}

void game_displaywall(ppX, ppY, wpX, wpY, wsX, wsY) {
	fill(ppX+wpX-35, ppY+60+wpY-5, wsX, wsY, 0x14);
}

int game_processcollision_X(int ppX, int ppY, int psX, int psY, int wpX, int wpY, int wsX, int wsY) {
	int pLeft = ppX;
	int pRight = ppX + psX;
	int pTop = ppY;
	int pBottom = ppY + psY;
	int wLeft = wpX;
	int wRight = wpX + wsX;
	int wTop = wpY;
	int wBottom = wpY + wsY;
	if (pBottom > wTop && pTop < wBottom) {
		if (pRight > wLeft && pLeft < wLeft) {
			return wLeft - psX;
		}
		if (pLeft < wRight && pRight > wRight) {
			return wRight;
		}
	}
	return ppX;
}

int game_processcollision_Y(int ppX, int ppY, int psX, int psY, int wpX, int wpY, int wsX, int wsY) {
    int pLeft = ppX;
    int pRight = ppX + psX;
    int pTop = ppY;
    int pBottom = ppY + psY;
    int wLeft = wpX;
    int wRight = wpX + wsX;
    int wTop = wpY;
    int wBottom = wpY + wsY;
    if (pRight > wLeft && pLeft < wRight) {
        if (pBottom > wTop && pTop < wTop) {
            return wTop - psY;
        }
        if (pTop < wBottom && pBottom > wBottom) {
            return wBottom;
        }
    }
    return ppY;
}

int ppX = 0;
int ppY = 60;
int psX = 10;
int psY = 10;
int nppX,nppY;
int velocityX = 0;
int velocityY = 0;

void game_processvelocity_X() {
	ppX += velocityX;
	nppX = game_processcollision_X(ppX,ppY,psX,psY,0,0,200,50);
	if (nppX != ppX) {
		velocityX = 0;
	}
	ppX = nppX;
}
void game_processvelocity_Y() {
	int check;
	ppY += velocityY;
	nppY = game_processcollision_Y(ppX,ppY,psX,psY,0,0,200,50);
	if (nppY != ppY) {
		velocityY = 0;
	}
	ppY = nppY;
}

void game_jump() {
	int check = game_processcollision_Y(ppX,ppY-1,psX,psY,0,0,200,50);
	if (check == ppY) {
		velocityY = 10;
	}
}
void game_move(int dir) {
	int check = game_processcollision_X(ppX+dir,ppY,psX,psY,0,0,200,50);
	if (check == ppX) {
		velocityX = 0;
		return;
	}
	if (dir < 0) {
		if (velocityX > -8) {
			velocityX -= 2;
		}
	}
	if (dir > 0) {
		if (velocityX < 8) {
			velocityX += 2;
		}
	}
}
void game_slam() {
	int check = game_processcollision_Y(ppX,ppY-1,psX,psY,0,0,200,50);
	if (check != ppY) {
		if (velocityY > -15) {
			velocityX = 0;
			velocityY -= 15;
		}
	}
}
void game_reset() {
	ppX = 0;
	ppY = 60;
	psX = 10;
	psY = 10;
	nppX = 0;
	nppY = 10;
	velocityX = 0;
	velocityY = 0;
}

void game_gravity() {
	int check = game_processcollision_Y(ppX,ppY-1,psX,psY,0,0,200,50);
	if (check != ppY) {
		velocityY -= 2;
	} else if (velocityY < 0) {
		velocityY = 0;
	}
	if (velocityX > 0) {
		velocityX -= 1;
	} else if (velocityX < 0) {
		velocityX += 1;
	}
}
int game_noinsidewall(int ppX, int ppY, int psX, int psY, int wpX, int wpY, int wsX, int wsY) {
	int edX = wpX+wsX;
	int edY = wpY+wsY;
	if (((ppX > wpX) && (ppX < edX)) && ((ppY > wpY) && (ppY < edY))) {
		velocityY = 0;
		return edY;
	}
	return ppY;
}

int processkey() {
	int key;
	if (!kbhit()) {key=0;} else {key=inp(0x60);}
	if (key==1) {
		return 1;
	}
	if (key==17) {
		game_jump();
	}
	if (key==30) {
		game_move(1);
	}
	if (key==32) {
		game_move(-1);
	}
	if (key==31) {
		game_slam();
	}
	if (key==19) {
		game_reset();
	}
	return 0;
}

int main() {
	setvidmode(0x13);
	while (1) {
		fill(0,0,320,200,0x43);
		game_displaymalloc();
		game_displaywall(ppX,ppY,0,0,200,100);
		applybuffer();
		game_processvelocity_Y();
		game_processvelocity_X();
		game_gravity();
		ppY = game_noinsidewall(ppX,ppY,psX,psY,0,0,200,50);
		if (processkey()) {
			break;
		}
	}
	setvidmode(0x03);
	return 0;
}