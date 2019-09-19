 

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "imgui.h"
#include "imguiRenderGL.h"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "InputGeom.h"
#include "TestCase.h"
#include "Filelist.h"
#include "SlideShow.h"

#include "Sample_SoloMesh.h"
#include "Sample_TileMesh.h"
#include "Sample_TempObstacles.h"
#include "Sample_Debug.h"



#include "improbable/worker.h"
#include "improbable/standard_library.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include "improbable/pathfinding.h"


#ifdef WIN32
#	define snprintf _snprintf
#	define putenv _putenv
#endif
//#include <glut.h>





FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE* __cdecl __iob_func(void)
{
	return _iob;
}




//======================================================================== SpatialOS===========================
/*
using EmptyRegistry = worker::Components<pathfinding::PathfindingServer>;

// Constants and parameters
const int ErrorExitStatus = 1;
const std::string kLoggerName = "startup.cc";
const std::uint32_t kGetOpListTimeoutInMilliseconds = 100;


worker::Connection ConnectWithLocator(const std::string hostname,
	const std::string project_name,
	const std::string deployment_id,
	const std::string login_token,
	const worker::ConnectionParameters& connection_parameters) {
	worker::LocatorParameters locator_parameters;
	locator_parameters.ProjectName = project_name;
	locator_parameters.CredentialsType = worker::LocatorCredentialsType::kLoginToken;
	locator_parameters.LoginToken.Token = login_token;

	worker::Locator locator{ hostname, locator_parameters };

	auto queue_status_callback = [&](const worker::QueueStatus& queue_status) {
		if (!queue_status.Error.empty()) {
			std::cerr << "Error while queueing: " << *queue_status.Error << std::endl;
			return false;
		}
		std::cout << "Worker of type '" << connection_parameters.WorkerType
			<< "' connecting through locator: queueing." << std::endl;
		return true;
	};

	auto future = locator.ConnectAsync(EmptyRegistry{}, deployment_id, connection_parameters, queue_status_callback);
	return future.Get();
}
//--------------------

worker::Connection ConnectWithReceptionist(const std::string hostname,
	const std::uint16_t port,
	const std::string& worker_id,
	const worker::ConnectionParameters& connection_parameters) {
	auto future = worker::Connection::ConnectAsync(EmptyRegistry{}, hostname, port, worker_id, connection_parameters);
	return future.Get();
}

//-------------------------------


std::string get_random_characters(size_t count) {
	const auto randchar = []() -> char {
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const auto max_index = sizeof(charset) - 1;
		return charset[std::rand() % max_index];
	};
	std::string str(count, 0);
	std::generate_n(str.begin(), count, randchar);
	return str;
}
//---------------------------------



*/
//===============================================================================================================







//====================  vahid
const float hexagon_r = 0.2;
const float hexagon_dx = hexagon_r*cos(30.0*M_PI / 180.0);
const float hexagon_dy = hexagon_r*sin(30.0*M_PI / 180.0);
const float hexagon_gx = 2.0*hexagon_dx;
const float hexagon_gy = 2.0*hexagon_dx*sin(60.0*M_PI / 180.0);

//=================================  
struct SampleItem
{
	Sample* (*create)();
	const char* name;
};

Sample* createSolo() { return new Sample_SoloMesh(); }
Sample* createTile() { return new Sample_TileMesh(); }
Sample* createTempObstacle() { return new Sample_TempObstacles(); }
Sample* createDebug() { return new Sample_Debug(); }

static SampleItem g_samples[] =
{
	{ createSolo, "Solo Mesh" },
	{ createTile, "Tile Mesh" },
	{ createTempObstacle, "Temp Obstacles" },
	//	{ createDebug, "Debug" },
};
static const int g_nsamples = sizeof(g_samples) / sizeof(SampleItem);
//=========================================================================vahid




void draw_hexagon(float x, float y, float z)
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(x - hexagon_dx, y - hexagon_dy, z);
	glVertex3f(x - hexagon_dx, y + hexagon_dy, z);
	glVertex3f(x, y + hexagon_r, z);
	glVertex3f(x + hexagon_dx, y + hexagon_dy, z);
	glVertex3f(x + hexagon_dx, y - hexagon_dy, z);
	glVertex3f(x, y - hexagon_r, z);
	glEnd();
}

