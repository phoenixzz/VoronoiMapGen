#include <windows.h>


#include "../SGPLibraryCode/SGPHeader.h"


#include "imgui/imgui.h"
#include "imgui/ImImpl_RenderDrawLists.h"

#include "filedialog/tinyfiledialogs.h"

#include "OuterOpenGLShaderManager.h"

#include "VoronoiMapSetting.h"

#include "model/MapGen.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "image/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image/stb_image_write.h"

#include "TestSample_Camera.h"


#ifdef _DEBUG
#include "vld.h"
#endif

#ifndef SGP_WINDOWS
#error Windows only example
#else

// Hide Console windows
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") 

using namespace sgp;


CTestCamera					g_pTestCamera;
extern SGPDeviceWin32*		g_SGPDeviceWin32;

COuterOpenGLShaderManager * g_OuterOpenGLShaderManager = NULL;
bool						g_ImGuiWindowFocus = false;

bool						g_NeedResetCameraMode = false;
					

Array<SGPVertex_UPOS_VERTEXCOLOR*> g_3DMapVertexList;
Array<uint16*> g_3DMapVertexIdxList;
Array<uint32> g_3DMapVertexNum;

Colour CVoionoiMapSetting::LineColour[eLineColorMax] = 
{
	Colour(51, 51, 90),		//COAST  0x33335a
	Colour(34, 85, 136),	//LAKESHORE: 0x225588
	Colour(34, 85, 136),	//RIVER: 0x225588
};

Colour CVoionoiMapSetting::SlopeColour[2] =
{
	Colour(51, 51, 51),		//LOW  0x333333
	Colour(255, 255, 255),	//HIGH 0xffffff
};

struct GUISetting
{
public:
	int imgwidth;
	int imgheight;
	uint32 randomseed;
	uint32 variantseed;
	int siteNum;
	int islandshape;
	int relaxationNum;
	int gridType;
	int rendermode;
	float lightdir[3];


	void Export(CVoionoiMapSetting * setting)
	{
		setting->uImageWidth = imgwidth;
		setting->uImageHeight = imgheight;
		setting->islandSeed = randomseed;
		setting->detailSeed = variantseed;
		setting->uSitesNum = siteNum;
		setting->IslandShapeType = static_cast<CVoionoiMapSetting::EIslandShape>(islandshape);
		setting->RelaxationNum = relaxationNum;
		setting->RandomGridType = static_cast<CVoionoiMapSetting::ERandomGrid>(gridType);
		setting->RenderMode = static_cast<CVoionoiMapSetting::ERenderMode>(rendermode);
		setting->lightDirection.Set(lightdir[0], lightdir[1], lightdir[2]);
	}
	void Set(CVoionoiMapSetting * setting)
	{
		imgwidth = setting->uImageWidth;
		imgheight = setting->uImageHeight;
		randomseed = setting->islandSeed;
		variantseed = setting->detailSeed;
		siteNum = setting->uSitesNum;
		islandshape = (int)setting->IslandShapeType;
		relaxationNum = setting->RelaxationNum;
		gridType = (int)setting->RandomGridType;
		rendermode = (int)setting->RenderMode;
		lightdir[0] = setting->lightDirection.x;
		lightdir[1] = setting->lightDirection.y;
		lightdir[2] = setting->lightDirection.z;
	}
	bool NeedResetCameraMode(CVoionoiMapSetting * setting)
	{
		if (setting->RenderMode == CVoionoiMapSetting::e3DSlopes && rendermode != (int)CVoionoiMapSetting::e3DSlopes)
			return true;
		if (setting->RenderMode != CVoionoiMapSetting::e3DSlopes && rendermode == (int)CVoionoiMapSetting::e3DSlopes)
			return true;
		return false;
	}

	//bool IsSettingEqual(CVoionoiMapSetting * setting)
	//{
	//	return	(randomseed == setting->islandSeed) &&
	//			(variantseed == setting->detailSeed) &&
	//			(imgwidth == setting->uImageWidth) && (imgheight == setting->uImageHeight) &&
	//			(siteNum == setting->uSitesNum) &&
	//			(islandshape == (int)setting->IslandShapeType) &&
	//			(relaxationNum == setting->RelaxationNum) &&
	//			(gridType == (int)setting->RandomGridType) &&
	//			(rendermode == (int)setting->RenderMode);
	//}
	//bool IsLightDirEqual(CVoionoiMapSetting * setting)
	//{
	//	return  (lightdir[0] == setting->lightDirection.x) &&
	//			(lightdir[1] == setting->lightDirection.y) &&
	//			(lightdir[2] == setting->lightDirection.z);
	//}
};

