#include <gtk/gtk.h>

int main (int argc, char** argv)
{
    GtkWidget *window;
    GtkWidget *label;

    /* GTK初期化 */
    gtk_init (&argc, &argv);

    /* ウインドウ生成 */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* ウインドウに表示されるタイトル */
    gtk_window_set_title (GTK_WINDOW (window), "Hello world");

    /* ウインドウ破棄時のコールバック設定 */
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

    /* ウインドウサイズの指定 */
    gtk_widget_set_size_request (window, 640, 480);

    /* ウインドウ表示位置を中央に指定 */
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS);

    /* ラベルをウインドウに追加 */
    label = gtk_label_new ("Hello world");
    gtk_container_add (GTK_CONTAINER (window), label);

    /* ウインドウの表示 */
    gtk_widget_show_all (window);

    /* メインループ */
    gtk_main ();

    return 0;
}

