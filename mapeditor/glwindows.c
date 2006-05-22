// glwindows.c  --  port of Win.pas from glWindows Delphi package
// Basic rendering code was directly ported from Win.pas;
// however, some architectural changes have been made.

//   - James Dunne
//	02/16/04

#include <GL/gl.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "glwindows.h"
#include "font.h"

// ---------------------------------------------------------------

TAppWindow	Window;	// = {Width:800; Height:600; CaptionBarHeight:26};
TMouse		Mouse;
GLuint		GUITexture;

TButton		*ButtonPressed;
TMouseDrag	MouseDrag;

void glWrite(int x, int y, char *p) {
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_BLEND);
	glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glTranslatef(x, y, 0.05);
		glListBase(fontBaseListId);
		glCallLists(strlen(p), GL_BYTE, p);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, GUITexture);
}

// TWindow

// Adds an item to a list:
#define LIST_ADD(itemtype, itemlist, itemcount, varname) \
	itemtype	*varname; \
	{ \
		int	i, found = 0; \
		\
		for (i=0; i<self->itemcount; ++i) { \
			if (self->itemlist[i] == NULL) { \
				found = -1; \
				self->itemlist[i] = varname = calloc(sizeof(itemtype), 1); \
				break; \
			} \
		} \
		if (found == 0) { \
			self->itemlist = realloc(self->itemlist, sizeof(itemtype *) * ++self->itemcount); \
			self->itemlist[self->itemcount - 1] = varname = calloc(sizeof(itemtype), 1); \
		} \
	}

// Removes an item from a list:  (removes 1st occurance only)
#define LIST_REMOVE(itemtype, itemlist, itemcount, itemref) \
	{ \
		int	i, j; \
		\
		for (i=0; i<self->itemcount; ++i) { \
			if (self->itemlist[i] == itemref) { \
				free(self->itemlist[i]); \
				self->itemlist[i] = NULL; \
				for (j=i+1; j<self->itemcount; ++j) \
					self->itemlist[j-1] = self->itemlist[j]; \
				self->itemcount--; \
				self->itemlist = realloc(self->itemlist, sizeof(itemtype *) * self->itemcount); \
				break; \
			} \
		} \
	}

//------------------------------------------------------------------
//  Adds a button onto the window
//------------------------------------------------------------------
TButton *TWindow_AddButton(TWindow *self, int btnX, int btnY, int btnWidth, int btnHeight, char *btnCaption)
{
	if (!self) return;
	LIST_ADD(TButton, Button, numButtons, newbutton)

	TButton_Init(newbutton, btnX, btnY, btnWidth, btnHeight, btnCaption);
	newbutton->Parent = self;
	newbutton->Index = self->numButtons - 1;

	return newbutton;
}

//------------------------------------------------------------------
//  Removes a button from the window
//------------------------------------------------------------------
void TWindow_RemoveButton(TWindow *self, TButton *object)
{
	if (!self) return;
	LIST_REMOVE(TButton, Button, numButtons, object)
}

//------------------------------------------------------------------
//  Removes all buttons from the window
//------------------------------------------------------------------
void TWindow_RemoveAllButtons(TWindow *self)
{
	// Keep removing the 1st item until no more items:
	while (self->numButtons > 0) {
		LIST_REMOVE(TButton, Button, numButtons, self->Button[0])
	}
}

// ------------------------------------------------------------------
//   Adds text to a window
// ------------------------------------------------------------------
TLabel *TWindow_AddLabel(TWindow *self, int tX, int tY, char *tCaption)
{
	if (!self) return;
	LIST_ADD(TLabel, Label, numLabels, newtext)

	TLabel_Init(newtext, tX, tY, tCaption);
	newtext->Parent = self;
	newtext->Index = self->numLabels - 1;

	return newtext;
}

// ------------------------------------------------------------------
//   Adds a panel onto the window
// ------------------------------------------------------------------
TPanel *TWindow_AddPanel(TWindow *self, int pX, int pY, int pWidth, int pHeight)
{
	if (!self) return;
	LIST_ADD(TPanel, Panel, numPanels, newpanel)

	TPanel_Init(newpanel, pX, pY, pWidth, pHeight);
	newpanel->Parent = self;
	newpanel->Index = self->numPanels - 1;

	return newpanel;
}

