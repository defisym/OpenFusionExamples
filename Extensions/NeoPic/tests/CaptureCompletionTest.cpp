// Compile with -I tests/completion-mocks; the production helper is tested directly.
#include "../CaptureCompletion.h"
#include <cassert>

int main() {
    ID3D11Device device;
    ID3D11DeviceContext context;
    CaptureCompletion completion;
    assert(completion.Poll() == E_UNEXPECTED);
    assert(completion.Prepare(nullptr, &context) == E_POINTER);
    context.type = D3D11_DEVICE_CONTEXT_DEFERRED;
    assert(completion.Prepare(&device, &context) == E_INVALIDARG);
    assert(device.allocations == 0);
    context.type = D3D11_DEVICE_CONTEXT_IMMEDIATE;
    device.result = E_FAIL;
    assert(completion.Prepare(&device, &context) == E_FAIL);
    assert(!completion.InFlight() && context.operations.empty());
    device.result = S_OK;
    assert(completion.Prepare(&device, &context) == S_OK);
    assert(completion.Prepare(&device, &context) == HRESULT_FROM_WIN32(ERROR_BUSY));
    assert(!completion.InFlight());
    context.replies = {{S_FALSE, FALSE}, {S_OK, FALSE}, {S_OK, TRUE}};
    completion.Submit();
    assert(completion.InFlight());
    assert((context.operations == std::vector<int>{1, 2})); // End then one Flush
    assert(completion.Poll() == S_FALSE);
    assert(completion.Poll() == S_FALSE); // S_OK alone does not mean done
    assert(completion.Poll() == S_OK);
    assert(context.polls == 3 && context.correctFlags && mockSleeps == 0);
    assert(context.operations.size() == 2); // Poll never flushes or waits
    completion.Reset();
    assert(!completion.InFlight() && device.event.refs == 1 && context.refs == 1);

    assert(completion.Prepare(&device, &context) == S_OK);
    completion.Submit();
    context.polls = 0;
    context.replies = {{E_FAIL, FALSE}};
    assert(completion.Poll() == E_FAIL); // propagate device/query errors
    completion.Reset();

    assert(completion.Prepare(&device, &context) == S_OK);
    completion.Submit();
    context.polls = 0;
    context.replies = {{2, TRUE}};
    assert(completion.Poll() == E_UNEXPECTED); // no other success code publishes
    completion.Reset();

    assert(completion.Prepare(&device, &context) == S_OK);
    completion.Submit();
    context.polls = 0;
    context.replies = {{S_FALSE, FALSE}, {S_OK, TRUE}};
    assert(completion.WaitForTeardown() == S_OK);
    assert(mockSleeps == 1); // the explicit teardown method is the only wait
    completion.Reset();
}
