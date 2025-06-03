/* A simple drawing software */
/* Made with Turbo C 2.00 */
/* and on DOSBox */
/* by IvanTheSkid */

#include <dos.h>
#include <stdio.h>
#include <conio.h>

void setvidmode(int mode) {
	union REGS in, out;
	in.h.ah = 0;
	in.h.al = mode;
	int86(0x10,&in,&out);
}

void setpixel(int x, int y, unsigned char color) {
	unsigned char far *vram = (unsigned char far *)0xA0000000L;
	vram[y * 320 + x] = color;
}

void mouse(int func) {
	union REGS in,out;
	in.x.ax = func;
	int86(0x33,&in,&out);
}

int getpixel(int x, int y) {
	unsigned char far *vram = (unsigned char far *)0xA0000000L;
	return vram[y * 320 + x];
}

int main() {
	int x = 1;
	int y = 1;
	int PREV_x = 1;
	int PREV_y = 1;
	int PREV_color = 1;
	setvidmode(0x13);
	while (x < 321) {
		while (y < 201) {
			setpixel(x,y,1);
			y++;
		}
		y = 1;
		x++;
	}
	mouse(0);
	delay(1000);
	while (!kbhit()) {
		union REGS m_in,m_out;
		m_in.x.ax = 3;
		int86(0x33,&m_in,&m_out);
		if (m_out.x.bx & 1) {
			setpixel(m_out.x.cx, m_out.x.dx, 15);
			PREV_color = 15;
		}
		if (m_out.x.bx & 2) {
			setpixel(m_out.x.cx, m_out.x.dx, 1);
			PREV_color = 1;
		}
		/* somehow i did this red mouse pointer thing, idk */
		if ((PREV_x != m_out.x.cx) || (PREV_y != m_out.x.dx)) {
			if (getpixel(PREV_x,PREV_y) == 0x3F) {
				setpixel(PREV_x,PREV_y,PREV_color);
			}
			PREV_x = m_out.x.cx;
			PREV_y = m_out.x.dx;
		}
		setpixel(m_out.x.cx,m_out.x.dx,0x3F);
	}
	setvidmode(0x03);
	return 0;
}