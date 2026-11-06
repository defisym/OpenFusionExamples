#include "../CaptureCrop.h"
#include <cassert>

int main() {
    using neopic_capture::ClientCrop;
    neopic_capture::Crop c{};
    // Client excludes root window title/menu, at native and scaled capture sizes.
    assert(ClientCrop(100, 200, 1280, 760, 100, 240, 1280, 720, 1280, 760, c));
    assert(c.left == 0 && c.top == 40 && c.right == 1280 && c.bottom == 760);
    assert(ClientCrop(100, 200, 1280, 760, 100, 240, 1280, 720, 1920, 1140, c));
    assert(c.left == 0 && c.top == 60 && c.right == 1920 && c.bottom == 1140);
    // A monitor to the left of the primary has negative desktop coordinates.
    assert(ClientCrop(-1920, -200, 1280, 760, -1920, -160, 1280, 720, 1280, 760, c));
    assert(c.left == 0 && c.top == 40 && c.right == 1280 && c.bottom == 760);
    // Fractional scale: round the two edges independently.
    assert(ClientCrop(0, 0, 10, 10, 1, 1, 8, 8, 13, 13, c));
    assert(c.left == 1 && c.top == 1 && c.right == 12 && c.bottom == 12);
    assert(!ClientCrop(0, 0, 0, 760, 0, 40, 1280, 720, 1280, 760, c));
    assert(!ClientCrop(0, 0, 1280, 760, -1, 40, 1280, 720, 1280, 760, c));
    assert(!ClientCrop(0, 0, 1280, 760, 0, 50, 1280, 720, 1280, 760, c));
    assert(!ClientCrop(0, 0, 1280, 760, 0, 40, 0, 720, 1280, 760, c));
    assert(!ClientCrop(0, 0, 1280, 760, 0, 40, 1280, 720, 0, 760, c));
}
