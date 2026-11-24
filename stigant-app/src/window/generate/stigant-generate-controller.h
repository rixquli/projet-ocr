#pragma once
#include <gtk/gtk.h>
#include "stigant-generate-model.h"

#define STIGANT_TYPE_GENERATE_CONTROLLER (stigant_generate_controller_get_type())
G_DECLARE_FINAL_TYPE(StigantGenerateController, stigant_generate_controller, STIGANT, GENERATE_CONTROLLER, GObject)

StigantGenerateController *stigant_generate_controller_new(StigantGenerateModel *model);