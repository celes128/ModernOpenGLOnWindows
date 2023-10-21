#include "Graphics.h"

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;

LRESULT CALLBACK LoadOpenGLWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

miny::Result Graphics::Initialize(in_ HWND hwnd, in_ SIZE winSize, in_ HINSTANCE hinstance)
{
	m_hwnd = hwnd;

	auto result = LoadOpenGL(hinstance);
	if (!result) {
		return miny::FAILED;
	}

	result = InitializeOpenGL(hwnd, winSize);
	if (!result) {
		return miny::FAILED;
	}

    return miny::OK;
}

void Graphics::Terminate()
{
	// Release the rendering context.
	if (m_hglrc) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
		m_hglrc = 0;
	}

	// Release the device context.
	if (m_hdc) {
		ReleaseDC(m_hwnd, m_hdc);
		m_hdc = 0;
	}
}

miny::Result Graphics::LoadOpenGL(HINSTANCE hinstance)
{
	// Create a temporary window in order to load OpenGL extensions
	LPCWSTR windowClassName = L"LoadOpenGLWindowClass";

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = LoadOpenGLWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = windowClassName;
	wcex.hIconSm = 0;

	auto atom = RegisterClassExW(&wcex);

	auto tmpHwnd = CreateWindowExW(WS_EX_APPWINDOW, windowClassName, L"Temporary Window", WS_POPUP,
		0, 0, 800, 600, NULL, NULL, hinstance, NULL);
	if (tmpHwnd == NULL) {
		return miny::FAILED;
	}

	// Hide the window
	ShowWindow(tmpHwnd, SW_HIDE);

	// Load the modern OpenGL functions
	auto result = LoadOpenGLFunctions(tmpHwnd);
	if (!result) {
		MessageBoxW(tmpHwnd, L"Could not load the OpenGL functions.", L"Error", MB_OK);
	}

	// Release stuff
	DestroyWindow(tmpHwnd);
	tmpHwnd = NULL;

	return result;
}

miny::Result	Graphics::LoadOpenGLFunctions(HWND hwnd)
{
    int error = 0;
    bool result = true;

    // Get the device context for this window.
    auto hdc = GetDC(hwnd);
    if (!hdc) {
        return miny::FAILED;
    }

    // Set a temporary default pixel format.
	PIXELFORMATDESCRIPTOR pixelFormat{
		sizeof(PIXELFORMATDESCRIPTOR),
		1
	};
    error = SetPixelFormat(hdc, 1, &pixelFormat);
    if (error != TRUE) {
        return miny::FAILED;
    }

    // Create a temporary rendering context.
    auto hrc = wglCreateContext(hdc);
    if (!hrc) {
        return miny::FAILED;
    }

    // Set the temporary rendering context as the current rendering context for this window.
    error = wglMakeCurrent(hdc, hrc);
    if (error != TRUE) {
        return miny::FAILED;
    }

    // Initialize the OpenGL extensions needed for this application.
    result = LoadExtensionList();
    if (!result) {
        return miny::FAILED;
    }

    // Release the temporary rendering context now that the extensions have been loaded.
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    hrc = NULL;

    ReleaseDC(hwnd, hdc);
    hdc = 0;

    return miny::OK;
}

