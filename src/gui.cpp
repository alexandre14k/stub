#include "gui.hpp"

GuiApp::GuiApp() : app(nullptr), window(nullptr),
    label(nullptr), provider(nullptr), count(0),
    going_up(true) {}

GuiApp::~GuiApp() {
    if (app) g_object_unref(app);
}

int GuiApp::run(int argc, char** argv) {
    app = gtk_application_new("org.stub",
        G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate",
        G_CALLBACK(on_activate), this);
    int status = g_application_run(G_APPLICATION(app),
        argc, argv);
    return status;
}

void GuiApp::push_msg(AppMsg msg) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    msg_queue.push(msg);
}

bool GuiApp::pop_msg(AppMsg& msg) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    if (msg_queue.empty()) return false;
    msg = msg_queue.front();
    msg_queue.pop();
    return true;
}

void GuiApp::on_activate(GtkApplication* app,
    gpointer data) {
    GuiApp* self = static_cast<GuiApp*>(data);

    self->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(self->window),
        "stub");
    gtk_window_set_default_size(
        GTK_WINDOW(self->window), 400, 200);

    self->provider = gtk_css_provider_new();
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(self->provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget* box = gtk_box_new(
        GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(self->window),
        box);

    self->label = gtk_label_new("Count: 0");
    gtk_widget_set_halign(self->label,
        GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), self->label);

    GtkWidget* btn = gtk_button_new_with_label(
        "Increment");
    gtk_widget_set_halign(btn, GTK_ALIGN_CENTER);
    g_signal_connect(btn, "clicked",
        G_CALLBACK(on_button_clicked), self);
    gtk_box_append(GTK_BOX(box), btn);

    GtkWidget* scale = gtk_scale_new_with_range(
        GTK_ORIENTATION_HORIZONTAL, 0, 255, 1);
    gtk_widget_set_size_request(scale, 200, -1);
    g_signal_connect(scale, "value-changed",
        G_CALLBACK(on_slider_changed), self);
    gtk_box_append(GTK_BOX(box), scale);

    gtk_window_present(GTK_WINDOW(self->window));
}

void GuiApp::on_button_clicked(GtkButton* btn,
    gpointer data) {
    GuiApp* self = static_cast<GuiApp*>(data);
    AppMsg msg{AppMsg::INCREMENT, 0};
    self->push_msg(msg);
    g_idle_add(on_process_msg, self);
}

void GuiApp::on_slider_changed(GtkRange* range,
    gpointer data) {
    GuiApp* self = static_cast<GuiApp*>(data);
    int val = gtk_range_get_value(range);
    AppMsg msg{AppMsg::SET_VALUE, val};
    self->push_msg(msg);
    g_idle_add(on_process_msg, self);
}

gboolean GuiApp::on_process_msg(gpointer data) {
    GuiApp* self = static_cast<GuiApp*>(data);
    AppMsg msg;

    if (self->pop_msg(msg)) {
        if (msg.type == AppMsg::INCREMENT) {
            if (self->going_up) {
                self->count++;
                if (self->count >= 255) {
                    self->count = 255;
                    self->going_up = false;
                }
            } else {
                self->count--;
                if (self->count <= 0) {
                    self->count = 0;
                    self->going_up = true;
                }
            }
        } else if (msg.type == AppMsg::SET_VALUE) {
            self->count = msg.value;
            if (self->count >= 255)
                self->going_up = false;
            else if (self->count <= 0)
                self->going_up = true;
        }

        m_string text = "Count: " +
            std::to_string(self->count);
        gtk_label_set_label(GTK_LABEL(self->label),
            text.c_str());
        self->apply_prism_color();
    }
    return G_SOURCE_REMOVE;
}

void GuiApp::apply_prism_color() {
    int hue = (count * 360) / 255;
    int lightness = 100 - (count * 100) / 255;

    m_string css = "window { background-color: hsl(" +
        std::to_string(hue) + ", 100%, " +
        std::to_string(lightness) + "%); }";

    gtk_css_provider_load_from_string(provider, css.c_str());
}