#pragma once
#include <gtk/gtk.h>
#include "stigant-model.h"

#define STIGANT_TYPE_CONTROLLER (stigant_controller_get_type())
G_DECLARE_FINAL_TYPE(StigantController, stigant_controller, STIGANT, CONTROLLER, GObject)

StigantController *stigant_controller_new(StigantModel *model);
void               stigant_controller_switch_generate_mode(StigantController *self);
void               stigant_controller_switch_solve_mode(StigantController *self);
