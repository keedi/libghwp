/*
 * ghwp-page.c
 *
 * Copyright (C) 2012  Hodong Kim <cogniti@gmail.com>
 * 
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ghwp-page.h"
#include "ghwp-document.h"

G_DEFINE_TYPE (GHWPPage, ghwp_page, G_TYPE_OBJECT);

static gboolean ghwp_page_draw_page (cairo_t* cr, GArray* elements);
static void ghwp_page_finalize (GObject* obj);

void ghwp_page_get_size (GHWPPage* self,
                         gdouble*  width,
                         gdouble*  height)
{
    g_return_if_fail (self != NULL);
    *width  = 595.0;
    *height = 842.0;
}


gboolean ghwp_page_render (GHWPPage* self, cairo_t* cr) {
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (cr != NULL, FALSE);
    cairo_save (cr);
    ghwp_page_draw_page (cr, self->elements);
    cairo_restore (cr);
    return TRUE;
}


#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library ft_lib;
static FT_Face    ft_face;
/*한 번만 초기화, 로드*/
static void
once_ft_init_and_new (void)
{
    static gsize ft_init = 0;

    if (g_once_init_enter (&ft_init)) {

        FT_Init_FreeType (&ft_lib);
        FT_New_Face (ft_lib,
                    "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
                     0,
                    &ft_face);

        g_once_init_leave (&ft_init, (gsize)1);
    }
}

static gboolean ghwp_page_draw_page (cairo_t* cr, GArray* elements) {
    g_return_val_if_fail (cr != NULL, FALSE);
    g_return_val_if_fail (elements != NULL, FALSE);

    gint      i, j;
    TextSpan *textspan;

    cairo_glyph_t        *glyphs = NULL; /* NULL로 지정하면 자동 할당됨 */
    int                   num_glyphs;
    cairo_scaled_font_t  *scaled_font;
    cairo_font_face_t    *font_face;
    cairo_matrix_t        font_matrix;
    cairo_matrix_t        ctm;
    cairo_font_options_t *font_options;
    cairo_text_extents_t  extents;

    gchar *ch;
    double x = 20.0;
    double y = 40.0;

    once_ft_init_and_new(); /*한 번만 초기화, 로드*/

    font_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);

    cairo_matrix_init_identity (&font_matrix);
    cairo_matrix_scale (&font_matrix, 12.0, 12.0);
    cairo_get_matrix (cr, &ctm);

    font_options = cairo_font_options_create ();
    cairo_get_font_options (cr, font_options);
    scaled_font = cairo_scaled_font_create (font_face,
                                           &font_matrix, &ctm, font_options);
    cairo_font_options_destroy (font_options);
    cairo_set_scaled_font(cr, scaled_font); /* 요 문장 없으면 fault 떨어짐 */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

    for (i = 0; i < elements->len; i++)
    {
        textspan = g_array_index (elements, TextSpan*, (guint) i);

        x = 20.0;

        for (j = 0; j < g_utf8_strlen(textspan->text, -1); j++) {
            ch = g_utf8_substring(textspan->text, j, j+1);

            cairo_scaled_font_text_to_glyphs (scaled_font,
                                              x, y, /* x, y 좌표 */
                                              ch, -1,
                                             &glyphs, &num_glyphs,
                                              NULL, NULL, NULL);
            g_free(ch);
            cairo_glyph_extents(cr, glyphs, num_glyphs, &extents);

            if (x >= 595.0 - extents.x_advance - 20.0) {
                glyphs[0].x = 20.0;
                glyphs[0].y += 16.0;
                x = 20.0 + extents.x_advance;
                y = y + 16.0;
            }
            else {
                x = x + extents.x_advance;
            }

            cairo_show_glyphs (cr, glyphs, num_glyphs);
        }
        y = y + 18.0;

    }
    cairo_glyph_free (glyphs);
    cairo_scaled_font_destroy (scaled_font);
    return TRUE;
}


GHWPPage* ghwp_page_new (void) {
	return (GHWPPage*) g_object_new (GHWP_TYPE_PAGE, NULL);
}


static void ghwp_page_class_init (GHWPPageClass * klass) {
	ghwp_page_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = ghwp_page_finalize;
}


static void ghwp_page_init (GHWPPage * self) {
    self->elements = g_array_new (TRUE, TRUE, sizeof (GObject*));
}


static void ghwp_page_finalize (GObject* obj)
{
	GHWPPage * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_PAGE, GHWPPage);
	g_array_free (self->elements, TRUE);
	G_OBJECT_CLASS (ghwp_page_parent_class)->finalize (obj);
}