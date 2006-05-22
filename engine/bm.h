// C Header File
// Created 6/26/2002; 7:58:45 PM

#ifndef BMHEADER_H
#define BMHEADER_H

extern fixed	screen_mx, screen_my;
extern fixed	old_screen_mx, old_screen_my;
extern fixed	screen_w, screen_h;
extern fixed	scroll_left, scroll_right;
extern fixed	scroll_top, scroll_bottom;
extern int		quit, game_paused;

int LoadLevel(const char *filename);

#endif
