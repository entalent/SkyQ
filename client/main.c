#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include "util/util.h"
#include "ui.h"
#include <gtk/gtk.h>
#include <time.h>

#define LISTEN_PORT 1234
#define BUFFER_SIZE 2048


extern int now_id;
extern int group_list_size;
extern int friend_list_size;
extern int n_g,t_t_g,f_g,l,n_f,l_f;
extern struct widget_pack sky_group,sky_friend;
extern GtkWidget *group_box[N];//eventbox
extern GtkWidget *label_grouplist[N];	//label_grouplist
extern GtkWidget *group_page;
extern int *num_group;
extern int *talk_to_group; //current group
extern int *focus_group;
extern char group_ID[100];//群ID(string)
extern int *lastfocus;
extern struct args_grouplist args_for_callback_grouplist[N];//回调函数参数
extern struct group_profile attribute_group[N];		 //eventbox属性
extern struct group_profile groupdata[N]; //获取的群组属性
extern struct user_profile userdata[N]; //获取的用户属性
extern GtkWidget *friend_box[N];//eventbox
extern GtkWidget *label_friendlist[N];	//label_friendlist
extern GtkWidget *friend_page;
extern int *num_friend;
extern char talk_to_user[100]; //current user
extern char focus_user[100];
extern char friend_ID[100];//加好友用
extern int lastfocus_friend;
extern struct args_friendlist args_for_callback_friendlist[N];//回调函数参数
extern struct user attribute_friend[N];		 //eventbox属性

extern GtkWidget* login_window;
extern GtkWidget* mainUI_window;
extern GtkWidget* regist_window;
extern GtkWidget* mainUI_window_scwinlist;
extern GtkWidget* mainUI_window_group_list_box;
extern GtkWidget* mainUI_window_about_box;
extern GtkWidget* pop_window;
//extern GtkWidget* new_small_window_label;



//=============================NETWORK===============================
int serverfd;
struct sockaddr_in server;
char current_username[32];

char buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];
//=============================NETWORK===============================

//void set_chat_title(GtkWidget *cmlabel, const char *buf);  
// buf should be like "<span foreground=\"white\"><big>Communicating With Bilv</big></span>"

char *title_format_1 = "<span foreground=\"white\"><big>";
char *title_format_2 = "</big></span>";
char title_buffer[1024];

void focusin_friend_callback() {
	*talk_to_group = 0;
	printf("focusin friend %s\n", talk_to_user);
	clear_chat_window();
	exec_cmd(13, "single", talk_to_user);
	// 藏 群成员列表
	change_chat_mode(1);
	sprintf(title_buffer, "%s%s%s", title_format_1, talk_to_user, title_format_2);
	set_chat_title(cmlabel, title_buffer);
}

void focusin_group_callback() {
	memset(talk_to_user, '\0', sizeof(talk_to_user));
	printf("focusin group %d\n", *talk_to_group);
	clear_chat_window();
	exec_cmd(13, "group", talk_to_group);
	//显示 群成员列表	
	change_chat_mode(2);
	exec_cmd(8, talk_to_group, NULL);
	sprintf(title_buffer, "%s%d%s", title_format_1, *talk_to_group, title_format_2);
	set_chat_title(cmlabel, title_buffer);
	
}
//TODO: 弹窗！！！

//加群
void confirm_group_callback() {
	printf("confirm group callback %s\n", group_ID);
	int groupID;
	sscanf(group_ID, "%d", &groupID);
	exec_cmd(10, &groupID, NULL);
	sleep(1);
	exec_cmd(7, NULL, NULL);
}

//创建群
void create_group_callback() {
	printf("create group callback\n");
	exec_cmd(9, NULL, NULL);
}

//加好友
void confirm_friend_callback() {
	printf("confirm friend callback %s\n", friend_ID);
	exec_cmd(4, friend_ID, NULL);
	sleep(1);
	exec_cmd(6, NULL, NULL);
}

