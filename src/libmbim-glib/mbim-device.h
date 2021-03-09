/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * libmbim-glib -- GLib/GIO based library to control MBIM devices
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2013 - 2014 Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef _LIBMBIM_GLIB_MBIM_DEVICE_H_
#define _LIBMBIM_GLIB_MBIM_DEVICE_H_

#if !defined (__LIBMBIM_GLIB_H_INSIDE__) && !defined (LIBMBIM_GLIB_COMPILATION)
#error "Only <libmbim-glib.h> can be included directly."
#endif

#include <glib-object.h>
#include <gio/gio.h>

#include "mbim-message.h"

G_BEGIN_DECLS

/**
 * SECTION:mbim-device
 * @title: MbimDevice
 * @short_description: Generic MBIM device handling routines
 *
 * #MbimDevice is a generic type in charge of controlling the access to the
 * managed MBIM port.
 *
 * A #MbimDevice can only handle one single MBIM port.
 */

#define MBIM_TYPE_DEVICE            (mbim_device_get_type ())
#define MBIM_DEVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MBIM_TYPE_DEVICE, MbimDevice))
#define MBIM_DEVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  MBIM_TYPE_DEVICE, MbimDeviceClass))
#define MBIM_IS_DEVICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MBIM_TYPE_DEVICE))
#define MBIM_IS_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  MBIM_TYPE_DEVICE))
#define MBIM_DEVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  MBIM_TYPE_DEVICE, MbimDeviceClass))

typedef struct _MbimDevice MbimDevice;
typedef struct _MbimDeviceClass MbimDeviceClass;
typedef struct _MbimDevicePrivate MbimDevicePrivate;

/**
 * MBIM_DEVICE_FILE:
 *
 * Symbol defining the #MbimDevice:device-file property.
 *
 * Since: 1.0
 */
#define MBIM_DEVICE_FILE "device-file"

/**
 * MBIM_DEVICE_TRANSACTION_ID:
 *
 * Symbol defining the #MbimDevice:device-transaction-id property.
 *
 * Since: 1.2
 */
#define MBIM_DEVICE_TRANSACTION_ID "device-transaction-id"

/**
 * MBIM_DEVICE_IN_SESSION:
 *
 * Symbol defining the #MbimDevice:device-in-session property.
 *
 * Since: 1.4
 */
#define MBIM_DEVICE_IN_SESSION "device-in-session"

/**
 * MBIM_DEVICE_SIGNAL_INDICATE_STATUS:
 *
 * Symbol defining the #MbimDevice::device-indicate-status signal.
 *
 * Since: 1.0
 */
#define MBIM_DEVICE_SIGNAL_INDICATE_STATUS "device-indicate-status"

/**
 * MBIM_DEVICE_SIGNAL_ERROR:
 *
 * Symbol defining the #MbimDevice::device-error signal.
 *
 * Since: 1.0
 */
#define MBIM_DEVICE_SIGNAL_ERROR "device-error"

/**
 * MBIM_DEVICE_SIGNAL_REMOVED:
 *
 * Symbol defining the #MbimDevice::device-removed signal.
 *
 * Since: 1.10
 */
#define MBIM_DEVICE_SIGNAL_REMOVED "device-removed"

/**
 * MbimDevice:
 *
 * The #MbimDevice structure contains private data and should only be accessed
 * using the provided API.
 *
 * Since: 1.0
 */
struct _MbimDevice {
    /*< private >*/
    GObject parent;
    MbimDevicePrivate *priv;
};

struct _MbimDeviceClass {
    /*< private >*/
    GObjectClass parent;
};

GType mbim_device_get_type (void);
G_DEFINE_AUTOPTR_CLEANUP_FUNC (MbimDevice, g_object_unref)

/**
 * mbim_device_new:
 * @file: a #GFile.
 * @cancellable: optional #GCancellable object, #NULL to ignore.
 * @callback: a #GAsyncReadyCallback to call when the initialization is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously creates a #MbimDevice object to manage @file.
 * When the operation is finished, @callback will be invoked. You can then call
 * mbim_device_new_finish() to get the result of the operation.
 *
 * Since: 1.0
 */