// ------------------------------------------------------------------
//   Adds a checkbox onto the window
// ------------------------------------------------------------------
TCheckBox *TWindow_AddCheckBox(TWindow *self, int cbX, int cbY, int Checked, char *Text)
{
	if (!self) return;
	LIST_ADD(TCheckBox, CheckBox, numCheckBoxes, newcb)

	TCheckBox_Init(newcb, cbX, cbY, Checked, Text);
	newcb->Parent = self;
	newcb->Index = self->numCheckBoxes - 1;

	return newcb;
}

// ------------------------------------------------------------------
//   Adds a radiobutton onto the window
// ------------------------------------------------------------------
TRadioButton *TWindow_AddRadioButton(TWindow *self, int rbX, int rbY, int rbGroup, int Checked, char *Text)
{
	if (!self) return;
	LIST_ADD(TRadioButton, RadioButton, numRadioButtons, newrb)

	TRadioButton_Init(newrb, rbX, rbY, rbGroup, Checked, Text);
	newrb->Parent = self;
	newrb->Index = self->numRadioButtons - 1;

	return newrb;
}

//------------------------------------------------------------------
//  Removes a radiobutton from the window
//------------------------------------------------------------------
void TWindow_RemoveRadioButton(TWindow *self, TRadioButton *object)
{
	if (!self) return;
	LIST_REMOVE(TRadioButton, RadioButton, numRadioButtons, object)
}

//------------------------------------------------------------------
//  Removes all radiobuttons from the window
//------------------------------------------------------------------
void TWindow_RemoveAllRadioButtons(TWindow *self)
{
	// Keep removing the 1st item until no more items:
	while (self->numRadioButtons > 0) {
		LIST_REMOVE(TRadioButton, RadioButton, numRadioButtons, self->RadioButton[0])
	}
}

// ------------------------------------------------------------------
//   Adds a new child window
// ------------------------------------------------------------------
void TWindow_AddWindow(TWindow *self, TWindow *Child)
{
	if (!self) return;
	self->Child = realloc(self->Child, sizeof(TWindow *) * ++self->numChildren);
	self->Child[self->numChildren - 1] = Child;
}


// ------------------------------------------------------------------
//   Initialise window and setup defaults
// ------------------------------------------------------------------
void TWindow_Init(TWindow *self, int wX, int wY, int wWidth, int wHeight, char *Title)
{
	if (!self) return;
	self->X = wX;
	self->Y = wY;
	self->ZOrder = 0;		 // used if you specifically want to set a window higher
	self->Width  = wWidth;
	self->Height = wHeight;
	self->Visible = TRUE;	 // start off visible
	self->Alpha = 0.9;		// defult for alpha blending
	self->Title = Title;
	self->StatusBar = NULL;
	self->onClick = NULL;
}


