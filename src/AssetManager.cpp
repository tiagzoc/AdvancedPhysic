#include "AssetManager.h"

void AssetManager::Init()
{

	pCube = new CUBE();
	pCube->Init();

	pSphere = new SPHERE();
    pSphere->Init();

    Unlit.Init("res/unlit.vert", "res/unlit.frag");
    Unlit.RegisterUniform("u_World");
    Unlit.RegisterUniform("u_View");
    Unlit.RegisterUniform("u_Proj");
    Unlit.RegisterUniform("u_WVP");
    Unlit.RegisterUniform("u_Texture0");

    Wireframe.Init("res/wireframe.vert", "res/wireframe.frag");
    Wireframe.RegisterUniform("u_World");
    Wireframe.RegisterUniform("u_View");
    Wireframe.RegisterUniform("u_Proj");
    Wireframe.RegisterUniform("u_WVP");
    Wireframe.RegisterUniform("u_WireFrameColor");
    Wireframe.RegisterUniform("u_MaxDistance");

    Obama.Init("res/Obama.png");
    TE.Init("res/tesla_einstein.png");
    EAU.Init("res/eau.jpg");

    mpHeads = new Texture[17];
    mpHeads[0].Init("res/heads/Alexandre.png");
    mpHeads[1].Init("res/heads/Antoine.png");
    mpHeads[2].Init("res/heads/Clement.png");
    mpHeads[3].Init("res/heads/Elia.png");
    mpHeads[4].Init("res/heads/Ethan.png");
    mpHeads[5].Init("res/heads/Hugo-C.png");
    mpHeads[6].Init("res/heads/Hugo-V.png");
    mpHeads[7].Init("res/heads/Ilan.png");
    mpHeads[8].Init("res/heads/Killian.png");
    mpHeads[9].Init("res/heads/Liam.png");
    mpHeads[10].Init("res/heads/Mithaniel.png");
    mpHeads[11].Init("res/heads/Noa.png");
    mpHeads[12].Init("res/heads/Paul-Henri.png");
    mpHeads[13].Init("res/heads/Thomas.png");
    mpHeads[14].Init("res/heads/Tiago.png");
    mpHeads[15].Init("res/heads/Tristan.png");
    mpHeads[16].Init("res/heads/Willem.png");

}

void AssetManager::Destroy()
{
    delete pCube;
    delete pSphere;
}