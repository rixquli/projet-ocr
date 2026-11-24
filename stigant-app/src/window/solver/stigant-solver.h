#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_SOLVER (stigant_solver_get_type())
G_DECLARE_FINAL_TYPE(StigantSolver, stigant_solver, STIGANT, SOLVER, GtkStack)

StigantSolver *stigant_solver_new();
GObject       *stigant_solver_lookup(StigantSolver *self, const char *page, const char *id);