#include "stigant-grid-builder.h"

#define N_COLORS 8

GtkWidget *
stigant_grid_builder_new (const char *cells, int size)
{
  GtkWidget *grid = gtk_grid_new ();
  GtkStyleContext *ctx = gtk_widget_get_style_context (grid);

  gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
  gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (grid, GTK_ALIGN_CENTER);

  gtk_style_context_add_class (ctx, "letter-grid");
  if (size >= 14)
    gtk_style_context_add_class (ctx, "dense"); /* cases plus petites */

  for (int r = 0; r < size; r++)
    for (int c = 0; c < size; c++)
      {
        char txt[2] = { cells[r * size + c], '\0' };
        GtkWidget *cell = gtk_label_new (txt);
        gtk_grid_attach (GTK_GRID (grid), cell, c, r, 1, 1);
      }

  gtk_widget_show_all (grid);
  return grid;
}

void
stigant_grid_builder_mark_word (GtkWidget *grid, int row, int col, int drow,
                                int dcol, int length, int color)
{
  char cls[8];
  g_snprintf (cls, sizeof cls, "c%d", color % N_COLORS);

  for (int k = 0; k < length; k++)
    {
      GtkWidget *cell = gtk_grid_get_child_at (GTK_GRID (grid), col + dcol * k,
                                               row + drow * k);
      if (!cell)
        continue;
      GtkStyleContext *ctx = gtk_widget_get_style_context (cell);
      gtk_style_context_add_class (ctx, "hit");
      gtk_style_context_add_class (ctx, cls);
    }
}

void
stigant_grid_builder_set_solved (GtkWidget *grid, gboolean solved)
{
  GtkStyleContext *ctx = gtk_widget_get_style_context (grid);

  if (solved)
    gtk_style_context_add_class (ctx, "solved");
  else
    gtk_style_context_remove_class (ctx, "solved");
}
