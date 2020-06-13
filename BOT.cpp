using namespace std;
#include <iostream>
#include <vector> 
#include<ctime>
#include <chrono>

// Rift Device Access
#include "OculusRiftDevice.class"

// Scene and Model Building classes
#include "Scene.class"
#include "Model.class"
#include "Vertex.class"

bool HandleMessages(void);

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	OculusRiftDevice *myRift = new OculusRiftDevice(hinst);
	Scene			 *myScene = myRift->Initialize();

	if (myScene != nullptr)
	{
		// "The Loading Scene" - REMOVE THIS default loading scene before adding any new models
        // Add it back in to see what the original template contained....
		myScene->Release();

        Model *m = new Model(Vector3f(4, 0, -45), nullptr );  // Runway lights, right side NO material texture used (nullptr)
		for (int nx = 0; nx < 150; nx++ )
		{
			m->AddSolidColorBox(0.0f, 0.0f, float(nx), 0.1f, 0.1f, float(nx)+0.1f, 0xff000044 );
		}

		m->AllocateBuffers();
		myScene->Add(m);
        m = new Model(Vector3f(-4, 0, -45), nullptr);   // Runway lights, right side NO material texture used (nullptr)
        for (int nx = 0; nx < 150; nx++)
        {
            m->AddSolidColorBox(0.0f, 0.0f, float(nx), 0.1f, 0.1f, float(nx) + 0.1f, 0xff000044); // Runway lights, left side
        }

        m->AllocateBuffers();
        myScene->Add(m);

		m = new Model(Vector3f(0, 0, 13), myScene->grid_material[Scene::MaterialTextures::RUNWAY]);

        m->AddVertex( Vertex(  4, 0, -61,  0,  0, 0xff151515)); // runway grey
        m->AddVertex( Vertex( -4, 0, -61,  1,  0, 0xff151515) );
        m->AddVertex( Vertex(  4, 0, 91.1, 0,  25, 0xff151515));
        m->AddVertex( Vertex( -4, 0, 91.1, 1,  25, 0xff151515));
        
        m->AddIndexes(vector<GLushort> { 0, 1, 2, 1, 2, 3,		// FRONT SIDE
										 3, 2, 1, 2, 1, 0  } ); // BACK SIDE 
			
		m->AllocateBuffers();
		myScene->Add(m);

        int curIndex = 0xff;

        auto start = std::chrono::system_clock::now();
        time_t t; struct tm * tt; 

        while (HandleMessages())
		{
            if (!myRift->Process(myScene))
				break;

            for( int x=0;x< myScene->Models[0]->Vertices.size(); x++ )
            {
                int curCube = int( float(x) / 24.0f );
                DWORD curColor = DWORD(0xff000000) | ( ( DWORD( curCube + curIndex ) & 0x000000ff ) << 16 );
                
                myScene->Models[0]->Vertices[x].C = curColor;
                myScene->Models[1]->Vertices[x].C = curColor;
            }

            myScene->Models[0]->FreeBuffers();
            myScene->Models[0]->AllocateBuffers();
            myScene->Models[1]->FreeBuffers();
            myScene->Models[1]->AllocateBuffers();

			Sleep(10);
            
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            /*if( elapsed_seconds.count() >= double(0.2))
                {
                start = end; */
                curIndex -= 1;
                if (curIndex < 0 )
                    curIndex = 0xff ;

          //      }
        }
	}

	delete myScene;
	delete myRift;
	return(0);
}

bool HandleMessages(void)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// GLOBAL keystroke handlers
		if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return false;
		}

	}
	return true;
}

