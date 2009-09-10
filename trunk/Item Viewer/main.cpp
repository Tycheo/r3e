#include "..\R3E\OpenGLWindow.hpp"
#include "..\R3E\TargetCamera.hpp"
#include "..\R3E\OpenGL.hpp"
#include "..\R3E\FileSystem.hpp"
#include "..\R3E\FlatFileSystem.hpp"
#include "..\R3E\Mesh1TexEntity.hpp"
#include "..\R3E\SceneManager.hpp"

#include <gl/glu.h>

class AxisLinesEntity : public Entity {
public:
	AxisLinesEntity(){
		mType = ENTITY_COLOUR;
		
		mBoundingBox.mMin = Vector3(0.0f, 0.0f, 0.0f);
		mBoundingBox.mMax = Vector3(0.5f, 0.5f, 0.5f);
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
		mCamera = new TargetCamera();
		mScene.SetCamera(mCamera);

		mScene.AddEntity((new Mesh1TexEntity())->SetMesh("OSW1.ZMS")->SetMaterial(Material("OSW1.DDS")));
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

		return TRUE;
	}

	virtual void DrawGL(){
		mScene.BeginScene();
		mScene.RenderScene();
		mScene.EndScene();
	}

	virtual void KeyDown(WPARAM wParam, LPARAM lParam){}
	virtual void KeyUp(WPARAM wParam, LPARAM lParam){}

	virtual void ResizeGL(int width, int height){
		mScene.ResizeScene(width, height);

		mWidth = width;
		mHeight = height;
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	FileSystem::SetFileSystem((FileSystem*)(new FlatFileSystem()));

	ROSEWindow wnd;
	wnd.SetTitle("ROSE Rendering Tutorial");
	wnd.SetSize(640, 480);

	if(!wnd.Create()) return 0;

	return wnd.Run();
}
