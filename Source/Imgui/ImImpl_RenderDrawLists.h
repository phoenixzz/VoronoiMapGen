#ifndef IMIMPL_RENDERDRAWLISTS_H
#define IMIMPL_RENDERDRAWLISTS_H


// Function declaration
IMGUI_API bool        ImGui_ImplSGP_Init(SGPDeviceWin32* pWin32Device, SGPDevice* device);
IMGUI_API void        ImGui_ImplSGP_Shutdown();
IMGUI_API void        ImGui_ImplSGP_NewFrame();

IMGUI_API bool        ImGui_ImplSGP_CreateDeviceObjects();




// Data
static SGPDevice*	g_Device = NULL;
static SGPDeviceWin32* g_Win32Device = NULL;
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;



// Notify OS Input Method Editor of text input position (e.g. when using Japanese/Chinese inputs, otherwise this isn't needed)
void ImImpl_ImeSetInputScreenPosFn(int x, int y)
{
	if (!g_Win32Device)
		return;

	HWND hwnd = g_Win32Device->getWindowHandle();
	if (HIMC himc = ImmGetContext(hwnd))
	{
		COMPOSITIONFORM cf;
		cf.ptCurrentPos.x = x;
		cf.ptCurrentPos.y = y;
		cf.dwStyle = CFS_FORCE_POSITION;
		ImmSetCompositionWindow(himc, &cf);
	}
}




void ImGui_ImplSGP_Shutdown()
{
    if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
    if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
    if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
    g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

    glDetachShader(g_ShaderHandle, g_VertHandle);
    glDeleteShader(g_VertHandle);
    g_VertHandle = 0;

    glDetachShader(g_ShaderHandle, g_FragHandle);
    glDeleteShader(g_FragHandle);
    g_FragHandle = 0;

    glDeleteProgram(g_ShaderHandle);
    g_ShaderHandle = 0;

    if (g_FontTexture)
    {
        glDeleteTextures(1, &g_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
    ImGui::Shutdown();
}

void ImGui_ImplSGP_NewFrame()
{
	if (!g_Win32Device)
		return;

    if (!g_FontTexture)
        ImGui_ImplSGP_CreateDeviceObjects();

    int fb_w, fb_h;

    RECT rect = {0};
	::GetClientRect(g_Win32Device->getWindowHandle(), &rect);
    fb_w = rect.right - rect.left;
    fb_h = rect.bottom - rect.top;

	double frameDeltaTime = g_Win32Device->getTimer()->GetElapsedTime() / 1000.0;

	// Setup display size (every frame to accommodate for window resizing)
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)fb_w, (float)fb_h);	// Display size, in pixels. For clamping windows positions.
    io.DeltaTime = (float)frameDeltaTime;                      // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	io.DisplayFramebufferScale = ImVec2(1, 1);


    // Start the frame
    ImGui::NewFrame();
}


void ImGui_ImplSGP_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	// Create OpenGL texture
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}

bool ImGui_ImplSGP_CreateDeviceObjects()
{
    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    ImGui_ImplSGP_CreateFontsTexture();

    return true;
}




// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplSGP_RenderDrawLists(ImDrawData* draw_data)
{
	//((COpenGLRenderDevice*)g_Device->getRenderDevice())->testGLError();
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);

    // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
    ImGuiIO& io = ImGui::GetIO();
    float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Setup orthographic projection matrix
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(g_VaoHandle);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

	glUseProgram(0);


	glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}


bool ImGui_ImplSGP_Init(SGPDeviceWin32* pWin32Device, SGPDevice* device)
{
    g_Device = device;
	g_Win32Device = pWin32Device;

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    io.RenderDrawListsFn = ImGui_ImplSGP_RenderDrawLists;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.

#ifdef _WIN32
	io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;    
#endif

    return true;
}






#endif //IMIMPL_RENDERDRAWLISTS_H