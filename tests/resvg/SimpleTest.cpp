#include "svg_raster.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int res = render_svg_to_png(argv[1], "output.png");
    if (res != 0) {
        std::cerr << "render_svg_to_png failed, code = " << res << "\n";
        return 1;
    }
    std::cout << "OK: output.png\n";
    return 0;
}
