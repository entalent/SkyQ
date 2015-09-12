#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ui.h"
//颜色
static GdkColor moved = {0,0xffff,0xe4e4,0xc4c4};
static GdkColor white= {0,0xffff,0xffff,0xffff};
static GdkColor focus={0,0xf4f4,0xa4a4,0x6060};
static GdkColor skyblue={0,0x8787,0xcece,0xebeb};
static GdkColor red={0,0xffff,0,0};
static GdkColor Blue = {0,0x2121,0x9696,0xf3f3};
static GdkColor AliceBlue = {0,0xf0f0,0xf8f8,0xffff};
//static GdkColor pink={0,0x,0x,0x};

typedef GdkColor color;
typedef gushort gs;

struct sticker_info{
    GtkWidget *sticker_window;
    GtkTextBuffer *view2_buffer;
    char *str;
    struct sticker_info **spointer;
};
int maximize_flag=0;
int init_welcome_flag=1;
int now_id;
int n_g,t_t_g,f_g,l,n_f,l_f;
n_g = 0;
l = -1;
n_f = 0;
l_f = -1;
f_g = -1;
struct widget_pack sky_group,sky_friend;
GtkWidget *group_box[N];//eventbox
GtkWidget *label_grouplist[N];	//label_grouplist
GtkWidget *group_page;
int *num_group = &n_g;
int *talk_to_group = &t_t_g;//当前面向的群组
int *focus_group = &f_g;	//刷新前面向的群组
char group_ID[100];//搜索时用
int *lastfocus = &l;
struct args_grouplist args_for_callback_grouplist[N];//回调函数参数
struct group_profile attribute_group[N];		 //eventbox属性
struct group_profile groupdata[N]; //获取的群组属性
GtkWidget *friend_box[N];//eventbox
GtkWidget *label_friendlist[N];	//label_friendlist
GtkWidget *friend_page;
int *num_friend = &n_f;
char talk_to_user[100];
char focus_user[100];
char friend_ID[100];//搜索时用
int *lastfocus_friend = &l_f;
struct args_friendlist args_for_callback_friendlist[N];//回调函数参数
struct user attribute_friend[N];		 //eventbox属性
struct user_profile userdata[N]; //获取的用户属性
int group_list_size;
int friend_list_size;

GtkWidget* login_window;
GtkWidget* mainUI_window;
GtkWidget* regist_window;
GtkWidget* mainUI_window_scwinlist;
GtkWidget* mainUI_window_group_list_box;
GtkWidget* mainUI_window_about_box;

GtkWidget *cmeventbox1,*cmeventbox2, *cmeventbox3 ;
GtkWidget *scwinlist;

//===============================================================
//用于改变左边按钮栏的颜色
typedef struct btn_swap{
    GtkWidget* main_box;
    GtkWidget* main_window[3];
    GtkWidget* main_button[3];
    GtkWidget* chat_box;
    GtkWidget* init_welcome_box;
    int* now_id;
    int aim_id;
    int* init_welcome_flag;
}window_status;
window_status w_status[3];
//用于最大化/还原窗口
typedef struct maximize_state{
    int *state;
    GtkWidget* window;
}maximize_state;
maximize_state maxi_state;

//============================================
struct widget_pack create_grouplist(int *num_group,int *talk_to_group,char *group_ID,int *focus_group,int *lastfocus,struct args_grouplist *args_for_callback_grouplist,struct group_profile *attribute_group,struct group_profile *groupdata,GtkWidget **group_box,GtkWidget **label_grouplist);//创建群列表
gint update_grouplist(struct args_grouplist *tmp,struct group_profile *attribute_group,struct group_profile *data,int *num_group,int *lastfocus,int *talk_to_group,int *focus_group,GtkWidget *group_page,GtkWidget **group_box,GtkWidget **label_grouplist);//更新群列表

