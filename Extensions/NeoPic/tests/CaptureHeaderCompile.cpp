// Compile the concrete backends and optional derived example with the real SDK.
#include "../FrameCaptureBenchmark.h"

static_assert(sizeof(CaptureBackend) == 4 && sizeof(EDITDATA::reserved) == 64);
static_assert(offsetof(EDITDATA, captureBackend) == sizeof(EDITDATA_V1));
static_assert(sizeof(EDITDATA) == sizeof(EDITDATA_V1) + sizeof(CaptureBackend) + 64);

void CompileFactories(LPRDATA rdPtr) {
    auto normal = CreateFrameCapture(rdPtr, CaptureBackend::Dc);
    auto measured = CreateFrameCaptureBenchmark(rdPtr, CaptureBackend::Secondary, {100, 10});
    normal->CaptureAndLoad(rdPtr);
    measured->CaptureAndLoad(rdPtr);
}
