#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>  //gethostbyaddr()

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <gtk-2.0/gtk/gtk.h>

#include "../includes/callbacks_gather_infos.h"
#include "../includes/utils.h"
#include "../includes/constants.h"

extern GtkWidget *main_win;

extern const gchar *server_ip;
extern const gchar *server_port;
extern int port;

extern GtkWidget *zone[4];

extern GtkWidget *hostname_text_view;
extern GtkWidget *architecture_text_view;
extern GtkWidget *os_text_view;
extern GtkWidget *os_bits_text_view;
extern GtkWidget *ip_text_view;
extern GtkWidget *memory_text_view;

void cb_gather_infos(GtkButton *button, gpointer user_data)
{
    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    GtkTextBuffer *text_buffer = NULL;
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;
    gchar *utf8_text = NULL;

    const gchar *gather_hostname_infos= "uname -n";
    size_t len_hostame_cmd = strlen(gather_hostname_infos) + 1;

    const gchar *gather_architecture_infos= "uname -m";
    size_t len_architecture_cmd = strlen(gather_architecture_infos) + 1;

    const gchar *gather_os_infos= "cat /etc/os-release | grep PRETTY_NAME";
    size_t len_os_cmd = strlen(gather_os_infos) + 1;

    const gchar *gather_ip_infos= "curl ifconfig.me";
    size_t len_ip_cmd = strlen(gather_ip_infos) + 1;

    const gchar *gather_memory_infos= "cat /proc/meminfo | grep MemTotal";
    size_t len_memory_cmd = strlen(gather_memory_infos) + 1;

    char *buffer_hostame_infos = NULL;
    char *buffer_architecture_infos = NULL;
    char *buffer_os_infos = NULL;
    char *buffer_ip_infos = NULL;
    char *buffer_memory_infos = NULL;

    gchar *os_bits = "";

    char *final_string;

    int err = 0;
    size_t flag = 23;

    port = atoi(server_port);

    inet_pton(AF_INET, server_ip, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    memset(&sin, 0, sizeof(sin));
    sin.sin_addr = *((struct in_addr *)he->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        error("socket()", "cb_gather_infos()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_gather_infos()");
        exit(-1);
    }

    printf("\t\tGATHER INFOS ....\n");

    if(send(sock, (char*)&flag, sizeof(flag), 0) == SOCKET_ERROR)
    {
        error("send() flag", "cb_gather_infos()");
        exit(-1);
    }

    /** Hostname **/
    if(send(sock, (char*)&len_hostame_cmd, sizeof(len_hostame_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_infos_cmd", "cb_gather_infos()");
        exit(-1);
    }

    if(send(sock, gather_hostname_infos, len_hostame_cmd, 0) == SOCKET_ERROR)
    {
        error("send() gather_hostname_infos", "cb_gather_infos()");
        exit(-1);
    }

    buffer_hostame_infos = malloc(BUFSIZ * sizeof(char));
    if(buffer_hostame_infos == NULL)
    {
        error("malloc() buffer_hostame_infos", "cb_gather_infos()");
        return;
    }

    if(recv(sock, buffer_hostame_infos, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "cb_gather_infos()");
        return;
    }

    printf("\n\nCommand output received : %s", buffer_hostame_infos);
    printf("\n\n");

    utf8_text = g_locale_to_utf8(buffer_hostame_infos, strlen(buffer_hostame_infos), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(hostname_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /** Architecture **/
    if(send(sock, (char*)&len_architecture_cmd, sizeof(len_architecture_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_infos_cmd", "cb_gather_infos()");
        exit(-1);
    }

    if(send(sock, gather_architecture_infos, len_architecture_cmd, 0) == SOCKET_ERROR)
    {
        error("send() gather_hostname_infos", "cb_gather_infos()");
        exit(-1);
    }

    buffer_architecture_infos = malloc(BUFSIZ * sizeof(char));
    if(buffer_architecture_infos == NULL)
    {
        error("malloc() buffer_architecture_infos", "cb_gather_infos()");
        return;
    }

    if(recv(sock, buffer_architecture_infos, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "cb_gather_infos()");
        return;
    }

    if(strncmp(buffer_architecture_infos, "x86_64", 6) == 0)
        os_bits = "64-bits";

    if(strncmp(buffer_architecture_infos, "i386", 6) == 0)
        os_bits = "32-bits";

    printf("\n\nCommand output received : %s", buffer_architecture_infos);
    printf("\n\n");

    utf8_text = g_locale_to_utf8(buffer_architecture_infos, strlen(buffer_architecture_infos), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(architecture_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /**Operating System**/
    if(send(sock, (char*)&len_os_cmd, sizeof(len_os_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_infos_cmd", "cb_gather_infos()");
        exit(-1);
    }

    if(send(sock, gather_os_infos, len_os_cmd, 0) == SOCKET_ERROR)
    {
        error("send() gather_hostname_infos", "cb_gather_infos()");
        exit(-1);
    }

    buffer_os_infos = malloc(BUFSIZ * sizeof(char));
    if(buffer_os_infos == NULL)
    {
        error("malloc() buffer_os_infos", "cb_gather_infos()");
        return;
    }

    if(recv(sock, buffer_os_infos, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_os_infos", "cb_gather_infos()");
        return;
    }

    printf("\n\nCommand output received : %s", buffer_os_infos);
    printf("\n\n");

    final_string = strpbrk(buffer_os_infos, "\"");

    utf8_text = g_locale_to_utf8(final_string, strlen(final_string), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(os_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /** OS bits **/
    utf8_text = g_locale_to_utf8(os_bits, strlen(os_bits), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(os_bits_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /** IP **/
    if(send(sock, (char*)&len_ip_cmd, sizeof(len_ip_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_ip_cmd", "cb_gather_infos()");
        exit(-1);
    }

    if(send(sock, gather_ip_infos, len_ip_cmd, 0) == SOCKET_ERROR)
    {
        error("send() gather_ip_infos", "cb_gather_infos()");
        exit(-1);
    }

    buffer_ip_infos = malloc(BUFSIZ * sizeof(char));
    if(buffer_ip_infos == NULL)
    {
        error("malloc() buffer_ip_infos", "cb_gather_infos()");
        return;
    }

    if(recv(sock, buffer_ip_infos, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_ip_infos", "cb_gather_infos()");
        return;
    }

    printf("\n\nCommand output received : %s", buffer_ip_infos);
    printf("\n\n");

    utf8_text = g_locale_to_utf8(buffer_ip_infos, strlen(buffer_ip_infos), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(ip_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /** OS bits **/
    utf8_text = g_locale_to_utf8(os_bits, strlen(os_bits), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(os_bits_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /** Memory **/
    if(send(sock, (char*)&len_memory_cmd, sizeof(len_memory_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_memory_cmd", "cb_gather_infos()");
        exit(-1);
    }

    if(send(sock, gather_memory_infos, len_memory_cmd, 0) == SOCKET_ERROR)
    {
        error("send() gather_memory_infos", "cb_gather_infos()");
        exit(-1);
    }

    buffer_memory_infos = malloc(BUFSIZ * sizeof(char));
    if(buffer_memory_infos == NULL)
    {
        error("malloc() buffer_memory_infos", "cb_gather_infos()");
        return;
    }

    if(recv(sock, buffer_memory_infos, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_memory_infos", "cb_gather_infos()");
        return;
    }

    printf("\n\nCommand output received : %s", buffer_memory_infos);
    printf("\n\n");


    char *final_memory_string;
    final_memory_string = strpbrk(buffer_memory_infos, " ");

    utf8_text = g_locale_to_utf8(final_memory_string, strlen(final_memory_string), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(memory_text_view));
    /** Set the default buffer text. */
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);


    (void)button;
    (void)user_data;

    free(buffer_hostame_infos);
    free(buffer_architecture_infos);
    free(buffer_os_infos);

    return;
}
