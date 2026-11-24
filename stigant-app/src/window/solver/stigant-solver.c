#include "stigant-solver-model.h"
#include "stigant-solver-controller.h"
#include "stigant-solver.h"

#define UI "/org/stigant/App/ui/"
#define MAX_WORDS 20

struct _StigantSolver
{

  GtkStack parent_instance;

  GHashTable              *pages;
  StigantSolverModel      *model;
  StigantSolverController *controller;
};

G_DEFINE_FINAL_TYPE(StigantSolver, stigant_solver, GTK_TYPE_STACK)

static void
add_page(StigantSolver *self, const char *resource,
         const char *page, const char *name)
{
  GtkBuilder *builder = gtk_builder_new_from_resource(resource);
  GtkWidget  *root    = GTK_WIDGET(gtk_builder_get_object(builder, page));

  gtk_stack_add_named(GTK_STACK(self), root, name);
  g_hash_table_insert(self->pages, g_strdup(page), builder);
}

static void
stigant_solver_finalize(GObject *object)
{
  StigantSolver *self = STIGANT_SOLVER(object);

  g_clear_object(&self->controller);
  g_clear_object(&self->model);
  g_hash_table_unref(self->pages);                               // Free all the builder
  G_OBJECT_CLASS(stigant_solver_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_solver_class_init(StigantSolverClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_solver_finalize;
}

static void
stigant_solver_init(StigantSolver *self)
{
  self->model      = stigant_solver_model_new();
  self->controller = stigant_solver_controller_new(self->model);

  self->pages = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref); // create a dictionary

  add_page(self, UI "solver/stigant-solve-home.ui", "solve_home", "solve-home");
  add_page(self, UI "solver/stigant-solve-work.ui", "solve_work", "solve-work");
}

StigantSolver *
stigant_solver_new()
{
  return g_object_new(STIGANT_TYPE_SOLVER, NULL);
}

GObject *
stigant_solver_lookup(StigantSolver *self, const char *page, const char *id)
{
  GtkBuilder *builder = g_hash_table_lookup(self->pages, page);
  return builder ? gtk_builder_get_object(builder, id) : NULL;
}