// ------------------------------------------------------------------
//   Render the window. Calls render button, child windows ...
// ------------------------------------------------------------------
void TWindow_Render(TWindow *self)
{
	int	i, j, w, h;
	if (!self) return;
	if (self->Visible) {
		glBindTexture(GL_TEXTURE_2D, GUITexture);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glColor4f(1.0, 1.0, 1.0, self->Alpha);

		glPushMatrix();
		glTranslatef(self->X, self->Y, self->ZOrder);

		glBegin(GL_QUADS);
			// top left corner of window.
			glTexCoord2f( 0.0/128.0, 0);					glVertex2f(0,  0);
			glTexCoord2f( 0.0/128.0, 26.0/128.0);			glVertex2f(0,  26);
			glTexCoord2f(64.0/128.0, 26.0/128.0);			glVertex2f(63, 26);
			glTexCoord2f(64.0/128.0, 0);					glVertex2f(63, 0);
		glEnd();
		// Repeatable texture:
		glBegin(GL_QUADS);
			for (i=63; i<self->Width-32; i += 32) {
				// draw the main body of the window
				if (i >= self->Width-64) w = ((self->Width-32) - i) % 32 + 64;
				else w = 96.0;
				// top of window.
				glTexCoord2f(64.0/128.0, 0);				glVertex2f(i, 0);
				glTexCoord2f(64.0/128.0, 26.0/128.0);		glVertex2f(i, 26);
				glTexCoord2f(   w/128.0, 26.0/128.0);		glVertex2f(i+w-64, 26);
				glTexCoord2f(   w/128.0, 0);				glVertex2f(i+w-64, 0);
			}
		glEnd();
		glBegin(GL_QUADS);
			// top right corder of window.
			glTexCoord2f(96.0/128.0, 0);					glVertex2f(self->Width-32,  0);
			glTexCoord2f(96.0/128.0, 26.0/128.0);			glVertex2f(self->Width-32, 26);
			glTexCoord2f(1, 26.0/128.0);					glVertex2f(self->Width,	26);
			glTexCoord2f(1, 0);								glVertex2f(self->Width,	 0);
		glEnd();
		glBegin(GL_QUADS);
			// left side of window.
			glTexCoord2f(0.0/128.0, 27.0/128.0);			glVertex2f(0, 26);
			glTexCoord2f(0.0/128.0, 1.0-27.0/128.0);		glVertex2f(0, self->Height-27);
			glTexCoord2f(6.0/128.0, 1.0-27.0/128.0);		glVertex2f(6, self->Height-27);
			glTexCoord2f(6.0/128.0, 27.0/128.0);			glVertex2f(6, 26);
		glEnd();
		// Repeatable texture:
		glBegin(GL_QUADS);
			for (j=26; j<self->Height-26; j += 31) {
				for (i=6; i<self->Width-7; i += 31) {
					// draw the main body of the window
					if (i >= self->Width-7-31) w = ((self->Width-7) - i) % 31 + 8;
					else w = 39.0;
					if (j >= self->Height-26-31) h = ((self->Height-27) - j) % 31 + 32;
					else h = 63.0;
					glTexCoord2f( 8.0/128.0, 32.0/128.0);	glVertex2f(i, j);
					glTexCoord2f( 8.0/128.0,	h/128.0);	glVertex2f(i, j+h-32);
					glTexCoord2f(   w/128.0,	h/128.0);	glVertex2f(i+w-8, j+h-32);
					glTexCoord2f(   w/128.0, 32.0/128.0);	glVertex2f(i+w-8, j);
				}
			}
		glEnd();
		glBegin(GL_QUADS);
			// right side of window.
			glTexCoord2f(1.0-7.0/128.0, 27.0/128.0);		glVertex2f(self->Width-7, 26);
			glTexCoord2f(1.0-7.0/128.0, 1.0-27.0/128.0);	glVertex2f(self->Width-7, self->Height-27);
			glTexCoord2f(1,   1.0-27.0/128.0);				glVertex2f(self->Width, self->Height-27);
			glTexCoord2f(1,   27.0/128.0);					glVertex2f(self->Width, 26);
		glEnd();
		glBegin(GL_QUADS);
			// bottom left corner of window.
			glTexCoord2f( 0.0/128.0, 1.0-27.0/128.0);		glVertex2f(0,  self->Height-27);
			glTexCoord2f( 0.0/128.0, 128.0/128.0);			glVertex2f(0,  self->Height);
			glTexCoord2f(64.0/128.0, 128.0/128.0);			glVertex2f(63, self->Height);
			glTexCoord2f(64.0/128.0, 1.0-27.0/128.0);		glVertex2f(63, self->Height-27);
		glEnd();
		glBegin(GL_QUADS);
			// bottom of window.
			glTexCoord2f(64.0/128.0, 1.0-27.0/128.0);		glVertex2f(63, self->Height-27);
			glTexCoord2f(64.0/128.0, 128.0/128.0);			glVertex2f(63, self->Height);
			glTexCoord2f(96.0/128.0, 128.0/128.0);			glVertex2f(self->Width-32, self->Height);
			glTexCoord2f(96.0/128.0, 1.0-27.0/128.0);		glVertex2f(self->Width-32, self->Height-27);
		glEnd();
		glBegin(GL_QUADS);
			// bottom right corder of window.
			glTexCoord2f(96.0/128.0, 1.0-27.0/128.0);		glVertex2f(self->Width-32, self->Height-27);
			glTexCoord2f(96.0/128.0, 128.0/128.0);			glVertex2f(self->Width-32, self->Height);
			glTexCoord2f(1, 128.0/128.0);					glVertex2f(self->Width, self->Height);
			glTexCoord2f(1, 1.0-27.0/128.0);				glVertex2f(self->Width, self->Height-27);
		glEnd();

		glBegin(GL_QUADS);
			// window close button
			glTexCoord2f(104.0/128.0, 32.0/128.0);			glVertex3f(self->Width-22, 8,  0.01);
			glTexCoord2f(104.0/128.0, 46.0/128.0);			glVertex3f(self->Width-22, 24, 0.01);
			glTexCoord2f(120.0/128.0, 46.0/128.0);			glVertex3f(self->Width-6, 24, 0.01);
			glTexCoord2f(120.0/128.0, 32.0/128.0);			glVertex3f(self->Width-6, 8,  0.01);
		glEnd();

		// Draw the Title:
		if (self->Title) glWrite(64, 8, self->Title);
		if (self->StatusBar) glWrite(8, self->Height - 26, self->StatusBar);

		// draw the Panels
		glTranslatef(0, 0, 0.02);
		for (i=0; i<self->numPanels; ++i)
			if (self->Panel[i])
				TPanel_Render(self->Panel[i]);

		// draw the Buttons
		glTranslatef(0, 0, 0.02);
		for (i=0; i<self->numButtons; ++i)
			if (self->Button[i])
				TButton_Render(self->Button[i]);

		// draw the Checkboxes
		for (i=0; i<self->numCheckBoxes; ++i)
			if (self->CheckBox[i])
				TCheckBox_Render(self->CheckBox[i]);

		// draw the RadioButtons
		for (i=0; i<self->numRadioButtons; ++i)
			if (self->RadioButton[i])
				TRadioButton_Render(self->RadioButton[i]);

		// draw the Text
		for (i=0; i<self->numLabels; ++i)
			if (self->Label[i])
				TLabel_Render(self->Label[i]);

		// User-defined paint function:
		if (self->Paint) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(self->X + 6, Window.Height - (self->Y + self->Height - 27), self->Width-12, self->Height - 27 - 26);
			
			// Translate to the upper-left corner of inside the window:
			glTranslatef(6, 26, 0.02);
			
			glBindTexture(GL_TEXTURE_2D, 0);
			self->Paint(self);
			glDisable(GL_SCISSOR_TEST);
		}

		glPopMatrix();
		glBlendFunc(GL_ONE,GL_ONE);
		glDisable(GL_BLEND);

		// draw the child windows
		glPushMatrix();
		glTranslatef(0, 0, 0.1);
		for (i=0; i<self->numChildren; ++i)
			TWindow_Render(self->Child[i]);
		glPopMatrix();
	}
}


