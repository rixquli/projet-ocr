#include "stigant-solver-controller.h"
#include "stigant-solver-model.h"

#define UI "/org/stigant/App/ui/"

struct _StigantSolverController
{
  GObject             parent_instance;
  StigantSolverModel *model;
};

G_DEFINE_FINAL_TYPE(StigantSolverController, stigant_solver_controller, G_TYPE_OBJECT)

static void
stigant_solver_controller_finalize(GObject *object)
{
  StigantSolverController *self = STIGANT_SOLVER_CONTROLLER(object);
  g_clear_object(&self->model);
  G_OBJECT_CLASS(stigant_solver_controller_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_solver_controller_class_init(StigantSolverControllerClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_solver_controller_finalize;
}

static void
stigant_solver_controller_init(StigantSolverController *self)
{
}

StigantSolverController *
stigant_solver_controller_new(StigantSolverModel *model)
{
  StigantSolverController *self = g_object_new(STIGANT_TYPE_SOLVER_CONTROLLER, NULL);
  self->model                   = g_object_ref(model); // the controller keeps its own reference
  return self;
}