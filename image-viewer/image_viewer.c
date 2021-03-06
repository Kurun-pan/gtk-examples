#include <gtk/gtk.h>

static void cb_button_quit_clicked (GtkWidget *button, gpointer user_data)
{
    GApplication *app = G_APPLICATION (user_data);
    g_application_quit (app);
}

static void cb_button_chooser_clicked (GtkWidget *button, gpointer user_data)
{
    /* ウィンドウの取得 */
    GtkApplication* app = GTK_APPLICATION (user_data);
    GtkWindow *window = gtk_application_get_active_window (app);

    /* ファイル選択ダイアログ生成 */
    GtkWidget *dialog = gtk_file_chooser_dialog_new (
                                        "画像ファイルを選択",
                                        GTK_WINDOW (window),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "キャンセル",
                                        GTK_RESPONSE_CANCEL,
                                        "選択",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);

    /* ダイアログ表示 */
    gtk_widget_show_all (dialog);

    /* ダイアログによるファイル選択処理 */
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        /* 選択したファイル名の取得 */
        gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

        /* 画像表示エリアの取得 */
        GtkWidget *image = GTK_WIDGET (g_object_get_data (G_OBJECT (window), "image"));
        g_return_if_fail (image != NULL);

        /* 画像ファイルをセット */
        gtk_image_set_from_file (GTK_IMAGE (image), filename);
        g_free (filename);
    }

    /* ダイアログの破棄 */
    gtk_widget_destroy (dialog);
}

static void cb_app_activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new (GTK_APPLICATION (app));

    gtk_widget_set_size_request (GTK_WIDGET (window), 640, 480);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS);

    /* 縦にウィジェットを配置するボックスの作成 */
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL , 2);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    {
        /* 画像表示エリアの設定 */
        GtkWidget *image = gtk_image_new ();
        g_object_set_data (G_OBJECT (window), "image", (gpointer)image);
        gtk_box_pack_start (GTK_BOX (vbox), image, TRUE, TRUE, 0);
    }
    {
        /* フッターメニュー用のコンテナを作成 */
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
        gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

        /* 終了ボタンの設定 */
        GtkWidget *button_quit = gtk_button_new_with_label("終了");
        g_signal_connect (button_quit, "clicked", G_CALLBACK (cb_button_quit_clicked), app);
        gtk_box_pack_start (GTK_BOX (hbox), button_quit, TRUE, TRUE, 0);
        
        /* 画像ファイル選択ボタンの設定 */
        GtkWidget *button_chooser = gtk_button_new_with_label("画像ファイル選択");
        g_signal_connect (button_chooser, "clicked", G_CALLBACK (cb_button_chooser_clicked), app);
        gtk_box_pack_start (GTK_BOX (hbox), button_chooser, TRUE, TRUE, 0);
    }

    gtk_widget_show_all (GTK_WIDGET (window));
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.image-viewer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_app_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}