//  TButton

// ------------------------------------------------------------------
//   Initialise a button
// ------------------------------------------------------------------
void TButton_Init(TButton *self, int bX, int bY, int bWidth, int bHeight, char *bCaption)
{
	if (!self) return;
	self->X = bX;
	self->Y = bY;
	self->Width   = bWidth;
	self->Height  = bHeight;
	self->Caption = bCaption;
}


// ------------------------------------------------------------------
//   Render the button
// ------------------------------------------------------------------
void TButton_Render(TButton *self)
{
	if (!self) return;
	char	*caption = self->Caption;
	if (caption == NULL) caption = "<null>";
	if (self->Pressed) {
		glBegin(GL_QUADS);
			// left side
			glTexCoord2f(57.0/128.0, 32.0/128.0); glVertex2f(self->X,   self->Y);
			glTexCoord2f(57.0/128.0, 57.0/128.0); glVertex2f(self->X,   self->Y+self->Height);
			glTexCoord2f(60.0/128.0, 57.0/128.0); glVertex2f(self->X+4, self->Y+self->Height);
			glTexCoord2f(60.0/128.0, 32.0/128.0); glVertex2f(self->X+4, self->Y);

			// middle
			glTexCoord2f(60.0/128.0, 32.0/128.0); glVertex2f(self->X+4, self->Y);
			glTexCoord2f(60.0/128.0, 57.0/128.0); glVertex2f(self->X+4, self->Y+self->Height);
			glTexCoord2f(66.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width-4, self->Y+self->Height);
			glTexCoord2f(66.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width-4, self->Y);

			// right side
			glTexCoord2f(66.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width-4, self->Y);
			glTexCoord2f(66.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width-4, self->Y+self->Height);
			glTexCoord2f(71.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width, self->Y+self->Height);
			glTexCoord2f(71.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width, self->Y);
		glEnd();

		glWrite(self->X + 1 + (self->Width - fontWidth(caption)) / 2, self->Y+(self->Height/2)-7, caption);
	} else {
		glBegin(GL_QUADS);
			// left side
			glTexCoord2f(41.0/128.0, 32.0/128.0); glVertex2f(self->X,   self->Y);
			glTexCoord2f(41.0/128.0, 57.0/128.0); glVertex2f(self->X,   self->Y+self->Height);
			glTexCoord2f(46.0/128.0, 57.0/128.0); glVertex2f(self->X+6, self->Y+self->Height);
			glTexCoord2f(46.0/128.0, 32.0/128.0); glVertex2f(self->X+6, self->Y);

			// middle
			glTexCoord2f(46.0/128.0, 32.0/128.0); glVertex2f(self->X+6, self->Y);
			glTexCoord2f(46.0/128.0, 57.0/128.0); glVertex2f(self->X+6, self->Y+self->Height);
			glTexCoord2f(50.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width-6, self->Y+self->Height);
			glTexCoord2f(50.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width-6, self->Y);

			// right side
			glTexCoord2f(50.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width-6, self->Y);
			glTexCoord2f(50.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width-6, self->Y+self->Height);
			glTexCoord2f(55.0/128.0, 57.0/128.0); glVertex2f(self->X+self->Width, self->Y+self->Height);
			glTexCoord2f(55.0/128.0, 32.0/128.0); glVertex2f(self->X+self->Width, self->Y);
		glEnd();

		glWrite(self->X + (self->Width - fontWidth(caption)) / 2, self->Y+(self->Height/2)-8, caption);
	}
}