//FIXME: 切换到另一种模式时，把前一种的talk_to清零！！
void send_message_callback(char *message) {
	printf("======================================================\n%s %d\n", talk_to_user, *talk_to_group);
	printf("send message callback %s\n", message);
	if(strlen(talk_to_user) >= 1){
		exec_cmd(5, talk_to_user, message);
	}
	else if(*talk_to_group >= 1) {
		exec_cmd(12, talk_to_group, message);
	}
}

int group_member_count = 0;
struct group_username group_members[32];

const int SHOW_ALL_MESSAGE = 0;

struct message_list_item {
	//sendfrom and time
	char message_prop[64];
	//all message's size doesn't exceed 1024 byte!
	char content[1024];
};
int group_message_list_size;
struct message_list_item group_message_list[64];

int single_message_list_size;
struct message_list_item single_message_list[64];


int login(char *username, char *password) {
	printf("login %s %s\n", username, password);
	exec_cmd(2, username, password);
	return 1;
}

int regist(char *username, char *password) {
	printf("regist %s %s\n", username, password);
	exec_cmd(1, username, password);
	return 1;
}

//the database
sqlite3 *db;

//initialize the database
//initialize while user login
int db_init(char *filename) {
	if(SQLITE_OK != sqlite3_open(filename, &db))
		return 0;
	if(SQLITE_OK != sqlite3_exec(db, "create table if not exists chatrecord_single(sendto TEXT NOT NULL, sendfrom TEXT NOT NULL, sendtime TEXT NOT NULL, content TEXT NOT NULL)",
		NULL, NULL, NULL)) return 0;
	if(SQLITE_OK != sqlite3_exec(db, "create table if not exists chatrecord_group(sendto INTEGER NOT NULL, sendfrom TEXT NOT NULL, sendtime TEXT NOT NULL,  content TEXT NOT NULL)",
		NULL, NULL, NULL)) return 0;
	return 1;
}

void init_main_window() {
	sprintf(title_buffer, "%s%s%s", title_format_1, "请选择联系人或群组", title_format_2);
	set_chat_title(cmlabel, title_buffer);
	//clear_chat_window();
	//change_chat_mode(1);
}

void login_action(char *username) {
	char db_name[64];
	sprintf(db_name, "client-%s.db", username);
	
	if(!db_init(db_name)){
		printf("database initialization failed.\n");
	} else {
		strcpy(current_username, username);
		printf("log in as %s\n", current_username);
	}
	gdk_threads_enter();
	mainUI_window_show();
	init_main_window();
	gdk_threads_leave();
	exec_cmd(6, NULL, NULL);
	exec_cmd(7, NULL, NULL);
}

void logout_action() {
	//close database and clear current_username
	sqlite3_close(db);
	db = NULL;
	memset(current_username, '\0', sizeof(current_username));
}

void show_chatrecord_single(char *username) {
	if(strcmp(talk_to_user, username) != 0){
		return ;
	}
	printf("\n\nshow chatrecord single\n\n\n");
	clear_chat_window();
	char sqlcmd[1024];
	sprintf(sqlcmd, "select * from chatrecord_single where (sendto=\'%s\' and sendfrom=\'%s\') or (sendto=\'%s\' and sendfrom=\'%s\') order by sendtime asc limit 0, 50", current_username, username, username, current_username);
	char ** res; char * errmsg; int nrow = 0, ncol = 0;
	if(SQLITE_OK != sqlite3_get_table(db, sqlcmd, &res, &nrow, &ncol, &errmsg)) {
		printf("query chat record failed\n");	
		return ;
	}
	printf("chat record with user %s\n", username);
	int i, j; int nindex = ncol;
	single_message_list_size = nrow;
	char info_buf[1024];
	for(i=0;i<nrow;i++){
		for(j=0;j<ncol;j++){
			if(j == 1) {
				sprintf(single_message_list[i].message_prop, "%s ", res[nindex]);
			} else if(j == 2) {
				strcat(single_message_list[i].message_prop, res[nindex]);
			} else if(j == 3) {
				sprintf(single_message_list[i].content, res[nindex]);
			}
			//printf("%s ", res[nindex]);
			nindex++;
		}
		strcat(single_message_list[i].message_prop, "\n");
		printf("print msg %s\n", single_message_list[i].content);
		print_msg(&viewinfo, single_message_list[i].message_prop, single_message_list[i].content);
		//printf("\n");
	}
}

