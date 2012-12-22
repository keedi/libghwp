/*
 * ghwp-page.h
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

#ifndef _GHWP_PAGE_H_
#define _GHWP_PAGE_H_

#include <glib-object.h>

#include <cairo.h>

G_BEGIN_DECLS

#define GHWP_TYPE_PAGE             (ghwp_page_get_type ())
#define GHWP_PAGE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PAGE, GHWPPage))
#define GHWP_PAGE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PAGE, GHWPPageClass))
#define GHWP_IS_PAGE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PAGE))
#define GHWP_IS_PAGE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PAGE))
#define GHWP_PAGE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PAGE, GHWPPageClass))

typedef struct _GHWPPage GHWPPage;
typedef struct _GHWPPageClass GHWPPageClass;
typedef struct _GHWPPagePrivate GHWPPagePrivate;

struct _GHWPPage {
	GObject parent_instance;
	GHWPPagePrivate * priv;
	GArray* elements;
};

struct _GHWPPageClass {
	GObjectClass parent_class;
};

GType ghwp_page_get_type (void) G_GNUC_CONST;

GHWPPage* ghwp_page_new (void);
void ghwp_page_get_size (GHWPPage* self, gdouble* width, gdouble* height);
gboolean ghwp_page_render (GHWPPage* self, cairo_t* cr);

G_END_DECLS

#endif /* _GHWP_PAGE_H_ */