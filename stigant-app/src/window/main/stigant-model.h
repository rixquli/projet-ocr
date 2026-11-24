#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_MODEL (stigant_model_get_type())
G_DECLARE_FINAL_TYPE(StigantModel, stigant_model, STIGANT, MODEL, GObject)

typedef enum
{
  STIGANT_MODE_GENERATE,
  STIGANT_MODE_SOLVE
} StigantMode;

#define STIGANT_TYPE_MODE (stigant_mode_get_type())
GType stigant_mode_get_type(void);

StigantModel *stigant_model_new(void);
void          stigant_model_switch_generate_mode(StigantModel *self);
void          stigant_model_switch_solve_mode(StigantModel *self);
StigantMode   stigant_model_get_mode(StigantModel *self);
