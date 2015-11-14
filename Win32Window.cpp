#include "Win32Window.h"


int Win32Window::idCount;

Win32Window::Win32Window() 
{
    appName = std::string("Demo Application");
    hWnd = NULL;
    width = 800;
    height = 600;
    timer_interval = 50;
    hDoubleBufferDC = NULL;
    hDoubleBufferBM = NULL;
    idCount = 10;
}
 



Win32Window::Win32Window(std::string name, const int w, const int h)
{
    hWnd = NULL;
    timer_interval = 50;
    hDoubleBufferDC = NULL;
    hDoubleBufferBM = NULL;
    idCount = 10;

    appName = name;
    width = w;
    height = h;
}


Win32Window::~Win32Window()
{
    //LRESULT SendMessage£¨HWND hWnd£¬UINT Msg£¬WPARAM wParam£¬LPARAM IParam£©
    SendMessage(hWnd, WM_CLOSE,0, 0);
    DeleteCriticalSection(&displock);

    for(std::vector<TextBox*>::iterator it= m_textboxs.begin();
        it != m_textboxs.end(); it ++)
    {
        TextBox *p = *it;
        delete p;
    }

    for(std::vector<Button*>::iterator it= m_buttons.begin();
        it != m_buttons.end(); it ++)
    {
        Button *p = *it;
        delete p;
    }
    
    for(std::vector<Label*>::iterator it= m_labels.begin();
        it != m_labels.end(); it ++)
    {
        Label *p = *it;
        delete p;
    }
       
}


int Win32Window::init()
{
    int rtval;

    rtval = init_win32();
    if(rtval != 0)
        return rtval;

    init_others();
    return 0;
}

