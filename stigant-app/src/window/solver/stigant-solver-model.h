#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_SOLVER_MODEL (stigant_solver_model_get_type())
G_DECLARE_FINAL_TYPE(StigantSolverModel, stigant_solver_model, STIGANT, SOLVER_MODEL, GObject)

StigantSolverModel *stigant_solver_model_new();
