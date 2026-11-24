#pragma once
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *stigant_grid_builder_new (const char *cells, int size);

void stigant_grid_builder_mark_word (GtkWidget *grid, int row, int col,
                                     int drow, int dcol, int length,
                                     int color);

void stigant_grid_builder_set_solved (GtkWidget *grid, gboolean solved);

G_END_DECLS
