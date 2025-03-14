#pragma once

#include <windows.h>
#include <cstdint>
#include <thread>
#include "shared_memory_queue.h"
#include "nv12_scale.h"
#include "libdshowcapture/source/output-filter.hpp"
#include "libdshowcapture/source/dshow-formats.hpp"
#include "WinHandle.hpp"

#define DEFAULT_CX 1920
#define DEFAULT_CY 1080
#define DEFAULT_INTERVAL 333333ULL

typedef struct
{
    int cx;
    int cy;
    nv12_scale_t scaler;
    const uint8_t* source_data;
    uint8_t* scaled_data;
} placeholder_t;

class VCamFilter : public DShow::OutputFilter
{
    std::thread th;

    video_queue_t* vq = nullptr;
    int queue_mode = 0;
    bool in_obs = false;
    enum queue_state prev_state = SHARED_QUEUE_STATE_INVALID;
    placeholder_t placeholder;
    uint32_t cx = DEFAULT_CX;
    uint32_t cy = DEFAULT_CY;
    DShow::VideoFormat format;
    uint64_t interval = DEFAULT_INTERVAL;
    WinHandle thread_start;
    WinHandle thread_stop;

    nv12_scale_t scaler = {};

    bool stopped() const
    {
        return WaitForSingleObject(thread_stop, 0) != WAIT_TIMEOUT;
    }

    inline uint64_t GetTime();

    void Thread();
    void Frame(uint64_t ts);
    void ShowOBSFrame(uint8_t* ptr);
    void ShowDefaultFrame(uint8_t* ptr);
    void UpdatePlaceholder(void);
    const int GetOutputBufferSize(void);

protected:
    const wchar_t* FilterName() const override;

public:
    VCamFilter();
    ~VCamFilter() override;

    STDMETHODIMP Pause() override;
};