//  TPanel

// ------------------------------------------------------------------
//   Initialise a panel
// ------------------------------------------------------------------
void TPanel_Init(TPanel *self, int pX, int pY, int pWidth, int pHeight)
{
	if (!self) return;
	self->X = pX;
	self->Y = pY;
	self->Width   = pWidth;
	self->Height  = pHeight;
	self->Visible = TRUE;
}


// ------------------------------------------------------------------
//   Render the Panel
// ------------------------------------------------------------------
void TPanel_Render(TPanel *self) {
	if (!self) return;
	if (self->Visible) {
		glBegin(GL_QUADS);
			// top left corner of panel.
			glTexCoord2f( 8.0/128.0, 64.0/128.0); glVertex2f(self->X, self->Y);
			glTexCoord2f( 8.0/128.0, 74.0/128.0); glVertex2f(self->X, self->Y+10);
			glTexCoord2f(18.0/128.0, 74.0/128.0); glVertex2f(self->X+10, self->Y+10);
			glTexCoord2f(18.0/128.0, 64.0/128.0); glVertex2f(self->X+10, self->Y);

			// top of panel.
			glTexCoord2f(18.0/128.0, 64.0/128.0); glVertex2f(self->X+10, self->Y);
			glTexCoord2f(18.0/128.0, 74.0/128.0); glVertex2f(self->X+10, self->Y+10);
			glTexCoord2f(30.0/128.0, 74.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y+10);
			glTexCoord2f(30.0/128.0, 64.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y);

			// top right corder of panel.
			glTexCoord2f(30.0/128.0, 64.0/128.0); glVertex2f(self->X+self->Width-10, self->Y);
			glTexCoord2f(30.0/128.0, 74.0/128.0); glVertex2f(self->X+self->Width-10, self->Y+10);
			glTexCoord2f(40.0/128.0, 74.0/128.0); glVertex2f(self->X+self->Width, self->Y+10);
			glTexCoord2f(40.0/128.0, 64.0/128.0); glVertex2f(self->X+self->Width, self->Y);

			// left side of panel.
			glTexCoord2f( 8.0/128.0, 74.0/128.0); glVertex2f(self->X, self->Y+10);
			glTexCoord2f( 8.0/128.0, 86.0/128.0); glVertex2f(self->X, self->Y+self->Height-10);
			glTexCoord2f(18.0/128.0, 86.0/128.0); glVertex2f(self->X+10, self->Y+self->Height-10);
			glTexCoord2f(18.0/128.0, 74.0/128.0); glVertex2f(self->X+10, self->Y+10);

			// middle of panel.
			glTexCoord2f(18.0/128.0, 74.0/128.0); glVertex2f(self->X+10, self->Y+10);
			glTexCoord2f(18.0/128.0, 86.0/128.0); glVertex2f(self->X+10, self->Y+self->Height-10);
			glTexCoord2f(30.0/128.0, 86.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y+self->Height-10);
			glTexCoord2f(30.0/128.0, 74.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y+10);

			// right side of panel.
			glTexCoord2f(30.0/128.0, 74.0/128.0); glVertex2f(self->X+self->Width-10, self->Y+10);
			glTexCoord2f(30.0/128.0, 86.0/128.0); glVertex2f(self->X+self->Width-10, self->Y+self->Height-10);
			glTexCoord2f(40.0/128.0, 86.0/128.0); glVertex2f(self->X+self->Width, self->Y+self->Height-10);
			glTexCoord2f(40.0/128.0, 74.0/128.0); glVertex2f(self->X+self->Width, self->Y+10);

			// bottom left corner of panel.
			glTexCoord2f( 8.0/128.0, 86.0/128.0); glVertex2f(self->X, self->Y+self->Height-10);
			glTexCoord2f( 8.0/128.0, 96.0/128.0); glVertex2f(self->X, self->Y+self->Height);
			glTexCoord2f(18.0/128.0, 96.0/128.0); glVertex2f(self->X+10, self->Y+self->Height);
			glTexCoord2f(18.0/128.0, 86.0/128.0); glVertex2f(self->X+10, self->Y+self->Height-10);

			// bottom middle of panel.
			glTexCoord2f(18.0/128.0, 86.0/128.0); glVertex2f(self->X+10, self->Y+self->Height-10);
			glTexCoord2f(18.0/128.0, 96.0/128.0); glVertex2f(self->X+10, self->Y+self->Height);
			glTexCoord2f(30.0/128.0, 96.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y+self->Height);
			glTexCoord2f(30.0/128.0, 86.0/128.0); glVertex2f(self->X+10+(self->Width-20), self->Y+self->Height-10);

			// bottom right corner of panel.
			glTexCoord2f(30.0/128.0, 86.0/128.0); glVertex2f(self->X+self->Width-10, self->Y+self->Height-10);
			glTexCoord2f(30.0/128.0, 96.0/128.0); glVertex2f(self->X+self->Width-10, self->Y+self->Height);
			glTexCoord2f(40.0/128.0, 96.0/128.0); glVertex2f(self->X+self->Width, self->Y+self->Height);
			glTexCoord2f(40.0/128.0, 86.0/128.0); glVertex2f(self->X+self->Width, self->Y+self->Height-10);
		glEnd();
	}
}