void show_chatrecord_group(int groupID) {
	printf("%d %d\n", groupID, *talk_to_group);
	if(groupID != *talk_to_group){
		return ;
	}
	printf("\n\nshow chatrecord group\n\n\n");
	clear_chat_window();
	char sqlcmd[1024];
	sprintf(sqlcmd, "select * from chatrecord_group where (sendto=%d) order by sendtime asc limit 0, 50", groupID);
	char ** res; char * errmsg; int nrow = 0, ncol = 0;
	if(SQLITE_OK != sqlite3_get_table(db, sqlcmd, &res, &nrow, &ncol, &errmsg)) {
		printf("query chat record failed\n");	
		return ;
	}
	printf("chat record with group %d\n", groupID);
	int i, j; int nindex = ncol;
	group_message_list_size = nrow;
	for(i=0;i<nrow;i++){
		for(j=0;j<ncol;j++){
			if(j == 1) {
				sprintf(group_message_list[i].message_prop, "%s ", res[nindex]);
			} else if(j == 2) {
				strcat(group_message_list[i].message_prop, res[nindex]);
			} else if(j == 3) {
				sprintf(group_message_list[i].content, res[nindex]);
			}
			//printf("%s ", res[nindex]);
			nindex++;
		}
		strcat(group_message_list[i].message_prop, "\n");
		printf("print msg %s\n", group_message_list[i].content);
		print_msg(&viewinfo, group_message_list[i].message_prop, group_message_list[i].content);
		//printf("\n");
	}
}

//save chat record for single chat
void save_chatrecord_single(const char * jsonstr) {
	printf("save chatrecord single %s\n", jsonstr);
	cJSON *root = cJSON_Parse(jsonstr);
	if(root == NULL) return ;
	char *sendfrom = cJSON_GetObjectItem(root, "sendfrom")->valuestring;
	char *sendto = cJSON_GetObjectItem(root, "sendto")->valuestring;
	char *sendtime = cJSON_GetObjectItem(root, "sendtime")->valuestring;
	char *content = cJSON_GetObjectItem(root, "content")->valuestring;
	char sqlcmd[2048];
	sprintf(sqlcmd, "insert into chatrecord_single values(\'%s\', \'%s\', \'%s\', \'%s\')", sendto, sendfrom, sendtime, content);
	if(db != NULL)	
		sqlite3_exec(db, sqlcmd, NULL, NULL, NULL);
	free(root);
	if(strcmp(sendto, current_username) == 0)
		show_chatrecord_single(sendfrom);
	if(strcmp(sendfrom, current_username) == 0)
		show_chatrecord_single(sendto);
}

//save chat record for group chat
void save_chatrecord_group(const char *jsonstr) {
	printf("save chatrecord group %s\n", jsonstr);
	cJSON *root = cJSON_Parse(jsonstr);
	if(root == NULL) return ;

	int sendto = cJSON_GetObjectItem(root, "sendto")->valueint;
	printf("%d\n", sendto);
	char *sendfrom = cJSON_GetObjectItem(root, "sendfrom")->valuestring;
	printf("%s\n", sendfrom);
	char *sendtime = cJSON_GetObjectItem(root, "sendtime")->valuestring;
	printf("%s\n", sendtime);
	char *content = cJSON_GetObjectItem(root, "content")->valuestring;
	printf("len=  %d\n", strlen(content));
	printf("%s\n", content);

	char sqlcmd[2048];
	printf("before sprintf\n");
	sprintf(sqlcmd, "insert into chatrecord_group values(%d, \'%s\', \'%s\', \'%s\')", sendto, sendfrom, sendtime, content);
	printf("after sprintf\n");
	if(db != NULL)
		sqlite3_exec(db, sqlcmd, NULL, NULL, NULL);
	printf("wokao\n");
	show_chatrecord_group(sendto);
}