void mbim_device_new (GFile               *file,
                      GCancellable        *cancellable,
                      GAsyncReadyCallback  callback,
                      gpointer             user_data);

/**
 * mbim_device_new_finish:
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with mbim_device_new().
 *
 * Returns: (transfer full): a newly created #MbimDevice, or #NULL if @error is set.
 *
 * Since: 1.0
 */
MbimDevice *mbim_device_new_finish (GAsyncResult  *res,
                                    GError       **error);

/**
 * mbim_device_get_file: (skip)
 * @self: a #MbimDevice.
 *
 * Get the #GFile associated with this #MbimDevice.
 *
 * Returns: (transfer full): a #GFile that must be freed with g_object_unref().
 *
 * Since: 1.0
 */
GFile *mbim_device_get_file (MbimDevice *self);

/**
 * mbim_device_peek_file: (skip)
 * @self: a #MbimDevice.
 *
 * Get the #GFile associated with this #MbimDevice, without increasing the reference count
 * on the returned object.
 *
 * Returns: (transfer none): a #GFile. Do not free the returned object, it is owned by @self.
 *
 * Since: 1.0
 */
GFile *mbim_device_peek_file (MbimDevice *self);

/**
 * mbim_device_get_path:
 * @self: a #MbimDevice.
 *
 * Get the system path of the underlying MBIM device.
 *
 * Returns: the system path of the device.
 *
 * Since: 1.0
 */
const gchar *mbim_device_get_path (MbimDevice *self);

/**
 * mbim_device_get_path_display:
 * @self: a #MbimDevice.
 *
 * Get the system path of the underlying MBIM device in UTF-8.
 *
 * Returns: UTF-8 encoded system path of the device.
 *
 * Since: 1.0
 */
const gchar *mbim_device_get_path_display (MbimDevice *self);

/**
 * mbim_device_is_open:
 * @self: a #MbimDevice.
 *
 * Checks whether the #MbimDevice is open for I/O.
 *
 * Returns: %TRUE if @self is open, %FALSE otherwise.
 *
 * Since: 1.0
 */
gboolean mbim_device_is_open (MbimDevice *self);

/**
 * MbimDeviceOpenFlags:
 * @MBIM_DEVICE_OPEN_FLAGS_NONE: None.
 * @MBIM_DEVICE_OPEN_FLAGS_PROXY: Try to open the port through the 'mbim-proxy'.
 *
 * Flags to specify which actions to be performed when the device is open.
 *
 * Since: 1.10
 */
typedef enum { /*< since=1.10 >*/
    MBIM_DEVICE_OPEN_FLAGS_NONE  = 0,
    MBIM_DEVICE_OPEN_FLAGS_PROXY = 1 << 0
} MbimDeviceOpenFlags;

/**
 * mbim_device_open_full:
 * @self: a #MbimDevice.
 * @flags: a set of #MbimDeviceOpenFlags.
 * @timeout: maximum time, in seconds, to wait for the device to be opened.
 * @cancellable: optional #GCancellable object, #NULL to ignore.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously opens a #MbimDevice for I/O.
 *
 * This method is an extension of the generic mbim_device_open(), which allows
 * launching the #MbimDevice with proxy support.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_open_full_finish() to get the result of the operation.
 *
 * Since: 1.10
 */
void mbim_device_open_full (MbimDevice          *self,
                            MbimDeviceOpenFlags  flags,
                            guint                timeout,
                            GCancellable        *cancellable,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data);

/**
 * mbim_device_open_full_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an asynchronous open operation started with mbim_device_open_full().
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.10
 */
gboolean mbim_device_open_full_finish (MbimDevice    *self,
                                       GAsyncResult  *res,
                                       GError       **error);

/**
 * mbim_device_open:
 * @self: a #MbimDevice.
 * @timeout: maximum time, in seconds, to wait for the device to be opened.
 * @cancellable: optional #GCancellable object, #NULL to ignore.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously opens a #MbimDevice for I/O.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_open_finish() to get the result of the operation.
 *
 * Since: 1.0
 */
void mbim_device_open (MbimDevice           *self,
                       guint                 timeout,
                       GCancellable         *cancellable,
                       GAsyncReadyCallback   callback,
                       gpointer              user_data);

