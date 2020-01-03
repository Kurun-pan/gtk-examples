#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>

void draw_realize (GtkWidget *widget, gpointer user_data);
void draw_unrealize (GtkWidget *widget, gpointer user_data);
gboolean draw_render (GtkWidget *widget, GdkGLContext *context, gpointer user_data);

#endif  // __DRAW_H__
