#include <CamObj.h>

CamCont::CamCont(const int& channel)
{
    // Inits Phx Grabber
    PhxGrabber::Options opt;
    opt.channelNumber = channel;
    mGrabber = std::make_shared<PhxGrabber>();
    mGrabber->Init(opt);
    mGrabber->Open();
    mGrabber->Start();
    
    // Init texture
    mTex = std::make_shared<CamTex>();

    // Init demosaic
    mDem = std::make_shared<DemosaicFbo>();
    mDem->Init();
}

CamCont::~CamCont()
{
    mGrabber->Close();
}

void CamCont::Draw()
{
    ImGui::Image((ImTextureID)(intptr_t)mDem->ColorTexture(), ImVec2(mTex->w,  mTex->h));
}

void CamCont::Update(const CamProp& prop)
{

    mProp = prop;

    PhxGrabber::Frame frame = mGrabber->GetBuffer();
    if(frame) mTex->update(frame);

    auto pat = static_cast<BayerPattern>(mProp.patternIdx);

    // After CamTex update:
    mDem->Render(mTex->id,  mTex->w,  mTex->h,
                (BayerPattern)pat, mProp.black, mProp.wbR, mProp.wbG, mProp.wbB, mProp.gamma);
}

void CamCont::Kill()
{
    mGrabber->Close();
}

GLuint CamCont::Texture() const
{
    return mDem->ColorTexture();
}

namespace X3
{
    namespace geom
    {
        CamObj::CamObj()
        {
            mName = "Cameras";

            // Inits cameras (with correct channel)
            mCam1 = std::make_shared<CamCont>();
            mCam2 = std::make_shared<CamCont>(3);
        }

        CamObj::~CamObj()
        {
            mCam1->Kill();
            mCam2->Kill();
        }

        void CamObj::RenderUi()
        {

            // White balance / tone (tweak live)
            ImGui::Text("Name: %s", mName.c_str());
            // let the user pick the pattern(s); these ints drive the next update pass
            ImGui::Combo("Bayer pattern Cam1", &MasterProp.patternIdx, "RGGB\0BGGR\0GRBG\0GBRG\0");
            ImGui::SliderFloat("WB R", &MasterProp.wbR, 0.5f, 4.0f);
            ImGui::SliderFloat("WB G", &MasterProp.wbG, 0.5f, 4.0f);
            ImGui::SliderFloat("WB B", &MasterProp.wbB, 0.5f, 4.0f);
            ImGui::SliderFloat("Black", &MasterProp.black, 0.0f, 0.05f);
            ImGui::SliderFloat("Gamma", &MasterProp.gamma, 1.0f, 3.0f);

            // show the color textures produced in UpdateCameras()
            mCam1->Draw();
            mCam2->Draw();

        }

        void CamObj::Update()
        {
            mCam1->Update(MasterProp);
            mCam2->Update(MasterProp);
        }

        void CamObj::Kill()
        {
            printf("safely closed \n");
            mCam1->Kill();
            mCam2->Kill();
        }
    } // namespace geom
} // namespace X3