/**
 * mbim_device_open_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an asynchronous open operation started with mbim_device_open().
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.0
 */
gboolean mbim_device_open_finish (MbimDevice    *self,
                                  GAsyncResult  *res,
                                  GError       **error);

/**
 * mbim_device_close:
 * @self: a #MbimDevice.
 * @timeout: maximum time, in seconds, to wait for the device to be closed.
 * @cancellable: optional #GCancellable object, #NULL to ignore.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously closes a #MbimDevice for I/O.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_close_finish() to get the result of the operation.
 *
 * Since: 1.0
 */
void mbim_device_close (MbimDevice          *self,
                        guint                timeout,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data);

/**
 * mbim_device_close_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an asynchronous close operation started with mbim_device_close().
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.0
 */
gboolean mbim_device_close_finish (MbimDevice    *self,
                                   GAsyncResult  *res,
                                   GError       **error);

/**
 * mbim_device_close_force:
 * @self: a #MbimDevice.
 * @error: Return location for error or %NULL.
 *
 * Forces the #MbimDevice to be closed.
 *
 * Returns: %TRUE if @self if no error happens, otherwise %FALSE and @error is set.
 *
 * Since: 1.0
 */
gboolean mbim_device_close_force (MbimDevice  *self,
                                  GError     **error);

/**
 * mbim_device_get_next_transaction_id:
 * @self: A #MbimDevice.
 *
 * Acquire the next transaction ID of this #MbimDevice.
 * The internal transaction ID gets incremented.
 *
 * Returns: the next transaction ID.
 *
 * Since: 1.0
 */
guint32 mbim_device_get_next_transaction_id (MbimDevice *self);

/**
 * mbim_device_get_transaction_id:
 * @self: A #MbimDevice.
 *
 * Acquire the transaction ID of this #MbimDevice without
 * incrementing the internal transaction ID.
 *
 * Returns: the current transaction ID.
 *
 * Since: 1.24.4
 */
guint32 mbim_device_get_transaction_id (MbimDevice *self);

/**
 * mbim_device_command:
 * @self: a #MbimDevice.
 * @message: the message to send.
 * @timeout: maximum time, in seconds, to wait for the response.
 * @cancellable: a #GCancellable, or %NULL.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously sends a #MbimMessage to the device.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_command_finish() to get the result of the operation.
 *
 * Since: 1.0
 */
void mbim_device_command (MbimDevice          *self,
                          MbimMessage         *message,
                          guint                timeout,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data);

/**
 * mbim_device_command_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with mbim_device_command().
 *
 * Returns: a #MbimMessage response, or #NULL if @error is set. The returned value should be freed with mbim_message_unref().
 *
 * Since: 1.0
 */
MbimMessage *mbim_device_command_finish (MbimDevice    *self,
                                         GAsyncResult  *res,
                                         GError       **error);

/**
 * MBIM_DEVICE_SESSION_ID_AUTOMATIC:
 *
 * Symbol defining a session id that will be automatically allocated during
 * runtime when creating net links.
 *
 * Since: 1.26
 */
#define MBIM_DEVICE_SESSION_ID_AUTOMATIC G_MAXUINT

/**
 * MBIM_DEVICE_SESSION_ID_MIN:
 *
 * Symbol defining the minimum supported session id..
 *
 * Since: 1.26
 */
#define MBIM_DEVICE_SESSION_ID_MIN 0

/**
 * MBIM_DEVICE_SESSION_ID_MAX:
 *
 * Symbol defining the maximum supported session id.
 *
 * Since: 1.26
 */
#define MBIM_DEVICE_SESSION_ID_MAX 0xFF

/**
 * mbim_device_add_link:
 * @self: a #MbimDevice.
 * @session_id: the session id for the link, in the
 *   [#MBIM_DEVICE_SESSION_ID_MIN,#MBIM_DEVICE_SESSION_ID_MAX] range, or
 *   #MBIM_DEVICE_SESSION_ID_AUTOMATIC to find the first available session id.
 * @base_ifname: the interface which the new link will be created on.
 * @ifname_prefix: the prefix suggested to be used for the name of the new link
 *   created.
 * @cancellable: a #GCancellable, or %NULL.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously creates a new virtual network device node on top of
 * @base_ifname. This allows having multiple net interfaces running on top of
 * another using multiplexing.
 *
 * If the kernel driver doesn't allow this functionality, a
 * %MBIM_CORE_ERROR_UNSUPPORTED error will be returned.
 *
 * The operation may fail if the given interface name is not associated to the
 * MBIM control port managed by the #MbimDevice.
 *
 * Depending on the kernel driver in use, the given @ifname_prefix may be
 * ignored. The user should not assume that the returned link interface name is
 * prefixed with @ifname_prefix as it may not be the case.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_add_link_finish() to get the result of the operation.
 *
 * Since: 1.26
 */
