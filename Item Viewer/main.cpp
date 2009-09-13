#include "..\R3E\OpenGLWindow.hpp"
#include "..\R3E\TargetCamera.hpp"
#include "..\R3E\OpenGL.hpp"
#include "..\R3E\FlatFileSystem.hpp"
#include "..\R3E\SceneManager.hpp"
#include "..\R3E\ROSEData.hpp"
#include "..\R3E\Player.hpp"
#include "..\R3E\PTL.hpp"

#include <gl/glu.h>

class AxisLinesEntity : public Entity {
public:
	AxisLinesEntity(){
		mType = ENTITY_COLOUR;
		
		mBoundingBox.mMin = Vector3(0.0f, 0.0f, 0.0f);
		mBoundingBox.mMax = Vector3(0.5f, 0.5f, 0.5f);

		UpdateBoundingBox();
	}

	virtual ~AxisLinesEntity(){}

	virtual void Render(){
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(5.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 5.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 5.0f);
		glEnd();
	}

private:
};

class ROSEWindow : public OpenGLWindow {
public:
	TargetCamera* mCamera;
	SceneManager mScene;
	Player* mPlayer;

	ROSEWindow(){
		mTotalFrames = 0;

		mCamera = new TargetCamera();
		mScene.SetCamera(mCamera);

		mScene.AddEntity(new AxisLinesEntity());
	}

	virtual ~ROSEWindow(){}

	virtual void MouseWheel(WPARAM wParam, LPARAM lParam){
		short delta = (wParam >> 16) & 0xFFFF;
		mCamera->AddDistance(float(delta) / 40.0f);
	}

	virtual void MouseMove(WPARAM wParam, LPARAM lParam){
		static POINTS last;		
		POINTS cur = MAKEPOINTS(lParam);

		if(!(wParam & MK_RBUTTON)){
			last = cur;
			return;
		}

		SHORT dX = last.x - cur.x;
		mCamera->RotateH(float(dX) / 100.0f);

		SHORT dY = last.y - cur.y;
		mCamera->RotateV(float(dY) / 100.0f);

		last = cur;
	}

	virtual BOOL InitGL(){		
		OpenGL::InitGL();
		mScene.Init();

		mPlayer = new Player();

		mPlayer->SetSkeleton("3DDATA\\AVATAR\\MALE.ZMD");
		mPlayer->SetAnimation("3DDATA\\MOTION\\AVATAR\\EMPTY_RUN_M1.ZMO");

		mPlayer->SetItem(ROSE::IT_BODY, 2);
		mPlayer->SetItem(ROSE::IT_FOOT, 2);
		mPlayer->SetItem(ROSE::IT_ARM, 2);
		mPlayer->SetItem(ROSE::IT_HAIR, 1);
		mPlayer->SetItem(ROSE::IT_FACE, 2);
		mPlayer->SetItem(ROSE::IT_WEAPON, 1);
		mPlayer->SetItem(ROSE::IT_SUBWPN, 1);
		mPlayer->SetItem(ROSE::IT_BACK, 223);

		mScene.AddEntity(mPlayer);

		return TRUE;
	}

	virtual void DrawGL(){
		static clock_t start = clock();
		++mTotalFrames;

		mScene.BeginScene();
		mScene.RenderScene();
		mScene.EndScene();
		
		int secsSinceStart = ((clock() - start) / 1000);
		if(secsSinceStart == 0) return;
		int FPS = mTotalFrames / secsSinceStart;
		char buffer[32];
		sprintf_s(buffer, 32, "FPS: %i", FPS);
		SetTitle(buffer);
	}

	virtual void KeyDown(WPARAM wParam, LPARAM lParam){}
	virtual void KeyUp(WPARAM wParam, LPARAM lParam){
		if(wParam == '1'){
			mScene.SetCulling(false);
		}else if(wParam == '2'){
			mScene.SetCulling(true);
		}
	}

	virtual void ResizeGL(int width, int height){
		mScene.ResizeScene(width, height);

		mWidth = width;
		mHeight = height;
	}

	unsigned int mTotalFrames;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	if(strlen(lpCmdLine) > 0){
		FileSystem::SetFileSystem((FileSystem*)(new FlatFileSystem(lpCmdLine)));
	}else{
		FileSystem::SetFileSystem((FileSystem*)(new FlatFileSystem("D:\\Games\\RuffVFS Clean\\")));
	}

	ROSEWindow wnd;
	wnd.SetTitle("ROSE 3D Engine");
	wnd.SetSize(640, 480);

	if(!wnd.Create()) return 0;

	return wnd.Run();
}
