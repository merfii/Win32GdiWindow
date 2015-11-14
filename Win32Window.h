#pragma once

#include <Windows.h>
#include <process.h>
#include <string>
#include <vector>
#include <fstream>

typedef void (*PTRFUN)(void); 


class Button;
class Label;
class TextBox;

class Win32Window
{
public:
	HWND hWnd;
	HINSTANCE hInst;
	int width, height;
	std::string appName;
	char *title;

	HDC hDoubleBufferDC;
	HBITMAP hDoubleBufferBM;
	
	Win32Window();
	//SetWindowText(m_hwnd, text.c_str());
	Win32Window(std::string name, const int w, const int h);
	virtual ~Win32Window();

	int init();
	int mainLoop();
	void sleep(int ms);

	void DISPLOCK();
	void DISPUNLOCK();
	void redraw();

	Button* addButton(std::string text, int x, int y, int width, int height, PTRFUN call_func, bool defaultButton = false);
	TextBox* addTextbox(std::string text, int x, int y, int width, int height);
	Label* addLabel(std::string text, int x, int y, int width, int height);

	void messageBox(std::string title, std::string message, UINT style);
	void setTimerInterval(int ms);

	virtual void threadFunc() =0;
	virtual void timerFunc() =0;
	virtual void mousePressed(int leftOrRight, int x, int y) =0;
	virtual void keyboardPressed(int keycode) =0; 

// Painting functions
	void clear();
	void drawLine(int x0, int y0, int x1, int y1, COLORREF color, int linewidth = 2);	
	void drawRect(int x0, int y0, int x1, int y1, COLORREF color, int linewidth = 2);
	void drawCircle(int Ox, int Oy, int radius, COLORREF color, int linewidth = 2);
	void drawPolyline(const POINT *lppt, int n, COLORREF color, int linewidth = 2);
	void drawBezier(const POINT *lppt, int n, COLORREF color, int linewidth = 2);
	void drawText(char *str, int x, int y, int size, COLORREF color);

	void fillEllipse(int x0, int y0, int x1, int y1, COLORREF color);
	void fillRect(int x0, int y0, int x1, int y1, COLORREF color);

	void drawImageGraydrawImageGray(unsigned char *buffer, int width, int height, int x, int y);
	void drawImageRGBdrawImageRGB(unsigned char *buffer, int width, int height, int x, int y);
	void drawImageBITMAP(BITMAP bitmap, int x, int y, float scale = 1.0f);

private:
	int init_win32();
	void init_others();
	void draw_update(HDC hdc);

	static int getNewId();
	LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void staticThreadFunc(void *parg);

	std::vector<Button *> m_buttons;
	std::vector<TextBox *> m_textboxs;
	std::vector<Label *> m_labels;
	
	CRITICAL_SECTION displock;
	int timer_interval;

	static int idCount;
};




//hooks for user
void user_func();
void keyPressed(int key);

/*
MessageBox 
int WINAPI MessageBox(
  _In_opt_ HWND    hWnd,
  _In_opt_ LPCTSTR lpText,
  _In_opt_ LPCTSTR lpCaption,
  _In_     UINT    uType
);


type
	MB_OK 	0x00L
	MB_OKCANCEL		0x01L
	MB_YESNO	0x04L

icon: 
	MB_ICONINFORMATION	0x40L
	MB_ICONERROR		0x10L
	MB_ICONWARNING		0x30L

default button:
	MB_DEFBUTTON1 0x00000000L
	The first button is the default button.
	MB_DEFBUTTON1 is the default unless MB_DEFBUTTON2, MB_DEFBUTTON3, or MB_DEFBUTTON4 is specified.

return value:
	IDABORT 3   The Abort button was selected.
	IDCANCEL 2	The Cancel button was selected.
	IDCONTINUE 11	The Continue button was selected.
	IDIGNORE 5		The Ignore button was selected.
	IDNO 7		The No button was selected.
	IDOK 1 		The OK button was selected.
	IDRETRY 4 	The Retry button was selected.
	IDTRYAGAIN 10 	The Try Again button was selected.
	IDYES 6		The Yes button was selected.
*/


class Button
{
	friend class Win32Window;
public:
	Button(std::string text, int x, int y, int width, int height, int id, PTRFUN call_func, bool defaultButton):
	m_hwnd(NULL),
	m_x(x), 
	m_y(y),
	m_width(width),
	m_height(height),
	m_id(id),
	m_click(call_func),
	m_text(text),
	m_default(defaultButton)
	{

	}


private:
	HWND m_hwnd;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_id;

	PTRFUN m_click;
	std::string m_text;
	bool m_default;
};


class TextBox
{
	friend class Win32Window;
public:
	TextBox(std::string text, int x, int y, int width, int height,int id):
	m_hwnd(NULL),
	m_text(text),
	m_x(x),
	m_y(y),
	m_width(width),
	m_height(height),
	m_id(id)
	{

	}


private:
	HWND m_hwnd;
	std::string m_text;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_id;

};

class Label
{
	friend class Win32Window;
public:
	Label(std::string text, int x, int y, int width, int height, int id):
	m_hwnd(NULL),
	m_text(text),
	m_x(x),
	m_y(y),
	m_width(width),
	m_height(height),
	m_id(id)
	{

	}


private:
	HWND m_hwnd;
	std::string m_text;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_id;

};
