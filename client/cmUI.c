#include <gtk/gtk.h>
#include <string.h>
#include <malloc.h>
#include "ui.h"

struct sticker_info{
    GtkWidget *sticker_window;
    GtkTextBuffer *view2_buffer;
    char *str;
    struct sticker_info **spointer;
};

static GdkColor Blue = {0,0x2121,0x9696,0xf3f3};
static GdkColor AliceBlue = {0,0xf0f0,0xf8f8,0xffff};

void print_msg(struct text_view_info *viewinfo, const char* info, const char* content);
void print_content(struct text_view_info *viewinfo, const char* content, int state);
void print_sticker(struct text_view_info *viewinfo, const char* content);
void send_button_press(GtkWidget *widget,struct text_view_info *viewinfo);
void sticker_button_press(GtkWidget *widget, GdkEvent *event, struct text_view_info *viewinfo);
void create_message_dialog (GtkMessageType type, gchar* message);
void scroll_to_the_end(struct text_view_info *viewinfo);
void insert_sticker(GtkWidget *widget,GdkEventButton *event, struct sticker_info *sinfo);
void destory_sticker_window(GtkWidget *widget,GdkEventCrossing *event,GtkWidget* data);
void group_friend_list_add(GtkWidget* list, const char *buf);
void group_friend_list_clear(GtkWidget* list);
void update_group_friend_list();
void change_chat_mode(int mode);
void update_group_friend_list(int num,struct group_username * name);
void set_chat_title(GtkWidget *cmlabel, const char *buf);

extern GtkWidget *cmeventbox2;
//3rd arg
extern GtkWidget *group_friend_list;

extern struct text_view_info viewinfo;
extern GtkWidget *cmlabel;
extern GtkWidget *scwinlist;


void send_button_press(GtkWidget *widget,struct text_view_info *viewinfo){
    GtkTextIter start, end;

    gtk_text_buffer_get_start_iter(viewinfo->view2_buffer,&start);
    gtk_text_buffer_get_end_iter(viewinfo->view2_buffer,&end);

    char *buffer=gtk_text_buffer_get_text(viewinfo->view2_buffer,&start,&end,TRUE);//FALSE);/
    int len=strlen(buffer);

    if (len > 0){
        //print_msg(viewinfo,"我：\n",buffer);
        gtk_text_buffer_set_text(viewinfo->view2_buffer,"", -1);
	send_message_callback(buffer);
    }
    else create_message_dialog(GTK_MESSAGE_INFO,"The message is not allowed to be empty!");
}

void print_msg(struct text_view_info *viewinfo, const char* info, const char* content){
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
    gtk_text_buffer_insert_with_tags_by_name(viewinfo->view1_buffer,&end,info,-1,"blue_foreground",NULL);
    //gtk_text_buffer_insert(buffer,&end,content,-1);
    print_content(viewinfo,content,1);
    scroll_to_the_end(viewinfo);
}

void print_content(struct text_view_info *viewinfo, const char* content, int state){
    GtkTextIter end;
    int i=0;
    if (strlen(content) == 0) return;
    while (1){
        if (content[i] == '\0') {
            gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
            gtk_text_buffer_insert(viewinfo->view1_buffer,&end,content,-1);
            break;
        }
        else if (content[i] == '/') {
            //打印处'/'前面的部分
            if (i > 0){
                char precontent[i+1];
                int j=0;
                for (j=0;j<i;j++) precontent[j]=content[j];
                precontent[j]='\0';
                gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
                gtk_text_buffer_insert(viewinfo->view1_buffer,&end,precontent,-1);
            }

            //如果是表情，插入缓冲区,否则将这段字符插入
            if (content[i+1] != '\0' && content[i+2] != '\0'){
                char tmp[4];
                tmp[0]='/';
                tmp[1]=content[i+1];
                tmp[2]=content[i+2];
                tmp[3]='\0';
                print_sticker(viewinfo,tmp);

                //打印后面部分
                print_content(viewinfo,&content[i+3],0);
                break;
            }
            else {
                gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
                gtk_text_buffer_insert(viewinfo->view1_buffer,&end,"/",-1);
                print_content(viewinfo,&content[i+1],0);
                break;
            }
        }
        i++;
    }
    if (state == 1) {
        gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
        gtk_text_buffer_insert(viewinfo->view1_buffer,&end,"\n",-1);
    }
}

