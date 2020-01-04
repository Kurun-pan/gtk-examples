#include <gtk/gtk.h>
#include <unistd.h>
#include <glib.h>

static GtkWidget *s_label = NULL;

static GThread *s_thread = NULL;
static volatile gboolean s_thread_request_quit = FALSE;

static void thread_stop ()
{
    s_thread_request_quit = TRUE;
    g_thread_join (s_thread);
    g_thread_unref (s_thread);
}

static gboolean label_countup (gpointer user_data)
{
    static gint s_label_count = 0;

    if (s_label != NULL && !s_thread_request_quit) {
        gchar label[256];
        memset (label, 0x0, 256);
        sprintf (label, "count = %d", s_label_count);
        gtk_label_set_text ( GTK_LABEL (s_label), label);
        s_label_count++;
    }
    /* TRUEを返すと本関数が再度コールされます！ */
    return FALSE;
}

static gpointer thread_loop (gpointer userData)
{
    /* [参考] スレッドを利用しなくても同じことをg_timeout_addを利用しても実現可能です */
    while (!s_thread_request_quit) {
        /* [注意] UI更新処理はメインのUIスレッドで実施！ */
        g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
                         (GSourceFunc) label_countup,
                         NULL,
                         NULL);
        sleep (1);
    }
    return NULL;
}

static void cb_window_destory ()
{
    thread_stop();
}

static void cb_application_activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "g_thread サンプル");
    gtk_widget_set_size_request (window, 320, 240);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS);
    g_signal_connect (window, "destroy", G_CALLBACK (cb_window_destory), NULL);

    s_label = gtk_label_new ("count = 0");
    gtk_container_add (GTK_CONTAINER (window), s_label);

    gtk_widget_show_all (window);

    /* スレッド作成 */
    s_thread = g_thread_new ("thread_loop",
                             thread_loop,
                             NULL);
    g_warn_if_fail (s_thread != NULL);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.thread", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