//  TCheckbox

// ------------------------------------------------------------------
//   Initialise a checkbox
// ------------------------------------------------------------------
void TCheckBox_Init(TCheckBox *self, int cbX, int cbY, int cbChecked, char *Text)
{
	self->X = cbX;
	self->Y = cbY;
	self->Visible = TRUE;
	self->Checked = cbChecked;
	self->Text = Text;
	self->Width = fontWidth(Text);
}


// ------------------------------------------------------------------
//   Render a Checkbox
// ------------------------------------------------------------------
void TCheckBox_Render(TCheckBox *self)
{
	char	*caption = self->Text;
	if (caption == NULL) caption = "<null>";
	if (self->Visible) {
		if (self->Checked) {
			glBegin(GL_QUADS);
				glTexCoord2f(88.0/128.0, 32.0/128.0); glVertex2f(self->X, self->Y);
				glTexCoord2f(88.0/128.0, 48.0/128.0); glVertex2f(self->X, self->Y+16);
				glTexCoord2f(104.0/128.0, 48.0/128.0); glVertex2f(self->X+16, self->Y+16);
				glTexCoord2f(104.0/128.0, 32.0/128.0); glVertex2f(self->X+16, self->Y);
			glEnd();
			glWrite(self->X + 18, self->Y, caption);
		} else {
			glBegin(GL_QUADS);
				glTexCoord2f(72.0/128.0, 32.0/128.0); glVertex2f(self->X, self->Y);
				glTexCoord2f(72.0/128.0, 48.0/128.0); glVertex2f(self->X, self->Y+16);
				glTexCoord2f(88.0/128.0, 48.0/128.0); glVertex2f(self->X+16, self->Y+16);
				glTexCoord2f(88.0/128.0, 32.0/128.0); glVertex2f(self->X+16, self->Y);
			glEnd();
			glWrite(self->X + 18, self->Y, caption);
		}
	}
}


//  TRadioButton

// ------------------------------------------------------------------
//   Initialise a Radio Button
// ------------------------------------------------------------------
void TRadioButton_Init(TRadioButton *self, int rbX, int rbY, int rbGroup, int rbChecked, char *Text)
{
	self->X = rbX;
	self->Y = rbY;
	self->Visible = TRUE;
	self->Checked = rbChecked;
	self->Group = rbGroup;
	self->Text = Text;
	self->Width = fontWidth(Text);
}


