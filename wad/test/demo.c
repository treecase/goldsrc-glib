#include <gio/gio.h>
#include <gtk/gtk.h>
#include <wad/wad.h>

static GdkPaintable *make_miptex_paintable(WadMiptexFile *miptex)
{
    guchar const *image = (guchar *)miptex->mip_images[0]->data;
    WadRgb const *palette = (WadRgb *)miptex->palette->data;

    guchar *data = g_new(guchar, miptex->width * miptex->height * 4);
    for (size_t i = 0; i < miptex->width * miptex->height; ++i) {
        guchar pixel = image[i];
        data[(i * 4) + 0] = palette[pixel].rgb[0];
        data[(i * 4) + 1] = palette[pixel].rgb[1];
        data[(i * 4) + 2] = palette[pixel].rgb[2];
        data[(i * 4) + 3] = 0xff;
    }

    g_autoptr(GBytes) bytes
        = g_bytes_new_take(data, miptex->width * miptex->height * 4);

    GdkTexture *texture = gdk_memory_texture_new(
        miptex->width,
        miptex->height,
        GDK_MEMORY_R8G8B8A8,
        bytes,
        miptex->width * 4
    );
    return GDK_PAINTABLE(texture);
}

// Application

static void activate(GApplication *)
{
    g_printerr("Usage: wad-test WADFILE\n");
}

static void open(GApplication *application, GFile *files[], int, char *, void *)
{
    GFile *file = files[0];
    g_autofree char *basename = g_file_get_basename(file);
    g_print("Opening %s...\n", basename);

    g_autoptr(WadRoot) root = wad_root_new();
    g_autoptr(GError) error = nullptr;
    wad_root_load_from_file(root, file, &error);
    if (error) {
        g_printerr(
            "%s: wad_root_load_from_file -- %s\n",
            basename,
            error->message
        );
        return;
    }
    g_print("Load successful.\n");

    GtkWidget *flowbox = gtk_flow_box_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scrolled_window),
        flowbox
    );
    GtkWidget *window
        = gtk_application_window_new(GTK_APPLICATION(application));
    gtk_window_set_child(GTK_WINDOW(window), scrolled_window);
    gtk_widget_set_size_request(window, 1280, 960);

    WadTextureArchive *archive = wad_root_get_archive(root);
    g_autofree char const **names = wad_texture_archive_get_names(archive);
    for (char const **name = names; *name != nullptr; ++name) {
        GValue const *value = wad_texture_archive_get_texture(archive, *name);
        GType const type = G_VALUE_TYPE(value);
        if (type == WAD_TYPE_FONT_FILE) {
            WadFontFile *font = g_value_get_boxed(value);
        } else if (type == WAD_TYPE_MIPTEX_FILE) {
            WadMiptexFile *miptex = g_value_get_boxed(value);

            g_autoptr(GdkPaintable) texture = make_miptex_paintable(miptex);

            GtkWidget *picture = gtk_picture_new_for_paintable(texture);
            gtk_picture_set_can_shrink(GTK_PICTURE(picture), FALSE);
            gtk_widget_set_halign(picture, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(picture, GTK_ALIGN_CENTER);

            GtkWidget *label = gtk_label_new(*name);

            GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
            gtk_box_append(GTK_BOX(box), picture);
            gtk_box_append(GTK_BOX(box), label);

            gtk_flow_box_append(GTK_FLOW_BOX(flowbox), box);
        } else if (type == WAD_TYPE_QPIC_FILE) {
            WadQpicFile *font = g_value_get_boxed(value);
        }
    }

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[])
{
    g_autoptr(GtkApplication) application = gtk_application_new(
        "com.github.treecase.goldsrc.WadTest",
        G_APPLICATION_HANDLES_OPEN
    );
    g_signal_connect(application, "activate", G_CALLBACK(activate), nullptr);
    g_signal_connect(application, "open", G_CALLBACK(open), nullptr);
    return g_application_run(G_APPLICATION(application), argc, argv);
}
