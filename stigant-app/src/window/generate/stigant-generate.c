#include "stigant-generate-model.h"
#include "stigant-generate-controller.h"
#include "stigant-generate.h"

#define UI "/org/stigant/App/ui/"
#define MAX_WORDS 20

struct _StigantGenerate
{

  GtkStack parent_instance;

  GHashTable                *pages;
  StigantGenerateModel      *model;
  StigantGenerateController *controller;
};

G_DEFINE_FINAL_TYPE(StigantGenerate, stigant_generate, GTK_TYPE_STACK)

static void
add_page(StigantGenerate *self, const char *resource,
         const char *page, const char *name)
{
  GtkBuilder *builder = gtk_builder_new_from_resource(resource);
  GtkWidget  *root    = GTK_WIDGET(gtk_builder_get_object(builder, page));

  gtk_stack_add_named(GTK_STACK(self), root, name);
  g_hash_table_insert(self->pages, g_strdup(page), builder);
}

static void
stigant_generate_finalize(GObject *object)
{
  StigantGenerate *self = STIGANT_GENERATE(object);

  g_clear_object(&self->controller);
  g_clear_object(&self->model);
  g_hash_table_unref(self->pages);                                 // Free all the builder
  G_OBJECT_CLASS(stigant_generate_parent_class)->finalize(object); // let the parent free itself
}

static void
stigant_generate_class_init(StigantGenerateClass *klass)
{
  G_OBJECT_CLASS(klass)->finalize = stigant_generate_finalize;
}

static void
stigant_generate_init(StigantGenerate *self)
{
  self->model      = stigant_generate_model_new();
  self->controller = stigant_generate_controller_new(self->model);

  self->pages = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref); // create a dictionary

  add_page(self, UI "generator/stigant-gen-home.ui", "gen_home", "gen-home");
  add_page(self, UI "generator/stigant-gen-result.ui", "gen_result", "gen-result");

  g_signal_connect(
      stigant_generate_lookup(self, "gen_home", "diff_easy"),
      "clicked",
      G_CALLBACK(on_change_level),
      self);
  g_signal_connect(
      stigant_generate_lookup(self, "gen_home", "diff_medium"),
      "clicked",
      G_CALLBACK(on_change_level),
      self);

  g_signal_connect(
      stigant_generate_lookup(self, "gen_home", "diff_hard"),
      "clicked",
      G_CALLBACK(on_change_level),
      self);

  // Binding function

  // Validate Generation
  g_signal_connect(
      stigant_generate_lookup(self, "gen_home", "gen_btn"),
      "clicked",
      G_CALLBACK(on_generate_clicked),
      self);

  // Generation Word selection
  g_signal_connect(
      stigant_generate_lookup(self, "gen_home", "word_entry"),
      "activate",
      G_CALLBACK(on_word_entry_activate),
      self);

  // Generation Word preset button
  GList *suggs = gtk_container_get_children(GTK_CONTAINER(stigant_generate_lookup(self, "gen_home", "sugg_box")));
  for (GList *it = suggs; it != NULL; it = it->next)
    {
      g_signal_connect(it->data, "clicked", G_CALLBACK(on_sugg_clicked), self);
    }
  g_list_free(suggs);
}

static GList *
get_word_chips(StigantGenerate *self)
{
  return gtk_container_get_children(
      GTK_CONTAINER(stigant_generate_lookup(self, "gen_home", "tags_box")));
}

static guint
count_words(StigantGenerate *self)
{
  GList *chips = get_word_chips(self);
  guint  n     = g_list_length(chips);
  g_list_free(chips);
  return n;
}

static gboolean
has_word(StigantGenerate *self, const char *word)
{
  gboolean found = FALSE;
  GList   *chips = get_word_chips(self);

  for (GList *it = chips; it != NULL && !found; it = it->next)
    {
      GtkWidget  *chip = gtk_bin_get_child(GTK_BIN(it->data));
      const char *w    = g_object_get_data(G_OBJECT(chip), "word");
      found            = (w && g_strcmp0(w, word) == 0);
    }

  g_list_free(chips);
  return found;
}

static void
on_chip_remove_clicked(GtkButton *button, gpointer user_data)
{
  StigantGenerate *self = STIGANT_GENERATE(user_data);
  GtkWidget       *chip = g_object_get_data(G_OBJECT(button), "chip");
  gtk_widget_destroy(gtk_widget_get_parent(chip));
}