void draw_hexagon_grid(float x, float y, float z, int ni, int nj)
{
	int i, j; float x0;
	x -= float(ni - 1)*hexagon_gx*0.5; // just shift x,y to start position (i=0,j=0)
	x -= float(nj - 1)*hexagon_dx*0.5;
	y -= float(nj - 1)*hexagon_gy*0.5;
	for (x0 = x, j = 0; j<nj; j++, x0 += hexagon_dx, x = x0, y += hexagon_gy)
	for (i = 0; i<ni; i++, x += hexagon_gx)
		draw_hexagon(x, y, z);
}


void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
}

void myDisplay(void)
{
	

	float x, y, z;
	int counter, id;

	const int size = 1;
	const float w = 1.5;   // (2 * size) *(3 / 4);  // ==> 1.5   for siz=1   and 4.5 for size =3 
	const float blueStep = 0.5;  // size / 2;
	const int step = size * 3;

	std::ifstream in("GPS.txt");
	in >> id >> x >> y >> z >> counter;

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(x + size, 0, z);
	glVertex3f(x + blueStep, 0, z + size);
	glVertex3f(x - blueStep, 0, z - size);
	glVertex3f(x + blueStep, 0, z - size);
	glEnd();//end the shape we are currently working on

	glFlush();




	//glutSwapBuffers();
}







//---------------------------------------------------------------------------

//