class MyEventReceiver : public ISGPEventReceiver
{
public:
 
	// This is the one method that we have to implement
	virtual bool OnEvent(const SSGPEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Remember whether each key is down
		if (event.EventType == SGPET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		// Key Up
		if (event.EventType == SGPET_KEY_INPUT_EVENT &&
			event.KeyInput.PressedDown == false)
		{
			
		}

		// Mouse Event
		if (event.EventType == SGPET_MOUSE_INPUT_EVENT)
		{
			// ZOOM
			if( event.MouseInput.Event == SGPMIE_MOUSE_WHEEL )
			{
				if (!ImGui::IsMouseHoveringAnyWindow())
				{
					ZoomingInt = event.MouseInput.Wheel;
				}
			}

			if( event.MouseInput.Event == SGPMIE_LMOUSE_PRESSED_DOWN )
			{
				bLButtonUp = false;
				bLButtonDown = true;
			}
			if( event.MouseInput.Event == SGPMIE_LMOUSE_LEFT_UP )
			{
				bLButtonUp = true;
				bLButtonDown = false;
			}

			if( event.MouseInput.Event == SGPMIE_RMOUSE_PRESSED_DOWN )
			{
				bRButtonDown = true;
			}
			if( event.MouseInput.Event == SGPMIE_RMOUSE_LEFT_UP )
			{
				bRButtonDown = false;
			}

			if( event.MouseInput.Event == SGPMIE_MMOUSE_PRESSED_DOWN )
			{
				bMButtonDown = true;
			}
			if( event.MouseInput.Event == SGPMIE_MMOUSE_LEFT_UP )
			{
				bMButtonDown = false;
			}

			LastXPos = event.MouseInput.X;
			LastYPos = event.MouseInput.Y;
		}

		///////////////////////////////////////////////////////////////////////////
		// ImGuiIO event
		if (event.EventType == SGPET_GUI_EVENT && event.UserEvent.UserData1 == SGPGE_RESIZE)
		{
			RECT rect = {0};
			::GetClientRect(g_SGPDeviceWin32->getWindowHandle(), &rect);

			io.DisplaySize.x = (float)(rect.right - rect.left);
			io.DisplaySize.y = (float)(rect.bottom - rect.top);

		}

		if (event.EventType == SGPET_KEY_INPUT_EVENT && event.KeyInput.Char != 0)
		{
			io.AddInputCharacter(event.KeyInput.Char);
		}

		if (event.EventType == SGPET_MOUSE_INPUT_EVENT)
		{
			if (event.MouseInput.Event == SGPMIE_MOUSE_MOVED)
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			if (event.MouseInput.Event == SGPMIE_MOUSE_WHEEL)
			{
			    io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				io.MouseWheel = event.MouseInput.Wheel > 0  ? 1.0f : -1.0f;   // it's 120 or -120
			}
			if(event.MouseInput.Event == SGPMIE_LMOUSE_DOUBLE_CLICK)
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if(event.MouseInput.Event == SGPMIE_MMOUSE_DOUBLE_CLICK)
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);

				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if(event.MouseInput.Event == SGPMIE_RMOUSE_DOUBLE_CLICK)
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);

				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if( event.MouseInput.Event == SGPMIE_LMOUSE_PRESSED_DOWN )
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				//io.MouseDown[0] = true;
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if( event.MouseInput.Event == SGPMIE_LMOUSE_LEFT_UP )
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				//io.MouseDown[0] = false;
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if( event.MouseInput.Event == SGPMIE_MMOUSE_PRESSED_DOWN || event.MouseInput.Event == SGPMIE_RMOUSE_PRESSED_DOWN)
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				//io.MouseDown[1] = true;
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			if( event.MouseInput.Event == SGPMIE_MMOUSE_LEFT_UP || event.MouseInput.Event == SGPMIE_RMOUSE_LEFT_UP)
			{
				io.KeyCtrl = IsKeyDown(KEY_CONTROL);
				io.KeyShift = IsKeyDown(KEY_SHIFT);
				//io.MouseDown[2] = false;
				io.MousePos = ImVec2((float)event.MouseInput.X, (float)event.MouseInput.Y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
			}
			io.MouseDown[0] = bLButtonDown;
			io.MouseDown[1] = bRButtonDown;
			io.MouseDown[2] = bMButtonDown;
		}

		return false;
	}


	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(SGP_KEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver() 
		: LastXPos(0), LastYPos(0), 
		bMButtonDown(false), bRButtonDown(false), bLButtonDown(false),
		bLButtonUp(false), ZoomingInt(0)
	{
		for (uint32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	uint32 getXPosition() { return LastXPos; }
	uint32 getYPosition() { return LastYPos; }

	bool bMButtonDown;
	bool bRButtonDown;
	bool bLButtonDown;


	bool bLButtonUp;
	int8 ZoomingInt;
private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];

	uint32 LastXPos;
	uint32 LastYPos;

};

void CreateMap3DVertex( Array<Array<SGPVertex_UPOS_VERTEXCOLOR>> &MapVertex,
	Array<Array<uint16>> &MapVertexIdx)
{
	for (int i=0; i<g_3DMapVertexList.size(); i++)
		delete [] g_3DMapVertexList[i];
	g_3DMapVertexList.clear();
	
	for (int j=0; j<g_3DMapVertexIdxList.size(); j++)
		delete [] g_3DMapVertexIdxList[j];
	g_3DMapVertexIdxList.clear();
	g_3DMapVertexNum.clear();

	for(int i=0; i<MapVertex.size(); i++)
	{
		g_3DMapVertexList.add(new SGPVertex_UPOS_VERTEXCOLOR [MapVertex[i].size()]);
		memcpy(g_3DMapVertexList[i], MapVertex[i].getRawDataPointer(), sizeof(SGPVertex_UPOS_VERTEXCOLOR)*MapVertex[i].size());
		g_3DMapVertexNum.add(MapVertex[i].size());
	}
	for(int j=0; j<MapVertexIdx.size(); j++)
	{
		g_3DMapVertexIdxList.add(new uint16 [MapVertexIdx[j].size()]);
		memcpy(g_3DMapVertexIdxList[j], MapVertexIdx[j].getRawDataPointer(), sizeof(uint16)*MapVertexIdx[j].size());
	}
}

void RenderMap3D(ISGPRenderDevice* renderdevice)
{
	if (g_3DMapVertexList.size() == 0 || g_3DMapVertexIdxList.size() == 0)
		return;
	if (g_3DMapVertexList.size() != g_3DMapVertexIdxList.size())
		return;

	SGPSkin vertexcolorskin;

	for(int i=0; i<g_3DMapVertexList.size(); i++)
	{
		renderdevice->GetVertexCacheManager()->FillTriangles(
			SGPVT_UPOS_VERTEXCOLOR,
			g_3DMapVertexNum[i],
			g_3DMapVertexNum[i],
			g_3DMapVertexList[i],
			g_3DMapVertexIdxList[i],
			vertexcolorskin);
	}
}

void RenderQuad(ISGPRenderDevice *pRenderDevice, COuterOpenGLShaderManager *pShaderManager, uint32 PhotoTexture, SRect showRect)
{
	static Matrix4x4 ScaleOffsetMat;

	CTextureResource* pTex = pRenderDevice->GetTextureManager()->getTextureByID(PhotoTexture);
	if (pTex == NULL)
	{
		Logger::getCurrentLogger()->writeToLog(String(L"Could not Load OpenGL Image!"), ELL_ERROR);
		return;
	}

	const SViewPort& vp = pRenderDevice->getViewPort();

	ScaleOffsetMat.Identity();
	ScaleOffsetMat._11 = (float)(showRect.Right - showRect.Left) / (float)vp.Width;
	ScaleOffsetMat._22 = (float)(showRect.Bottom - showRect.Top) / (float)vp.Height;
	ScaleOffsetMat.Translate(
		(float)showRect.Left * 2.0f / (float)vp.Width - 1 + ScaleOffsetMat._11,
		-((float)showRect.Top * 2.0f / (float)vp.Height - 1 + ScaleOffsetMat._22),
		0);


	COpenGLRenderDevice *pGLDevice = static_cast<COpenGLRenderDevice*>(pRenderDevice);


	ISGPMaterialSystem::MaterialList &Mat_List = pRenderDevice->GetMaterialSystem()->GetMaterialList();
	const ISGPMaterialSystem::SGPMaterialInfo &QuadMaterial_info = Mat_List.getReference(ISGPMaterialSystem::eMaterial_fullscreenquad);
	pGLDevice->getOpenGLMaterialRenderer()->PushMaterial( QuadMaterial_info.m_material, MM_Add );
	pGLDevice->getOpenGLMaterialRenderer()->ComputeMaterialPass();
	pGLDevice->getOpenGLMaterialRenderer()->OnePassPreRenderMaterial(0);

	pShaderManager->GetGLSLShaderProgram(Filters_Normal)->useProgram();
	pShaderManager->GetGLSLShaderProgram(Filters_Normal)->setShaderUniform("modelMatrix", ScaleOffsetMat);

	pTex->pSGPTexture->BindTexture2D(0);
	pShaderManager->GetGLSLShaderProgram(Filters_Normal)->setShaderUniform("gSampler0", 0);

	pRenderDevice->GetVertexCacheManager()->RenderFullScreenQuadWithoutMaterial();

	pGLDevice->getOpenGLMaterialRenderer()->OnePassPostRenderMaterial(0);
	pGLDevice->getOpenGLMaterialRenderer()->PopMaterial();
}

void InitTextureAndPBO(ISGPRenderDevice* renderdevice, CVoionoiMapSetting* pVoionoiSetting, uint32 *pTexID, COpenGLPixelBufferObject **ppPBO, uint32 Idx)
{
	if (pTexID[Idx] != 0)
		renderdevice->GetTextureManager()->unRegisterTextureByID(pTexID[Idx]);

	pTexID[Idx] = renderdevice->GetTextureManager()->registerEmptyTexture(
														SDimension2D(pVoionoiSetting->uImageWidth, pVoionoiSetting->uImageHeight),
														String("Map Texture ") + String(Idx),
														SGPPF_A8R8G8B8);
	renderdevice->GetTextureManager()->getTextureByID(pTexID[Idx])->pSGPTexture->unBindTexture2D(0);

	if (ppPBO[Idx] != NULL)
	{
		ppPBO[Idx]->deletePBO();
		delete ppPBO[Idx];
	}
	ppPBO[Idx] = new COpenGLPixelBufferObject(static_cast<COpenGLRenderDevice*>(renderdevice));
	ppPBO[Idx]->createPBO();
	ppPBO[Idx]->bindPBO(GL_PIXEL_UNPACK_BUFFER);
	ppPBO[Idx]->initPBOBuffer(pVoionoiSetting->uImageWidth * pVoionoiSetting->uImageHeight * sizeof(uint32), GL_STREAM_DRAW);
	ppPBO[Idx]->unbindPBO();
	

	// bind the render texture and PBO
	renderdevice->GetTextureManager()->getTextureByID(pTexID[Idx])->pSGPTexture->BindTexture2D(0);

	// bind PBO to update pixel values
	ppPBO[Idx]->bindPBO(GL_PIXEL_UNPACK_BUFFER);
	// map the buffer object into client's memory
	// Note that glMapBuffer() causes sync issue.
	// If GPU is working with this buffer, glMapBuffer() will wait(stall)
	// for GPU to finish its job. To avoid waiting (stall), you can call
	// first glBufferData() with NULL pointer before glMapBuffer().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBuffer() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	ppPBO[Idx]->initPBOBuffer(pVoionoiSetting->uImageWidth * pVoionoiSetting->uImageHeight * sizeof(uint32), GL_STREAM_DRAW);
	GLubyte* pDst = (GLubyte*)ppPBO[Idx]->mapBufferToMemory(GL_WRITE_ONLY);
	unsigned char* pSrc = pVoionoiSetting->image;
	if(pDst)
	{		
		for(uint32 j = 0; j < pVoionoiSetting->uImageHeight; j++)
		{
			for(uint32 i = 0; i < pVoionoiSetting->uImageWidth; i++)
			{
				// ARGB color channel
				*((uint32*)pDst) = (pSrc != NULL) ? Colour(pSrc[0], pSrc[1], pSrc[2]).getARGB() : 0xFFFFFFFF;
				pDst += 4;
				if (pSrc != NULL) pSrc += 3;
			}
		}
		ppPBO[Idx]->unmapBuffer();

	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pVoionoiSetting->uImageWidth, pVoionoiSetting->uImageHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	ppPBO[Idx]->unbindPBO();	
}


/*
   Now ask for the driver and create the Windows specific window.
*/
int main()
{
	// create log
	ConsoleLogger logger( String("VoronoiMapGen") );
	logger.setLogLevel( ELL_DEBUG );
	Logger::setCurrentLogger( &logger );

	// create device
	MyEventReceiver receiver;

	SGPDevice* device = createDevice(
		SGPDT_OPENGL,
		1024, 768, 32, 
		false, false, false,
		true,
		&logger,
		&receiver);

	// could not create selected driver.
	if (device == NULL)
	{
		Logger::setCurrentLogger(nullptr);
		return 1;
	}

	device->setResizable(true);

	sgp::SGPDeviceWin32* pDeviceWin32 = (sgp::SGPDeviceWin32*)device;
	pDeviceWin32->getWindowHandle();
	ISGPRenderDevice* renderdevice = device->getRenderDevice();
	Random* globalrandom = device->getRandomizer();

	// could not get render driver or globalrandom.
	if (!renderdevice || !globalrandom)
	{
		Logger::setCurrentLogger(nullptr);
		return 1;
	}

	// Setting Working Dir
	String WorkingDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getFullPathName();
	WorkingDir = WorkingDir + File::separatorString + String("Data");
	renderdevice->setWorkingDirection(WorkingDir);


	// Camera
	SViewPort viewarea;
	viewarea.Width = renderdevice->getScreenSize().Width;
	viewarea.Height = renderdevice->getScreenSize().Height;
	renderdevice->setViewPort(viewarea);


	// Init Setting
	CVoionoiMapSetting VoionoiSetting;
	VoionoiSetting.uSitesNum = 1000;
	VoionoiSetting.uImageWidth = 600;
	VoionoiSetting.uImageHeight = 600;
	VoionoiSetting.IslandShapeType = CVoionoiMapSetting::ePerlinIsland;
	VoionoiSetting.RandomGridType = CVoionoiMapSetting::eRelaxed;
	VoionoiSetting.RenderMode = CVoionoiMapSetting::eBiome;
	VoionoiSetting.SetGlobalRandom(globalrandom);
	VoionoiSetting.SetIslandRandomSeed(20928);
	VoionoiSetting.SetMapDetailRandomSeed(2);
	VoionoiSetting.RelaxationNum = 3;
	GUISetting guiset;
	guiset.Set(&VoionoiSetting);



	// Camera
	if (VoionoiSetting.RenderMode == CVoionoiMapSetting::e3DSlopes)
	{
		renderdevice->setCameraMode(SGPCT_PERSPECTIVE);
		renderdevice->setNearFarClipPlane(1.0f, 1000.0f);	
		renderdevice->setFov(60.0f);
	
		g_pTestCamera.SetPos(100, 200, -100);
		g_pTestCamera.SetRotation(-float_Pi/5, 0, 0);
	}
	else
	{
		renderdevice->setCameraMode(SGPCT_TWOD);
		renderdevice->setNearFarClipPlane(1.0f, 10.0f);
	}

	g_OuterOpenGLShaderManager = new COuterOpenGLShaderManager(static_cast<COpenGLRenderDevice*>(renderdevice), &logger);

	uint32 DefaultTexture = renderdevice->GetTextureManager()->getTextureIDByName(String("White"));
	uint32 RenderTextureID[2] = {0, 0};
	COpenGLPixelBufferObject *RenderTexturePBO[2] = { NULL, NULL };

	uint32 CurTextureIdx = 0;
	uint32 NextTextureIdx = (CurTextureIdx + 1) % 2;
	if (VoionoiSetting.RenderMode != CVoionoiMapSetting::e3DSlopes)
	{
		InitTextureAndPBO(renderdevice, &VoionoiSetting, RenderTextureID, RenderTexturePBO, NextTextureIdx);
	}


	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	device->setWindowCaption(L"Voronoi MapGeneration - Version alpha");


	ImGui_ImplSGP_Init(pDeviceWin32, device);
	ImGuiIO& io = ImGui::GetIO();
	String customFontPath = WorkingDir + "\\Font\\YaHei_Consolas.ttf";
	io.Fonts->AddFontFromFileTTF(customFontPath.toUTF8().getAddress(), 16.0f, NULL, io.Fonts->GetGlyphRangesChinese());
	
	uint32 LastXPos = receiver.getXPosition();
	uint32 LastYPos = receiver.getYPosition();
	double averageMS = 0;
	double averageFPS = 0;
	while(device->run() && renderdevice)
	{
        // Setup inputs
        for (uint32 i = 0; i < KEY_KEY_CODES_COUNT; i++)
		{
			io.KeysDown[i] = receiver.IsKeyDown((SGP_KEY_CODE)i);
		}
        io.KeyCtrl = receiver.IsKeyDown(KEY_CONTROL);
        io.KeyShift = receiver.IsKeyDown(KEY_SHIFT);

		ImGui_ImplSGP_NewFrame();

		// Work out a frame delta time.
		const double frameDeltaTime = device->getTimer()->GetElapsedTime() / 1000.0; // Time in seconds
		renderdevice->setRenderDeviceTime(device->getTimer()->getTime(), frameDeltaTime);

		if(receiver.bLButtonDown && ImGui::IsMouseHoveringAnyWindow())
		{
			g_ImGuiWindowFocus = true;
		}
		
		if(!receiver.bLButtonDown && g_ImGuiWindowFocus)
		{
			g_ImGuiWindowFocus = false;
		}
		if(!ImGui::IsMouseHoveringAnyWindow() && receiver.IsKeyDown(KEY_ESCAPE))
		{
			device->closeDevice();
		}
		if (VoionoiSetting.RenderMode == CVoionoiMapSetting::e3DSlopes)
		{
			if (g_NeedResetCameraMode)
			{
				renderdevice->setCameraMode(SGPCT_PERSPECTIVE);
				renderdevice->setNearFarClipPlane(1.0f, 1000.0f);	
				renderdevice->setFov(60.0f);
	
				g_pTestCamera.SetPos(100, 200, -100);
				g_pTestCamera.SetRotation(-float_Pi/5, 0, 0);
				g_NeedResetCameraMode = false;
			}
			if( receiver.bMButtonDown )
			{
				g_pTestCamera.SetPanX( (float(receiver.getXPosition()) - float(LastXPos)) * -0.1f );
				g_pTestCamera.SetPanY( (float(receiver.getYPosition()) - float(LastYPos)) * 0.1f );
			}
			else
			{
				g_pTestCamera.SetPanX( 0 );
				g_pTestCamera.SetPanY( 0 );
			}
			if( receiver.bRButtonDown )
			{
				g_pTestCamera.SetRotationSpeedX((float(receiver.getYPosition()) - float(LastYPos)) * -0.2f);
				g_pTestCamera.SetRotationSpeedY((float(receiver.getXPosition()) - float(LastXPos)) * 0.2f);
			}
			else
			{
				g_pTestCamera.SetRotationSpeedX(0);
				g_pTestCamera.SetRotationSpeedY(0);
			}		

			g_pTestCamera.Update( (float)frameDeltaTime );	// DeltaTime in seconds
			g_pTestCamera.SetZoom(0);
		
			// update Camera position
			renderdevice->setViewMatrix3D( 
				g_pTestCamera.GetRight(),
				g_pTestCamera.GetUp(),
				g_pTestCamera.GetDir(),
				g_pTestCamera.GetPos() );
		}
		else
		{
			if (g_NeedResetCameraMode)
			{
				renderdevice->setCameraMode(SGPCT_TWOD);
				renderdevice->setNearFarClipPlane(1.0f, 10.0f);
				g_NeedResetCameraMode = false;
			}
		}

		LastXPos = receiver.getXPosition();
		LastYPos = receiver.getYPosition();

		averageMS += (frameDeltaTime*1000.0 - averageMS) * 0.1;
		averageFPS = 1000.0 / averageMS;

		// Setting correct ViewPort
		viewarea.Width = renderdevice->getScreenSize().Width;
		viewarea.Height = renderdevice->getScreenSize().Height;
		renderdevice->setViewPort(viewarea);



		renderdevice->setClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		renderdevice->beginScene(true, true, false);

		if (VoionoiSetting.RenderMode == CVoionoiMapSetting::e3DSlopes)
		{
			RenderMap3D(renderdevice);
		}
		else
		{
			SRect drawRect;
			drawRect.Left = 100;
			drawRect.Top = 100;
			drawRect.Right = drawRect.Left + VoionoiSetting.uImageWidth;
			drawRect.Bottom = drawRect.Top + VoionoiSetting.uImageHeight;
			if (RenderTextureID[NextTextureIdx] != 0)
				RenderQuad(renderdevice, g_OuterOpenGLShaderManager, RenderTextureID[NextTextureIdx], drawRect);
			else
				RenderQuad(renderdevice, g_OuterOpenGLShaderManager, DefaultTexture, drawRect);
		}

		renderdevice->FlushRenderBatch();



		// UI Group
		{
			// UI Group
			ImGui::SetNextWindowPos(ImVec2(650, 50), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Basic Settings", NULL, ImVec2(350, 650));
			ImGui::Spacing();

			ImVec2 sz(100,30);
            bool bGeneratePressed = ImGui::Button("Generate", sz);
			ImGui::Separator();

			ImGui::Spacing();
			ImGui::Text("Image Size:");
			ImGui::PushItemWidth(120);
			ImGui::DragInt("width", &guiset.imgwidth, 1, 64, 65535); ImGui::SameLine();
			ImGui::DragInt("height", &guiset.imgheight, 1, 64, 65535);
            ImGui::PopItemWidth();

			ImGui::Spacing();
			ImGui::Text("Island Shape:");
			int islandSeed = guiset.randomseed;
			int detailSeed = guiset.variantseed;
			ImGui::PushItemWidth(120);
			bool bRandomPressed = ImGui::Button("Random");
			if (bRandomPressed)
			{
				globalrandom->setSeedRandomly();
				islandSeed = globalrandom->nextInt(100000);
				detailSeed = globalrandom->nextInt(9) + 1;
			}			
			ImGui::SameLine();
			bRandomPressed |= ImGui::InputInt("Shape #", &islandSeed, 0, 0);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(50);
			ImGui::SameLine();
			bRandomPressed |= ImGui::InputInt("##detail", &detailSeed, 0, 0);
			ImGui::PopItemWidth();
			if (bRandomPressed)
			{
				guiset.randomseed = islandSeed;
				guiset.variantseed = detailSeed;
			}
			ImGui::RadioButton("Radial", &guiset.islandshape, 0); ImGui::SameLine();
			ImGui::RadioButton("Perlin", &guiset.islandshape, 1); ImGui::SameLine();
			ImGui::RadioButton("Square", &guiset.islandshape, 2); ImGui::SameLine();
			ImGui::RadioButton("Blob", &guiset.islandshape, 3);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Point Selection:");
			ImGui::RadioButton("Relaxed", &guiset.gridType, 0); ImGui::SameLine();
			ImGui::RadioButton("PoissonDisk", &guiset.gridType, 1); ImGui::SameLine();
			ImGui::RadioButton("Squared", &guiset.gridType, 2); ImGui::SameLine();
			ImGui::RadioButton("Hexagon", &guiset.gridType, 3);
			ImGui::SliderInt("RelaxedNum", &guiset.relaxationNum, 0, 10);
			ImGui::DragInt("SiteNum", &guiset.siteNum, 50, 10, 10000);


			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("View:");
			const char* modeitems[] = { "Polygons", "Biomes", "Elevation", "Moisture", "SmoothPolygon", "2D slopes", "3D slopes" };
			ImGui::Combo("##rendermode", &guiset.rendermode, modeitems, 7);

			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Export:"); ImGui::SameLine();
			bool PNGPressed = ImGui::Button("PNG");
			ImGui::SameLine();
			bool XMLPressed = ImGui::Button("XML");

			static bool CannotSavePNG = false;
			if (PNGPressed)
				CannotSavePNG = (guiset.rendermode == (int)CVoionoiMapSetting::e3DSlopes);
			if (CannotSavePNG)
			{
				PNGPressed = false;
				ImGui::OpenPopup("Saving PNG Fail!");
				if (ImGui::BeginPopupModal("Saving PNG Fail!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Can not save PNG image in 3DSlopes render mode.\nThis operation cannot be finished!\n\n");
					ImGui::Separator();

					if (ImGui::Button("OK", ImVec2(120,0))) 
					{ 
						ImGui::CloseCurrentPopup(); 
						CannotSavePNG = false;
					}
					ImGui::EndPopup();
				}				
			}

			const char* output_filename = NULL;
			if (XMLPressed)
			{
				const char* pFilterPatterns[] = {"*.xml"};
				output_filename = tinyfd_saveFileDialog(
					"Please Choose a position to save...",
					String(WorkingDir + File::separatorString + String("Output/map.xml")).toUTF8().getAddress(),
					1,
					pFilterPatterns,
					"XML files"
					);
			}
			else if (PNGPressed)
			{
				const char* pFilterPatterns[] = {"*.png"};
				output_filename = tinyfd_saveFileDialog(
					"Please Choose a position to save...",
					String(WorkingDir + File::separatorString + String("Output/map.png")).toUTF8().getAddress(),
					1,
					pFilterPatterns,
					"PNG files"
					);

				if (output_filename)
				{
					int out_w = 1024;
					int out_h = 1024;
					unsigned char* output_pixels = (unsigned char*)malloc(out_w * out_h * 3 * sizeof(char));
					int success = stbir_resize_uint8_generic(
						VoionoiSetting.image,
						VoionoiSetting.uImageWidth, 
						VoionoiSetting.uImageHeight,
						0, 
						output_pixels,
						out_w,
						out_h, 
						0,
						3,
						-1,
						0,
						STBIR_EDGE_CLAMP,
						STBIR_FILTER_MITCHELL,
						STBIR_COLORSPACE_LINEAR,
						0);

					success = stbi_write_png(output_filename, out_w, out_h, 3, output_pixels, out_h * 3 * sizeof(char));
				

					free (output_pixels);
					output_pixels = NULL;
				}				
			}

			ImGui::End();

			if (bGeneratePressed || (XMLPressed && output_filename))
			{
				// Check GUISetting data
				g_NeedResetCameraMode = guiset.NeedResetCameraMode(&VoionoiSetting);


				guiset.Export(&VoionoiSetting);

				MapGen m;
				m.ResetMap();
				m.LoadMap(&VoionoiSetting);
				if (VoionoiSetting.RenderMode == CVoionoiMapSetting::e3DSlopes)
				{
					Array<Array<SGPVertex_UPOS_VERTEXCOLOR>> MapVertex;
					Array<Array<uint16>> MapVertexIdx;
					m.Fill3DVertex(MapVertex, MapVertexIdx);
					CreateMap3DVertex(MapVertex, MapVertexIdx);
				}
				else
				{
					CurTextureIdx = (CurTextureIdx + 1) % 2;
					NextTextureIdx = (CurTextureIdx + 1) % 2;

					InitTextureAndPBO(renderdevice, &VoionoiSetting, RenderTextureID, RenderTexturePBO, NextTextureIdx);
				}

				if (XMLPressed)
					m.ExportXML(output_filename);
			}

			if (VoionoiSetting.RenderMode == CVoionoiMapSetting::e3DSlopes)
			{
				ImGui::SetNextWindowPos(ImVec2(10,10));
				if (!ImGui::Begin("Controller: Using Mouse", NULL, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
				{
					ImGui::End();					
				}
				else
				{
					ImGui::Text("press and hold the MIDDLE mouse button to pan\npress and hold the RIGHT mouse button to rotate");
					ImGui::Separator();
					ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
					ImGui::End();
				}
			}
		}

		ImGui::Render();

		renderdevice->endScene();
	}

	

	// Cleanup
    ImGui_ImplSGP_Shutdown();

	if (g_OuterOpenGLShaderManager != NULL)
	{
		delete g_OuterOpenGLShaderManager;
		g_OuterOpenGLShaderManager = NULL;
	}

	if (RenderTexturePBO[0])
	{
		RenderTexturePBO[0]->deletePBO();
		delete RenderTexturePBO[0];
		RenderTexturePBO[0] = NULL;
	}
	if (RenderTexturePBO[1])
	{
		RenderTexturePBO[1]->deletePBO();
		delete RenderTexturePBO[1];
		RenderTexturePBO[1] = NULL;
	}

	if( RenderTextureID[0] != 0 )
		renderdevice->GetTextureManager()->unRegisterTextureByID(RenderTextureID[0]);
	if( RenderTextureID[1] != 0 )
		renderdevice->GetTextureManager()->unRegisterTextureByID(RenderTextureID[1]);


	for (int i=0; i<g_3DMapVertexList.size(); i++)
		delete [] g_3DMapVertexList[i];
	g_3DMapVertexList.clear();
	
	for (int j=0; j<g_3DMapVertexIdxList.size(); j++)
		delete [] g_3DMapVertexIdxList[j];
	g_3DMapVertexIdxList.clear();


	delete device;
	device = NULL;

	Logger::setCurrentLogger(nullptr);

	return 0;
}

#endif