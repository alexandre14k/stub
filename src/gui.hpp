#pragma once
#include "root.hpp"
#include <gtk/gtk.h>

struct AppMsg {
    enum Type { INCREMENT, SET_VALUE } type;
    int value;
};

class GuiApp {
public:
    GuiApp();
    ~GuiApp();
    int run(int argc, char** argv);

private:
    GtkApplication* app;
    GtkWidget* window;
    GtkWidget* label;
    GtkCssProvider* provider;
    int count;
    bool going_up;

    std::queue<AppMsg> msg_queue;
    std::mutex queue_mutex;

    void push_msg(AppMsg msg);
    bool pop_msg(AppMsg& msg);

    static void on_activate(GtkApplication* app,
        gpointer data);
    static void on_button_clicked(GtkButton* btn,
        gpointer data);
    static void on_slider_changed(GtkRange* range,
        gpointer data);
    static gboolean on_process_msg(gpointer data);
    void apply_prism_color();
};