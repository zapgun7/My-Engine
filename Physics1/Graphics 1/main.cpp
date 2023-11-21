#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
#include <time.h>

cPhysics* g_pPhysics = NULL;

int main(int argc, char** argv)
{
	srand(time(NULL));

	::g_pPhysics = new cPhysics();
	cGraphicsMain* graphics = cGraphicsMain::getGraphicsMain();// This initializes it if not yet made 
	g_pPhysics->setGraphics();

	while (graphics->Update() == 0)
	{
		// Running...
	}
	graphics->Destroy();

	return 0;
}