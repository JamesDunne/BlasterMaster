// glwindows.c  --  port of Win.pas from glWindows Delphi package
// Basic rendering code was directly ported from Win.pas;
// however, some architectural changes have been made.

//   - James Dunne
//	02/16/04

#ifndef GLWINDOWS_H
#define GLWINDOWS_H

#ifndef TRUE
#define	TRUE	-1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#include <SDL/SDL.h>

struct TWindow;

typedef struct {
	int		Width, Height;
	int		CaptionBarHeight;
} TAppWindow;

typedef struct {
	int		X, Y;
	int		Button;
	int		Held;
} TMouse;

typedef struct {
	int		Drag;
	int		X, Y;
	struct TWindow	*Window;
} TMouseDrag;

typedef struct TButton {
	int		X, Y;
	int		Width, Height;
	char	*Caption;
	int		Pressed;
	int		Index;
	struct TWindow	*Parent;
	void	(*onClick)(struct TButton *self);
} TButton;
void	TButton_Init(TButton *self, int bX, int bY, int bWidth, int bHeight, char *bCaption);
void	TButton_Render(TButton *self);

typedef struct TLabel {
	int		X, Y;
	int		Visible;
	char	*Caption;
	int		Index;
	struct TWindow	*Parent;
} TLabel;
void	TLabel_Init(TLabel *self, int tX, int tY, char *Text);
void	TLabel_Render(TLabel *self);

typedef struct TPanel {
	int		X, Y;
	int		Width, Height;
	int		Visible;
	int		Index;
	struct TWindow	*Parent;
} TPanel;
void	TPanel_Init(TPanel *self, int pX, int pY, int pWidth, int pHeight);
void	TPanel_Render(TPanel *self);

typedef struct TCheckBox {
	int		X, Y;
	int		Width;
	int		Visible;
	int		Checked;
	char	*Text;
	int		Index;
	struct TWindow	*Parent;
	void	(*onClick)(struct TCheckBox *self);
} TCheckBox;
void	TCheckBox_Init(TCheckBox *self, int cbX, int cbY, int cbChecked, char *Text);
void	TCheckBox_Render(TCheckBox *self);

typedef struct TRadioButton {
	int		X, Y;
	int		Width;
	int		Visible;
	int		Checked;
	int		Group;
	char	*Text;
	int		Index;
	struct TWindow	*Parent;
	void	(*onClick)(struct TRadioButton *self);
} TRadioButton;
void	TRadioButton_Init(TRadioButton *self, int rbX, int rbY, int rbGroup, int rbChecked, char *Text);
void	TRadioButton_Render(TRadioButton *self);
void	TRadioButton_Click(TRadioButton *self);

typedef struct TWindow {
	int		X, Y;
	int		Width, Height;
	int		Visible;
	double	ZOrder;
	double	Alpha;
	char	*Title, *StatusBar;
	// Callbacks:
	void	(*onClick)(struct TWindow *self, int MouseX, int MouseY, int MouseButton);
	int		(*onKeyDown)(SDL_KeyboardEvent *kev);
	int		(*onKeyUp)(SDL_KeyboardEvent *kev);
	void	(*Paint)(struct TWindow *self);
	// Components:
	TButton			**Button;
	TLabel			**Label;
	TPanel			**Panel;
	TCheckBox		**CheckBox;
	TRadioButton	**RadioButton;
	struct TWindow	**Child;
	// Count:
	int		numButtons;
	int		numLabels;
	int		numPanels;
	int		numCheckBoxes;
	int		numRadioButtons;
	int		numChildren;
} TWindow;

void			TWindow_Init(TWindow *self, int wX, int wY, int wWidth, int wHeight, char *Title);
void			TWindow_AddWindow(TWindow *self, TWindow *Child);

// Adding controls to a window:
TButton			*TWindow_AddButton(TWindow *self, int btnX, int btnY, int btnWidth, int btnHeight, char *btnCaption);
TLabel			*TWindow_AddLabel(TWindow *self, int tX, int tY, char *tCaption);
TPanel			*TWindow_AddPanel(TWindow *self, int pX, int pY, int pWidth, int pHeight);
TCheckBox		*TWindow_AddCheckBox(TWindow *self, int cbX, int cbY, int Checked, char *Text);
TRadioButton	*TWindow_AddRadioButton(TWindow *self, int rbX, int rbY, int rbGroup, int Checked, char *Text);

// Removing controls from a window:
void	TWindow_RemoveButton(TWindow *self, TButton *object);
void	TWindow_RemoveLabel(TWindow *self, TLabel *object);
void	TWindow_RemovePanel(TWindow *self, TPanel *object);
void	TWindow_RemoveCheckBox(TWindow *self, TCheckBox *object);
void	TWindow_RemoveRadioButton(TWindow *self, TRadioButton *object);

// Removing all controls from a window:
void	TWindow_RemoveAllButtons(TWindow *self);
void	TWindow_RemoveAllLabels(TWindow *self);
void	TWindow_RemoveAllPanels(TWindow *self);
void	TWindow_RemoveAllCheckBoxs(TWindow *self);
void	TWindow_RemoveAllRadioButtons(TWindow *self);

// Render the window:
void	TWindow_Render(TWindow *self);

extern	GLuint		GUITexture;

extern	TAppWindow	Window;
extern	TMouse		Mouse;

void	BuildFont();
void	DrawMouse();
void	OnMouseUp();
int		OnMouseDown(TWindow *Wnd);

#endif
