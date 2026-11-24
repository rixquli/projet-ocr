#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_GENERATE_MODEL (stigant_generate_model_get_type())
G_DECLARE_FINAL_TYPE(StigantGenerateModel, stigant_generate_model, STIGANT, GENERATE_MODEL, GObject)

StigantGenerateModel *stigant_generate_model_new();
