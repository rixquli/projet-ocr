#include "stigant-model.h"

#define UI "/org/stigant/App/ui/"

G_DEFINE_ENUM_TYPE(StigantMode, stigant_mode,
                   G_DEFINE_ENUM_VALUE(STIGANT_MODE_GENERATE, "generate_mode"),
                   G_DEFINE_ENUM_VALUE(STIGANT_MODE_SOLVE, "solve_mode"))
struct _StigantModel
{
  GObject     parent_instance;
  StigantMode mode;
};

enum
{
  PROP_0,
  PROP_MODE,
  N_PROPERTIES
};
static GParamSpec *properties[N_PROPERTIES];

G_DEFINE_FINAL_TYPE(StigantModel, stigant_model, G_TYPE_OBJECT)

static void
stigant_model_finalize(GObject *object)
{
  // StigantModel *self = STIGANT_MODEL(object);
  G_OBJECT_CLASS(stigant_model_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_model_get_property(GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  StigantModel *self = STIGANT_MODEL(object);

  switch (prop_id)
    {
    case PROP_MODE:
      g_value_set_enum(value, self->mode);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
stigant_model_set_property(GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  StigantModel *self = STIGANT_MODEL(object);

  switch (prop_id)
    {
    case PROP_MODE:
      self->mode = g_value_get_enum(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
stigant_model_class_init(StigantModelClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_model_finalize;

  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->get_property = stigant_model_get_property;
  object_class->set_property = stigant_model_set_property;

  properties[PROP_MODE] = g_param_spec_enum("stigant-mode", NULL, NULL, STIGANT_TYPE_MODE, STIGANT_MODE_SOLVE, G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties(
      object_class,
      N_PROPERTIES,
      properties);
}

static void
stigant_model_init(StigantModel *self)
{
}

StigantModel *
stigant_model_new()
{
  return g_object_new(STIGANT_TYPE_MODEL, NULL);
}

void
stigant_model_switch_solve_mode(StigantModel *self)
{
  self->mode = STIGANT_MODE_SOLVE;
  g_object_notify_by_pspec(
      G_OBJECT(self),
      properties[PROP_MODE]);
}

void
stigant_model_switch_generate_mode(StigantModel *self)
{
  self->mode = STIGANT_MODE_GENERATE;
  g_object_notify_by_pspec(
      G_OBJECT(self),
      properties[PROP_MODE]);
}

StigantMode
stigant_model_get_mode(StigantModel *self)
{
  return self->mode;
}