#include <gtk/gtk.h>
#include "draw.h"

static void cb_application_activate (GtkApplication* app, gpointer user_data)
{
    GtkBuilder* builder = gtk_builder_new ();
    g_return_if_fail (builder != NULL);

    GError* error = NULL;
    if (!gtk_builder_add_from_file (builder, "./main.glade", &error)) {
        if (error) {
            g_error ("Failed to load: %s", error->message);
            g_error_free (error);
            return ;
        }
    }

    /* メインウインドウ設定 */
    GtkApplicationWindow *window = GTK_APPLICATION_WINDOW (gtk_builder_get_object (builder, "application_window"));
    g_warn_if_fail (window != NULL);
    g_object_set (window, "application", app, NULL);
    gtk_window_set_title (GTK_WINDOW (window), "3Dグラフィックス OpenGLサンプル");

    /* 3D GLArea設定 */
    GtkWidget *gl_area = GTK_WIDGET (gtk_builder_get_object (builder, "gl_area"));
    g_warn_if_fail (gl_area != NULL);

    g_signal_connect (gl_area, "realize", G_CALLBACK (draw_realize), NULL);
    g_signal_connect (gl_area, "render", G_CALLBACK (draw_render), NULL);
    g_signal_connect (gl_area, "unrealize", G_CALLBACK (draw_unrealize), NULL);

    gtk_widget_show_all (GTK_WIDGET (window));
    g_object_unref (builder);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.opengl", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