// ------------------------------------------------------------------
//   Render Radio Button
// ------------------------------------------------------------------
void TRadioButton_Render(TRadioButton *self)
{
	char	*caption = self->Text;
	if (caption == NULL) caption = "<null>";
	if (self->Visible) {
		if (self->Checked) {
			glBegin(GL_QUADS);
				glTexCoord2f(88.0/128.0, 48.0/128.0);	glVertex2f(self->X, self->Y);
				glTexCoord2f(88.0/128.0, 64.0/128.0);	glVertex2f(self->X, self->Y+16);
				glTexCoord2f(104.0/128.0, 64.0/128.0);	glVertex2f(self->X+16, self->Y+16);
				glTexCoord2f(104.0/128.0, 48.0/128.0);	glVertex2f(self->X+16, self->Y);
			glEnd();
			glWrite(self->X + 18, self->Y, caption);
		} else {
			glBegin(GL_QUADS);
				glTexCoord2f(72.0/128.0, 48.0/128.0); glVertex2f(self->X, self->Y);
				glTexCoord2f(72.0/128.0, 64.0/128.0); glVertex2f(self->X, self->Y+16);
				glTexCoord2f(88.0/128.0, 64.0/128.0); glVertex2f(self->X+16, self->Y+16);
				glTexCoord2f(88.0/128.0, 48.0/128.0); glVertex2f(self->X+16, self->Y);
			glEnd();
			glWrite(self->X + 18, self->Y, caption);
		}
	}
}

void TRadioButton_Click(TRadioButton *self)
{
	TWindow	*Wnd = self->Parent;
	int	j;

	// uncheck all the other radio buttons in self group
	for (j=0; j<Wnd->numRadioButtons; ++j)
		if (Wnd->RadioButton[j]->Group == self->Group)
			Wnd->RadioButton[j]->Checked = FALSE;

	self->Checked = TRUE;

	if (self->onClick)	   // if there is a click void, run it
		self->onClick(self);
}

//  TLabel

// ------------------------------------------------------------------
//   Initialise the Label
// ------------------------------------------------------------------
void TLabel_Init(TLabel *self, int tX, int tY, char *Text)
{
	self->X = tX;
	self->Y = tY;
	self->Caption = Text;
}

// ------------------------------------------------------------------
//   Render Label Text
// ------------------------------------------------------------------
void TLabel_Render(TLabel *self)
{
	char	*caption = self->Caption;
	if (caption == NULL) caption = "<null>";
	glWrite(self->X, self->Y, caption);
}

//  TMouse

