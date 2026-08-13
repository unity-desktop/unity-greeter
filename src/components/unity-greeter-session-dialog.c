#include "unity-greeter-session-dialog.h"

#include "unity-greeter-session-list.h"

struct _UnityGreeterSessionDialog
{
  AdwDialog parent_instance;

  AdwPreferencesGroup *sessions_group;
};

enum {
  SIGNAL_SESSION_SELECTED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityGreeterSessionDialog, unity_greeter_session_dialog, ADW_TYPE_DIALOG)

static void
on_check_toggled (GtkCheckButton *check, gpointer user_data)
{
  if (!gtk_check_button_get_active (check))
    return;

  UnityGreeterSessionDialog *self = UNITY_GREETER_SESSION_DIALOG (user_data);
  const gchar *id = g_object_get_data (G_OBJECT (check), "session-id");
  if (id != NULL)
    g_signal_emit (self, signals[SIGNAL_SESSION_SELECTED], 0, id);
}

static void
populate (UnityGreeterSessionDialog *self,
          GListModel                *sessions,
          const gchar                *selected_id)
{
  GtkCheckButton *radio_root = NULL;
  guint n = g_list_model_get_n_items (sessions);

  for (guint i = 0; i < n; i++)
    {
      g_autoptr (UnityGreeterSession) session =
        UNITY_GREETER_SESSION (g_list_model_get_item (sessions, i));

      GtkWidget *check = gtk_check_button_new ();
      gtk_widget_set_valign (check, GTK_ALIGN_CENTER);

      if (radio_root == NULL)
        radio_root = GTK_CHECK_BUTTON (check);
      else
        gtk_check_button_set_group (GTK_CHECK_BUTTON (check), radio_root);

      const gchar *id = unity_greeter_session_get_id (session);
      g_object_set_data_full (G_OBJECT (check), "session-id",
                              g_strdup (id), g_free);

      if (g_strcmp0 (selected_id, id) == 0)
        gtk_check_button_set_active (GTK_CHECK_BUTTON (check), TRUE);

      AdwActionRow *row = ADW_ACTION_ROW (adw_action_row_new ());
      adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row),
                                     unity_greeter_session_get_name (session));
      adw_action_row_set_activatable_widget (row, check);

      const gchar *comment = unity_greeter_session_get_comment (session);
      if (comment != NULL && *comment != '\0')
        adw_action_row_set_subtitle (row, comment);

      adw_action_row_add_prefix (row, check);

      g_signal_connect (check, "toggled", G_CALLBACK (on_check_toggled), self);

      adw_preferences_group_add (self->sessions_group, GTK_WIDGET (row));
    }
}

static void
unity_greeter_session_dialog_dispose (GObject *object)
{
  UnityGreeterSessionDialog *self = UNITY_GREETER_SESSION_DIALOG (object);
  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_GREETER_TYPE_SESSION_DIALOG);
  G_OBJECT_CLASS (unity_greeter_session_dialog_parent_class)->dispose (object);
}

static void
unity_greeter_session_dialog_class_init (UnityGreeterSessionDialogClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = unity_greeter_session_dialog_dispose;

  gtk_widget_class_set_template_from_resource (
    widget_class, "/org/unity/Greeter/unity-greeter-session-dialog.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterSessionDialog, sessions_group);

  signals[SIGNAL_SESSION_SELECTED] = g_signal_new (
    "session-selected", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
unity_greeter_session_dialog_init (UnityGreeterSessionDialog *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

AdwDialog *
unity_greeter_session_dialog_new (GListModel *sessions,
                                  const gchar *selected_id)
{
  g_return_val_if_fail (G_IS_LIST_MODEL (sessions), NULL);

  UnityGreeterSessionDialog *self =
    g_object_new (UNITY_GREETER_TYPE_SESSION_DIALOG, NULL);
  populate (self, sessions, selected_id);
  return ADW_DIALOG (self);
}