void mbim_device_add_link (MbimDevice          *self,
                           guint                session_id,
                           const gchar         *base_ifname,
                           const gchar         *ifname_prefix,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data);

/**
 * mbim_device_add_link_finish:
 * @self: a #QmiDevice.
 * @res: a #GAsyncResult.
 * @session_id: the session ID for the link created.
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with mbim_device_add_link().
 *
 * Returns: The name of the net interface created, %NULL if @error is set.
 *
 * Since: 1.26
 */
gchar *mbim_device_add_link_finish (MbimDevice    *self,
                                    GAsyncResult  *res,
                                    guint         *session_id,
                                    GError       **error);

/**
 * mbim_device_delete_link:
 * @self: a #MbimDevice.
 * @ifname: the name of the link to remove.
 * @cancellable: a #GCancellable, or %NULL.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously deletes a virtual network interface that has been previously
 * created with mbim_device_add_link().
 *
 * If the kernel driver doesn't allow this functionality, a
 * %MBIM_CORE_ERROR_UNSUPPORTED error will be returned.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_delete_link_finish() to get the result of the operation.
 *
 * Since: 1.26
 */
void mbim_device_delete_link (MbimDevice          *self,
                              const gchar         *ifname,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data);

/**
 * mbim_device_delete_link_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with mbim_device_delete_link().
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.26
 */
gboolean mbim_device_delete_link_finish (MbimDevice    *self,
                                         GAsyncResult  *res,
                                         GError       **error);

/**
 * mbim_device_delete_all_links:
 * @self: a #MbimDevice.
 * @base_ifname: the interface where all links are available.
 * @cancellable: a #GCancellable, or %NULL.
 * @callback: a #GAsyncReadyCallback to call when the operation is finished.
 * @user_data: the data to pass to callback function.
 *
 * Asynchronously deletes all virtual network interfaces that have been previously
 * created with mbim_device_add_link() in @base_ifname.
 *
 * When the operation is finished @callback will be called. You can then call
 * mbim_device_delete_link_finish() to get the result of the operation.
 *
 * <note><para>
 * There is no guarantee that other processes haven't created new links by the
 * time this method returns. This method should be used with caution, or in setups
 * where only one single process is expected to do MBIM network interface link
 * management.
 * </para></note>
 *
 * Since: 1.26
 */
void mbim_device_delete_all_links (MbimDevice          *self,
                                   const gchar         *base_ifname,
                                   GCancellable        *cancellable,
                                   GAsyncReadyCallback  callback,
                                   gpointer             user_data);

/**
 * mbim_device_delete_all_links_finish:
 * @self: a #MbimDevice.
 * @res: a #GAsyncResult.
 * @error: Return location for error or %NULL.
 *
 * Finishes an operation started with mbim_device_delete_all_links().
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.26
 */
gboolean mbim_device_delete_all_links_finish (MbimDevice    *self,
                                              GAsyncResult  *res,
                                              GError       **error);

/**
 * mbim_device_list_links:
 * @self: a #MbimDevice.
 * @base_ifname: the base interface.
 * @out_links: (out)(transfer full)(element-type utf8): a placeholder for the
 *   output #GPtrArray of link names.
 * @error: Return location for error or %NULL.
 *
 * Returns: %TRUE if successful, %FALSE if @error is set.
 *
 * Since: 1.26
 */
gboolean mbim_device_list_links (MbimDevice   *self,
                                 const gchar  *base_ifname,
                                 GPtrArray   **out_links,
                                 GError      **error);

G_END_DECLS

#endif /* _LIBMBIM_GLIB_MBIM_DEVICE_H_ */
