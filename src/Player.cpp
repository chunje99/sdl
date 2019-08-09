//==============================================================================
#include "config.h"
#include "CApp.h"

//==============================================================================

DEFINE_string(file, "", "video file name");
int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
#if defined OS_LINUX
    gflags::ParseCommandLineFlags(&argc, &argv, true);
#elif defined OS_MAC
    google::ParseCommandLineFlags(&argc, &argv, true);
#endif
    LOG(INFO) << "FileName = " << FLAGS_file;

    CApp theApp;
    theApp.SetFileName(FLAGS_file.c_str());

    return theApp.OnExecute();
    return 0;
}

//==============================================================================