void handle_message(char *message){
	//gdk_threads_enter();
	printf("handle message start, serverfd = %d\n", serverfd);
    cJSON *root = cJSON_Parse(message);
    if(root == NULL){
        printf("json parse error, message is \"%s\"\n", message);
        return ;
    }
    char *type = cJSON_GetObjectItem(root, "type")->valuestring;
    if(strcmp(type, "register-receipt") == 0){
        int status = cJSON_GetObjectItem(root, "status")->valueint;
        char *username = cJSON_GetObjectItem(root, "username")->valuestring;
		gdk_threads_enter();
        if(status == 1 && username != NULL){
            printf("registered new user %s successfully\n", username);
			create_new_pop_window("regist successful!");
        } else {
			printf("register new user %s fail\n", username);
			create_new_pop_window("regist failed!");
		}
		gdk_threads_leave();
    } else if(strcmp(type, "login-receipt") == 0){
		//received the login receipt from server
        int status = cJSON_GetObjectItem(root, "status")->valueint;
        char *username = cJSON_GetObjectItem(root, "username")->valuestring;
        if(status == 1 && username != NULL){
			login_action(username);
        } else {
			create_new_pop_window("login failed\n");
			exit(1);
		}
    } else if(strcmp(type, "force-logout-notif") == 0){
        char *username = cJSON_GetObjectItem(root, "username");
        if(strcmp(current_username, username) == 0){
            printf("user %s force logout\n", username);
			logout_action();
        }
    } else if(strcmp(type, "message/text") == 0){
		char *sendfrom = cJSON_GetObjectItem(root, "sendfrom")->valuestring;
		char *sendtime = cJSON_GetObjectItem(root, "sendtime")->valuestring;
		char *content = cJSON_GetObjectItem(root, "content")->valuestring;
		/*
		printf("user %s sent a message to you at %s, \nmessage is %s\n",
			sendfrom, sendtime, content);*/
		//save chat record when receiving new message
		gdk_threads_enter();
		save_chatrecord_single(message);	
		gdk_threads_leave();
		//exec_cmd(13, "single", sendfrom);
    } else if(strcmp(type, "message/text/group") == 0){
		printf("[%d]\n", root == NULL);
		int sendto = cJSON_GetObjectItem(root, "sendto")->valueint;
		printf("%d\n", sendto);
		char *sendfrom = cJSON_GetObjectItem(root, "sendfrom")->valuestring;
		printf("%s\n", sendfrom);
		char *sendtime = cJSON_GetObjectItem(root, "sendtime")->valuestring;
		printf("%s\n", sendtime);
		char *content = cJSON_GetObjectItem(root, "content")->valuestring;
		printf("%s\n", content);
		
		/*
		char content_text[1025];
		int i = 0;
		putchar('\"');
		for(i = 0; *(content+i) != 0; i++) {
			content_text[i] = *(content+i);
			putchar(*(content+i));
		}
		putchar('\"');
		content_text[i] = '\0';
		printf("you have a message from group %d, sent by %s at %s:%s", sendto, sendfrom, sendtime, content_text);
		//for group message, save chat record only when receiving a new message
		*/
		gdk_threads_enter();
		save_chatrecord_group(message);
		
		gdk_threads_leave();
		//exec_cmd(13, "group", &sendto);
    } else if(strcmp(type, "friend-list") == 0){
		//friend list
		int size = cJSON_GetObjectItem(root, "size")->valueint;
		cJSON *list = cJSON_GetObjectItem(root, "list");
		int i;
		printf("friend list of user %s:\n", current_username);
		friend_list_size = size;
		for(i = 0; i < size; i++){
			cJSON *item = cJSON_GetArrayItem(list, i);
			char *username = cJSON_GetObjectItem(item, "username")->valuestring;
			int status = cJSON_GetObjectItem(item, "status")->valueint;
			printf("%s [%s]\n", username, (status ? "online":"offline"));
			strcpy(userdata[i].username, username);
			userdata[i].status = status;
		}
		printf("friend list size = %d\n", friend_list_size);
		gdk_threads_enter();
		update_friendlist(UPDATE_FRIENDLIST);
		gdk_threads_leave();
    } else if(strcmp(type, "group-create-receipt") == 0){
		int status = cJSON_GetObjectItem(root, "status")->valueint;
		if(status == 1) {
			int groupID = cJSON_GetObjectItem(root, "groupID")->valueint;
			printf("create group success, new group ID is %d\n", groupID);
		} else {
			printf("create group failed");
		}
    } else if(strcmp(type, "group-list") == 0){
		//group list
		int size = cJSON_GetObjectItem(root, "size")->valueint;
		cJSON *list = cJSON_GetObjectItem(root, "list");
		int i;
		printf("groups of user %s:\n", current_username);
		group_list_size = size;
		for(i = 0; i < size; i++){
			cJSON *item = cJSON_GetArrayItem(list, i);
			int groupID = cJSON_GetObjectItem(item, "groupID")->valueint;
			printf("group %d\n", groupID);
			groupdata[i].groupID = groupID;
		}
		gdk_threads_enter();
		update_grouplist(UPDATE_GROUPLIST);
		gdk_threads_leave();
    } else if(strcmp(type, "group-profile") == 0){
		//group profile
		//FIXME: maybe there is some bug...
		int size = cJSON_GetObjectItem(root, "member-count")->valueint;
		int groupID = cJSON_GetObjectItem(root, "groupID")->valueint;
		printf("profile of group %d:\n%d members in total\n", groupID, size);
		cJSON *list = cJSON_GetObjectItem(root, "list");
		int i;
		group_member_count = size;
		printf("size = %d\n", group_member_count);
		for(i = 0; i < size; i++){
			cJSON *item = cJSON_GetArrayItem(list, i);
			char *username = cJSON_GetObjectItem(item, "username")->valuestring;
			//printf("%s\n", username);
			memset(group_members[i].username, '\0', sizeof(group_members[i].username));
			if(i < 1024) {strcpy(group_members[i].username, username);
				printf("%s\n", group_members[i].username);			
			}
		}
		gdk_threads_enter();
		update_group_friend_list(group_member_count, group_members);
		gdk_threads_leave();
    } else if(strcmp(type, "group-join-receipt") == 0){
		int groupID = cJSON_GetObjectItem(root, "groupID")->valueint;
		int status = cJSON_GetObjectItem(root, "status")->valueint;
		printf("join group %d %s\n", groupID, (status ? "success" : "fail"));
		gdk_threads_enter();
		update_grouplist(UPDATE_GROUPLIST);
		gdk_threads_leave();
    } else if(strcmp(type, "group-quit-receipt") == 0){
		int groupID = cJSON_GetObjectItem(root, "groupID")->valueint;
		int status = cJSON_GetObjectItem(root, "status")->valueint;
		printf("quit group %d %s\n", groupID, (status ? "success" : "fail"));
    } else if(strcmp(type, "add-to-contact-receipt") == 0){
		char *contact = cJSON_GetObjectItem(root, "contact")->valuestring;
		int status = cJSON_GetObjectItem(root, "status")->valueint;
		printf("add contact %s %s\n", contact, (status ? "success" : "fail"));
		gdk_threads_enter();
		update_friendlist(UPDATE_FRIENDLIST);
		gdk_threads_leave();
    }
	printf("handle message over, serverfd = %d\n", serverfd);
	//gdk_threads_leave();
}

