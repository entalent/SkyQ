#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "ui.h"

//void mainUI();
//void new_small_window(const char* title,const char*text);
gint destroy_widget(GtkWidget *widget,GtkWidget *a);
gboolean password_check(const char* username,const char* password);
typedef struct loginmsg{
    GtkWidget* widget;
    GtkWidget* fwidget;
    GtkWidget* window;
    GtkWidget* username;
    GtkWidget* password;
}login_msg;

login_msg logmsg;

extern GtkWidget* login_window;
extern GtkWidget* mainUI_window;
extern GtkWidget* regist_window;
extern GtkWidget* mainUI_window_scwinlist;
extern GtkWidget* mainUI_window_group_list_box;
extern GtkWidget* mainUI_window_about_box;

void login_on_button_clicked(GtkWidget* button,login_msg* data){
	printf("_--------\n");
    const char* username=gtk_entry_get_text(GTK_ENTRY(data->username));
	printf("1--------\n");
    const char* password=gtk_entry_get_text(GTK_ENTRY(data->password));
	printf("2--------\n");
    //puts(username);
    //puts(password);
    if(strlen(username)==0||strlen(password)==0){
        create_new_pop_window("username or password cannot be empty!");
        return ;
    }

	printf("3--------\n");
    if(password_check(username,password)==FALSE){
        return ;
    }
	printf("4--------\n");
    GtkWidget* label=gtk_label_new("waiting login...");
	printf("5--------\n");
    gtk_widget_hide(data->widget);
    gtk_box_pack_start(GTK_BOX(data->fwidget),label,TRUE,TRUE,20);
    gtk_widget_show(label);
	printf("123123\n");
    login(username, password);
    gtk_widget_hide_all(data->window);
    //gtk_main_quit();
    
}

void login_to_regist_on_button_clicked(GtkWidget* button,gpointer data){
    gtk_widget_hide_all(data);
    //gtk_main_quit();
    gtk_widget_show_all(regist_window);
}

gboolean window_drag(GtkWidget *widget, GdkEventButton *event, GdkWindowEdge edge)
{
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button, event->
x_root, event->y_root, event->time);
    }

    return FALSE;
}

GtkWidget* loginUI(){
    GtkWidget* window;
    GtkWidget* main_box;
    GtkWidget* box;
    GtkWidget* urnamebox;
    GtkWidget* psdbox;
    GtkWidget* btnbox;
    GtkWidget* urnamelabel;
    GtkWidget* psdlabel;
    GtkWidget* button;
    GtkWidget* registbtn;
    GtkWidget* urname;
    GtkWidget* password;
    GtkWidget* sep;
    //图片buf
    GdkPixbuf* pixbuf = NULL;
    GdkPixmap* pixmap = NULL;
    GdkBitmap* bitmap = NULL;
    //
    GtkWidget* vbox,*hbox;
    GtkWidget* box1,*box2;
    GtkWidget* quit_button;
	//GtkWidget* mainUI_window=mainUI();
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

    main_box=gtk_vbox_new(FALSE,0);
    vbox = gtk_vbox_new(FALSE,0);
    hbox = gtk_hbox_new(FALSE,0);
    box1 = gtk_vbox_new(FALSE,0);
    box2 = gtk_vbox_new(FALSE,0);
    gtk_window_set_decorated(GTK_WINDOW(window),FALSE);
    //==========================显示小埋===============================
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_size_request(window, 844, 415);
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(window), "button_press_event", G_CALLBACK(window_drag), NULL);


    pixbuf = gdk_pixbuf_new_from_file ("./xiaomai.png",NULL);
    gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &bitmap, 128);
    gtk_widget_shape_combine_mask(window, bitmap, 0, 0);
    GtkStyle *style = gtk_style_copy(window->style);
    if(style->bg_pixmap[GTK_STATE_NORMAL])
        g_object_unref(style->bg_pixmap[GTK_STATE_NORMAL]);
    style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref(pixmap);
    gtk_widget_set_style(window, style);
    //=====================================================================

    gtk_widget_set_size_request(vbox,844,155);
    gtk_box_pack_start(GTK_BOX(main_box),vbox,FALSE,FALSE,0);
    gtk_widget_set_size_request(hbox,844,250);
    gtk_box_pack_start(GTK_BOX(main_box),hbox,FALSE,FALSE,0);
    gtk_widget_set_size_request(box1,400,250);
    gtk_box_pack_start(GTK_BOX(hbox),box1,FALSE,FALSE,0);
    gtk_widget_set_size_request(box2,316,250);
    gtk_box_pack_start(GTK_BOX(hbox),box2,FALSE,FALSE,0);

    gtk_window_set_title(GTK_WINDOW(window),"login");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),20);
    //main box init
    box = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),main_box);
    gtk_box_pack_start(GTK_BOX(box2),box,TRUE,TRUE,0);
    urnamebox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),urnamebox,FALSE,FALSE,5);
    psdbox=gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),psdbox,FALSE,FALSE,5);
    sep=gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,5);
    btnbox=gtk_hbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(box),btnbox,FALSE,FALSE,5);
    //uesrname box init
    urnamelabel = gtk_label_new("uesrname: ");
    urname=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(urnamebox),urnamelabel,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(urnamebox),urname,FALSE,FALSE,5);
    //password box init
    psdlabel = gtk_label_new("password: ");
    password=gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password),FALSE);
    gtk_box_pack_start(GTK_BOX(psdbox),psdlabel,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(psdbox),password,FALSE,FALSE,5);

    button = gtk_button_new_with_label("login");
    registbtn= gtk_button_new_with_label("regist");
    quit_button = gtk_button_new_with_label("Quit");

    //login_msg logmsg;
    logmsg.window=window;
    logmsg.fwidget=main_box;
    logmsg.widget=box;
    logmsg.username=urname;
    logmsg.password=password;
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(login_on_button_clicked),&logmsg);
    //g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
    g_signal_connect(G_OBJECT(registbtn),"clicked",G_CALLBACK(login_to_regist_on_button_clicked),window);
    g_signal_connect(G_OBJECT(quit_button),"clicked",G_CALLBACK(gtk_main_quit),NULL);
    //g_signal_connect_swapped(G_OBJECT(registbtn),"clicked",G_CALLBACK(gtk_widget_destroy),window);
    gtk_box_pack_start(GTK_BOX(btnbox),button,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(btnbox),registbtn,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(btnbox),quit_button,FALSE,FALSE,5);
    //gtk_widget_show_all(window);
    //gtk_main();
    return window;
}
