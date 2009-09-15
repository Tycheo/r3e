#include "..\R3E\OpenGLWindow.hpp"
#include "..\R3E\TargetCamera.hpp"
#include "..\R3E\OpenGL.hpp"
#include "..\R3E\BufferedFileSystem.hpp"
#include "..\R3E\SceneManager.hpp"
#include "..\R3E\ROSEData.hpp"
#include "..\R3E\Player.hpp"
#include "..\R3E\PTL.hpp"
#include "..\R3E\NpcManager.hpp"

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

	ROSEWindow(){
		mTotalFrames = 0;

		mCamera = new TargetCamera();
		mScene.SetCamera(mCamera);

		//mScene.AddEntity(new AxisLinesEntity());
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
		ROSE::Data::Load();


		float cRadius = 2.0f;
		float cRadiusIncr = 2.0f;
		unsigned int maxNpc = 20;
		float angIncr = M_PI / float(maxNpc / 2);
		float cAng = 0.0f;
		unsigned int cCount = 1;
		
		for(unsigned int j = 0; j < cCount; ++j){
			for(unsigned int i = 0; i < maxNpc; ++i){
				Player* mPlayer = new Player();

				mPlayer->SetGender(0);
				mPlayer->SetAnimation("3DDATA\\MOTION\\AVATAR\\DANCE_01_M1.ZMO");

				mPlayer->SetItem(ROSE::IT_BODY, 156);
				mPlayer->SetItem(ROSE::IT_FOOT, 1);
				mPlayer->SetItem(ROSE::IT_ARM, 1);
				mPlayer->SetItem(ROSE::IT_CAP, 870);
				mPlayer->SetItem(ROSE::IT_FACE, 2);
				mPlayer->SetItem(ROSE::IT_MASK, 161);
				mPlayer->Transform(Matrix4::CreateScaling(Vector3(0.5f, 0.5f, 0.5f)));
				mPlayer->Transform(Matrix4::CreateRotationZ(-M_PI_2));
				mPlayer->Transform(Matrix4::CreateTranslation(Vector3(cRadius, 0.0f, 0.0f)));
				mPlayer->Transform(Matrix4::CreateRotationZ(cAng));

				mScene.AddEntity(mPlayer);
				cAng += angIncr;
			}
			cRadius += cRadiusIncr;
		}
		
		Player* mPlayer = new Player();

		mPlayer->SetGender(0);
		mPlayer->SetAnimation("3DDATA\\MOTION\\AVATAR\\DANCE_01_M1.ZMO");

		mPlayer->SetItem(ROSE::IT_BODY, 156);
		mPlayer->SetItem(ROSE::IT_FOOT, 1);
		mPlayer->SetItem(ROSE::IT_ARM, 1);
		mPlayer->SetItem(ROSE::IT_CAP, 870);
		mPlayer->SetItem(ROSE::IT_FACE, 2);
		mPlayer->SetItem(ROSE::IT_MASK, 161);
		mPlayer->Transform(Matrix4::CreateScaling(Vector3(2.0f, 2.0f, 2.0f)));

		mScene.AddEntity(mPlayer);

		return TRUE;
	}

	virtual void DrawGL(){
		static clock_t start = clock();
		++mTotalFrames;

		OpenGL::mVertexCount = 0;
		mScene.BeginScene();
		mScene.RenderScene();
		mScene.EndScene();
		
		int secsSinceStart = ((clock() - start) / 1000);
		if(secsSinceStart == 0) return;
		int FPS = mTotalFrames / secsSinceStart;
		char buffer[32];
		sprintf_s(buffer, 32, "FPS: %i, Vertices: %u", FPS, OpenGL::mVertexCount);
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
		FileSystem::SetFileSystem((FileSystem*)(new BufferedFileSystem(lpCmdLine)));
	}else{
		FileSystem::SetFileSystem((FileSystem*)(new BufferedFileSystem("D:\\Games\\RuffVFS Clean\\")));
	}

	ROSEWindow wnd;
	wnd.SetTitle("ROSE 3D Engine");
	wnd.SetSize(1024, 768);

	if(!wnd.Create()) return 0;

	return wnd.Run();
}