void exec_cmd(int op, void *arg1, void *arg2) {
    cJSON *root = cJSON_CreateObject();
    if(root == NULL){
        printf("create json object failed\n");
        return;
    }
    if(op >= 3 && strlen(current_username) == 0){
        printf("haven't logged in");
        return ;
    }
    switch(op){

    //send to server:      0 <any string>
    case 0:
    send_function((char *) arg1);
    break;

	//register:            1 <username> <password>
	case 1:
    cJSON_AddStringToObject(root, "type", "register-message");
    cJSON_AddStringToObject(root, "username", (char *) arg1);
    cJSON_AddStringToObject(root, "password", (char *) arg2);
    send_function(cJSON_Print(root));
    break;

	//login:               2 <username> <password>
	case 2:
    cJSON_AddStringToObject(root, "type", "login-message");
    cJSON_AddStringToObject(root, "username", (char *) arg1);
    cJSON_AddStringToObject(root, "password", (char *) arg2);
    send_function(cJSON_Print(root));
    break;

	//logout:              3\n
	case 3:
    cJSON_AddStringToObject(root, "type", "logout-message");
    cJSON_AddStringToObject(root, "username", current_username);
	send_function(cJSON_Print(root));
	logout_action();
    break;

	//add friend:          4 <username>
	case 4:
    cJSON_AddStringToObject(root, "type", "add-to-contact-request");
    cJSON_AddStringToObject(root, "username", current_username);
    cJSON_AddStringToObject(root, "contact", (char *) arg1);
    send_function(cJSON_Print(root));
    break;


	//text message:        5 <sendto> <content>
	case 5:
	if(strlen((char*)arg2) > 1024){
		return;
	}
    cJSON_AddStringToObject(root, "type", "message/text");
    cJSON_AddStringToObject(root, "sendto", (char *) arg1);
    cJSON_AddStringToObject(root, "sendfrom", current_username);
    cJSON_AddStringToObject(root, "sendtime", get_formatted_time());
    cJSON_AddStringToObject(root, "content", (char *) arg2);
    send_function(cJSON_Print(root));
	//save the chat record while sending message
	save_chatrecord_single(cJSON_Print(root));
    break;

	//show friend list:    6
	case 6:
	cJSON_AddStringToObject(root, "type", "friend-list-request");
    cJSON_AddStringToObject(root, "username", current_username);
    send_function(cJSON_Print(root));
    break;

	//show group list:     7
	case 7:
	cJSON_AddStringToObject(root, "type", "group-list-request");
    cJSON_AddStringToObject(root, "username", current_username);
    send_function(cJSON_Print(root));
    break;

	//show group profile:  8 <groupID>
	case 8:
    cJSON_AddStringToObject(root, "type", "group-profile-request");
    cJSON_AddStringToObject(root, "username", current_username);
    cJSON_AddNumberToObject(root, "groupID", *(int*)arg1);
    send_function(cJSON_Print(root));
    break;

	//create new group:    9
	case 9:
    cJSON_AddStringToObject(root, "type", "group-create-request");
    cJSON_AddStringToObject(root, "username", current_username);
    send_function(cJSON_Print(root));
    break;

	//join group:          10 <groupID>
	case 10:
    cJSON_AddStringToObject(root, "type", "group-join-request");
    cJSON_AddStringToObject(root, "username", current_username);
    cJSON_AddNumberToObject(root, "groupID", *(int*)arg1);
    send_function(cJSON_Print(root));
    break;

	//quit group:          11 <groupID>
	case 11:
    cJSON_AddStringToObject(root, "type", "group-quit-request");
    cJSON_AddStringToObject(root, "username", current_username);
    cJSON_AddNumberToObject(root, "groupID", *(int*)arg1);
    send_function(cJSON_Print(root));
    break;

	//send group message:  12 <content>
	case 12:
	if(strlen((char*)arg2) > 1024){
		return;
	}
    cJSON_AddStringToObject(root, "type", "message/text/group");
    cJSON_AddNumberToObject(root, "sendto", *(int*)arg1);
    cJSON_AddStringToObject(root, "sendfrom", current_username);
    cJSON_AddStringToObject(root, "sendtime", get_formatted_time());
    cJSON_AddStringToObject(root, "content", (char *)arg2);
	cJSON_AddStringToObject(root, "msgID", "segmentfault");
    send_function(cJSON_Print(root));
    break;

	//show history message:13 <group|single> <groupID|username>
	case 13:
	if(strcmp((char *)arg1, "single") == 0) {
		show_chatrecord_single((char *)arg2);
	} else if(strcmp((char *)arg1, "group") == 0) {
		show_chatrecord_group(*(int*)arg2);
	}
    break;
    default:break;
	};
}

