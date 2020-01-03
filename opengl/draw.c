#include <gtk/gtk.h>
#include <epoxy/gl.h>
#include <math.h>
#include "draw.h"

enum {
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2,

    N_AXIS = Z_AXIS + 1
};

/* Rotation angles on each axis */
static float rotation_angles[N_AXIS] = { 0.0 };

/* The object we are drawing */
static const GLfloat vertex_data[] = {
    0.f,   0.5f,   0.f, 1.f,
    0.5f, -0.366f, 0.f, 1.f,
   -0.5f, -0.366f, 0.f, 1.f,
};

static GLuint position_buffer;
static GLuint program;
static GLuint mvp_location;

/* Initialize the GL buffers */
static void init_buffers (GLuint *vao_out, GLuint *buffer_out)
{
    GLuint vao, buffer;

    /* We only use one VAO, so we always keep it bound */
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    /* This is the buffer that holds the vertices */
    glGenBuffers (1, &buffer);
    glBindBuffer (GL_ARRAY_BUFFER, buffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_data), vertex_data, GL_STATIC_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    if (vao_out != NULL)
        *vao_out = vao;

    if (buffer_out != NULL)
        *buffer_out = buffer;
}

/* Create and compile a shader */
static GLuint create_shader (int type, const char *src)
{
    GLuint shader;
    int status;

    shader = glCreateShader (type);
    glShaderSource (shader, 1, &src, NULL);
    glCompileShader (shader);

    glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len;
        char *buffer;

        glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_len);

        buffer = g_malloc (log_len + 1);
        glGetShaderInfoLog (shader, log_len, NULL, buffer);

        g_warning ("Compile failure in %s shader:\n%s",
                   type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                   buffer);

        g_free (buffer);

        glDeleteShader (shader);

        return 0;
    }

    return shader;
}

/* Initialize the shaders and link them into a program */
static void init_shaders (const char *vertex_path, const char *fragment_path, GLuint *program_out, GLuint *mvp_out)
{
    GLuint vertex, fragment;
    GLuint program = 0;
    GLuint mvp = 0;
    int status;
    GBytes *source;

    source = g_resources_lookup_data (vertex_path, 0, NULL);
    vertex = create_shader (GL_VERTEX_SHADER, g_bytes_get_data (source, NULL));
    g_bytes_unref (source);

    if (vertex == 0) {
        *program_out = 0;
        return;
    }

    source = g_resources_lookup_data (fragment_path, 0, NULL);
    fragment = create_shader (GL_FRAGMENT_SHADER, g_bytes_get_data (source, NULL));
    g_bytes_unref (source);

    if (fragment == 0) {
        glDeleteShader (vertex);
        *program_out = 0;
        return;
    }

    program = glCreateProgram ();
    glAttachShader (program, vertex);
    glAttachShader (program, fragment);

    glLinkProgram (program);

    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len;
        char *buffer;

        glGetProgramiv (program, GL_INFO_LOG_LENGTH, &log_len);

        buffer = g_malloc (log_len + 1);
        glGetProgramInfoLog (program, log_len, NULL, buffer);

        g_warning ("Linking failure:\n%s", buffer);

        g_free (buffer);

        glDeleteProgram (program);
        program = 0;

        goto out;
    }

    /* Get the location of the "mvp" uniform */
    mvp = glGetUniformLocation (program, "mvp");

    glDetachShader (program, vertex);
    glDetachShader (program, fragment);

out:
    glDeleteShader (vertex);
    glDeleteShader (fragment);

    if (program_out != NULL)
        *program_out = program;

    if (mvp_out != NULL)
        *mvp_out = mvp;
}

