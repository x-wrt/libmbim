/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * mbimcli -- Command line interface to control MBIM devices
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2013 Aleksander Morgado <aleksander@gnu.org>
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include <libmbim-glib.h>

#include "mbimcli.h"

/* Context */
typedef struct {
    MbimDevice *device;
    GCancellable *cancellable;
} Context;
static Context *ctx;

/* Options */
static gboolean query_device_caps_flag;
static gboolean query_subscriber_ready_status_flag;
static gboolean query_radio_state_flag;
static gboolean query_device_services_flag;
static gboolean query_pin_flag;

static GOptionEntry entries[] = {
    { "basic-connect-query-device-caps", 0, 0, G_OPTION_ARG_NONE, &query_device_caps_flag,
      "Query device capabilities",
      NULL
    },
    { "basic-connect-query-subscriber-ready-status", 0, 0, G_OPTION_ARG_NONE, &query_subscriber_ready_status_flag,
      "Query subscriber ready status",
      NULL
    },
    { "basic-connect-query-radio-state", 0, 0, G_OPTION_ARG_NONE, &query_radio_state_flag,
      "Query radio state",
      NULL
    },
    { "basic-connect-query-device-services", 0, 0, G_OPTION_ARG_NONE, &query_device_services_flag,
      "Query device services",
      NULL
    },
    { "basic-connect-query-pin", 0, 0, G_OPTION_ARG_NONE, &query_pin_flag,
      "Query PIN state",
      NULL
    },
    { NULL }
};

GOptionGroup *
mbimcli_basic_connect_get_option_group (void)
{
	GOptionGroup *group;

	group = g_option_group_new ("basic-connect",
	                            "Basic Connect options",
	                            "Show Basic Connect Service options",
	                            NULL,
	                            NULL);
	g_option_group_add_entries (group, entries);

	return group;
}

gboolean
mbimcli_basic_connect_options_enabled (void)
{
    static guint n_actions = 0;
    static gboolean checked = FALSE;

    if (checked)
        return !!n_actions;

    n_actions = (query_device_caps_flag +
                 query_subscriber_ready_status_flag +
                 query_radio_state_flag +
                 query_device_services_flag +
                 query_pin_flag);

    if (n_actions > 1) {
        g_printerr ("error: too many Basic Connect actions requested\n");
        exit (EXIT_FAILURE);
    }

    checked = TRUE;
    return !!n_actions;
}

static void
context_free (Context *context)
{
    if (!context)
        return;

    if (context->cancellable)
        g_object_unref (context->cancellable);
    if (context->device)
        g_object_unref (context->device);
    g_slice_free (Context, context);
}

static void
shutdown (gboolean operation_status)
{
    /* Cleanup context and finish async operation */
    context_free (ctx);
    mbimcli_async_operation_done (operation_status);
}

