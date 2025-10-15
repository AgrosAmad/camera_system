#include <CamObj.h>

namespace X3
{
    namespace geom
    {
        CamObj::CamObj()
        {
            mName = "Cameras";

            // Inits phx wrapped, opens and prints info
            mCamGrab = std::make_shared<PhxGrabber>();
            mCamGrab->Init();
            mCamGrab->Open();
            mCamGrab->Start();

            PhxGrabber::Options opt;
            opt.channelNumber = 3;
            mCamGrab2 = std::make_shared<PhxGrabber>();
            mCamGrab2->Init(opt);
            mCamGrab2->Open();
            mCamGrab2->Start();

            // Init texture
            mCamTex = std::make_shared<CamTex>();
            mCamTex2 = std::make_shared<CamTex>();
        }

        CamObj::~CamObj()
        {
            mCamGrab->Close();
            mCamGrab2->Close();
        }

        void CamObj::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
            ImGui::Image((ImTextureID)(intptr_t)mCamTex->id,ImVec2(mCamTex->w, mCamTex->h));
            ImGui::Image((ImTextureID)(intptr_t)mCamTex2->id,ImVec2(mCamTex2->w, mCamTex2->h));
        }

        void CamObj::Update()
        {
            PhxGrabber::Frame frame = mCamGrab->GetBuffer();
            if(frame) mCamTex->update(frame);

            PhxGrabber::Frame frame2 = mCamGrab2->GetBuffer();
            if(frame2) mCamTex2->update(frame2);
        }

        void CamObj::Kill()
        {
            printf("safe closed \n");
            mCamGrab->Close();
            mCamGrab2->Close();
        }
    } // namespace geom
} // namespace X3