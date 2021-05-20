// FrameBotClient.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "FrameBotClient.h"
#include "NetworkModule.h"

/*_________________________________________________________________________________*/
HDC			hDC = NULL;
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;
HINSTANCE	hInstance;

GLuint		base;				// Base Display List For The Font Set
GLfloat		cnt1;				// 1st Counter Used To Move Text & For Coloring
GLfloat		cnt2;				// 2nd Counter Used To Move Text & For Coloring

bool		keys[256];			// Array Used For The Keyboard Routine
bool		active = TRUE;		// Window Active Flag Set To TRUE By Default
bool		fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
/*_________________________________________________________________________________*/

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

GLvoid BuildFont(GLvoid)								
{
	HFONT	font;										
	HFONT	oldfont;									

	base = glGenLists(96);								

	font = CreateFont(-24,							    
						0,								
						0,								
						0,								
						FW_BOLD,						
						FALSE,							
						FALSE,							
						FALSE,							
						ANSI_CHARSET,					
						OUT_TT_PRECIS,					
						CLIP_DEFAULT_PRECIS,			
						ANTIALIASED_QUALITY,			
						FF_DONTCARE | DEFAULT_PITCH,	
						L"Courier New");				

	oldfont = (HFONT)SelectObject(hDC, font);           
	wglUseFontBitmaps(hDC, 32, 96, base);				
	SelectObject(hDC, oldfont);							
	DeleteObject(font);									
}

GLvoid KillFont(GLvoid)									
{
	glDeleteLists(base, 96);							
}

GLvoid glPrint(const char* fmt, ...)					
{
	char		text[256];								
	va_list		ap;										

	if (fmt == NULL)									
		return;											

	va_start(ap, fmt);									
	vsprintf_s(text, fmt, ap);							
	va_end(ap);											

	glPushAttrib(GL_LIST_BIT);							
	glListBase(base - 32);										
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);	
	glPopAttrib();												
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)								
{
	if (height == 0)															
	{
		height = 1;																
	}

	glViewport(0, 0, width, height);											

	glMatrixMode(GL_PROJECTION);												
	glLoadIdentity();															
																	
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);													
	glLoadIdentity();															

	glMatrixMode(GL_MODELVIEW);													
	glLoadIdentity();															
}

int InitGL(GLvoid)														
{
	glShadeModel(GL_SMOOTH);											
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);								
	glClearDepth(1.0f);													
	glEnable(GL_DEPTH_TEST);											
	glDepthFunc(GL_LEQUAL);												
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					

	// Build The Font
	BuildFont();														

	return TRUE;														
}

int DrawGLScene(GLvoid)													
{
	int size = 0;
	float* points = nullptr;
	GetPointCloud(&size, &points);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					
	glLoadIdentity();													
	glTranslatef(0.14f, -0.4f, -1.0f);									
																		
	glColor3f(1, 1, 0);

	// Position The Text On The Screen
	glRasterPos2f(0.0f, 0.00f);
	glPrint("STRESS TEST [%d]", (int)active_clients);					
	glRasterPos2f(0.0f, 0.05f);
	glPrint("Delay : %dms", global_delay);

	glColor3f(1, 1, 1);

	glPointSize(2.0);
	glBegin(GL_POINTS);

	for (int i = 0; i < size; i++)
	{
		float x, y, z;

		x = points[i * 2] / 800.f - 1.25f;
		y = 1.25f - points[i * 2 + 1] / 800.f;
		z = -1.0f;
		glVertex3f(x, y, z);
	}
	glEnd();

	return TRUE;									
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(L"OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}

	KillFont();
}

BOOL CreateGLWindow(const wchar_t* title, int width, int height, BYTE bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = L"OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		L"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,					
						 UINT	uMsg,										
						 WPARAM	wParam,										
						 LPARAM	lParam)										
{
	switch (uMsg)										
	{
	case WM_ACTIVATE:									
	{
		if (!HIWORD(wParam))							
		{
			active = TRUE;								
		}
		else
		{
			active = FALSE;								
		}

		return 0;										
	}

	case WM_SYSCOMMAND:									
	{
		switch (wParam)									
		{
		case SC_SCREENSAVE:								
		case SC_MONITORPOWER:							
			return 0;									
		}
		break;											
	}

	case WM_CLOSE:										
	{
		PostQuitMessage(0);								
		return 0;										
	}

	case WM_KEYDOWN:									
	{
		keys[wParam] = TRUE;							
		return 0;										
	}

	case WM_KEYUP:										
	{
		keys[wParam] = FALSE;							
		return 0;										
	}

	case WM_SIZE:										
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  
		return 0;										
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE	hInstance,									
	HINSTANCE	hPrevInstance,												
	LPSTR		lpCmdLine,													
	int			nCmdShow)													
{
	MSG		msg;															
	BOOL	done = FALSE;													

	fullscreen = FALSE;														

	// Create Our OpenGL Window
	if (!CreateGLWindow(L"Stress Test Client", 640, 480, 16, fullscreen))
	{
		return 0;															
	}

	InitializeNetwork();

	while (!done)										
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	
		{
			if (msg.message == WM_QUIT)					
			{
				done = TRUE;							
			}
			else										
			{
				TranslateMessage(&msg);					
				DispatchMessage(&msg);					
			}
		}
		else											
		{
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	
			{
				done = TRUE;							
			}
			else									
			{
				SwapBuffers(hDC);					
			}

			if (keys[VK_F1])						
			{
				keys[VK_F1] = FALSE;					
				KillGLWindow();						
				fullscreen = !fullscreen;				
														
				if (!CreateGLWindow(L"NeHe's Bitmap Font Tutorial", 640, 480, 16, fullscreen))
				{
					return 0;						
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									
	return ((int)msg.wParam);						
}