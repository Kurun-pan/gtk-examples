#include <gtk/gtk.h>

static cairo_surface_t *s_cairo_surface = NULL;

static void clear_surface ()
{
    cairo_t *cr = cairo_create (s_cairo_surface);

    /* サーフェス全体を白で塗り潰し */
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static void draw_brush (GtkWidget *widget, gdouble x, gdouble y)
{
    #define BRUSH_SIZE 10

    cairo_t *cr = cairo_create (s_cairo_surface);

    /* ブラシとして■を描画 */
    cairo_rectangle (cr, x - BRUSH_SIZE / 2, y - BRUSH_SIZE / 2, BRUSH_SIZE, BRUSH_SIZE);
    cairo_fill (cr);
    cairo_destroy (cr);

    /* 再描画エリアを指定してリクエスト */
    gtk_widget_queue_draw_area (widget, x - BRUSH_SIZE / 2, y - BRUSH_SIZE / 2, BRUSH_SIZE, BRUSH_SIZE);
}

static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (s_cairo_surface != NULL)
        cairo_surface_destroy (s_cairo_surface);

    /* RGBのみのサーフェスを新規作成 */
    s_cairo_surface = gdk_window_create_similar_surface (
                                               gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));

    /* 初期状態の設定 */
    clear_surface ();

    return TRUE;
}

static gboolean cb_draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    /* サーフェスから描画パターンを作成 */
    cairo_set_source_surface (cr, s_cairo_surface, 0, 0);
    /* 描画 */
    cairo_paint (cr);

    return TRUE;
}

static gboolean cb_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
     if (s_cairo_surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY/*左クリック*/)
        draw_brush (widget, event->x, event->y);
    else if (event->button == GDK_BUTTON_SECONDARY/*右クリック*/) {
        clear_surface ();
        gtk_widget_queue_draw (widget);
    }

    return TRUE;
}

static gboolean cb_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    if (s_cairo_surface == NULL)
        return FALSE;

    /* マウス左ボタンが押されていれば描画 */
    if (event->state & GDK_BUTTON1_MASK)
        draw_brush (widget, event->x, event->y);

    return TRUE;
}

static void cb_window_destory ()
{
    /* サーフェス破棄 */
    if (s_cairo_surface != NULL)
        cairo_surface_destroy (s_cairo_surface);
}

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
    gtk_window_set_title (GTK_WINDOW (window), "2Dグラフィックス Cairoを使ったペイント");
    g_signal_connect (window, "destroy", G_CALLBACK (cb_window_destory), NULL);

    /* 2D DrawingArea設定 */
    {
        GtkWidget *drawing_area = GTK_WIDGET (gtk_builder_get_object (builder, "drawing_area"));
        g_warn_if_fail (drawing_area != NULL);

        g_signal_connect (drawing_area, "draw", G_CALLBACK (cb_draw), NULL);
        g_signal_connect (drawing_area, "configure-event", G_CALLBACK (configure_event_cb), NULL);
        g_signal_connect (drawing_area, "motion-notify-event", G_CALLBACK (cb_motion_notify_event), NULL);
        g_signal_connect (drawing_area, "button-press-event", G_CALLBACK (cb_button_press_event), NULL);

        gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                           | GDK_BUTTON_PRESS_MASK
                                           | GDK_POINTER_MOTION_MASK);
    }

    gtk_widget_show_all (GTK_WIDGET (window));
    g_object_unref (builder);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.cairo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_application_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