struct widget_pack create_friendlist(int *num_friend,char *talk_to_user,char *friend_ID,char *focus_user,int *lastfocus_friend,struct args_friendlist *args_for_callback_friendlist,struct user *attribute_friend,struct user_profile *userdata,GtkWidget **friend_box,GtkWidget **label_friendlist);//创建好友列表
gint update_friendlist(struct args_friendlist *tmp,struct user *attribute_friend,struct user_profile *data,int *num_friend,int *lastfocus_friend,char *talk_to_user,char *focus_user,GtkWidget *friend_page,GtkWidget **friend_box,GtkWidget **label_friendlist);//更新好友列表

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
void change_chat_mode(int mode);


//左边按钮栏的点击回调函数
void on_main_button_clicked(GtkWidget* button,GdkEventButton *event,window_status* data){
    int now_id=*(data->now_id),aim_id=data->aim_id;
    if(now_id!=aim_id){
        gtk_widget_modify_bg(GTK_WIDGET(button),GTK_STATE_NORMAL,&focus);
        gtk_widget_modify_bg(data->main_button[now_id],GTK_STATE_NORMAL,&white);
        gtk_widget_hide_all(data->main_window[now_id]);
        gtk_widget_show_all(data->main_window[aim_id]);
        *(data->now_id)=aim_id;
        if(aim_id==2){
            gtk_widget_hide_all(data->chat_box);
            //printf("2222\n%d\n",*(data->init_welcome_flag));
            if(*(data->init_welcome_flag)==0)
                gtk_widget_hide_all(data->init_welcome_box);
        }
        else{
            if(*(data->init_welcome_flag)==0)
                gtk_widget_show_all(data->init_welcome_box);
            else
                gtk_widget_show_all(data->chat_box);
        }
    }
}
//左边按钮栏的移入回调函数
void on_event_box_moved(GtkWidget* event_box,GdkEventCrossing *event,window_status* data){
    if(*(data->now_id)!=data->aim_id)
        gtk_widget_modify_bg(GTK_WIDGET(event_box),GTK_STATE_NORMAL,&moved);
}
//左边按钮栏的移出回调函数
void on_leave_box_moved(GtkWidget* event_box,GdkEventCrossing *event,window_status* data){
    if(*(data->now_id)!=data->aim_id)
        gtk_widget_modify_bg(GTK_WIDGET(event_box),GTK_STATE_NORMAL,&white);
}

void title_bar_on_event_box_moved(GtkWidget* event_box,GdkEventCrossing *event,GdkColor* data){
    gtk_widget_modify_bg(GTK_WIDGET(event_box),GTK_STATE_NORMAL,data);
}

void title_bar_on_leave_box_moved(GtkWidget* event_box,GdkEventCrossing *event,gpointer data){
    gtk_widget_modify_bg(GTK_WIDGET(event_box),GTK_STATE_NORMAL,&white);
}

void minimize_event(GtkWidget* event_box,GdkEventCrossing *event,GtkWidget* data){
    gtk_window_iconify(GTK_WINDOW(data));
}

void maximize_event(GtkWidget* event_box,GdkEventCrossing *event,maximize_state* data){
    if(*data->state==0){
        gtk_window_maximize(GTK_WINDOW(data->window));
        *data->state=1;
    }
    else{
        gtk_window_unmaximize(GTK_WINDOW(data->window));
        *data->state=0;
    }
}

gboolean button_press(GtkWidget *widget,GdkEventButton *event,gint data)
{
    if(event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if(event->button == 1)// 1代表鼠标左键!! 2 鼠标中间那个东东!! 3 就是剩下的那个
        { //gtk_widget_get_toplevel 返回顶层窗口 就是window
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,event->x_root, event->y_root,event->time);
        }
    }
    return TRUE;
}


GtkWidget* get_event_box_with_label(const char* label){
    GtkWidget* eventbox=gtk_event_box_new();
    GtkWidget* lbl=gtk_label_new(label);
    gtk_container_add(GTK_CONTAINER(eventbox),lbl);
    gtk_widget_modify_bg(GTK_WIDGET(eventbox),GTK_STATE_NORMAL,&white);
    return eventbox;
}


