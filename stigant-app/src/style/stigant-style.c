#include "stigant-style.h"
#include <fontconfig/fontconfig.h>

static const char *FONTS[] = {
  "Inter-Regular.ttf",
  "Inter-Medium.ttf",
  "InterDisplay-Medium.ttf",
};

static const char *CSS_FILES[] = {
  "base.css",
  "header.css",
  "solver.css",
  "generator.css",
};

// From documentation
static void
load_fonts(void)
{
  for (guint i = 0; i < G_N_ELEMENTS(FONTS); i++)
    {
      char *path = g_build_filename(FONT_DIR, FONTS[i], NULL);
      if (!FcConfigAppFontAddFile(FcConfigGetCurrent(), (const FcChar8 *) path))
        g_warning("Police introuvable : %s", path);
      g_free(path);
    }
}

// From documentation
static void
load_css(void)
{
  GdkScreen *screen = gdk_screen_get_default();

  for (guint i = 0; i < G_N_ELEMENTS(CSS_FILES); i++)
    {
      char           *res      = g_strconcat("/org/stigant/App/style/", CSS_FILES[i], NULL);
      GtkCssProvider *provider = gtk_css_provider_new();

      gtk_css_provider_load_from_resource(provider, res);
      gtk_style_context_add_provider_for_screen(screen,
                                                GTK_STYLE_PROVIDER(provider),
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
      g_object_unref(provider);
      g_free(res);
    }
}

void
stigant_style_load(void)
{
  load_fonts();
  load_css();
}