void
stigant_generate_add_word(StigantGenerate *self, const char *raw)
{
  // Clean Text
  char *trimmed = g_strstrip(g_strdup(raw));
  char *word    = g_utf8_strup(trimmed, -1);
  g_free(trimmed);

  if (*word == '\0' || has_word(self, word) || count_words(self) >= MAX_WORDS)
    {
      g_free(word);
      return;
    }

  // Create the widget
  GtkWidget *chip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_style_context_add_class(gtk_widget_get_style_context(chip), "tag");
  gtk_widget_set_halign(chip, GTK_ALIGN_START);

  GtkWidget *label = gtk_label_new(word);
  gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

  GtkWidget *close = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(close), GTK_RELIEF_NONE);
  gtk_button_set_image(GTK_BUTTON(close),
                       gtk_image_new_from_resource("/org/stigant/App/icons/stigant-close-sm.svg"));
  gtk_widget_set_valign(close, GTK_ALIGN_CENTER);
  gtk_widget_set_tooltip_text(close, "Retirer");

  gtk_box_pack_start(GTK_BOX(chip), label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(chip), close, FALSE, FALSE, 0);

  g_object_set_data_full(G_OBJECT(chip), "word", word, g_free);
  g_object_set_data(G_OBJECT(close), "chip", chip);
  g_signal_connect(close, "clicked", G_CALLBACK(on_chip_remove_clicked), self);

  gtk_container_add(GTK_CONTAINER(stigant_generate_lookup(self, "gen_home", "tags_box")), chip);
  gtk_widget_show_all(chip);
}

void
on_word_entry_activate(GtkEntry *entry, gpointer user_data)
{
  StigantGenerate *self  = STIGANT_GENERATE(user_data);
  char           **parts = g_strsplit_set(gtk_entry_get_text(entry), ", ;", -1);

  for (char **p = parts; *p != NULL; p++)
    {
      stigant_generate_add_word(self, *p);
    }

  g_strfreev(parts);
  gtk_entry_set_text(entry, "");
}

void
on_sugg_clicked(GtkButton *button, gpointer user_data)
{
  stigant_generate_add_word(STIGANT_GENERATE(user_data), gtk_widget_get_name(GTK_WIDGET(button)));
}

GList *
stigant_generate_get_words(StigantGenerate *self)
{
  GList *words = NULL;
  GList *chips = get_word_chips(self);

  for (GList *it = chips; it != NULL; it = it->next)
    words = g_list_append(words, g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(it->data))), "word"));

  g_list_free(chips);
  return words;
}

void
on_generate_clicked(GtkButton *button, gpointer user_data)
{
  StigantGenerate *self = STIGANT_GENERATE(user_data);
  gtk_revealer_set_reveal_child(
      GTK_REVEALER(stigant_generate_lookup(self, "gen_home", "err_revealer")),
      count_words(self) < 3);
  if (count_words(self) < 3)
    {
      return;
    }

  GtkRadioButton *active_level_radio = resolve_radio(GTK_RADIO_BUTTON(stigant_generate_lookup(self, "gen_home", "diff_easy")));
  GtkRadioButton *active_size_radio  = resolve_radio(GTK_RADIO_BUTTON(stigant_generate_lookup(self, "gen_home", "size_10")));

  if (active_level_radio)
    {
      g_print(
          "Niveau : %s\n",
          gtk_button_get_label(GTK_BUTTON(active_level_radio)));
    }
  g_print(
      "Titre : %s\n",
      gtk_entry_get_text(GTK_ENTRY(stigant_generate_lookup(self, "gen_home", "title_entry"))));
  if (active_size_radio)
    {
      g_print(
          "Size : %s\n",
          gtk_button_get_label(GTK_BUTTON(active_size_radio)));
    }

  GList *words = stigant_generate_get_words(self);
  g_print("Mots :");
  for (GList *it = words; it != NULL; it = it->next)
    g_print(" %s", (const char *) it->data);
  g_print("\n");

  gtk_revealer_set_reveal_child(
      GTK_REVEALER(stigant_generate_lookup(self, "gen_home", "err_revealer")),
      g_list_length(words) < 3);
  g_list_free(words);
}

void
on_change_level(GtkRadioButton *button, gpointer user_data)
{
  StigantGenerate *self  = STIGANT_GENERATE(user_data);
  gint             level = atoi(
      gtk_widget_get_name(GTK_WIDGET(button)));

  switch (level)
    {
    case 0:
      gtk_label_set_label(
          GTK_LABEL(stigant_generate_lookup(self, "gen_home", "diff_hint")),
          "Horizontal et vertical uniquement");
      break;

    case 1:
      gtk_label_set_label(
          GTK_LABEL(stigant_generate_lookup(self, "gen_home", "diff_hint")),
          "Horizontal, vertical et diagonales");
      break;

    case 2:
      gtk_label_set_label(
          GTK_LABEL(stigant_generate_lookup(self, "gen_home", "diff_hint")),
          "Tous les sens, y compris à l'envers");
      break;
    }
}

GtkRadioButton *
resolve_radio(GtkRadioButton *master_radio)
{
  GSList *group = gtk_radio_button_get_group(master_radio);

  for (GSList *item = group; item != NULL; item = item->next)
    {
      GtkRadioButton *radio = GTK_RADIO_BUTTON(item->data);

      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
        return radio;
    }

  return NULL;
}

StigantGenerate *
stigant_generate_new()
{
  return g_object_new(STIGANT_TYPE_GENERATE, NULL);
}

GObject *
stigant_generate_lookup(StigantGenerate *self, const char *page, const char *id)
{
  GtkBuilder *builder = g_hash_table_lookup(self->pages, page);
  return builder ? gtk_builder_get_object(builder, id) : NULL;
}