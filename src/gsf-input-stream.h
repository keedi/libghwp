/*
 * gsf-input-stream.h
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

#ifndef _GSF_INPUT_STREAM_H_
#define _GSF_INPUT_STREAM_H_

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GSF_TYPE_INPUT_STREAM             (gsf_input_stream_get_type ())
#define GSF_INPUT_STREAM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSF_TYPE_INPUT_STREAM, GsfInputStream))
#define GSF_INPUT_STREAM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSF_TYPE_INPUT_STREAM, GsfInputStreamClass))
#define GSF_IS_INPUT_STREAM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSF_TYPE_INPUT_STREAM))
#define GSF_IS_INPUT_STREAM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSF_TYPE_INPUT_STREAM))
#define GSF_INPUT_STREAM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSF_TYPE_INPUT_STREAM, GsfInputStreamClass))

typedef struct _GsfInputStream GsfInputStream;
typedef struct _GsfInputStreamClass GsfInputStreamClass;
typedef struct _GsfInputStreamPrivate GsfInputStreamPrivate;

struct _GsfInputStream {
	GInputStream parent_instance;
	GsfInputStreamPrivate * priv;
};

struct _GsfInputStreamClass {
	GInputStreamClass parent_class;
};

struct _GsfInputStreamPrivate {
	GsfInput* input;
};

GType gsf_input_stream_get_type (void) G_GNUC_CONST;
GsfInputStream* gsf_input_stream_new (GsfInput* input);
gssize gsf_input_stream_size (GsfInputStream* self);

G_END_DECLS

#endif /* _GSF_INPUT_STREAM_H_ */