static void compute_mvp (float *res, float phi, float theta, float psi)
{
    float x = phi * (G_PI / 180.f);
    float y = theta * (G_PI / 180.f);
    float z = psi * (G_PI / 180.f);
    float c1 = cosf (x), s1 = sinf (x);
    float c2 = cosf (y), s2 = sinf (y);
    float c3 = cosf (z), s3 = sinf (z);
    float c3c2 = c3 * c2;
    float s3c1 = s3 * c1;
    float c3s2s1 = c3 * s2 * s1;
    float s3s1 = s3 * s1;
    float c3s2c1 = c3 * s2 * c1;
    float s3c2 = s3 * c2;
    float c3c1 = c3 * c1;
    float s3s2s1 = s3 * s2 * s1;
    float c3s1 = c3 * s1;
    float s3s2c1 = s3 * s2 * c1;
    float c2s1 = c2 * s1;
    float c2c1 = c2 * c1;

    /* initialize to the identity matrix */
    res[0] = 1.f; res[4] = 0.f;  res[8] = 0.f; res[12] = 0.f;
    res[1] = 0.f; res[5] = 1.f;  res[9] = 0.f; res[13] = 0.f;
    res[2] = 0.f; res[6] = 0.f; res[10] = 1.f; res[14] = 0.f;
    res[3] = 0.f; res[7] = 0.f; res[11] = 0.f; res[15] = 1.f;

    /* apply all three rotations using the three matrices:
     *
     * ⎡  c3 s3 0 ⎤ ⎡ c2  0 -s2 ⎤ ⎡ 1   0  0 ⎤
     * ⎢ -s3 c3 0 ⎥ ⎢  0  1   0 ⎥ ⎢ 0  c1 s1 ⎥
     * ⎣   0  0 1 ⎦ ⎣ s2  0  c2 ⎦ ⎣ 0 -s1 c1 ⎦
     */
    res[0] = c3c2;  res[4] = s3c1 + c3s2s1;  res[8] = s3s1 - c3s2c1; res[12] = 0.f;
    res[1] = -s3c2; res[5] = c3c1 - s3s2s1;  res[9] = c3s1 + s3s2c1; res[13] = 0.f;
    res[2] = s2;    res[6] = -c2s1;         res[10] = c2c1;          res[14] = 0.f;
    res[3] = 0.f;   res[7] = 0.f;           res[11] = 0.f;           res[15] = 1.f;
}

static void draw_triangle ()
{
    float mvp[16];

    /* Compute the model view projection matrix using the
     * rotation angles specified through the GtkRange widgets
     */
    compute_mvp (mvp, rotation_angles[X_AXIS], rotation_angles[Y_AXIS], rotation_angles[Z_AXIS]);

    /* Use our shaders */
    glUseProgram (program);

    /* Update the "mvp" matrix we use in the shader */
    glUniformMatrix4fv (mvp_location, 1, GL_FALSE, &mvp[0]);

    /* Use the vertices in our buffer */
    glBindBuffer (GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /* Draw the three vertices as a triangle */
    glDrawArrays (GL_TRIANGLES, 0, 3);

    /* We finished using the buffers and program */
    glDisableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glUseProgram (0);
}

void draw_realize (GtkWidget *widget, gpointer user_data)
{
    const char *vertex_path, *fragment_path;
    GdkGLContext *context;

    gtk_gl_area_make_current (GTK_GL_AREA (widget));
    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;

    context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
    if (gdk_gl_context_get_use_es (context)) {
        vertex_path = "/gtk/examples/opengl/glarea-gles.vs.glsl";
        fragment_path = "/gtk/examples/opengl/glarea-gles.fs.glsl";
    }
    else {
        vertex_path = "/gtk/examples/opengl/glarea-gl.vs.glsl";
        fragment_path = "/gtk/examples/opengl/glarea-gl.fs.glsl";
    }

    init_buffers (&position_buffer, NULL);
    init_shaders (vertex_path, fragment_path, &program, &mvp_location);
}

void draw_unrealize (GtkWidget *widget, gpointer user_data)
{
    gtk_gl_area_make_current (GTK_GL_AREA (widget));
    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;

    glDeleteBuffers (1, &position_buffer);
    glDeleteProgram (program);
}

gboolean draw_render (GtkWidget *widget, GdkGLContext *context, gpointer user_data)
{
    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
      return FALSE;

    /* Clear the viewport */
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT);

    /* Draw our object */
    draw_triangle ();

    /* Flush the contents of the pipeline */
    glFlush ();

    return TRUE;
}