static void
query_device_caps_ready (MbimDevice   *device,
                         GAsyncResult *res)
{
    MbimMessage *response;
    GError *error = NULL;
    MbimDeviceType device_type;
    const gchar *device_type_str;
    MbimCellularClass cellular_class;
    gchar *cellular_class_str;
    MbimVoiceClass voice_class;
    const gchar *voice_class_str;
    MbimSimClass sim_class;
    gchar *sim_class_str;
    MbimDataClass data_class;
    gchar *data_class_str;
    MbimSmsCaps sms_caps;
    gchar *sms_caps_str;
    MbimCtrlCaps ctrl_caps;
    gchar *ctrl_caps_str;
    guint32 max_sessions;
    gchar *custom_data_class;
    gchar *device_id;
    gchar *firmware_info;
    gchar *hardware_info;

    response = mbim_device_command_finish (device, res, &error);
    if (!response) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    if (!mbim_message_basic_connect_device_caps_query_response_parse (
            response,
            &device_type,
            &cellular_class,
            &voice_class,
            &sim_class,
            &data_class,
            &sms_caps,
            &ctrl_caps,
            &max_sessions,
            &custom_data_class,
            &device_id,
            &firmware_info,
            &hardware_info,
            &error)) {
        g_printerr ("error: couldn't parse response message: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    device_type_str = mbim_device_type_get_string (device_type);
    cellular_class_str = mbim_cellular_class_build_string_from_mask (cellular_class);
    voice_class_str = mbim_device_type_get_string (voice_class);
    sim_class_str = mbim_sim_class_build_string_from_mask (sim_class);
    data_class_str = mbim_data_class_build_string_from_mask (data_class);
    sms_caps_str = mbim_sms_caps_build_string_from_mask (sms_caps);
    ctrl_caps_str = mbim_ctrl_caps_build_string_from_mask (ctrl_caps);

#undef VALIDATE_UNKNOWN
#define VALIDATE_UNKNOWN(str) (str ? str : "unknown")

    g_print ("[%s] Device capabilities retrieved:\n"
             "\t      Device type: '%s'\n"
             "\t   Cellular class: '%s'\n"
             "\t      Voice class: '%s'\n"
             "\t        Sim class: '%s'\n"
             "\t       Data class: '%s'\n"
             "\t         SMS caps: '%s'\n"
             "\t        Ctrl caps: '%s'\n"
             "\t     Max sessions: '%u'\n"
             "\tCustom data class: '%s'\n"
             "\t        Device ID: '%s'\n"
             "\t    Firmware info: '%s'\n"
             "\t    Hardware info: '%s'\n",
             mbim_device_get_path_display (device),
             VALIDATE_UNKNOWN (device_type_str),
             VALIDATE_UNKNOWN (cellular_class_str),
             VALIDATE_UNKNOWN (voice_class_str),
             VALIDATE_UNKNOWN (sim_class_str),
             VALIDATE_UNKNOWN (data_class_str),
             VALIDATE_UNKNOWN (sms_caps_str),
             VALIDATE_UNKNOWN (ctrl_caps_str),
             max_sessions,
             VALIDATE_UNKNOWN (custom_data_class),
             VALIDATE_UNKNOWN (device_id),
             VALIDATE_UNKNOWN (firmware_info),
             VALIDATE_UNKNOWN (hardware_info));

    g_free (cellular_class_str);
    g_free (sim_class_str);
    g_free (data_class_str);
    g_free (sms_caps_str);
    g_free (ctrl_caps_str);
    g_free (custom_data_class);
    g_free (device_id);
    g_free (firmware_info);
    g_free (hardware_info);

    mbim_message_unref (response);
    shutdown (TRUE);
}

static void
query_subscriber_ready_status_ready (MbimDevice   *device,
                                     GAsyncResult *res)
{
    MbimMessage *response;
    GError *error = NULL;
    MbimSubscriberReadyState ready_state;
    const gchar *ready_state_str;
    gchar *subscriber_id;
    gchar *sim_iccid;
    MbimReadyInfoFlag ready_info;
    gchar *ready_info_str;
    guint32 telephone_numbers_count;
    gchar **telephone_numbers;
    gchar *telephone_numbers_str;

    response = mbim_device_command_finish (device, res, &error);
    if (!response) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    if (!mbim_message_basic_connect_subscriber_ready_status_query_response_parse (
            response,
            &ready_state,
            &subscriber_id,
            &sim_iccid,
            &ready_info,
            &telephone_numbers_count,
            &telephone_numbers,
            &error)) {
        g_printerr ("error: couldn't parse response message: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    telephone_numbers_str = (telephone_numbers ? g_strjoinv (", ", telephone_numbers) : NULL);
    ready_state_str = mbim_subscriber_ready_state_get_string (ready_state);
    ready_info_str = mbim_ready_info_flag_build_string_from_mask (ready_info);

#undef VALIDATE_UNKNOWN
#define VALIDATE_UNKNOWN(str) (str ? str : "unknown")

    g_print ("[%s] Subscriber ready status retrieved:\n"
             "\t      Ready state: '%s'\n"
             "\t    Subscriber ID: '%s'\n"
             "\t        SIM ICCID: '%s'\n"
             "\t       Ready info: '%s'\n"
             "\tTelephone numbers: (%u) '%s'\n",
             mbim_device_get_path_display (device),
             VALIDATE_UNKNOWN (ready_state_str),
             VALIDATE_UNKNOWN (subscriber_id),
             VALIDATE_UNKNOWN (sim_iccid),
             VALIDATE_UNKNOWN (ready_info_str),
             telephone_numbers_count, VALIDATE_UNKNOWN (telephone_numbers_str));

    g_free (subscriber_id);
    g_free (sim_iccid);
    g_free (ready_info_str);
    g_strfreev (telephone_numbers);
    g_free (telephone_numbers_str);

    mbim_message_unref (response);
    shutdown (TRUE);
}

static void
query_radio_state_ready (MbimDevice   *device,
                         GAsyncResult *res)
{
    MbimMessage *response;
    GError *error = NULL;
    MbimRadioSwitchState hardware_radio_state;
    const gchar *hardware_radio_state_str;
    MbimRadioSwitchState software_radio_state;
    const gchar *software_radio_state_str;

    response = mbim_device_command_finish (device, res, &error);
    if (!response) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    if (!mbim_message_basic_connect_radio_state_query_response_parse (
            response,
            &hardware_radio_state,
            &software_radio_state,
            &error)) {
        g_printerr ("error: couldn't parse response message: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    hardware_radio_state_str = mbim_radio_switch_state_get_string (hardware_radio_state);
    software_radio_state_str = mbim_radio_switch_state_get_string (software_radio_state);

#undef VALIDATE_UNKNOWN
#define VALIDATE_UNKNOWN(str) (str ? str : "unknown")

    g_print ("[%s] Radio state retrieved:\n"
             "\t     Hardware Radio State: '%s'\n"
             "\t     Software Radio State: '%s'\n",
             mbim_device_get_path_display (device),
             VALIDATE_UNKNOWN (hardware_radio_state_str),
             VALIDATE_UNKNOWN (software_radio_state_str));

    mbim_message_unref (response);
    shutdown (TRUE);
}

static void
query_device_services_ready (MbimDevice   *device,
                             GAsyncResult *res)
{
    MbimMessage *response;
    GError *error = NULL;
    MbimDeviceServiceElement **device_services;
    guint32 device_services_count;
    guint32 max_dss_sessions;

    response = mbim_device_command_finish (device, res, &error);
    if (!response) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    if (!mbim_message_basic_connect_device_services_query_response_parse (
            response,
            &device_services_count,
            &max_dss_sessions,
            &device_services,
            &error)) {
        g_printerr ("error: couldn't parse response message: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    g_print ("[%s] Device services retrieved:\n"
             "\tMax DSS sessions: '%u'\n",
             mbim_device_get_path_display (device),
             max_dss_sessions);
    if (device_services_count == 0)
        g_print ("\t        Services: None\n");
    else {
        guint32 i;

        g_print ("\t        Services: (%u)\n", device_services_count);
        for (i = 0; i < device_services_count; i++) {
            MbimService service;
            gchar *uuid_str;
            GString *cids;
            guint32 j;

            service = mbim_uuid_to_service (&device_services[i]->device_service_id);
            uuid_str = mbim_uuid_get_printable (&device_services[i]->device_service_id);

            cids = g_string_new ("");
            for (j = 0; j < device_services[i]->cids_count; j++) {
                if (service == MBIM_SERVICE_INVALID) {
                    g_string_append_printf (cids, "%u", device_services[i]->cids[j]);
                    if (j < device_services[i]->cids_count - 1)
                        g_string_append (cids, ", ");
                } else {
                    g_string_append_printf (cids, "%s%s (%u)",
                                            j == 0 ? "" : "\t\t                   ",
                                            mbim_cid_get_printable (service, device_services[i]->cids[j]),
                                            device_services[i]->cids[j]);
                    if (j < device_services[i]->cids_count - 1)
                        g_string_append (cids, ",\n");
                }
            }

            g_print ("\n"
                     "\t\t          Service: '%s'\n"
                     "\t\t             UUID: [%s]:\n"
                     "\t\t      DSS payload: %u\n"
                     "\t\tMax DSS instances: %u\n"
                     "\t\t             CIDs: %s\n",
                     service == MBIM_SERVICE_INVALID ? "unknown" : mbim_service_get_string (service),
                     uuid_str,
                     device_services[i]->dss_payload,
                     device_services[i]->max_dss_instances,
                     cids->str);

            g_string_free (cids, TRUE);
            g_free (uuid_str);
        }
    }

    mbim_device_service_element_array_free (device_services);

    mbim_message_unref (response);
    shutdown (TRUE);
}

static void
pin_ready (MbimDevice   *device,
               GAsyncResult *res)
{
    MbimMessage *response;
    GError *error = NULL;
    MbimPinType pin_type;
    const gchar *pin_type_str;
    MbimPinState pin_state;
    const gchar *pin_state_str;
    guint32 remaining_attempts;

    response = mbim_device_command_finish (device, res, &error);
    if (!response) {
        g_printerr ("error: operation failed: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

    if (!mbim_message_basic_connect_pin_query_response_parse (
            response,
            &pin_type,
            &pin_state,
            &remaining_attempts,
            &error)) {
        g_printerr ("error: couldn't parse response message: %s\n", error->message);
        g_error_free (error);
        shutdown (FALSE);
        return;
    }

#undef VALIDATE_UNKNOWN
#define VALIDATE_UNKNOWN(str) (str ? str : "unknown")

    pin_type_str = mbim_pin_type_get_string (pin_type);
    pin_state_str = mbim_pin_state_get_string (pin_state);

    g_print ("[%s] Pin Info:\n"
             "\t     PinType: '%s'\n"
             "\t     PinState: '%s'\n"
             "\t     RemainingAttempts: '%u'\n",
             mbim_device_get_path_display (device),
             pin_type_str,
             pin_state_str,
             remaining_attempts);
    mbim_message_unref (response);
    shutdown (TRUE);
}

void
mbimcli_basic_connect_run (MbimDevice   *device,
                           GCancellable *cancellable)
{
    /* Initialize context */
    ctx = g_slice_new (Context);
    ctx->device = g_object_ref (device);
    if (cancellable)
        ctx->cancellable = g_object_ref (cancellable);

    /* Request to get capabilities? */
    if (query_device_caps_flag) {
        MbimMessage *request;

        g_debug ("Asynchronously querying device capabilities...");
        request = (mbim_message_basic_connect_device_caps_query_request_new (NULL));
        mbim_device_command (ctx->device,
                             request,
                             10,
                             ctx->cancellable,
                             (GAsyncReadyCallback)query_device_caps_ready,
                             NULL);
        mbim_message_unref (request);
        return;
    }

    /* Request to get subscriber ready status? */
    if (query_subscriber_ready_status_flag) {
        MbimMessage *request;

        g_debug ("Asynchronously querying subscriber ready status...");
        request = (mbim_message_basic_connect_subscriber_ready_status_query_request_new (NULL));
        mbim_device_command (ctx->device,
                             request,
                             10,
                             ctx->cancellable,
                             (GAsyncReadyCallback)query_subscriber_ready_status_ready,
                             NULL);
        mbim_message_unref (request);
        return;
    }

    /* Request to get radio state? */
    if (query_radio_state_flag) {
        MbimMessage *request;

        g_debug ("Asynchronously querying radio state...");
        request = (mbim_message_basic_connect_radio_state_query_request_new (NULL));
        mbim_device_command (ctx->device,
                             request,
                             10,
                             ctx->cancellable,
                             (GAsyncReadyCallback)query_radio_state_ready,
                             NULL);
        mbim_message_unref (request);
        return;
    }

    /* Request to query device services? */
    if (query_device_services_flag) {
        MbimMessage *request;

        g_debug ("Asynchronously querying device services...");
        request = (mbim_message_basic_connect_device_services_query_request_new (NULL));
        mbim_device_command (ctx->device,
                             request,
                             10,
                             ctx->cancellable,
                             (GAsyncReadyCallback)query_device_services_ready,
                             NULL);
        mbim_message_unref (request);
        return;
    }

    if (query_pin_flag) {
        MbimMessage *request;

        g_debug ("Asynchronously querying PIN state...");
        request = (mbim_message_basic_connect_pin_query_request_new (NULL));
        mbim_device_command (ctx->device,
                             request,
                             10,
                             ctx->cancellable,
                             (GAsyncReadyCallback)pin_ready,
                             NULL);
        mbim_message_unref (request);
        return;
    }

    g_warn_if_reached ();
}