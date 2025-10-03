#include "CamGui.h"

CamGui::CamGui()
{
    // Inits engine
    mEngine = std::make_shared<X3::Engine>();
}

void CamGui::Run()
{
    // Runs engine
    mEngine->Run();
}
