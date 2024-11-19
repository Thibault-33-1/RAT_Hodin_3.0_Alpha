#ifndef CALLBACKS_REMOTE_SHELL_H_INCLUDED
#define CALLBACKS_REMOTE_SHELL_H_INCLUDED

/** Tab Remote Shell **/

void cb_start_remote_shell(GtkButton *button, gpointer user_data);
void cb_stop_remote_shell(GtkButton *button, gpointer user_data);
void cb_send_cmd(GtkWidget *widget, gpointer user_data);

#endif
