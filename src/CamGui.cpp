#include "CamGui.h"

CamGui::CamGui()
{
    // Inits engine
    mEngine = std::make_shared<X3::Engine>();

    // Inits camera object and add it to scene
    auto scene = mEngine->GetScene();
    mCamObj = std::make_shared<X3::geom::Camera>("camera", 1);
    scene->Add(mCamObj);
}

void CamGui::Run()
{
    // Runs engine
    mEngine->Run();
}
