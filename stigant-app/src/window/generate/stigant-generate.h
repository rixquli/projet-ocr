#pragma once
#include <gtk/gtk.h>

#define STIGANT_TYPE_GENERATE (stigant_generate_get_type())
G_DECLARE_FINAL_TYPE(StigantGenerate, stigant_generate, STIGANT, GENERATE, GtkStack)

StigantGenerate *stigant_generate_new();
void             on_generate_clicked(GtkButton *button, gpointer user_data);
GObject         *stigant_generate_lookup(StigantGenerate *self, const char *page, const char *id);
GtkRadioButton  *resolve_radio(GtkRadioButton *master_radio);
void             stigant_generate_add_word(StigantGenerate *self, const char *word);
GList           *stigant_generate_get_words(StigantGenerate *self);
void             on_word_entry_activate(GtkEntry *entry, gpointer user_data);
void             on_sugg_clicked(GtkButton *button, gpointer user_data);
void             on_change_level(GtkRadioButton *button, gpointer user_data);