int Win32Window::init_win32()
{
    hInst = GetModuleHandle(NULL);
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hInstance = hInst;
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND; 
    /* Use default icon and mouse-pointer */
    wincl.lpszMenuName = NULL;                 /* No menu */

    wincl.lpszClassName = appName.c_str();


    wincl.lpfnWndProc = &staticWindowProc;      /* This function is called by windows */

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return -1;

    /* The class is registered, let's create the program*/
    hWnd = CreateWindow (
               (LPSTR)appName.c_str(),     /* Classname */
               (LPSTR)appName.c_str(),       /* Title Text */
               (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               width,                 /* The programs width */
               height,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hInst,       /* Program Instance handler */
               (LPVOID)this                 /* No Window Creation data */
           );

    for(std::vector<Button*>::iterator it= m_buttons.begin();
        it != m_buttons.end(); it ++)
    {
        Button *p = *it; 
        p->m_hwnd = CreateWindow("BUTTON",
            (LPSTR)p->m_text.c_str(),
            WS_CHILD | WS_VISIBLE | (p->m_default ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON), 
            p->m_x,
            p->m_y, 
            p->m_width,
            p->m_height, 
            hWnd, 
            (HMENU)p->m_id,
            hInst,
            NULL);
        SendMessage(p->m_hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
    }

    for(std::vector<TextBox*>::iterator it= m_textboxs.begin();
        it != m_textboxs.end(); it ++)
    {
        TextBox *p = *it; 
        p->m_hwnd = CreateWindow("EDIT",
            (LPSTR)p->m_text.c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER, 
            p->m_x,
            p->m_y, 
            p->m_width,
            p->m_height, 
            hWnd, 
            (HMENU)p->m_id,
            hInst,
            NULL);
        SendMessage(p->m_hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(TRUE, 0));
        SendMessage(p->m_hwnd, EM_SETMARGINS, static_cast<WPARAM>(EC_LEFTMARGIN | EC_RIGHTMARGIN), MAKELPARAM(2, 2));
    }

    for(std::vector<Label*>::iterator it = m_labels.begin();
     it != m_labels.end(); ++it)
    {
        Label *p= *it;

        p->m_hwnd = CreateWindow("STATIC",
            (LPSTR) p->m_text.c_str(), 
            WS_CHILD | WS_VISIBLE,
            p->m_x, p->m_y, p->m_width, p->m_height, 
            hWnd, 
            (HMENU)p->m_id, hInst, NULL);

        SendMessage(p->m_hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(TRUE, 0));
    }
}


void Win32Window::init_others()
{
    InitializeCriticalSection(&displock);
    _beginthread(staticThreadFunc, 0, this);

    SetTimer(hWnd , 1, timer_interval, NULL);
}

int Win32Window::mainLoop()
{
    MSG messages;
    /* Make the window visible on the screen */
    ShowWindow (hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

     /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

LRESULT CALLBACK Win32Window::staticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Win32Window *thisWin;
    if(uMsg == WM_CREATE)
    {
        thisWin = (Win32Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)thisWin);
    }
    else
        thisWin = (Win32Window*)GetWindowLongPtr(hWnd, GWL_USERDATA);

    if(thisWin == NULL)
        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    return thisWin->windowProc(hWnd, uMsg, wParam, lParam);
}

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
 

LRESULT CALLBACK Win32Window::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch(uMsg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        draw_update(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_ERASEBKGND:
        break;

    case WM_TIMER:
        timerFunc();
        break;

    case WM_KEYDOWN:
        keyboardPressed((char)wParam);
        break;
    case WM_LBUTTONDBLCLK: 
    case WM_LBUTTONDOWN:
        mousePressed(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
        break;
    
    case WM_RBUTTONDBLCLK: 
    case WM_RBUTTONDOWN:
        mousePressed(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
        break;

    case WM_COMMAND:
        for(std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
        {
            Button *p = *it;
            if(LOWORD(wParam) ==p->m_id)
            {
                if(HIWORD(wParam) == BN_CLICKED)
                    if(p->m_click)
                        p->m_click();
                break;
            }
        }

            break;
    case WM_CREATE:
        break;
    case WM_CLOSE:
        DeleteObject(hDoubleBufferBM);
        DeleteDC    (hDoubleBufferDC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}


void Win32Window::draw_update(HDC hdc)
{
    DISPLOCK();
    if(hDoubleBufferDC == NULL)
    {
        hDoubleBufferDC = CreateCompatibleDC(hdc);
        hDoubleBufferBM = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hDoubleBufferDC, hDoubleBufferBM);
        clear();
    }

    // Transfer the off-screen DC to the screen
    BitBlt(hdc, 0, 0,
           width,
           height,
           hDoubleBufferDC, 0, 0, SRCCOPY);
    DISPUNLOCK();
 }


 void Win32Window::drawLine(int x0, int y0, int x1, int y1, COLORREF color, int linewidth)
 {
    if(hDoubleBufferDC)
    {
        HPEN hPen = CreatePen(PS_SOLID, linewidth, color);
        SelectObject(hDoubleBufferDC, hPen);
        MoveToEx(hDoubleBufferDC, x0, y0, NULL);
        LineTo(hDoubleBufferDC, x1, y1);
        DeleteObject(hPen);
    }

/*
        hPen = CreatePen(PS_DOT,1,RGB(0,255,0));
        SelectObject(hdc, hPen);
        SetBkColor(hdc, RGB(0,0,0));
        Rectangle(hdc, 10,10,200,200);

        hPen = CreatePen(PS_DASHDOTDOT,1,RGB(0,255,255));
        SelectObject(hdc, hPen);
        SetBkColor(hdc, RGB(255,0,0));
        SelectObject(hdc,CreateSolidBrush(RGB(0,0,0)));
        Rectangle(hdc, 210,10,400,200);

        bgRgn = CreateRectRgnIndirect(&clientRect);
        hBrush = CreateSolidBrush(RGB(200,200,200));
        FillRgn(hdc, bgRgn, hBrush);
*/

 }

 void Win32Window::drawRect(int x0, int y0, int x1, int y1, COLORREF color, int linewidth)
 {
    if(hDoubleBufferDC)
    {
        HPEN hPen = CreatePen(PS_SOLID, linewidth, color);
        SelectObject(hDoubleBufferDC, hPen);
        MoveToEx(hDoubleBufferDC, x0, y0, NULL);
        LineTo(hDoubleBufferDC, x1, y0);
        LineTo(hDoubleBufferDC, x1, y1);
        LineTo(hDoubleBufferDC, x0, y1);
        LineTo(hDoubleBufferDC, x0, y0);
        DeleteObject(hPen);
    }
}

void Win32Window::drawCircle(int Ox, int Oy, int radius, COLORREF color, int linewidth)
{
    HPEN hPen = CreatePen(PS_SOLID, linewidth, color);
    SelectObject(hDoubleBufferDC, hPen);
    SetArcDirection(hDoubleBufferDC,AD_CLOCKWISE);
    Arc(hDoubleBufferDC, 
        Ox - radius, Oy - radius, Ox + radius, Oy + radius, 
        Ox, Oy - radius, Ox, Oy + radius);
    SetArcDirection(hDoubleBufferDC,AD_COUNTERCLOCKWISE);
    Arc(hDoubleBufferDC, 
        Ox - radius, Oy - radius, Ox + radius, Oy + radius, 
        Ox, Oy - radius, Ox, Oy + radius);
    DeleteObject(hPen);
}


void Win32Window::drawText(char *str, int x, int y, int size, COLORREF color)
{
 
    HFONT font = CreateFont(
        size, 0, 0, 0, FW_NORMAL, 
        FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, 
        OUT_TT_PRECIS, 
        CLIP_DEFAULT_PRECIS, 
        PROOF_QUALITY, 
        FF_DONTCARE, 
        "Microsoft Sans Serif");  

    RECT rect;
    rect.left = x-3;
    rect.right = x + size * strlen(str)+5;
    rect.top = y-3;
    rect.bottom = y+size+3;

    SelectObject(hDoubleBufferDC, font);
    SetTextColor(hDoubleBufferDC, color);
    DrawText(hDoubleBufferDC, str, strlen(str), &rect, DT_LEFT|DT_TOP);

    DeleteObject(font);
}


void Win32Window::fillRect(int x0, int y0, int x1, int y1, COLORREF color)
 {
    if(hDoubleBufferDC)
    {
        RECT rect={x0,y0,x1, y1};
        FillRect(hDoubleBufferDC,&rect,CreateSolidBrush(color));
    }
}

void Win32Window::fillEllipse(int x0, int y0, int x1, int y1, COLORREF color)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    SelectObject(hDoubleBufferDC, hPen);
    HBRUSH brush = CreateSolidBrush(color);
    SelectObject(hDoubleBufferDC, brush);
    Ellipse(hDoubleBufferDC,x0, y0, x1, y1);
    DeleteObject(brush);
    DeleteObject(hPen);
}


/*
typedef struct tagBITMAP {
  LONG   bmType;
  LONG   bmWidth;
  LONG   bmHeight;
  LONG   bmWidthBytes;
  WORD   bmPlanes;
  WORD   bmBitsPixel;
  LPVOID bmBits;
} BITMAP, *PBITMAP;
*/

void Win32Window::drawImageGray(unsigned char *buffer, int width, int height, int x, int y)
{
    //TODO

    //create buffer
    unsigned char *tmpBuf = malloc(width * height);
    for (int i  = 0; i < ; ++i)
    {



    }

    //create paired BITMAP

    {
        unsigned line_len = calc_row_len(width, bits_per_pixel);
        unsigned img_size = line_len * height;
        unsigned rgb_size = calc_palette_size(0, bits_per_pixel) * sizeof(RGBQUAD);
        unsigned full_size = sizeof(BITMAPINFOHEADER) + rgb_size + img_size;

        BITMAPINFO *bmp = (BITMAPINFO *) new unsigned char[full_size];

        bmp->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
        bmp->bmiHeader.biWidth  = width;
        bmp->bmiHeader.biHeight = height;
        bmp->bmiHeader.biPlanes = 1;
        bmp->bmiHeader.biBitCount = (unsigned short)bits_per_pixel;
        bmp->bmiHeader.biCompression = 0;
        bmp->bmiHeader.biSizeImage = img_size;
        bmp->bmiHeader.biXPelsPerMeter = 0;
        bmp->bmiHeader.biYPelsPerMeter = 0;
        bmp->bmiHeader.biClrUsed = 0;
        bmp->bmiHeader.biClrImportant = 0;
    }

    {
            m_img_size  = calc_row_len(bmp->bmiHeader.biWidth, 
                                       bmp->bmiHeader.biBitCount) * 
                          bmp->bmiHeader.biHeight;

            m_full_size = calc_full_size(bmp);
            m_bmp       = bmp;
            m_buf       = calc_img_ptr(bmp);
    }

    ::SetDIBitsToDevice(
    hDc,            // handle to device context
    dvc_x,           // x-coordinate of upper-left corner of 
    dvc_y,           // y-coordinate of upper-left corner of 
    dvc_width,       // source rectangle width
    dvc_height,      // source rectangle height
    bmp_x,           // x-coordinate of lower-left corner of 
    bmp_y,           // y-coordinate of lower-left corner of 
    0,               // first scan line in array
    bmp_height,      // number of scan lines
    m_buf,           // address of array with DIB bits
    m_bmp,           // address of structure BITMAPINFO
    DIB_RGB_COLORS   // RGB or palette indexes
    );


    free tmpBuf;
}


void Win32Window::drawImageRGB(unsigned char *buffer, int width, int height, int x, int y)
{
    //TODO

}



void Win32Window::drawImageBITMAP(BITMAP bitmap, int x, int y, float scale)
{
    //TODO
    int bmp_x = 0;
    unsigned bmp_y = 0;
    unsigned bmp_width  = m_bmp->bmiHeader.biWidth;
    unsigned bmp_height = m_bmp->bmiHeader.biHeight; 
    
    int dvc_width  = bmp_width * scale;
    int dvc_height = bmp_height * scale;

    SetStretchBltMode(h_dc, COLORONCOLOR);
    StretchDIBits(
            h_dc,            // handle of device context 
            x,           // x-coordinate of upper-left corner 
            y,           // y-coordinate of upper-left corner
            dvc_width,       // width
            dvc_height,      // height
            0,
            0,           // x, y -coordinates of upper-left corner 
            bmp_width,       // width 
            bmp_height,      // height
            m_buf,           // address of bitmap bits 
            m_bmp,           // address of BITMAPINFO
            DIB_RGB_COLORS,  // usage 
            SRCCOPY          // raster operation code 
        );



}



Button* Win32Window::addButton(std::string text, int x, int y, int width, int height, PTRFUN call_func, bool defaultButton)
{
    Button *but = new Button(text, x, y, width, height, getNewId(), call_func, defaultButton);
    m_buttons.push_back(but);
}


TextBox* Win32Window::addTextbox(std::string text, int x, int y, int width, int height)
{
    TextBox *tb = new TextBox(text, x, y, width, height, getNewId());
    m_textboxs.push_back(tb);
}


Label* Win32Window::addLabel(std::string text, int x, int y, int width, int height)
{
    Label *label = new Label(text, x, y, width, height, getNewId());
    m_labels.push_back(label);
}

void Win32Window::messageBox(std::string title, std::string message, UINT style)
{
    MessageBox(hWnd, (LPSTR)message.c_str(), (LPSTR)title.c_str(),style);
}


void Win32Window::sleep(int ms)
{
    Sleep(ms);
}


void Win32Window::staticThreadFunc(void *parg)
{
    Win32Window *p = (Win32Window*)parg;
    p->threadFunc();
}

void Win32Window::clear()
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    FillRect(hDoubleBufferDC, &rc, (HBRUSH)(COLOR_BACKGROUND));
    redraw();
}


void Win32Window::redraw()
{
    InvalidateRect(hWnd, NULL, TRUE); 
    //PostMessage(hWnd, WM_USER_INVALRECT, 0, 0);
    //RedrawWindow(hWnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
}

void Win32Window::setTimerInterval(int ms)
{
    timer_interval = ms; 
    SetTimer(hWnd , 1, timer_interval, NULL);
}


void Win32Window::DISPLOCK()
{
    EnterCriticalSection(&displock);
}

void Win32Window::DISPUNLOCK()
{
    LeaveCriticalSection(&displock);
}


int Win32Window::getNewId()
{
   return ++idCount;
}



/*************Utility****************/

static void checkScreenProperty(HDC hdc)
{
        HBITMAP memBmp=CreateCompatibleBitmap(hdc,
                        GetSystemMetrics(SM_CXSCREEN),
                        GetSystemMetrics(SM_CYSCREEN));
        BITMAPINFO binfo;
        binfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        binfo.bmiHeader.biBitCount=0;
        GetDIBits(hdc,memBmp,
                  0,binfo.bmiHeader.biHeight,NULL,
                  (BITMAPINFO*)&binfo,
                  DIB_RGB_COLORS);
        printf("biWidth %ld, biHeight %ld, biBitCount %d\n",
              binfo.bmiHeader.biWidth,
              binfo.bmiHeader.biHeight,
              binfo.bmiHeader.biBitCount);
        DeleteObject(memBmp);

}

