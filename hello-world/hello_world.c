#include <gtk/gtk.h>

static void activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *label;

    /* ウインドウ生成 */
    window = gtk_application_window_new (app);

    /* ウインドウに表示されるタイトル */
    gtk_window_set_title (GTK_WINDOW (window), "Hello world");

    /* ウインドウサイズの指定 */
    gtk_widget_set_size_request (window, 640, 480);

    /* ウインドウ表示位置を中央に指定 */
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS);

    /* ラベルをウインドウに追加 */
    label = gtk_label_new ("Hello world");
    gtk_container_add (GTK_CONTAINER (window), label);

    /* ウインドウの表示 */
    gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    /* アプリケーション作成. gtk_initはこの内部で呼ばれるため、不要 */
    app = gtk_application_new ("org.gtk3.helloworld", G_APPLICATION_FLAGS_NONE);

    /* GtkApplicationインスタンス生成後最初のシグナルの設定 */
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    /* アプリケーション起動&メインループ */
    status = g_application_run (G_APPLICATION (app), argc, argv);

    /* アプリケーションインスタンス解放 */
    g_object_unref (app);

    return status;
}

