#pragma once
#include <gtk/gtk.h>
#include "stigant-solver-model.h"

#define STIGANT_TYPE_SOLVER_CONTROLLER (stigant_solver_controller_get_type())
G_DECLARE_FINAL_TYPE(StigantSolverController, stigant_solver_controller, STIGANT, SOLVER_CONTROLLER, GObject)

StigantSolverController *stigant_solver_controller_new(StigantSolverModel *model);