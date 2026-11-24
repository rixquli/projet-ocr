#include "stigant-controller.h"
#include "stigant-model.h"

#define UI "/org/stigant/App/ui/"

struct _StigantController
{
  GObject       parent_instance;
  StigantModel *model;
};

G_DEFINE_FINAL_TYPE(StigantController, stigant_controller, G_TYPE_OBJECT)

static void
stigant_controller_finalize(GObject *object)
{
  StigantController *self = STIGANT_CONTROLLER(object);
  g_clear_object(&self->model);
  G_OBJECT_CLASS(stigant_controller_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_controller_class_init(StigantControllerClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_controller_finalize;
}

static void
stigant_controller_init(StigantController *self)
{
}

StigantController *
stigant_controller_new(StigantModel *model)
{
  StigantController *self = g_object_new(STIGANT_TYPE_CONTROLLER, NULL);
  self->model             = g_object_ref(model); // the controller keeps its own reference
  return self;
}

void
stigant_controller_switch_solve_mode(StigantController *self)
{
  stigant_model_switch_solve_mode(self->model);
}

void
stigant_controller_switch_generate_mode(StigantController *self)
{
  stigant_model_switch_generate_mode(self->model);
}