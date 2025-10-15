#include <PhxGrabber.h>
#include <CamGui.h>
#include <thread>

int main()
{
   auto app = CamGui();
   app.Run();
   return 0;
}

// static PhxGrabber::Frame waitForFrame(PhxGrabber& cam, uint32_t timeout_ms) {
//     using namespace std::chrono;
//     auto t0 = steady_clock::now();
//     for (;;) {
//         if (auto f = cam.GetBuffer()) return f;
//         if (duration_cast<milliseconds>(steady_clock::now() - t0).count() >= timeout_ms) {
//             return {};
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(5));
//     }
// }

// int main() {
//     try {
//         PhxGrabber cam;

//         // ---------- FIRST RUN ----------
//         std::cout << "\n=== FIRST RUN ===\n";
//         PhxGrabber::Options opt;
//         opt.boardNumber  = 1;
//         opt.channelNumber= 1;
//         opt.acquisitionBufferCount = 16;

//         cam.Init(opt);
//         cam.Open();
//         // (optional) make dst match camera source
//         // cam.SetGrabberDstToSource();
//         cam.Start();

//         // Wait up to 1500 ms for a frame
//         if (auto f = waitForFrame(cam, 1500)) {
//             std::cout << "Got frame " << f.width << "x" << f.height << "\n";
//             cam.SaveFrameRaw(f, "frame_first.raw", true); // releases buffer
//         } else {
//             std::cout << "No frame within timeout.\n";
//         }
//         cam.Close();
//         std::cout << "Closed first run.\n";

//         // ---------- SECOND RUN ----------
//         std::cout << "\n=== SECOND RUN ===\n";
//         cam.Init(opt);
//         cam.Open();
//         // cam.SetGrabberDstToSource();
//         cam.Start();

//         if (auto f = waitForFrame(cam, 1500)) {
//             std::cout << "Got frame " << f.width << "x" << f.height << "\n";
//             cam.SaveFrameRaw(f, "frame_second.raw", true);
//         } else {
//             std::cout << "No frame within timeout (second run).\n";
//         }
//         cam.Close();
//         std::cout << "Closed second run.\n";

//         std::cout << "\nAll done.\n";
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << "\n";
//     }
//     return 0;
// }