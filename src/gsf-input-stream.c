/*
 * gsf-input-stream.c
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

#include <glib.h>
#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-infile-msole.h>

#include "gsf-input-stream.h"

G_DEFINE_TYPE (GsfInputStream, gsf_input_stream, G_TYPE_INPUT_STREAM);

static gssize gsf_input_stream_real_read (GInputStream *base,
                                          void         *buffer,
                                          gsize         buffer_len,
                                          GCancellable *cancellable,
                                          GError      **error);
static gboolean gsf_input_stream_real_close (GInputStream *base,
                                             GCancellable *cancellable,
                                             GError      **error);
static void gsf_input_stream_finalize (GObject* obj);

GsfInputStream*
gsf_input_stream_new (GsfInput* input)
{
	g_return_val_if_fail (input != NULL, NULL);
	GsfInputStream *self;
	self = (GsfInputStream*) g_object_new (GSF_TYPE_INPUT_STREAM, NULL);
	self->priv->input = g_object_ref (input);
	return self;
}

static gssize gsf_input_stream_real_read (GInputStream *base,
                                          void         *buffer,
                                          gsize         buffer_len,
                                          GCancellable *cancellable,
                                          GError      **error)
{
	GsfInputStream *self = (GsfInputStream*) base;
	gint64 stamp = gsf_input_remaining (self->priv->input);

	if (gsf_input_remaining (self->priv->input) < ((gint64) buffer_len)) {
		gsf_input_read (self->priv->input,
					    (gsize) gsf_input_remaining (self->priv->input),
					    buffer);
	} else {
		gsf_input_read (self->priv->input, (gsize) buffer_len, buffer);
	}
	return (gssize) (stamp - gsf_input_remaining (self->priv->input));
}

static gboolean
gsf_input_stream_real_close (GInputStream* base,
                             GCancellable* cancellable,
                             GError**      error)
{
	/* pseudo TRUE */
	return TRUE;
}

gssize gsf_input_stream_size (GsfInputStream* self) {
	g_return_val_if_fail (self != NULL, 0L);
	return (gssize) gsf_input_size (self->priv->input);
}

static void
gsf_input_stream_class_init (GsfInputStreamClass * klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GInputStreamClass* parent_class = G_INPUT_STREAM_CLASS (klass);
	g_type_class_add_private (klass, sizeof (GsfInputStreamPrivate));
	parent_class->read_fn  = gsf_input_stream_real_read;
	parent_class->close_fn = gsf_input_stream_real_close;
	object_class->finalize = gsf_input_stream_finalize;
}

static void
gsf_input_stream_finalize (GObject* obj)
{
	GsfInputStream * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, GSF_TYPE_INPUT_STREAM, GsfInputStream);
	g_object_unref (self->priv->input);
	G_OBJECT_CLASS (gsf_input_stream_parent_class)->finalize (obj);
}

static void
gsf_input_stream_init (GsfInputStream *gsf_input_stream)
{
	gsf_input_stream->priv = G_TYPE_INSTANCE_GET_PRIVATE (gsf_input_stream,
	                                                      GSF_TYPE_INPUT_STREAM,
	                                                      GsfInputStreamPrivate);
}