//=================================================================
int main(int argc, char** argv)
{
	//glutInit(&argc, argv);  //vahid
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Could not initialise SDL\n");
		return -1;
	}

	// Center window
	char env[] = "SDL_VIDEO_CENTERED=1";
	putenv(env);

	// Init OpenGL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	//#ifndef WIN32
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	//#endif

	const SDL_VideoInfo* vi = SDL_GetVideoInfo();

	bool presentationMode = false;

	int width, height;
	SDL_Surface* screen = 0;

	if (presentationMode)
	{
		width = vi->current_w;
		height = vi->current_h;
		screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_FULLSCREEN);
	}
	else
	{
		width = vi->current_w - 20;
		height = vi->current_h - 80;
		screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
	}

	if (!screen)
	{
		printf("Could not initialise SDL opengl\n");
		return -1;
	}

	glEnable(GL_MULTISAMPLE);

	SDL_WM_SetCaption("Recast Demo", 0);

	if (!imguiRenderGLInit("DroidSans.ttf"))
	{
		printf("Could not init GUI renderer.\n");
		SDL_Quit();
		return -1;
	}

	float t = 0.0f;
	float timeAcc = 0.0f;
	Uint32 lastTime = SDL_GetTicks();
	int mx = 0, my = 0;
	float rx = 45;
	float ry = -45;
	float moveW = 0, moveS = 0, moveA = 0, moveD = 0;
	float camx = 0, camy = 0, camz = 0, camr = 1000;
	float origrx = 0, origry = 0;
	int origx = 0, origy = 0;
	float scrollZoom = 0;
	bool rotate = false;
	bool movedDuringRotate = false;
	float rays[3], raye[3];
	bool mouseOverMenu = false;
	bool showMenu = !presentationMode;
	bool showLog = false;
	bool showTools = true;
	bool showLevels = false;
	bool showSample = false;
	bool showTestCases = false;
	bool showHierarchy = false;

	int propScroll = 0;
	int logScroll = 0;
	int toolsScroll = 0;

	char sampleName[64] = "Choose Sample...";

	FileList files;
	char meshName[128] = "Choose Mesh...";

	float mpos[3] = { 0, 0, 0 };
	bool mposSet = false;

	SlideShow slideShow;
	slideShow.init("slides/");

	InputGeom* geom = 0;
	Sample* sample = 0;
	TestCase* test = 0;

	BuildContext ctx;

	glEnable(GL_CULL_FACE);

	float fogCol[4] = { 0.32f, 0.31f, 0.30f, 1.0f };
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, camr*0.1f);
	glFogf(GL_FOG_END, camr*1.25f);
	glFogfv(GL_FOG_COLOR, fogCol);

	glDepthFunc(GL_LEQUAL);

	bool done = false;
	while (!done)
	{
		// Handle input events.
		int mscroll = 0;
		bool processHitTest = false;
		bool processHitTestShift = false;
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				// Handle any key presses here.
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					done = true;
				}
				else if (event.key.keysym.sym == SDLK_t)
				{
					showLevels = false;
					showSample = false;
					showTestCases = true;
					scanDirectory("Tests", ".txt", files);
				}
				else if (event.key.keysym.sym == SDLK_TAB)
				{
					showMenu = !showMenu;
				}
				else if (event.key.keysym.sym == SDLK_SPACE)
				{
					if (sample)
						sample->handleToggle();
				}
				else if (event.key.keysym.sym == SDLK_1)
				{
					if (sample)
						sample->handleStep();
				}
				else if (event.key.keysym.sym == SDLK_9)
				{
					if (geom)
						geom->save("geomset.txt");
				}
				else if (event.key.keysym.sym == SDLK_0)
				{
					delete geom;
					geom = new InputGeom;
					if (!geom || !geom->load(&ctx, "geomset.txt"))
					{
						delete geom;
						geom = 0;

						showLog = true;
						logScroll = 0;
						ctx.dumpLog("Geom load log %s:", meshName);
					}
					if (sample && geom)
					{
						sample->handleMeshChanged(geom);
					}

					if (geom || sample)
					{
						const float* bmin = 0;
						const float* bmax = 0;
						if (sample)
						{
							bmin = sample->getBoundsMin();
							bmax = sample->getBoundsMax();
						}
						else if (geom)
						{
							bmin = geom->getMeshBoundsMin();
							bmax = geom->getMeshBoundsMax();
						}
						// Reset camera and fog to match the mesh bounds.
						if (bmin && bmax)
						{
							camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
								rcSqr(bmax[1] - bmin[1]) +
								rcSqr(bmax[2] - bmin[2])) / 2;
							camx = (bmax[0] + bmin[0]) / 2 + camr;
							camy = (bmax[1] + bmin[1]) / 2 + camr;
							camz = (bmax[2] + bmin[2]) / 2 + camr;
							camr *= 3;
						}
						rx = 45;
						ry = -45;
						glFogf(GL_FOG_START, camr*0.2f);
						glFogf(GL_FOG_END, camr*1.25f);
					}
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					slideShow.nextSlide();
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					slideShow.prevSlide();
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					if (!mouseOverMenu)
					{
						// Rotate view
						rotate = true;
						movedDuringRotate = false;
						origx = mx;
						origy = my;
						origrx = rx;
						origry = ry;
					}
				}
				else if (event.button.button == SDL_BUTTON_WHEELUP)
				{
					if (mouseOverMenu)
						mscroll--;
					else
						scrollZoom -= 1.0f;
				}
				else if (event.button.button == SDL_BUTTON_WHEELDOWN)
				{
					if (mouseOverMenu)
						mscroll++;
					else
						scrollZoom += 1.0f;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				// Handle mouse clicks here.
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					rotate = false;
					if (!mouseOverMenu)
					{
						if (!movedDuringRotate)
						{
							processHitTest = true;
							processHitTestShift = true;
						}
					}
				}
				else if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (!mouseOverMenu)
					{
						processHitTest = true;
						processHitTestShift = (SDL_GetModState() & KMOD_SHIFT) ? true : false;
					}
				}

				break;

			case SDL_MOUSEMOTION:
				mx = event.motion.x;
				my = height - 1 - event.motion.y;
				if (rotate)
				{
					int dx = mx - origx;
					int dy = my - origy;
					rx = origrx - dy*0.25f;
					ry = origry + dx*0.25f;
					if (dx*dx + dy*dy > 3 * 3)
						movedDuringRotate = true;
				}
				break;

			case SDL_QUIT:
				done = true;
				break;

			default:
				break;
			}
		}

		unsigned char mbut = 0;
		if (SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK)
			mbut |= IMGUI_MBUT_LEFT;
		if (SDL_GetMouseState(0, 0) & SDL_BUTTON_RMASK)
			mbut |= IMGUI_MBUT_RIGHT;

		Uint32	time = SDL_GetTicks();
		float	dt = (time - lastTime) / 1000.0f;
		lastTime = time;

		t += dt;


		// Hit test mesh.
		if (processHitTest && geom && sample)
		{
			float hitt;
			bool hit = geom->raycastMesh(rays, raye, hitt);

			if (hit)
			{
				if (SDL_GetModState() & KMOD_CTRL)
				{
					// Marker
					mposSet = true;
					mpos[0] = rays[0] + (raye[0] - rays[0])*hitt;
					mpos[1] = rays[1] + (raye[1] - rays[1])*hitt;
					mpos[2] = rays[2] + (raye[2] - rays[2])*hitt;
				}
				else
				{
					float pos[3];
					pos[0] = rays[0] + (raye[0] - rays[0])*hitt;
					pos[1] = rays[1] + (raye[1] - rays[1])*hitt;
					pos[2] = rays[2] + (raye[2] - rays[2])*hitt;


					sample->handleClick(rays, pos, processHitTestShift);
				}
			}
			else
			{
				if (SDL_GetModState() & KMOD_CTRL)
				{
					// Marker
					mposSet = false;
				}
			}
		}

		// Update sample simulation.
		const float SIM_RATE = 20;
		const float DELTA_TIME = 1.0f / SIM_RATE;
		timeAcc = rcClamp(timeAcc + dt, -1.0f, 1.0f);
		int simIter = 0;
		while (timeAcc > DELTA_TIME)
		{
			timeAcc -= DELTA_TIME;
			if (simIter < 5)
			{
				if (sample)
					sample->handleUpdate(DELTA_TIME);
			}
			simIter++;
		}

		// Clamp the framerate so that we do not hog all the CPU.
		const float MIN_FRAME_TIME = 1.0f / 40.0f;
		if (dt < MIN_FRAME_TIME)
		{
			int ms = (int)((MIN_FRAME_TIME - dt)*1000.0f);
			if (ms > 10) ms = 10;
			if (ms >= 0)
				SDL_Delay(ms);
		}


		// Update and render
		glViewport(0, 0, width, height);
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);    // background color 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);

		// Render 3d
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(50.0f, (float)width / (float)height, 1.0f, camr);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(rx, 1, 0, 0);
		glRotatef(ry, 0, 1, 0);
		glTranslatef(-camx, -camy, -camz);

		// Get hit ray position and direction.
		GLdouble proj[16];
		GLdouble model[16];
		GLint view[4];
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetIntegerv(GL_VIEWPORT, view);
		GLdouble x, y, z;
		gluUnProject(mx, my, 0.0f, model, proj, view, &x, &y, &z);
		rays[0] = (float)x; rays[1] = (float)y; rays[2] = (float)z;
		gluUnProject(mx, my, 1.0f, model, proj, view, &x, &y, &z);
		raye[0] = (float)x; raye[1] = (float)y; raye[2] = (float)z;

		// Handle keyboard movement.
		Uint8* keystate = SDL_GetKeyState(NULL);
		moveW = rcClamp(moveW + dt * 4 * (keystate[SDLK_w] ? 1 : -1), 0.0f, 1.0f);
		moveS = rcClamp(moveS + dt * 4 * (keystate[SDLK_s] ? 1 : -1), 0.0f, 1.0f);
		moveA = rcClamp(moveA + dt * 4 * (keystate[SDLK_a] ? 1 : -1), 0.0f, 1.0f);
		moveD = rcClamp(moveD + dt * 4 * (keystate[SDLK_d] ? 1 : -1), 0.0f, 1.0f);

		float keybSpeed = 22.0f;
		if (SDL_GetModState() & KMOD_SHIFT)
			keybSpeed *= 8.0f;   //speed

		float movex = (moveD - moveA) * keybSpeed * dt;
		float movey = (moveS - moveW) * keybSpeed * dt;

		movey += scrollZoom * 2.0f;
		scrollZoom = 0;

		camx += movex * (float)model[0];
		camy += movex * (float)model[4];
		camz += movex * (float)model[8];

		camx += movey * (float)model[2];
		camy += movey * (float)model[6];
		camz += movey * (float)model[10];

		glEnable(GL_FOG);

		if (sample)
			sample->handleRender();
		if (test)
			test->handleRender();

		glDisable(GL_FOG);

		// Render GUI
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		mouseOverMenu = false;

		imguiBeginFrame(mx, my, mbut, mscroll);

		if (sample)
		{
			sample->handleRenderOverlay((double*)proj, (double*)model, (int*)view);
		}
		if (test)
		{
			if (test->handleRenderOverlay((double*)proj, (double*)model, (int*)view))
				mouseOverMenu = true;
		}

		// Help text.
		if (showMenu)
		{
			const char msg[] = "W/S/A/D: Move  RMB: Rotate";
			imguiDrawText(280, height - 20, IMGUI_ALIGN_LEFT, msg, imguiRGBA(255, 255, 255, 128));
		}

		if (showMenu)
		{
			if (imguiBeginScrollArea("Properties", width - 250 - 10, 10, 250, height - 20, &propScroll))
				mouseOverMenu = true;

			if (imguiCheck("Show Log", showLog))
				showLog = !showLog;
			if (imguiCheck("Show Tools", showTools))
				showTools = !showTools;

			imguiSeparator();
			imguiLabel("Sample");
			if (imguiButton(sampleName))
			{
				if (showSample)
				{
					showSample = false;
				}
				else
				{
					showSample = true;
					showLevels = false;
					showTestCases = false;
				}
			}

			imguiSeparator();
			imguiLabel("Input Mesh");
			if (imguiButton(meshName))
			{
				if (showLevels)
				{
					showLevels = false;
				}
				else
				{
					showSample = false;
					showTestCases = false;
					showLevels = true;
					scanDirectory("Meshes", ".obj", files);
				}


				/*
				//vahid20
				if (strcmp(meshName,"dungeon.obj")){


				std::ofstream fout2("vs.txt", std::ios_base::app | std::ios_base::out);
				fout2<<"ok";

				}*/


			}
			if (geom)
			{
				char text[64];
				snprintf(text, 64, "Verts: %.1fk  Tris: %.1fk",
					geom->getMesh()->getVertCount() / 1000.0f,
					geom->getMesh()->getTriCount() / 1000.0f);
				imguiValue(text);
			}
			imguiSeparator();

			if (geom && sample)
			{
				imguiSeparatorLine();

				sample->handleSettings();

				if (imguiButton("Build"))
				{

				//vahid
				


					ctx.resetLog();
					if (!sample->handleBuild())
					{
						showLog = true;
						logScroll = 0;
					}
					ctx.dumpLog("Build log %s:", meshName);
					showHierarchy = true;

					// Clear test.
					delete test;
					test = 0;
				}

//========================================vahid

				 

//==========================================================
				if (showHierarchy)
				{
					imguiSeparatorLine();

					sample->handleHierarchySettings();

					if (imguiButton("Build Hierarchy"))
					{
						ctx.resetLog();
						if (!sample->handleHierarchy())
						{
							showLog = true;
							logScroll = 0;
						}

						ctx.dumpLog("Build Hierarchy");
						// Clear test.
						delete test;
						test = 0;
					}
				}
			}

			if (sample)
			{
				imguiSeparatorLine();
				sample->handleDebugMode();
			}

			imguiEndScrollArea();
		}

		// Sample selection dialog.
		if (showSample)
		{
			static int levelScroll = 0;
			if (imguiBeginScrollArea("Choose Sample", width - 10 - 250 - 10 - 200, height - 10 - 250, 200, 250, &levelScroll))
				mouseOverMenu = true;

			Sample* newSample = 0;
			for (int i = 0; i < g_nsamples; ++i)
			{
				if (imguiItem(g_samples[i].name))
				{
					newSample = g_samples[i].create();
					if (newSample)
						strcpy(sampleName, g_samples[i].name);
				}
			}
			if (newSample)
			{
				delete sample;
				sample = newSample;
				sample->setContext(&ctx);
				if (geom && sample)
				{
					sample->handleMeshChanged(geom);
				}
				showSample = false;
			}

			if (geom || sample)
			{
				const float* bmin = 0;
				const float* bmax = 0;
				if (sample)
				{
					bmin = sample->getBoundsMin();
					bmax = sample->getBoundsMax();
				}
				else if (geom)
				{
					bmin = geom->getMeshBoundsMin();
					bmax = geom->getMeshBoundsMax();
				}
				// Reset camera and fog to match the mesh bounds.
				if (bmin && bmax)
				{
					camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
						rcSqr(bmax[1] - bmin[1]) +
						rcSqr(bmax[2] - bmin[2])) / 2;
					camx = (bmax[0] + bmin[0]) / 2 + camr;
					camy = (bmax[1] + bmin[1]) / 2 + camr;
					camz = (bmax[2] + bmin[2]) / 2 + camr;
					camr *= 3;
				}
				rx = 45;
				ry = -45;
				glFogf(GL_FOG_START, camr*0.1f);
				glFogf(GL_FOG_END, camr*1.25f);
			}

			imguiEndScrollArea();
		}

		// Level selection dialog.
		if (showLevels)
		{
			static int levelScroll = 1;
			if (imguiBeginScrollArea("Choose Level", width - 10 - 250 - 10 - 200, height - 10 - 450, 200, 450, &levelScroll))
				mouseOverMenu = true;

			int levelToLoad = -1;
			for (int i = 0; i < files.size; ++i)
			{
				if (imguiItem(files.files[i]))
					levelToLoad = i;
			}

			if (levelToLoad != -1)
			{
				strncpy(meshName, files.files[levelToLoad], sizeof(meshName));
				meshName[sizeof(meshName)-1] = '\0';
				showLevels = false;

				delete geom;
				geom = 0;

				char path[256];
				strcpy(path, "Meshes/");
				strcat(path, meshName);

				geom = new InputGeom;
				if (!geom || !geom->loadMesh(&ctx, path))
				{
					delete geom;
					geom = 0;

					showLog = true;
					logScroll = 0;
					ctx.dumpLog("Geom load log %s:", meshName);
				}
				if (sample && geom)
				{
					sample->handleMeshChanged(geom);
				}

				if (geom || sample)
				{
					const float* bmin = 0;
					const float* bmax = 0;
					if (sample)
					{
						bmin = sample->getBoundsMin();
						bmax = sample->getBoundsMax();
					}
					else if (geom)
					{
						bmin = geom->getMeshBoundsMin();
						bmax = geom->getMeshBoundsMax();
					}
					// Reset camera and fog to match the mesh bounds.
					if (bmin && bmax)
					{
						camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
							rcSqr(bmax[1] - bmin[1]) +
							rcSqr(bmax[2] - bmin[2])) / 2;
						camx = (bmax[0] + bmin[0]) / 2 + camr;
						camy = (bmax[1] + bmin[1]) / 2 + camr;
						camz = (bmax[2] + bmin[2]) / 2 + camr;
						camr *= 3;
					}
					rx = 45;
					ry = -45;
					glFogf(GL_FOG_START, camr*0.1f);
					glFogf(GL_FOG_END, camr*1.25f);
				}
			}

			imguiEndScrollArea();

		}

		// Test cases
		if (showTestCases)
		{
			static int testScroll = 0;
			if (imguiBeginScrollArea("Choose Test To Run", width - 10 - 250 - 10 - 200, height - 10 - 450, 200, 450, &testScroll))
				mouseOverMenu = true;

			int testToLoad = -1;
			for (int i = 0; i < files.size; ++i)
			{
				if (imguiItem(files.files[i]))
					testToLoad = i;
			}

			if (testToLoad != -1)
			{
				char path[256];
				strcpy(path, "Tests/");
				strcat(path, files.files[testToLoad]);
				test = new TestCase;
				if (test)
				{
					// Load the test.
					if (!test->load(path))
					{
						delete test;
						test = 0;
					}

					// Create sample
					Sample* newSample = 0;
					for (int i = 0; i < g_nsamples; ++i)
					{
						if (strcmp(g_samples[i].name, test->getSampleName()) == 0)
						{
							newSample = g_samples[i].create();
							if (newSample) strcpy(sampleName, g_samples[i].name);
						}
					}
					if (newSample)
					{
						delete sample;
						sample = newSample;
						sample->setContext(&ctx);
						showSample = false;
					}

					// Load geom.
					strcpy(meshName, test->getGeomFileName());
					meshName[sizeof(meshName)-1] = '\0';

					delete geom;
					geom = 0;

					strcpy(path, "Meshes/");
					strcat(path, meshName);

					geom = new InputGeom;
					if (!geom || !geom->loadMesh(&ctx, path))
					{
						delete geom;
						geom = 0;
						showLog = true;
						logScroll = 0;
						ctx.dumpLog("Geom load log %s:", meshName);
					}
					if (sample && geom)
					{
						sample->handleMeshChanged(geom);
					}

					// This will ensure that tile & poly bits are updated in tiled sample.
					if (sample)
						sample->handleSettings();

					ctx.resetLog();
					if (sample && !sample->handleBuild())
					{
						ctx.dumpLog("Build log %s:", meshName);
					}

					if (geom || sample)
					{
						const float* bmin = 0;
						const float* bmax = 0;
						if (sample)
						{
							bmin = sample->getBoundsMin();
							bmax = sample->getBoundsMax();
						}
						else if (geom)
						{
							bmin = geom->getMeshBoundsMin();
							bmax = geom->getMeshBoundsMax();
						}
						// Reset camera and fog to match the mesh bounds.
						if (bmin && bmax)
						{
							camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
								rcSqr(bmax[1] - bmin[1]) +
								rcSqr(bmax[2] - bmin[2])) / 2;
							camx = (bmax[0] + bmin[0]) / 2 + camr;
							camy = (bmax[1] + bmin[1]) / 2 + camr;
							camz = (bmax[2] + bmin[2]) / 2 + camr;
							camr *= 3;
						}
						rx = 45;
						ry = -45;
						glFogf(GL_FOG_START, camr*0.2f);
						glFogf(GL_FOG_END, camr*1.25f);
					}

					// Do the tests.
					if (sample)
						test->doTests(sample->getNavMesh(), sample->getNavMeshQuery());
				}
			}

			imguiEndScrollArea();
		}


		// Log
		if (showLog && showMenu)
		{
			if (imguiBeginScrollArea("Log", 250 + 20, 10, width - 300 - 250, 200, &logScroll))
				mouseOverMenu = true;
			for (int i = 0; i < ctx.getLogCount(); ++i)
				imguiLabel(ctx.getLogText(i));
			imguiEndScrollArea();
		}

		// Tools
		if (!showTestCases && showTools && showMenu) // && geom && sample)
		{
			if (imguiBeginScrollArea("Tools", 10, 10, 250, height - 20, &toolsScroll))
				mouseOverMenu = true;

			if (sample)
				sample->handleTools();

			imguiEndScrollArea();
		}

		slideShow.updateAndDraw(dt, (float)width, (float)height);

		// Marker
		if (mposSet && gluProject((GLdouble)mpos[0], (GLdouble)mpos[1], (GLdouble)mpos[2],
			model, proj, view, &x, &y, &z))
		{
			// Draw marker circle
			glLineWidth(5.0f);
			glColor4ub(240, 220, 0, 196);
			glBegin(GL_LINE_LOOP);
			const float r = 25.0f;
			for (int i = 0; i < 20; ++i)
			{
				const float a = (float)i / 20.0f * RC_PI * 2;
				const float fx = (float)x + cosf(a)*r;
				const float fy = (float)y + sinf(a)*r;
				glVertex2f(fx, fy);
			}
			glEnd();
			glLineWidth(1.0f);
		}

		imguiEndFrame();
		imguiRenderGLDraw();

		glEnable(GL_DEPTH_TEST);
		SDL_GL_SwapBuffers();
	}

	imguiRenderGLDestroy();

	SDL_Quit();

	delete sample;
	delete geom;

	return 0;
}
