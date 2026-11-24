#include "stigant-generate-model.h"

#define UI "/org/stigant/App/ui/"

struct _StigantGenerateModel
{
  GObject parent_instance;
};

enum
{
  PROP_0,
  N_PROPERTIES
};
static GParamSpec *properties[N_PROPERTIES];

G_DEFINE_FINAL_TYPE(StigantGenerateModel, stigant_generate_model, G_TYPE_OBJECT)

static void
stigant_generate_model_finalize(GObject *object)
{
  G_OBJECT_CLASS(stigant_generate_model_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_generate_model_get_property(GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  StigantGenerateModel *self = STIGANT_GENERATE_MODEL(object);

  switch (prop_id)
    {

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
stigant_generate_model_set_property(GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  StigantGenerateModel *self = STIGANT_GENERATE_MODEL(object);

  switch (prop_id)
    {

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
stigant_generate_model_class_init(StigantGenerateModelClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_generate_model_finalize;

  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = stigant_generate_model_get_property;
  object_class->set_property = stigant_generate_model_set_property;
}

static void
stigant_generate_model_init(StigantGenerateModel *self)
{
}

StigantGenerateModel *
stigant_generate_model_new()
{
  return g_object_new(STIGANT_TYPE_GENERATE_MODEL, NULL);
}