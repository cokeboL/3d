#include "headers.h"
#include "terrain.h"
#include "Rectangle.h"
#include "log.h"
#include "program.h"
#include "globals.h"
#include "cube.h"
#include "cudaUtils.h"
#include "texture.h"
#include "renderRoot.h"
#include "particleGroup.h"
#include "rand.h"
#include "waves.h"
#include "pousseeArchimede.h"
#include "constantForce.h"
#include "constantMassForce.h"
#include "frottementFluide.h"
#include "frottementFluideAvance.h"
#include "dynamicScheme.h"
#include "killParticles.h"
#include "seaFlow.h"
#include "attractor.h"
#include "springsSystem.h"
#include "audible.h"
#include "splines/CardinalSpline.h"
#include "skybox.h"
#include "marchingCubes.h"
#include "seaweedGroup.h"
#include "bubblesGenerator.h"

#include <qapplication.h>
#include <QWidget>
#include <vector>
#include <ctime>

#include <ostream>
#include <cassert>
#include <sstream>

using namespace std;
using namespace log4cpp;

int main(int argc, char** argv) {

        //random
        srand(time(NULL));

        //logs
        log4cpp::initLogs();

        //cuda
        CudaUtils::logCudaDevices(log_console);

        log_console.infoStream() << "[Rand Init] ";
        log_console.infoStream() << "[Logs Init] ";

        // glut initialisation (mandatory) 
        glutInit(&argc, argv);
        log_console.infoStream() << "[Glut Init] ";

        // Read command lines arguments.
        QApplication application(argc,argv);
        log_console.infoStream() << "[Qt Init] ";

        // Instantiate the viewer (mandatory)
        Viewer *viewer = new Viewer();
        viewer->setWindowTitle("Sea diver");
        viewer->show();
        Globals::viewer = viewer;

        //glew initialisation (mandatory)
        log_console.infoStream() << "[Glew Init] " << glewGetErrorString(glewInit());

        //openal 
        Audible::initOpenALContext();
        alutInit(&argc, argv);
        log_console.infoStream() << "[Alut Init] ";

        //global vars
        Globals::init();
        Globals::print(std::cout);
        Globals::check();

        //texture manager
        Texture::init();

        log_console.infoStream() << "Running with OpenGL " << Globals::glVersion << " and glsl version " << Globals::glShadingLanguageVersion << " !";
        //FIN INIT//

        RenderRoot *root = new RenderRoot(); 

        //Skybox
        //Note sur la pack 'SkyboxSet1' il faut retourner les textures selon l'horizontale a cause du convertToGLFormat
        Skybox *skybox = new Skybox("textures/skybox/SkyboxSet1/SunSet/", 
                        "SunSetRight2048.png SunSetLeft2048.png SunSetUp2048.png SunSetDown2048.png SunSetBack2048.png SunSetFront2048.png",
                        "png");
        skybox->scale(50);
        root->addChild("skybox", skybox);

        //Waves
        Waves *waves = new Waves(0.0,0.0,100.0,100.0, 10.0, skybox->getCubeMap());
        waves->translate(0,0,0);
        root->addChild("vagues", waves);

        //Terrain
        MarchingCubes *terrain = new MarchingCubes(128,128,128,100.0f/128);
        terrain->translate(-50,-65,-50);
        root->addChild("terrain", terrain);

        //Bulles
        BubblesGenerator *bubbles = new BubblesGenerator(100,50,500,10);
        root->addChild("zParticles", bubbles);

        //Terrain2
        //Terrain *terrain = new Terrain(black_img, rgb_heightmap.width(), rgb_heightmap.height(), true); 
        //terrain->rotate(qglviewer::Quaternion(qglviewer::Vec(1,0,0), 3.14/2)); 
        //root->addChild("terrain", terrain);
        
        SeeweedGroup *seeweeds = new SeeweedGroup(20000,10,1.0f);
        for (int i = 0; i < 30; i++) {
                seeweeds->spawnGroup(qglviewer::Vec(Random::randf(-45,40),-26,Random::randf(0,25)), 100, NULL, NULL);
        }
        seeweeds->releaseParticles();
        root->addChild("seeweeds", seeweeds);


        //Configure viewer
        viewer->addRenderable(root);

        //Run main loop.
        application.exec();

        //Exit
        Audible::closeOpenALContext();
        alutExit();

        return EXIT_SUCCESS;
}

