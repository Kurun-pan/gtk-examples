#include <gtk/gtk.h>
#include <libintl.h>

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
                                        gettext("画像ファイル選択"),
                                        GTK_WINDOW (window),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        gettext("キャンセル"),
                                        GTK_RESPONSE_CANCEL,
                                        gettext("選択"),
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
    GtkBuilder* builder = gtk_builder_new ();
    g_return_if_fail (builder != NULL);

    /* gladeファイルから画面情報を読み込み */
    GError* error = NULL;
    if (!gtk_builder_add_from_file (builder, "./image_viewer.glade", &error)) {
        if (error) {
            g_error ("Failed to load: %s", error->message);
            g_error_free (error);
            return ;
        }
    }

    /* IDを指定して、GtkApplicationWindowを取得 */
    GtkApplicationWindow *window = GTK_APPLICATION_WINDOW (gtk_builder_get_object (builder, "application_window"));
    g_warn_if_fail (window != NULL);
    g_object_set (window, "application", app, NULL);

    gtk_widget_set_size_request (GTK_WIDGET (window), 640, 480);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS);

    /* 終了ボタンの設定 */
    GtkWidget *button_quit = GTK_WIDGET (gtk_builder_get_object (builder, "button_quit"));
    g_signal_connect (button_quit, "clicked", G_CALLBACK (cb_button_quit_clicked), app);
    
    /* 画像ファイル選択ボタンの設定 */
    GtkWidget *button_chooser = GTK_WIDGET (gtk_builder_get_object (builder, "button_chooser"));
    g_signal_connect (button_chooser, "clicked", G_CALLBACK (cb_button_chooser_clicked), app);

    /* 画像表示エリアの設定 */
    GtkWidget *image = GTK_WIDGET (gtk_builder_get_object (builder, "image"));
    g_object_set_data (G_OBJECT (window), "image", (gpointer)image);

    gtk_widget_show_all (GTK_WIDGET (window));
    g_object_unref (builder);
}

static void init_locale()
{
    /* システム設定のlocaleに従う */
    setlocale(LC_ALL, "");

    /* ./locale/フォルダ以下の利用するmoファイルを指定 */
    bindtextdomain("strings",  "./locale/");

    /* 使用するドメイン(moファイル名)の指定 */
    textdomain("strings");
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    /* ローカライズ設定 */
    init_locale();

    app = gtk_application_new ("org.gtk3.image-viewer-localized", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (cb_app_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}

