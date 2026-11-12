#pragma once
#include <memory>
#include "CaptureBackend.h"

// One synchronous capture and ownership handoff, independent of its source.
// Optional subclasses can decorate this operation without changing the action.
class FrameCapture {
    CaptureBackend backend;
protected:
    explicit FrameCapture(CaptureBackend value) : backend(value) {}
    virtual HRESULT CaptureSurface(LPRDATA rdPtr, std::unique_ptr<cSurface>& surface) = 0;
    virtual SIZE OutputSize(LPRDATA rdPtr, SIZE sourceSize) const;
    virtual bool DiagnosticsEnabled() const { return true; }
    virtual void ReleaseInput() {}
    void Report(HRESULT hr, const char* stage) const;
public:
    virtual ~FrameCapture() = default;
    FrameCapture(const FrameCapture&) = delete;
    FrameCapture& operator=(const FrameCapture&) = delete;
    CaptureBackend Backend() const { return backend; }
    virtual void Update(LPRDATA rdPtr) { (void)rdPtr; }
    virtual HRESULT CaptureAndLoad(LPRDATA rdPtr, SIZE* outputSize = nullptr);
};

std::unique_ptr<FrameCapture> CreateFrameCapture(LPRDATA rdPtr, CaptureBackend backend);
