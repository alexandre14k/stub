#include "root.hpp"
#include "link.hpp"
#include "gui.hpp"

int main(int argc, char** argv) {
    Link lib_anchor;
    lib_anchor.execute();

    GuiApp app;
    return app.run(argc, argv);
}