void print_sticker(struct text_view_info *viewinfo, const char* content){
    GtkTextIter end;
    GtkWidget *image;
    GtkTextChildAnchor *anchor;

    gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);

    int flag = 1;
    if (strcmp(content,"/am") == 0) image = gtk_image_new_from_file("./sticker/am.gif");
    else if (strcmp(content,"/dk") == 0) image = gtk_image_new_from_file("./sticker/dk.gif");
    else if (strcmp(content,"/fd") == 0) image = gtk_image_new_from_file("./sticker/fd.gif");
    else if (strcmp(content,"/sk") == 0) image = gtk_image_new_from_file("./sticker/sk.gif");
    else if (strcmp(content,"/wx") == 0) image = gtk_image_new_from_file("./sticker/wx.gif");
    else if (strcmp(content,"/zj") == 0) image = gtk_image_new_from_file("./sticker/zj.gif");
    else flag = 0;

    if (flag){
        anchor = gtk_text_buffer_create_child_anchor(viewinfo->view1_buffer,&end);//创建子控件的位置标记
        gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(viewinfo->view1),image,anchor);
        gtk_widget_show(image);
    }
    else gtk_text_buffer_insert(viewinfo->view1_buffer,&end,content,-1);
}

void sticker_button_press(GtkWidget *widget, GdkEvent *event, struct text_view_info *viewinfo){
    if(event->type == GDK_BUTTON_RELEASE){
        GdkEventButton *event_button = (GdkEventButton *) event;
        if(event_button->button == 1){
            GtkWidget *sticker_window;
            GtkWidget *eventbox;
            GtkWidget *sbox1, *sbox2, *sbox3, *sbox4, *sbox5, *sbox6;  //盛放6个表情的eventbox
            GtkWidget *table;
            GtkWidget *s1, *s2, *s3, *s4, *s5, *s6; //6个表情的image

            sticker_window = gtk_window_new(GTK_WINDOW_POPUP);
            gtk_window_set_default_size(GTK_WINDOW(sticker_window),100,100);
            gtk_widget_set_size_request(sticker_window,200,100);
            gtk_window_set_position(GTK_WINDOW(sticker_window), GTK_WIN_POS_MOUSE);

            eventbox = gtk_event_box_new();
            g_signal_connect(G_OBJECT(eventbox),"leave_notify_event",G_CALLBACK(destory_sticker_window),sticker_window);
            gtk_container_add(GTK_CONTAINER(sticker_window),eventbox);

            table = gtk_table_new(2,3,FALSE);
            gtk_container_add(GTK_CONTAINER(eventbox),table);

            sbox1 = gtk_event_box_new();
            sbox2 = gtk_event_box_new();
            sbox3 = gtk_event_box_new();
            sbox4 = gtk_event_box_new();
            sbox5 = gtk_event_box_new();
            sbox6 = gtk_event_box_new();

            struct sticker_info **spointer = (struct sticker_info **)malloc(sizeof(struct sticker_info *)*6);

            struct sticker_info *sinfo1 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo1->str = "/am";sinfo1->view2_buffer = viewinfo->view2_buffer;
            sinfo1->sticker_window = sticker_window;sinfo1->spointer = spointer;
            struct sticker_info *sinfo2 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo2->str = "/dk";sinfo2->view2_buffer = viewinfo->view2_buffer;
            sinfo2->sticker_window = sticker_window;sinfo2->spointer = spointer;
            struct sticker_info *sinfo3 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo3->str = "/fd";sinfo3->view2_buffer = viewinfo->view2_buffer;
            sinfo3->sticker_window = sticker_window;sinfo3->spointer = spointer;
            struct sticker_info *sinfo4 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo4->str = "/sk";sinfo4->view2_buffer = viewinfo->view2_buffer;
            sinfo4->sticker_window = sticker_window;sinfo4->spointer = spointer;
            struct sticker_info *sinfo5 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo5->str = "/wx";sinfo5->view2_buffer = viewinfo->view2_buffer;
            sinfo5->sticker_window = sticker_window;sinfo5->spointer = spointer;
            struct sticker_info *sinfo6 = (struct sticker_info *)malloc(sizeof(struct sticker_info));
            sinfo6->str = "/zj";sinfo6->view2_buffer = viewinfo->view2_buffer;
            sinfo6->sticker_window = sticker_window;sinfo6->spointer = spointer;

            spointer[0] = sinfo1;
            spointer[1] = sinfo2;
            spointer[2] = sinfo3;
            spointer[3] = sinfo4;
            spointer[4] = sinfo5;
            spointer[5] = sinfo6;

            g_signal_connect(G_OBJECT(sbox1),"button_press_event",G_CALLBACK(insert_sticker),sinfo1);
            g_signal_connect(G_OBJECT(sbox2),"button_press_event",G_CALLBACK(insert_sticker),sinfo2);
            g_signal_connect(G_OBJECT(sbox3),"button_press_event",G_CALLBACK(insert_sticker),sinfo3);
            g_signal_connect(G_OBJECT(sbox4),"button_press_event",G_CALLBACK(insert_sticker),sinfo4);
            g_signal_connect(G_OBJECT(sbox5),"button_press_event",G_CALLBACK(insert_sticker),sinfo5);
            g_signal_connect(G_OBJECT(sbox6),"button_press_event",G_CALLBACK(insert_sticker),sinfo6);

            s1 = gtk_image_new_from_file("./sticker/am.gif");
            s2 = gtk_image_new_from_file("./sticker/dk.gif");
            s3 = gtk_image_new_from_file("./sticker/fd.gif");
            s4 = gtk_image_new_from_file("./sticker/sk.gif");
            s5 = gtk_image_new_from_file("./sticker/wx.gif");
            s6 = gtk_image_new_from_file("./sticker/zj.gif");

            gtk_container_add(GTK_CONTAINER(sbox1),s1);
            gtk_container_add(GTK_CONTAINER(sbox2),s2);
            gtk_container_add(GTK_CONTAINER(sbox3),s3);
            gtk_container_add(GTK_CONTAINER(sbox4),s4);
            gtk_container_add(GTK_CONTAINER(sbox5),s5);
            gtk_container_add(GTK_CONTAINER(sbox6),s6);

            gtk_table_attach_defaults(GTK_TABLE(table),sbox1,0,1,0,1);
            gtk_table_attach_defaults(GTK_TABLE(table),sbox2,1,2,0,1);
            gtk_table_attach_defaults(GTK_TABLE(table),sbox3,2,3,0,1);
            gtk_table_attach_defaults(GTK_TABLE(table),sbox4,0,1,1,2);
            gtk_table_attach_defaults(GTK_TABLE(table),sbox5,1,2,1,2);
            gtk_table_attach_defaults(GTK_TABLE(table),sbox6,2,3,1,2);

            gtk_widget_show_all(sticker_window);
        }
    }
}