GtkWidget* get_about_box(){
    GtkWidget* main_box=gtk_vbox_new(FALSE,0);
    GtkWidget* about_box=gtk_vbox_new(FALSE,0);
    GtkWidget* ebox=gtk_event_box_new();
    GtkWidget* label=gtk_label_new(NULL);
    gtk_widget_modify_bg(ebox,GTK_STATE_NORMAL,&skyblue);
    gtk_box_pack_start(GTK_BOX(main_box),ebox,TRUE,TRUE,0);
    gtk_container_add(GTK_CONTAINER(ebox),about_box);
    gtk_label_set_markup(GTK_LABEL(label),"<span foreground=\"red\"><big>SkyQ Beta</big></span>    Ver1.0.0");
    gtk_box_pack_start(GTK_BOX(about_box),label,FALSE,FALSE,200);
    label=gtk_label_new("2015 SkyQ Project. All rights reserved.\n\nprogrammed by Hanqing Wang, Xiaoxiang Wei, Changyang Li and Wentian Zhao, tested by Shuying Yu.\n\nPowered by SQLite3 database and cJSON library.\n");
    gtk_box_pack_start(GTK_BOX(about_box),label,FALSE,FALSE,50);
    return main_box;
}

GtkWidget* mainUI(){
    //===============需要的变量================

	int i;
	
	//===================Using in the Communication Window=======================
	//struct text_view_info viewinfo;
	GtkWidget *scwin1, *scwin2;
	GtkWidget *cmtable;
	GtkWidget *button1, *button2;
    
    GtkWidget *logo_label;
    GtkWidget *msg_box, *send_box, *button_box, *group_friend_list_box;

	//===============================
    GtkWidget* window;//主窗体
    GtkWidget* main_window;//加入自制关闭等按钮
    GtkWidget* main_box;//实现功能的主box
    GtkWidget* title_bar_box;
    GtkWidget* btn_box;
    GtkWidget* friend_list_box;
    GtkWidget* group_list_box;
    GtkWidget* chat_box;
    GtkWidget* about_box=get_about_box();
    GtkWidget* quit_btn;
    GtkWidget* minimize_btn;
    GtkWidget* maximize_btn;
    GtkWidget* friend_btn;
    GtkWidget* group_btn;
    GtkWidget* about_btn;
    GtkWidget* init_welcome_box=gtk_vbox_new(FALSE,0);
    GtkWidget* init_welcome_label;


    //window_status w_status[3];
    //maximize_state maxi_state;
    
    //int maximize_flag=0;
    //int init_welcome_flag=1;


    maxi_state.state=&maximize_flag;

    	//=====================调用姿势==================
	//群组列表
	sky_group = create_grouplist(CREATE_GROUPLIST);
	group_page = sky_group.update_box;
	
	//好友列表
	sky_friend = create_friendlist(CREATE_FRIENDLIST);
	friend_page = sky_friend.update_box;

	friend_list_box=sky_friend.Mainbox;
	//gtk_widget_modify_bg(friend_list_box,GTK_STATE_NORMAL,&white);
	gtk_widget_set_size_request(GTK_WIDGET(friend_list_box),300,300);
	group_list_box=sky_group.Mainbox;
	//gtk_widget_modify_bg(group_list_box,GTK_STATE_NORMAL,&white);
	gtk_widget_set_size_request(GTK_WIDGET(group_list_box),300,300);
    chat_box=gtk_vbox_new(FALSE,0);


    //=====================加入自制关闭 最小化 最大化 按钮=============================
    title_bar_box=gtk_hbox_new(FALSE,0);
    quit_btn=get_event_box_with_label("X");
    gtk_widget_set_size_request(GTK_WIDGET(quit_btn),20,20);
    g_signal_connect(G_OBJECT(quit_btn),"enter_notify_event",G_CALLBACK(title_bar_on_event_box_moved),&red);
    g_signal_connect(G_OBJECT(quit_btn),"leave_notify_event",G_CALLBACK(title_bar_on_leave_box_moved),NULL);
    g_signal_connect(G_OBJECT(quit_btn),"button_press_event",G_CALLBACK(gtk_main_quit),NULL);
    minimize_btn=get_event_box_with_label("一");
    gtk_widget_set_size_request(GTK_WIDGET(minimize_btn),20,20);
    g_signal_connect(G_OBJECT(minimize_btn),"enter_notify_event",G_CALLBACK(title_bar_on_event_box_moved),&skyblue);
    g_signal_connect(G_OBJECT(minimize_btn),"leave_notify_event",G_CALLBACK(title_bar_on_leave_box_moved),NULL);
    maximize_btn=get_event_box_with_label("O");
    gtk_widget_set_size_request(GTK_WIDGET(maximize_btn),20,20);
    g_signal_connect(G_OBJECT(maximize_btn),"enter_notify_event",G_CALLBACK(title_bar_on_event_box_moved),&skyblue);
    g_signal_connect(G_OBJECT(maximize_btn),"leave_notify_event",G_CALLBACK(title_bar_on_leave_box_moved),NULL);
    gtk_box_pack_end(GTK_BOX(title_bar_box),quit_btn,FALSE,FALSE,0);
    gtk_box_pack_end(GTK_BOX(title_bar_box),maximize_btn,FALSE,FALSE,0);
    gtk_box_pack_end(GTK_BOX(title_bar_box),minimize_btn,FALSE,FALSE,0);
    GtkWidget* SkyQ_title_label=gtk_label_new("SkyQ");
    gtk_box_pack_end(GTK_BOX(title_bar_box),SkyQ_title_label,TRUE,TRUE,0);
//==============================================================

//==================main_window======================================
    main_window=gtk_vbox_new(FALSE,0);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(window),FALSE);
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
    main_box=gtk_hbox_new(FALSE,0);
    gtk_widget_modify_bg(window,GTK_STATE_NORMAL,&white);
    g_signal_connect(G_OBJECT(window), "button-press-event",G_CALLBACK(button_press), NULL);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(G_OBJECT(minimize_btn),"button_press_event",G_CALLBACK(minimize_event),window);
    maxi_state.window=window;
    g_signal_connect(G_OBJECT(maximize_btn),"button_press_event",G_CALLBACK(maximize_event),&maxi_state);
    //gtk_window_set_title(GTK_WINDOW(window),"SkyQ");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),0);
    gtk_window_set_default_size(GTK_WINDOW(window),1000,600);
    gtk_widget_set_size_request(window,1000,600);
    gtk_box_pack_start(GTK_BOX(main_window),title_bar_box,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(main_window),main_box,TRUE,TRUE,0);
    gtk_container_add(GTK_CONTAINER(window),main_window);
