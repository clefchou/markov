#include"domysql.h"


void Do_mysql::insert_list(char *c) {

	int errornum = 0,errorflag = 0;
	//sprintf(query, "create table %s( id int(4) not null primary key auto_increment,num int(1) not null)", c);
	sprintf(query, "insert into src_data(name,count) values('%s','%d')", c,0);
	while (mysql_query(&mysql, query) != 0) {  //执行创建语句
		printf("insert datalist error\n");
		sprintf(query, "insert into src_data(name,count) values('%s','%d')", c, 0);
		errorflag = 1;
	}
	if (errorflag) {
		printf("\n\n\n\\n\n\n error num = %d \n\n\n\n\n\n\n", errornum);
		errorflag = 0;
	}
	//printf("insert datalist %s success \n", c);


}
void Do_mysql::insert_result(char *c,double jp,double cp,double si,double cs,int len) {

	int errornum = 0, errorflag = 0;
	//sprintf(query, "create table %s( id int(4) not null primary key auto_increment,num int(1) not null)", c);
	sprintf(query, "insert into result_data(name,jp,cp,si,cs,len) values('%s','%.16lf','%.16lf','%.16lf','%.16lf','%d')", c,jp,cp,si,cs,len);
	while (mysql_query(&mysql, query) != 0) {  //执行创建语句
		printf("insert datalist error\n");
		sprintf(query, "insert into result_data(name,jp,cp,si,cs,len) values('%s','%.16lf','%.16lf','%.16lf','%.16lf','%d')", c, jp, cp, si, cs, len);
		errorflag = 1;
	}
	if (errorflag) {
		printf("\n\n\n\\n\n\n error num = %d \n\n\n\n\n\n\n", errornum);
		errorflag = 0;
	}
	//printf("insert datalist %s success \n", c);


}

void Do_mysql::update_list(char *c) {

	int errornum = 0, errorflag = 0;
	//sprintf(query, "create table %s( id int(4) not null primary key auto_increment,num int(1) not null)", c);
	sprintf(query, "update src_data set count=count+1 where name = '%s'", c);
	while (mysql_query(&mysql, query) != 0) {  //执行创建语句
		printf("update datalist error\n");
		sprintf(query, "update src_data set count=count+1 where name = '%s'", c);
		errorflag = 1;
	}
	if (errorflag) {
		printf("\n\n\n\\n\n\n error num = %d \n\n\n\n\n\n\n", errornum);
		errorflag = 0;
	}
	//printf("insert datalist %s success \n", c);


}

int Do_mysql::select_list(char *c) {

	int errornum = 0, errorflag = 0;
	//sprintf(query, "create table %s( id int(4) not null primary key auto_increment,num int(1) not null)", c);
	sprintf(query, "select (count) from src_data where name = '%s'", c);

	if (mysql_real_query(&mysql, query, (unsigned long)strlen(query))){
		cout << "mysql_real_query failure!" << endl;

	}
	res = mysql_store_result(&mysql);
	if (NULL == res){	//没有字段的记录
		printf("the count of %s is 0!\n", c);
		return -1;		
	}
	if (row = mysql_fetch_row(res)){
		int count = atoi(row[0]);
		printf("%s: %d\n", c, count);
		return count;
	}
	//printf("insert datalist %s success \n", c);
	return -1;

}

double Do_mysql::select_name(char *name,char *data) {

	int errornum = 0, errorflag = 0;
	sprintf(query, "select (%s) from result_data where name = '%s'", data,name);

	if (mysql_real_query(&mysql, query, (unsigned long)strlen(query))) {
		cout << "mysql_real_query failure!" << endl;

	}
	res = mysql_store_result(&mysql);
	if (NULL == res) {	//没有字段的记录

		return 0;
	}
	if (row = mysql_fetch_row(res)) {
		double count = atof(row[0]);
		//printf("the %s of %s : is %lf\n", data, name,count);
		return count;
	}
	//printf("insert datalist %s success \n", c);
	return 0;

}