//TODO: send in another thread?
int send_function(char *message) {
    memset(send_buffer, '\0', sizeof(send_buffer));
    strcpy(send_buffer, message);
    printf("sending message to server:\n%s\n", send_buffer);
	return send(serverfd, send_buffer, BUFFER_SIZE, 0);
}

void auto_update_thread() {
	while(1) {
		sleep(5);
		exec_cmd(6, NULL, NULL);
		sleep(10);
		exec_cmd(7, NULL, NULL);
		sleep(10);
	}
}

void recv_thread() {
	int numbytes;
	while(1) {
		printf("recv serverfd = %d\n", serverfd);
		memset(buffer, 0, BUFFER_SIZE);
		numbytes = recv(serverfd, buffer, BUFFER_SIZE, 0);
		printf("recv over, serverfd = %d, numbytes = %d\n", serverfd);
		if(numbytes == 0){
			printf("server offline.\n");
			exit(1);
		}
		if(numbytes == -1){
			perror("error receiving message\n");
			exit(1);
		}
		printf("recv judge over, serverfd = %d\n", serverfd);
		//buffer[numbytes] = '\0';
        	printf("received message from server: \"%s\"\n", buffer);
		printf("before handle message, serverfd = %d\n", serverfd);
		//gdk_threads_enter();
        handle_message(buffer);
		//gdk_threads_leave(); 
	}
}

