#include "Win32Window.h"
#include "bitmap.h"
#include <fstream>
#include <math.h>

std::ofstream LOG;
#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

class App: public Win32Window
{
public:
	App():
	Win32Window(std::string("12231036 马政飞"), 1200, 700)
	{

	}


	void threadFunc()
	{


	}	


	void timerFunc()
	{



	}
	
	void mousePressed(int leftOrRight, int x, int y)
	{
		/*
		static int m;
		LOG<<"Mouse"<<std::endl;
		if(leftOrRight == 0)
		{
			//left mouse pressed
			drawLine(10+10*m, 20+ 5* m, 50 + 10*m, 100+5*m, RGB(200,10,20));
			m++;
			redraw();

		}else if(leftOrRight == 1)
		{
			//right mouse pressed
			fillEllipse(10+10*m, 20+ 5* m, 50 + 10*m, 100+5*m, RGB(0,100,200));
			m++;
			redraw();

		}

*/
	}
	
	/*
	keycodes
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	*/
	void keyboardPressed(int keycode)
	{
		switch (keycode)
	    {
	    case VK_UP:
	       
	        break;

	    case VK_DOWN:
	        break;

	    case VK_LEFT:
	        
	        break;

	    case VK_RIGHT:
	        
	        break;

	       case VK_HOME:
	    	clear();
	        break;

	    case VK_END:
	        
	        break;

	    case VK_F1:
	        
	        break;

	    case VK_F2:
	        
	        break;

	    default:
	    	LOG <<"Key pressed " <<(int)keycode << std::endl; 


	    }

	}


};

static App app;
static pixel_map templ;
static pixel_map sample;

static void button1_pressed()
{
	//templ.load_from_bmp("moban2.bmp");
	//sample.load_from_bmp("shahe4096qb_gray.bmp");

	app.clear();
	templ.draw(app.hDoubleBufferDC, 700, 10, 1);
	sample.draw(app.hDoubleBufferDC, 10, 10, 1);
	app.redraw();
}



#define BUTTONS_INTERVAL 200
#define BUTTONS_X 150

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
	int retval;
	LOG.open("log.txt", std::fstream::out |std::fstream::trunc);

	app.addButton(std::string("载入图片"),	BUTTONS_X + 0 * BUTTONS_INTERVAL, app.height-80, 100, 30, button1_pressed);
	app.addButton(std::string("计算金字塔"),BUTTONS_X + 1 * BUTTONS_INTERVAL,app.height-80,100,30, NULL);
	app.addButton(std::string("模板匹配"),	BUTTONS_X + 2 * BUTTONS_INTERVAL,app.height-80,100,30, NULL);


	app.init();
	retval = app.mainLoop();

	LOG.close();
	return retval;
}