//=================================================================

    friend_btn=get_event_box_with_label("friend");
    group_btn=get_event_box_with_label("group");
    about_btn=get_event_box_with_label("about");


    btn_box=gtk_vbox_new(TRUE,0);
    gtk_box_pack_start(GTK_BOX(main_box),btn_box,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(btn_box),friend_btn,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(btn_box),group_btn,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(btn_box),about_btn,TRUE,TRUE,0);
    gtk_widget_set_size_request(GTK_WIDGET(btn_box),100,100);
    //gtk_box_pack_start(GTK_BOX(main_box),btn_box,FALSE,FALSE,0);



    gtk_box_pack_start(GTK_BOX(main_box),friend_list_box,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(main_box),group_list_box,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(main_box),about_box,TRUE,TRUE,2);

    gtk_widget_modify_bg(GTK_WIDGET(friend_btn),GTK_STATE_NORMAL,&focus);
    //int i;
    for(i=0;i<3;i++){
        w_status[i].main_box=main_box;
        w_status[i].aim_id=i;
        w_status[i].now_id=&now_id;
        w_status[i].main_window[0]=friend_list_box;
        w_status[i].main_window[1]=group_list_box;
        w_status[i].main_window[2]=about_box;
        w_status[i].chat_box=chat_box;
        w_status[i].main_button[0]=friend_btn;
        w_status[i].main_button[1]=group_btn;
        w_status[i].main_button[2]=about_btn;
        w_status[i].init_welcome_flag=&init_welcome_flag;
        w_status[i].init_welcome_box=init_welcome_box;
    }

    g_signal_connect(G_OBJECT(friend_btn),"enter_notify_event",G_CALLBACK(on_event_box_moved),&w_status[0]);
    g_signal_connect(G_OBJECT(friend_btn),"leave_notify_event",G_CALLBACK(on_leave_box_moved),&w_status[0]);
    g_signal_connect(G_OBJECT(friend_btn),"button_press_event",G_CALLBACK(on_main_button_clicked),&w_status[0]);


    g_signal_connect(G_OBJECT(group_btn),"enter_notify_event",G_CALLBACK(on_event_box_moved),&w_status[1]);
    g_signal_connect(G_OBJECT(group_btn),"leave_notify_event",G_CALLBACK(on_leave_box_moved),&w_status[1]);
    g_signal_connect(G_OBJECT(group_btn),"button_press_event",G_CALLBACK(on_main_button_clicked),&w_status[1]);


    g_signal_connect(G_OBJECT(about_btn),"enter_notify_event",G_CALLBACK(on_event_box_moved),&w_status[2]);
    g_signal_connect(G_OBJECT(about_btn),"leave_notify_event",G_CALLBACK(on_leave_box_moved),&w_status[2]);
    g_signal_connect(G_OBJECT(about_btn),"button_press_event",G_CALLBACK(on_main_button_clicked),&w_status[2]);

    ////////////////////////////Communication Window//////////////////////////////////////