miny::Result	Graphics::LoadExtensionList()
{
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB)
	{
		return false;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB)
	{
		return false;
	}

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (!wglSwapIntervalEXT)
	{
		return false;
	}

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if (!glAttachShader)
	{
		return false;
	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if (!glBindBuffer)
	{
		return false;
	}

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if (!glBindVertexArray)
	{
		return false;
	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if (!glBufferData)
	{
		return false;
	}

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if (!glCompileShader)
	{
		return false;
	}

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if (!glCreateProgram)
	{
		return false;
	}

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if (!glCreateShader)
	{
		return false;
	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if (!glDeleteBuffers)
	{
		return false;
	}

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if (!glDeleteProgram)
	{
		return false;
	}

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if (!glDeleteShader)
	{
		return false;
	}

	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if (!glDeleteVertexArrays)
	{
		return false;
	}

	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if (!glDetachShader)
	{
		return false;
	}

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if (!glEnableVertexAttribArray)
	{
		return false;
	}

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if (!glGenBuffers)
	{
		return false;
	}

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if (!glGenVertexArrays)
	{
		return false;
	}

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if (!glGetAttribLocation)
	{
		return false;
	}

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if (!glGetProgramInfoLog)
	{
		return false;
	}

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if (!glGetProgramiv)
	{
		return false;
	}

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if (!glGetShaderInfoLog)
	{
		return false;
	}

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if (!glGetShaderiv)
	{
		return false;
	}

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if (!glLinkProgram)
	{
		return false;
	}

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if (!glShaderSource)
	{
		return false;
	}

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if (!glUseProgram)
	{
		return false;
	}

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if (!glVertexAttribPointer)
	{
		return false;
	}

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if (!glBindAttribLocation)
	{
		return false;
	}

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if (!glGetUniformLocation)
	{
		return false;
	}

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if (!glUniformMatrix4fv)
	{
		return false;
	}

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if (!glActiveTexture)
	{
		return false;
	}

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if (!glUniform1i)
	{
		return false;
	}

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if (!glGenerateMipmap)
	{
		return false;
	}

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if (!glDisableVertexAttribArray)
	{
		return false;
	}

	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if (!glUniform3fv)
	{
		return false;
	}

	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if (!glUniform4fv)
	{
		return false;
	}

	return true;
}

miny::Result Graphics::InitializeOpenGL(HWND hwnd, SIZE winSize)
{
	int attributeListInt[19];
	int pixelFormat[1];
	unsigned int formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	int attributeList[5];
	float fieldOfView, screenAspect;
	char* vendorString, * rendererString;

	// Get the device context for this window.
	m_hdc = GetDC(hwnd);
	if (!m_hdc) {
		return miny::FAILED;
	}

	// Support for OpenGL rendering.
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	// Support for rendering to a window.
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	// Support for hardware acceleration.
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	// Support for 24bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

	// Support for 24 bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	// Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	// Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	// Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	// Support for a 8 bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	// Null terminate the attribute list.
	attributeListInt[18] = 0;

	// Query for a pixel format that fits the attributes we want.
	result = wglChoosePixelFormatARB(m_hdc, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1) {
		return miny::FAILED;
	}

	// If the video card/display can handle our desired pixel format then we set it as the current one.
	result = SetPixelFormat(m_hdc, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1) {
		return miny::FAILED;
	}

	// Set the 4.0 version of OpenGL in the attribute list.
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;

	// Null terminate the attribute list.
	attributeList[4] = 0;

	// Create a OpenGL 4.0 rendering context.
	m_hglrc = wglCreateContextAttribsARB(m_hdc, 0, attributeList);
	if (m_hglrc == NULL) {
		return miny::FAILED;
	}

	// Set the rendering context to active.
	result = wglMakeCurrent(m_hdc, m_hglrc);
	if (result != 1) {
		return miny::FAILED;
	}

	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Initialize the world/model matrix to the identity matrix.
	//BuildIdentityMatrix(m_worldMatrix);

	// Set the field of view and screen aspect ratio.
	fieldOfView = 3.14159265358979323846f / 4.0f;
	screenAspect = (float)winSize.cx / (float)winSize.cy;

	// Build the perspective projection matrix.
	//BuildPerspectiveFovLHMatrix(m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// Turn on vsync
	result = wglSwapIntervalEXT(1);
	if (!result) {
		return miny::FAILED;
	}

	return miny::OK;
}

LRESULT CALLBACK LoadOpenGLWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}break;

	default: {
		return DefWindowProc(hwnd, message, wParam, lParam);
	}break;
	}

	return 0;
}

void Graphics::Present()
{
	SwapBuffers(m_hdc);
}