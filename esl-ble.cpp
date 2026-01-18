#include <iostream>
#ifdef _MSC_VER
#else
#include <csignal>
#endif

#include "png2srgb8.h"
#include "ble-helper.h"

static char *fn;
static std::mutex mutexWriteState;
static std::condition_variable cvWriteState;
static bool stopRequest = false;

static void stop()
{
    std::unique_lock<std::mutex> lck(mutexWriteState);
    stopRequest = true;
    cvWriteState.notify_all();
}

#ifndef _MSC_VER
void signalHandler(int signal)
{
    switch (signal) {
        case SIGINT:
            std::cerr << MSG_INTERRUPTED << std::endl;
            stop();
            done();
            std::cerr << MSG_GRACEFULLY_STOPPED << std::endl;
            exit(0);
        case SIGSEGV:
            std::cerr << ERR_SEGMENTATION_FAULT << std::endl;
            printTrace();
            exit(-1);
        case SIGABRT:
            std::cerr << ERR_ABRT << std::endl;
            printTrace();
            exit(-2);
        case SIGHUP:
            std::cerr << ERR_HANGUP_DETECTED << std::endl;
            break;
        case SIGUSR2:	// 12
            std::cerr << MSG_SIG_FLUSH_FILES << std::endl;
            break;
        case 42:	// restart
            std::cerr << MSG_RESTART_REQUEST << std::endl;
            stop();
            done();
            run();
            break;
        default:
            break;
    }
}
#else
BOOL WINAPI winSignalHandler(DWORD signal) {
    std::cerr << "Interrupted.." << std::endl;
    stop();
    return true;
}
#endif

void setSignalHandler()
{
#ifdef _MSC_VER
    SetConsoleCtrlHandler(winSignalHandler,  true);
#else
    struct sigaction action {};
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = &signalHandler;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGHUP, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGUSR2, &action, nullptr);
    sigaction(42, &action, nullptr);
#endif
}

class RewriteESLOnFirstDiscover : public OnDiscover {
public:
    void discoverFirstTime(DiscoveredDevice &device) override
    {
        std::cout << macAddress2string(device.addr) << ' ' << device.metadata.toString() << ' ' << device.rssi << "\n";
        if (!discoverer)
            return;
        if (!discoverExtra)
            return;
        int r = discoverer->writeSRgb(&device, (Image2sRgb*) discoverExtra);
        if (r < 0) {
            std::cerr << "Error send image to the device " << r << std::endl;
        }
        stop();
    }

    void discoverNextTime(DiscoveredDevice &device) override
    {
        std::cout << macAddress2string(device.addr) << ' ' << device.metadata.toString() << ' ' << device.rssi << "\n";
    }
};

static void run()
{
    Png2sRgb png;
    int32_t sz = png.loadFile(fn);
    if (sz < 0) {
        std::cerr << "Error load image from the " << fn << " file" << std::endl;
        exit(sz);
    }

    std::cout << "Looking for ESL with " << png.w << "x" << png.h << " screen size" << std::endl;
    BLEHelper b(new RewriteESLOnFirstDiscover(), &png);
    b.startDiscovery();

    std::cout << "Press Ctrl+Break (or Ctrl+C) to interrupt" << std::endl;
    // filter device somehow
    // b.waitDiscover("ff:ff:92:13:76:14");
    auto devicesFound = b.waitDiscover(1);
    if (!devicesFound)
        exit(0);

    std::unique_lock<std::mutex> lock(mutexWriteState);
    bool *sr = &stopRequest;
    cvWriteState.wait_for(lock, std::chrono::seconds(60), [&b, sr] {
        return *sr;
    });

    b.stopDiscovery(10);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << argv[0] << " <file.png>" << std::endl;
        return -1;
    }

    fn = argv[1];
    setSignalHandler();

    run();
    return 0;
}