// ------------------------------------------------------------------
//   void to draw the mouse									 
// ------------------------------------------------------------------
void DrawMouse()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
		glTexCoord2f(40.0/128.0, 64.0/128.0); glVertex3f(Mouse.X,	Mouse.Y, 1);
		glTexCoord2f(72.0/128.0, 64.0/128.0); glVertex3f(Mouse.X+32, Mouse.Y, 1);
		glTexCoord2f(72.0/128.0, 32.0/128.0); glVertex3f(Mouse.X+32, Mouse.Y+32, 1);
		glTexCoord2f(40.0/128.0, 32.0/128.0); glVertex3f(Mouse.X,	Mouse.Y+32, 1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

// ------------------------------------------------------------------
//   void to check if the user clicked in a window or object
// ------------------------------------------------------------------
int OnMouseDown(TWindow *Wnd)
{
	int	WndClick;
	int	i, j;
	int	MouseX, MouseY;

	if (Wnd->Visible == FALSE) return;

	if (MouseDrag.Drag == TRUE) {
		MouseDrag.Window->X = MouseDrag.X + Mouse.X;
		MouseDrag.Window->Y = MouseDrag.Y + Mouse.Y;
		return TRUE;
	}

	WndClick = FALSE;

	// First check child windows since they can be on top.
	for (i=0; i<Wnd->numChildren; ++i)
		if ((Wnd->Child[i]->Visible) && (WndClick == FALSE))
			WndClick = OnMouseDown(Wnd->Child[i]);

	if (WndClick == TRUE) return TRUE;

	// test to see if user clicked in a window
	MouseX   = Mouse.X;
	MouseY   = Mouse.Y;
	if ((MouseX > Wnd->X) && (MouseX < Wnd->X + Wnd->Width))
		if ((MouseY > Wnd->Y) && (MouseY < Wnd->Y + Wnd->Height))
			WndClick = TRUE;

	// if something inside the window was clicked, then find the object
	if (WndClick == TRUE) {
		// Test to see if user clicked on window close icon
		if ((MouseX > Wnd->X + Wnd->Width-22) && (MouseX < Wnd->X + Wnd->Width-6))
			if ((MouseY > Wnd->Y+8) && (MouseY < Wnd->Y + 24)) {
				Wnd->Visible = FALSE;
				return TRUE;
			}

		// Test to see if user clicked in caption bar
		if ((MouseX > Wnd->X) && (MouseX < Wnd->X + Wnd->Width))
			if ((MouseY > Wnd->Y+1) && (MouseY < Wnd->Y + 26)) {
				MouseDrag.Drag = TRUE;
				MouseDrag.X	= Wnd->X - Mouse.X;
				MouseDrag.Y	= Wnd->Y - Mouse.Y;
				MouseDrag.Window = Wnd;
				return TRUE;
			}

		// recalculate coordinates relative to window
		MouseX = MouseX - Wnd->X;
		MouseY = MouseY - Wnd->Y;

		if ((ButtonPressed == NULL) && (Mouse.Held == FALSE)) {
			// Test and execute button click
			for (i=0; i<Wnd->numButtons; ++i) {
				if ((MouseX > Wnd->Button[i]->X) && (MouseX < Wnd->Button[i]->X + Wnd->Button[i]->Width))
					if ((MouseY > Wnd->Button[i]->Y) && (MouseY < Wnd->Button[i]->Y + Wnd->Button[i]->Height)) {
						Wnd->Button[i]->Pressed = TRUE;
						ButtonPressed = Wnd->Button[i];
						Mouse.Held = TRUE;
						return TRUE;
					}
			}

			// Test and execute checkbox click
			for (i=0; i<Wnd->numCheckBoxes; ++i) {
				if ((MouseX > Wnd->CheckBox[i]->X) && (MouseX < Wnd->CheckBox[i]->X + 18 + Wnd->CheckBox[i]->Width))
					if ((MouseY > Wnd->CheckBox[i]->Y) && (MouseY < Wnd->CheckBox[i]->Y + 16)) {
						Wnd->CheckBox[i]->Checked = !(Wnd->CheckBox[i]->Checked);
						if (Wnd->CheckBox[i]->onClick)	   // if there is a click void, run it
							Wnd->CheckBox[i]->onClick(Wnd->CheckBox[i]);
						Mouse.Held = TRUE;
						return TRUE;
					}
			}

			// Test and execute RadioButton click
			for (i=0; i<Wnd->numRadioButtons; ++i) {
				if ((MouseX > Wnd->RadioButton[i]->X) && (MouseX < Wnd->RadioButton[i]->X + 18 + Wnd->RadioButton[i]->Width))
					if ((MouseY > Wnd->RadioButton[i]->Y) && (MouseY < Wnd->RadioButton[i]->Y + 16)) {
						TRadioButton_Click(Wnd->RadioButton[i]);

						Mouse.Held = TRUE;
						return TRUE;
					}
			}
			
			// Execution will not reach this point unless a click has been unhandled
			// by all controls on the window.

			// User-defined onClick event:
			if (Wnd->onClick) Wnd->onClick(Wnd, MouseX, MouseY, Mouse.Button);
		}
	}

	return WndClick;
}


// ------------------------------------------------------------------
//   Event that runs when mouse button is released
// ------------------------------------------------------------------
void OnMouseUp()
{
	MouseDrag.Drag = FALSE;
	Mouse.Button = 0;
	Mouse.Held = FALSE;

	if (ButtonPressed != NULL)
		ButtonPressed->Pressed = FALSE;

	if (ButtonPressed != NULL) {
		// if there is a click procedure, run it.
		if (ButtonPressed->onClick)
			ButtonPressed->onClick(ButtonPressed);
		ButtonPressed->Pressed = FALSE;
		ButtonPressed = NULL;
	}
}

// ------------------------------------------------------------------
//   Event that runs when keyboard button is pressed
// ------------------------------------------------------------------
void OnKeyDown(SDL_KeyboardEvent *kev) {
	
}

// ------------------------------------------------------------------
//   Event that runs when keyboard button is released
// ------------------------------------------------------------------
void OnKeyUp(SDL_KeyboardEvent *kev) {
	
}