int init_client(char * serverIP) {
	if(-1 == (serverfd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("create socket");
		return 0;
	}

	struct hostent *he;
	if((he=gethostbyname(serverIP))==NULL){
		printf("gethostbyname() error\n");
		exit(1);
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(LISTEN_PORT);
	server.sin_addr=*((struct in_addr *)he->h_addr);

	if(-1 == connect(serverfd, (struct sockaddr *)(&server), sizeof(struct sockaddr))) {
		perror("connect");
		return 0;
	}
	return 1;
	/*
	int numbytes;
	//receive the welcome message from server
	
	if((numbytes=recv(serverfd,buffer,BUFFER_SIZE,0))==-1){
		perror("recv");
		exit(1);
	}
	buffer[numbytes]='\0';
	return (strcmp(buffer, "welcome") == 0);
	*/
}


int main(int argc,char * argv[]){
	if(argc < 2 || !init_client(argv[1])) {
		printf("init failed\n");
		exit(0);
	}
	if (!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);
	login_window=loginUI();
	regist_window=registUI();
	mainUI_window=mainUI();
	pop_window=new_small_window();
	gtk_widget_show_all(login_window);
	//g_thread_create((GThreadFunc)UI_thread, NULL, FALSE, NULL);  
	g_thread_create((GThreadFunc)recv_thread, NULL, FALSE, NULL);  
	g_thread_create((GThreadFunc)auto_update_thread, NULL, FALSE, NULL);
	//gdk_threads_enter(); 
	gtk_main(); 
	//gdk_threads_leave(); 

	return 0;
}