/*
    ///Create the main window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Start a communication");
	gtk_window_set_default_size(GTK_WINDOW(window),500,400);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
*/
	cmtable = gtk_table_new(400,1,FALSE);
	//gtk_container_add(GTK_CONTAINER(window),cmtable);
	//gtk_box_pack_start(GTK_BOX());

    ///Part1
	cmeventbox1 = gtk_event_box_new();
	gtk_widget_modify_bg(cmeventbox1,GTK_STATE_NORMAL,&Blue);

    //char *cmtitle="<span foreground=\"white\"><big>Communicating With Bilv</big></span>";
    //gchar *title=cmtitle;

    cmlabel = gtk_label_new(NULL);
    //gtk_label_set_markup(GTK_LABEL(cmlabel),title);

    gtk_container_add(GTK_CONTAINER(cmeventbox1),cmlabel);

    ///Part2
    msg_box = gtk_hbox_new(FALSE,0);

    scwin1= gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin1),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    viewinfo.view1 = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(viewinfo.view1),FALSE);
    gtk_container_add(GTK_CONTAINER(scwin1),viewinfo.view1);

    group_friend_list_box = gtk_vbox_new(FALSE,0);

    scwinlist = gtk_scrolled_window_new(NULL,NULL);
    group_friend_list = gtk_list_new();
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scwinlist),group_friend_list);
	gtk_list_set_selection_mode(GTK_LIST(group_friend_list),GTK_SELECTION_SINGLE); 	//设置列表框的选择模式(单选或多选)

    cmeventbox2 = gtk_event_box_new();
    gtk_widget_modify_bg(cmeventbox2,GTK_STATE_NORMAL,&AliceBlue);
    logo_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(logo_label),"<span foreground=\"blue\"><big>Welcome to SkyQ</big></span>");
    gtk_container_add(GTK_CONTAINER(cmeventbox2),logo_label);

    gtk_box_pack_start(GTK_BOX(group_friend_list_box),scwinlist,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(group_friend_list_box),cmeventbox2,TRUE,TRUE,0);

    gtk_box_pack_start(GTK_BOX(msg_box),scwin1,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(msg_box),group_friend_list_box,FALSE,FALSE,0);
    gtk_widget_set_size_request (group_friend_list_box, 180, 50);

    ///Part3
    cmeventbox3 = gtk_event_box_new();
	gtk_widget_modify_bg(cmeventbox3,GTK_STATE_NORMAL,&Blue);

    ///Part4
    send_box = gtk_hbox_new(FALSE,0);

    scwin2 = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    viewinfo.view2 = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scwin2),viewinfo.view2);

    button_box = gtk_vbox_new(TRUE,0);

    button1 = gtk_button_new_with_label("tmep");
	button2 = gtk_button_new_with_label("temp");
    g_signal_connect(G_OBJECT(button1),"button_release_event",G_CALLBACK(sticker_button_press),&viewinfo);
    g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(send_button_press),&viewinfo);
    gtk_widget_modify_bg(button1,GTK_STATE_NORMAL,&Blue);
    gtk_widget_modify_bg(button2,GTK_STATE_NORMAL,&Blue);
    GtkWidget *labelChild1 = gtk_bin_get_child(GTK_WIDGET(button1));
    GtkWidget *labelChild2 = gtk_bin_get_child(GTK_WIDGET(button2));
    gtk_label_set_markup(GTK_LABEL(labelChild1),"<span foreground=\"#FFFFFF\">Sticker</span>");
    gtk_label_set_markup(GTK_LABEL(labelChild2), "<span foreground=\"#FFFFFF\">Send</span>");

    gtk_box_pack_start(GTK_BOX(button_box),button1,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(button_box),button2,TRUE,TRUE,0);

    gtk_box_pack_start(GTK_BOX(send_box),scwin2,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(send_box),button_box,FALSE,FALSE,0);
    gtk_widget_set_size_request (button_box, 180, 50);

    ///
    gtk_table_attach_defaults(GTK_TABLE(cmtable),cmeventbox1,0,1,0,50);
    gtk_table_attach_defaults(GTK_TABLE(cmtable),msg_box,0,1,50,320);
    gtk_table_attach_defaults(GTK_TABLE(cmtable),cmeventbox3,0,1,320,325);
    gtk_table_attach_defaults(GTK_TABLE(cmtable),send_box,0,1,325,400);

    ///Set the tags of the TextView
    viewinfo.view1_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewinfo.view1));
    viewinfo.view2_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewinfo.view2));

    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "blue_foreground","foreground", "blue", NULL);//创建前景标记
    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "yellow_background","background","yellow",NULL);//创建背景标记
    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "simhei", "family", "Simhei",NULL);
    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "sans", "family", "Sans",NULL);//以上两行创建字体标记
    gtk_text_buffer_create_tag (viewinfo.view1_buffer, "heading","justification",GTK_JUSTIFY_LEFT,NULL);//居左
    gtk_text_buffer_create_tag (viewinfo.view1_buffer, "no_wrap","wrap_mode", GTK_WRAP_NONE, NULL);//不换行
    gtk_text_buffer_create_tag (viewinfo.view1_buffer, "word_wrap","wrap_mode", GTK_WRAP_WORD, NULL);//以词为单位换行
    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "center","justification", GTK_JUSTIFY_CENTER, NULL);//居中
    gtk_text_buffer_create_tag(viewinfo.view1_buffer, "right_justify","justification", GTK_JUSTIFY_RIGHT, NULL);//居右

    ///////////////////////Initiation complete////////////////////////////
    //group_friend_list_add(group_friend_list,"Biwei");
    //group_friend_list_add(group_friend_list,"Bilv");
    //print_msg(&viewinfo,"Bilv :\n","我并没有针对你们某个人的意思");


    gtk_box_pack_start(GTK_BOX(chat_box),cmtable,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(main_box),chat_box,TRUE,TRUE,0);
    ////////////////////////////Communication Window//////////////////////////////////////


//==========================welcome_page===================================
    init_welcome_label=gtk_label_new("Welcome to world of sky!");
    gtk_box_pack_start(GTK_BOX(init_welcome_box),init_welcome_label,TRUE,TRUE,0);
    //gtk_box_pack_start(GTK_BOX(main_box),init_welcome_box,TRUE,TRUE,0);
//======================================================================

    //gtk_widget_show_all(window);
	mainUI_window_scwinlist=scwinlist;
	mainUI_window_group_list_box=group_list_box;
	mainUI_window_about_box=about_box;
    //gtk_widget_hide_all(group_list_box);
    //gtk_widget_hide_all(about_box);
   // gtk_widget_hide_all(chat_box);
    //gtk_main();
	return window;
}

void mainUI_window_show(){
	gtk_widget_show_all(mainUI_window);
	gtk_widget_hide_all(mainUI_window_group_list_box);
    gtk_widget_hide_all(mainUI_window_about_box);
   	gtk_widget_hide_all(mainUI_window_scwinlist);
}

struct UI_state get_UI_state() {
	struct UI_state state;
	state.tab = now_id;
	strcpy(state.current_username, talk_to_user);
	state.current_groupID = &talk_to_group;
	return state;
}

