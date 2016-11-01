#ifndef Do_Mysql
#define Do_Mysql

#include"common.h"
#include "C:\Program Files\MySQL\MySQL Server 5.7\include\mysql.h"

class Do_mysql {
public:
	const char *user, *password;
	const char *db;
	unsigned int port;
	const char * unix_socket;
	char query[500];
    int longBytes, mklength;
	MYSQL mysql, *sock;
	MYSQL_RES *res;
	MYSQL_ROW row;
	Do_mysql() {
		//MYSQL mysql, *sock;
		
		mysql_init(&mysql);
		user = "root";	//用户名
		password = "zqtehyhg";	//密码
		db = "markov";	//用到的数据库
		port = 3306;	//数据库端口
		if (mysql_real_connect(&mysql, "localhost", user, password, db, port, NULL, 0))	//这里设置数据库连接的配置
			printf("connect success!\n");
	}
	void insert_list(char *listname);
	void update_list(char *listname);
	int select_list(char *listname);
	double markov(int len);
	result_H markov(int len,int type);
	result_H markov_H(char * src);
	int caculate(char* src);
	double select_name(char *name,char *data);
	void insert_result(char *c, double jp, double cp, double si, double cs, int len);
	
};


#endif