void create_message_dialog (GtkMessageType type, gchar* message){
    GtkWidget* dialogx;
    dialogx = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,type,GTK_BUTTONS_OK,message);
    gtk_dialog_run(GTK_DIALOG(dialogx));
    gtk_widget_destroy(dialogx);
}

void scroll_to_the_end(struct text_view_info *viewinfo){
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(viewinfo->view1_buffer,&end);
    GtkTextMark *mark=gtk_text_buffer_create_mark(viewinfo->view1_buffer,NULL,&end,1);
    gtk_text_buffer_move_mark(viewinfo->view1_buffer,mark,&end);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(viewinfo->view1),mark,0,1,1,1);
}

void insert_sticker(GtkWidget *widget,GdkEventButton *event, struct sticker_info *sinfo){
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(sinfo->view2_buffer,&end);
    gtk_text_buffer_insert(sinfo->view2_buffer,&end,sinfo->str,-1);

    GtkWidget *sticker_window = sinfo->sticker_window;
    struct sticker_info **p = sinfo->spointer;

    int i;
    for (i=0;i<6;i++) free(p[i]);
    free(p);

    gtk_widget_destroy(sticker_window);
}

void destory_sticker_window(GtkWidget *widget,GdkEventCrossing *event,GtkWidget* data){
    gtk_widget_destroy(data);
}

void group_friend_list_add(GtkWidget* list, const char *buf){
	GtkWidget *item;
	//创建一个列表项
	item=gtk_list_item_new_with_label(buf);
	gtk_container_add(GTK_CONTAINER(list),item);
	gtk_widget_show(item);
}

void group_friend_list_clear(GtkWidget* list){	//
	gtk_list_clear_items(GTK_LIST(list),0,-1);
}

void change_chat_mode(int mode){
    if (mode == 1){
        gtk_widget_show (cmeventbox2);
        gtk_widget_hide(scwinlist);
    }
    else {
        gtk_widget_show (scwinlist);
        gtk_widget_hide(cmeventbox2);
    }
}

void update_group_friend_list(int num,struct group_username *name){   //!
    group_friend_list_clear(group_friend_list);
    int i = 0;

    for (i=0;i<num;i++) {
		printf("name[%d] = %s\n", i, name[i].username);
		group_friend_list_add(group_friend_list, name[i].username);
	}
}

void set_chat_title(GtkWidget *cmlabel, const char *buf){
    gtk_label_set_markup(GTK_LABEL(cmlabel),buf);
}

void clear_chat_window(){
    gtk_text_buffer_set_text(viewinfo.view1_buffer,"", -1);
}






