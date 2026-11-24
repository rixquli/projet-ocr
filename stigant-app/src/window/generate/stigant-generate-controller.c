#include "stigant-generate-controller.h"
#include "stigant-generate-model.h"

#define UI "/org/stigant/App/ui/"

struct _StigantGenerateController
{
  GObject               parent_instance;
  StigantGenerateModel *model;
};

G_DEFINE_FINAL_TYPE(StigantGenerateController, stigant_generate_controller, G_TYPE_OBJECT)

static void
stigant_generate_controller_finalize(GObject *object)
{
  StigantGenerateController *self = STIGANT_GENERATE_CONTROLLER(object);
  g_clear_object(&self->model);
  G_OBJECT_CLASS(stigant_generate_controller_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_generate_controller_class_init(StigantGenerateControllerClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_generate_controller_finalize;
}

static void
stigant_generate_controller_init(StigantGenerateController *self)
{
}

StigantGenerateController *
stigant_generate_controller_new(StigantGenerateModel *model)
{
  StigantGenerateController *self = g_object_new(STIGANT_TYPE_GENERATE_CONTROLLER, NULL);
  self->model                     = g_object_ref(model); // the controller keeps its own reference
  return self;
}