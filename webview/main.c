#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static gchar s_url[256] = { 0 };

static gboolean cb_close_webview (WebKitWebView *webView, gpointer user_data)
{
    GtkWidget *window = GTK_WIDGET (user_data);
    gtk_widget_destroy (window);

    return TRUE;
}

static gint cb_command_line (GtkApplication* app, GApplicationCommandLine *command_line, gpointer user_data)
{
    gchar **argv;
    gint argc;

    /* コマンドライン引数情報を取得 */
    argv = g_application_command_line_get_arguments (command_line, &argc);
    if (argc == 2) {
        /* グローバル変数に保存 */
        sprintf (s_url, "%s", argv[1]);

        /* GtkApplicationをアクティベート */
        g_application_activate (G_APPLICATION (app));
    }
    else
      g_print ("Usage: webkit_sample URL\n");

    /* 取得したコマンドライン引数の開放処理が必須です */
    g_strfreev (argv);

    return 0;
}

static void cb_activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Webviewサンプル");
    gtk_window_set_default_size (GTK_WINDOW (window), 1024, 768);

    /**
     * WebKitGTK Reference Manual for WebKitGTK 2.26.2
     * https://webkitgtk.org/reference/webkit2gtk/2.26.2/
     **/
    // WebKit
    // API refs: https://webkitgtk.org/reference/webkit2gtk/2.26.2/WebKitWebView.html
    WebKitWebContext *context = webkit_web_context_new ();
#if 0
    WebKitNetworkProxySettings *proxy_settings = webkit_network_proxy_settings_new ("http://proxy-host-name:12345/", NULL);
    webkit_web_context_set_network_proxy_settings (context, WEBKIT_NETWORK_PROXY_MODE_CUSTOM, proxy_settings);
#endif

    GtkWidget *webView = webkit_web_view_new_with_context (context);

    /* WebKitSettings */
    /* API refs: https://webkitgtk.org/reference/webkit2gtk/2.26.2/WebKitSettings.html */
    {
        WebKitSettings *settings = webkit_web_view_get_settings (WEBKIT_WEB_VIEW (webView));
        /* JavaScriptを有効にする */
        webkit_settings_set_enable_javascript (settings, TRUE);
        /* 2D描画処理のハードウェアアクセラレーション有効にする */
        webkit_settings_set_enable_accelerated_2d_canvas (settings, TRUE);
        webkit_web_view_set_settings (WEBKIT_WEB_VIEW (webView), settings);
    }

    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (webView));
    g_signal_connect (webView, "close", G_CALLBACK (cb_close_webview), window);

    g_print ("URL = %s\n", s_url);
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webView), s_url);

    gtk_widget_grab_focus (GTK_WIDGET (webView));
    gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk3.webkit.example", G_APPLICATION_FLAGS_NONE | G_APPLICATION_HANDLES_COMMAND_LINE);
    g_signal_connect (app, "command-line", G_CALLBACK (cb_command_line), NULL);
    g_signal_connect (app, "activate", G_CALLBACK (cb_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
