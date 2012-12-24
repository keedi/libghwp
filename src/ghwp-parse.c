/*
 * ghwp-parse.c
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
#include <glib-object.h>
#include <gio/gio.h>

#include "ghwp-parse.h"

G_DEFINE_TYPE (GHWPContext, ghwp_context, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

#define GHWP_CONTEXT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_CONTEXT, GHWPContextPrivate))

gboolean ghwp_context_decode_header (GHWPContext *self,
                                     guchar      *buf,
                                     int          buf_len);
static void ghwp_context_finalize (GObject* obj);

GHWPContext* ghwp_context_new (GInputStream* stream)
{
    g_return_val_if_fail (stream != NULL, NULL);
    GHWPContext *self = (GHWPContext*) g_object_new (GHWP_TYPE_CONTEXT, NULL);
    self->priv->stream = g_object_ref (stream);
    return self;
}


gboolean ghwp_context_decode_header (GHWPContext *self,
                                     guchar      *buf,
                                     int          buf_len)
{
    GError *error = NULL;
    g_return_val_if_fail (self != NULL, FALSE);

    self->priv->header = (guint32) ((buf[3] << 24) |
                                    (buf[2] << 16) |
                                    (buf[1] <<  8) |
                                     buf[0]);

    self->tag_id = (guint16) ( self->priv->header        & 0x3ff);
    self->level  = (guint16) ((self->priv->header >> 10) & 0x3ff);
    self->size   = (guint32) ((self->priv->header >> 20) & 0xfff);

    if (self->size == ((guint32) 0xfff)) {

        g_input_stream_read_all (self->priv->stream,
                                 (void*)buf, (gsize) buf_len,
                                 &self->priv->bytes_read, NULL, &error);
        if (error != NULL) {
            g_critical ("file %s: line %d: uncaught error: %s (%s, %d)",
                __FILE__, __LINE__, error->message,
                g_quark_to_string (error->domain), error->code);
            g_clear_error (&error);
            return FALSE;
        }

        if (self->priv->bytes_read <= ((gsize) 0)) {
            return FALSE;
        }

        self->size = (guint32) ((buf[3] << 24) |
                                (buf[2] << 16) |
                                (buf[1] <<  8) |
                                 buf[0]);
    }

    return TRUE;
}

gboolean ghwp_context_pull (GHWPContext* self)
{
    g_return_val_if_fail (self != NULL, FALSE);

    GError * error = NULL;

    /* 4바이트 읽기 */
    g_input_stream_read_all (self->priv->stream, (void*) self->priv->buf,
                            (gsize) self->priv->buf_len,
                            &self->priv->bytes_read, NULL, &error);

    if (error != NULL) {
        g_critical ("file %s: line %d: uncaught error: %s (%s, %d)",
            __FILE__, __LINE__, error->message,
            g_quark_to_string (error->domain), error->code);
        g_clear_error (&error);
        return FALSE;
    }

    if (self->priv->bytes_read <= ((gsize) 0)) {
        return FALSE;
    }

    /* 4바이트 헤더 디코딩하기 */
    self->priv->ret = ghwp_context_decode_header (self,
                                                  self->priv->buf,
                                                  self->priv->buf_len);
    /* TODO self->priv->ret 대신에 GError을 사용하여 처리할 것 */
    if (self->priv->ret == FALSE) {
        return FALSE;
    }

    /* data 가져오기 */
    self->data = (g_free (self->data), NULL);
    self->data = g_new0 (guchar, self->size);

    g_input_stream_read_all (self->priv->stream, (void*) self->data,
                             (gsize) self->size, &self->priv->bytes_read,
                             NULL, &error);

    if (error != NULL) {
        self->data = (g_free (self->data), NULL);
        g_critical ("file %s: line %d: uncaught error: %s (%s, %d)",
            __FILE__, __LINE__, error->message,
            g_quark_to_string (error->domain), error->code);
        g_clear_error (&error);
        return FALSE;
    }

    if (self->priv->bytes_read <= ((gsize) 0)) {
        self->data = (g_free (self->data), NULL);
        return FALSE;
    }

    self->data_len = self->size;
    return TRUE;
}


static void ghwp_context_class_init (GHWPContextClass * klass) {
    ghwp_context_parent_class = g_type_class_peek_parent (klass);
    g_type_class_add_private (klass, sizeof (GHWPContextPrivate));
    G_OBJECT_CLASS (klass)->finalize = ghwp_context_finalize;
}


static void ghwp_context_init (GHWPContext * self)
{
    self->priv = GHWP_CONTEXT_GET_PRIVATE (self);
    self->priv->buf = g_new0 (guchar, 4);
    self->priv->buf_len = 4;
}


static void ghwp_context_finalize (GObject* obj) {
    GHWPContext * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_CONTEXT, GHWPContext);
    _g_object_unref0 (self->priv->stream);
    self->priv->buf = (g_free (self->priv->buf), NULL);
    self->data = (g_free (self->data), NULL);
    G_OBJECT_CLASS (ghwp_context_parent_class)->finalize (obj);
}
