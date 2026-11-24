#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_WINDOW (stigant_window_get_type())
G_DECLARE_FINAL_TYPE(StigantWindow, stigant_window, STIGANT, WINDOW, GtkApplicationWindow)

StigantWindow *stigant_window_new(GtkApplication *app);
GObject       *stigant_window_lookup(StigantWindow *self, const char *page, const char *id);
void           on_generate_mode_clicked(GtkButton *button, gpointer user_data);
void           on_solve_mode_clicked(GtkButton *button, gpointer user_data);
void           on_stigant_mode_changed(GObject *object, GParamSpec *pspec, gpointer user_data);
void           stigant_window_setup_header_stack(StigantWindow *self, GtkBuilder *builder);
void           stigant_window_setup_content_stack(StigantWindow *self, GtkBuilder *builder);