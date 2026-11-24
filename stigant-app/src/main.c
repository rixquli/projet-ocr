#include "style/stigant-style.h"
#include "window/main/stigant-window.h"
#include <gtk/gtk.h>

static void
on_startup (GApplication *app, gpointer user_data)
{
  stigant_style_load ();
}

static void
on_activate (GtkApplication *app, gpointer user_data)
{
  StigantWindow *win = stigant_window_new (
      app); // create the window (header + stacks + pages)

  gtk_widget_show_all (GTK_WIDGET (win)); // show every widget of the .ui
  gtk_window_present (GTK_WINDOW (win));  // bring it to the front
}

int
main (int argc, char **argv)
{
  GtkApplication *app = gtk_application_new (
      "org.stigant.App",
      G_APPLICATION_DEFAULT_FLAGS); // create the application
  g_signal_connect (app, "startup", G_CALLBACK (on_startup),
                    NULL); // Connect css / fonts
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

  int status
      = g_application_run (G_APPLICATION (app), argc, argv); // Start App
  g_object_unref (
      app); // Decreases the reference count of app to 0 => free the app
  return status;
}
