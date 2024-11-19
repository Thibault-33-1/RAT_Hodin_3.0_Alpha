#ifndef DOWNLOADER_H_INCLUDED
#define DOWNLOADER_H_INCLUDED


void download_files(const char *path, GtkWidget *downloader_dialog);
void download_binaries(const gchar *path, GtkWidget *progress_bar_binary);

void download_hosts_files(void);


#endif

