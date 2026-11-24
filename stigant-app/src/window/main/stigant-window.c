#include "stigant-window.h"
#include "stigant-controller.h"
#include "window/generate/stigant-generate.h"
#include "window/solver/stigant-solver.h"
#include "stigant-model.h"

#define UI "/org/stigant/App/ui/"

struct _StigantWindow
{

  GtkApplicationWindow parent_instance;

  GtkWidget         *header_bar;
  GtkStack          *header_stack;
  GtkStack          *content_stack;
  GHashTable        *pages;
  StigantModel      *model;
  StigantController *controller;
};

G_DEFINE_FINAL_TYPE(StigantWindow, stigant_window, GTK_TYPE_APPLICATION_WINDOW)

static void
add_page(StigantWindow *self, GtkStack *stack, const char *resource,
         const char *page, const char *name)
{
  GtkBuilder *builder = gtk_builder_new_from_resource(resource);
  GtkWidget  *root    = GTK_WIDGET(gtk_builder_get_object(builder, page));

  gtk_stack_add_named(stack, root, name);
  g_hash_table_insert(self->pages, g_strdup(page), builder);
}

static void
stigant_window_finalize(GObject *object)
{
  StigantWindow *self = STIGANT_WINDOW(object);

  g_clear_object(&self->controller);
  g_clear_object(&self->model);
  g_hash_table_unref(self->pages);                               // Free all the builder
  G_OBJECT_CLASS(stigant_window_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_window_class_init(StigantWindowClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_window_finalize;
}

static void
stigant_window_init(StigantWindow *self)
{
  // INIT the window
  gtk_window_set_title(GTK_WINDOW(self), "Stigant");
  gtk_window_set_default_size(GTK_WINDOW(self), 1280, 860);
  gtk_widget_set_size_request(GTK_WIDGET(self), 760, 600);
  gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(self)), "stigant"); // ALL the CSS RULE START BY .stigant because the window have the class stigant

  self->model      = stigant_model_new();
  self->controller = stigant_controller_new(self->model);

  self->pages = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref); // create a dictionary

  /* Loading UI / save widget / add the main widget to the window */
  GtkBuilder *builder = gtk_builder_new_from_resource(UI "layout/stigant-window.ui");
  stigant_window_setup_header_stack(self, builder);
  stigant_window_setup_content_stack(self, builder);
  g_object_unref(builder); // widgets are now owned by the window
}

void
stigant_window_setup_content_stack(StigantWindow *self, GtkBuilder *builder)
{
  self->content_stack = GTK_STACK(gtk_builder_get_object(builder, "content_stack"));
  gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->content_stack));

  GtkWidget *solver = GTK_WIDGET(stigant_solver_new());
  gtk_stack_add_named(self->content_stack, solver, "solver-stack");

  GtkWidget *generate = GTK_WIDGET(stigant_generate_new());
  gtk_stack_add_named(self->content_stack, generate, "gen-stack");
}

void
stigant_window_setup_header_stack(StigantWindow *self, GtkBuilder *builder)
{
  self->header_bar = GTK_WIDGET(gtk_builder_get_object(builder, "header_bar"));
  gtk_window_set_titlebar(GTK_WINDOW(self), self->header_bar);

  self->header_stack = GTK_STACK(gtk_builder_get_object(builder, "header_stack"));

  // add page to thhe stack of Header
  add_page(self, self->header_stack, UI "layout/stigant-mode-switch.ui", "mode_switch", "switch");
  g_signal_connect(
      stigant_window_lookup(self, "mode_switch", "solve_btn"),
      "clicked",
      G_CALLBACK(on_solve_mode_clicked),
      self);
  g_signal_connect(
      stigant_window_lookup(self, "mode_switch", "gen_btn"),
      "clicked",
      G_CALLBACK(on_generate_mode_clicked),
      self);

  g_signal_connect(
      self->model,
      "notify::stigant-mode",
      G_CALLBACK(on_stigant_mode_changed),
      self);

  add_page(self, self->header_stack, UI "layout/stigant-crumb.ui", "crumb", "crumb");
}

StigantWindow *
stigant_window_new(GtkApplication *app)
{
  return g_object_new(STIGANT_TYPE_WINDOW, "application", app, NULL);
}

GObject *
stigant_window_lookup(StigantWindow *self, const char *page, const char *id)
{
  GtkBuilder *builder = g_hash_table_lookup(self->pages, page);
  return builder ? gtk_builder_get_object(builder, id) : NULL;
}

void
on_solve_mode_clicked(GtkButton *button, gpointer user_data)
{
  StigantWindow *self = STIGANT_WINDOW(user_data);
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {
      stigant_controller_switch_solve_mode(self->controller);
    }
}

void
on_generate_mode_clicked(GtkButton *button, gpointer user_data)
{
  StigantWindow *self = STIGANT_WINDOW(user_data);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {
      stigant_controller_switch_generate_mode(self->controller);
    }
}

void
on_stigant_mode_changed(GObject    *object,
                        GParamSpec *pspec,
                        gpointer    user_data)
{
  StigantWindow *self = STIGANT_WINDOW(user_data);
  StigantMode    mode = stigant_model_get_mode(self->model);
  switch (mode)
    {
    case STIGANT_MODE_GENERATE:
      gtk_stack_set_visible_child_name(GTK_STACK(self->content_stack), "gen-stack");
      break;

    case STIGANT_MODE_SOLVE:
      gtk_stack_set_visible_child_name(GTK_STACK(self->content_stack), "solver-stack");
      break;
    }
}