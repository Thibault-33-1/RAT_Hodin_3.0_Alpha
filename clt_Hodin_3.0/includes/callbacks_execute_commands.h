#ifndef CALLBACKS_EXECUTE_COMMANDS_H_INCLUDED
#define CALLBACKS_EXECUTE_COMMANDS_H_INCLUDED

void cb_stream_remote_desktop(const gchar *IP);
void cb_stream_remote_webcam(const gchar *IP);
void cb_record_webcam(GtkButton *button, gpointer user_data);
void cb_record_micro(GtkButton *button, gpointer user_data);


void receive_remote_resolution(void);

void cb_take_screenshot(GtkButton *button, gpointer user_data);

void cb_multi_screenshot(GtkButton *button, gpointer user_data);

#endif

