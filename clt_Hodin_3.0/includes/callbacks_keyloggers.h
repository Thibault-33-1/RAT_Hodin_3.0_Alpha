#ifndef CALLBACKS_KEYLOGGERS_H_INCLUDED
#define CALLBACKS_KEYLOGGERS_H_INCLUDED


void cb_exit_the_keylogger(GtkButton *button, gpointer user_data);

void cb_ubuntu24_run_the_keylogger(GtkButton *button, gpointer user_data);
void cb_mint_run_the_keylogger(GtkButton *button, gpointer user_data);
void cb_debian_run_the_keylogger(GtkButton *button, gpointer user_data);
void cb_kali_run_the_keylogger(GtkButton *button, gpointer user_data);
void cb_fedora_run_the_keylogger(GtkButton *button, gpointer user_data);

void cb_download_log_file(GtkButton *button, gpointer user_data);

#endif
