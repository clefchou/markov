#include"domysql.h"
#define Markovlenth 10
#define Hinf 1.4
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
	/*查询是否已经存在在数据库中了，如果没有则添加第一个值*/
	sprintf(query, "select count from src_data where name = '%s'", c);
	if (mysql_real_query(&mysql, query, (unsigned long)strlen(query))) {
		cout << "mysql_real_query failure!" << endl;
	}
	res = mysql_store_result(&mysql);
	if (NULL == res) {	
		printf("search failure!\n");
	}
	else {	//有字段的记录，将count值+1
		if (row = mysql_fetch_row(res)) {
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
		}
		else {//没有字段的记录 创建新记录
			int len = strlen(c);
			sprintf(query, "insert into src_data(name,len,count) values ('%s','%d','1')", c, len);
			while (mysql_query(&mysql, query) != 0) {  //执行创建语句
				printf("insert datalist error\n");
				sprintf(query, "insert into src_data(name) values ('%s')", c);
				errorflag = 1;
			}
			if (errorflag) {
				printf("\n\n\n\\n\n\n error num = %d \n\n\n\n\n\n\n", errornum);
				errorflag = 0;
			}
		}
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
		//printf("%s: %d\n", c, count);
		return count;
	}
	//printf("insert datalist %s success \n", c);
	return -1;

}

double Do_mysql::markov(int len) {
	Do_mysql *cacu = new Do_mysql();
	cacu->longBytes = this->longBytes;
	int errornum = 0, errorflag = 0;
	sprintf(query, "select (name) from src_data where len = '%d'", len);

	if (mysql_real_query(&mysql, query, (unsigned long)strlen(query))) {
		cout << "mysql_real_query failure!" << endl;

	}
	res = mysql_store_result(&mysql);
	if (NULL == res) {	//没有字段的记录
		return 0;
	}
	
	while (row = mysql_fetch_row(res)) {
		char *name = row[0];
		//printf("1");
			cacu->caculate(name);
	}
	//printf("insert datalist %s success \n", c);
	return 0;

}

result_H Do_mysql::markov(int len,int type) {
	Do_mysql *Hcacu = new Do_mysql();
	result_H ret,r;
	r.H = r.H_c = r.rongyu = r.I = 0;
	Hcacu->longBytes = this->longBytes;
	int errornum = 0, errorflag = 0;
	sprintf(query, "select (name) from src_data where len = '%d'", len);

	if (mysql_real_query(&mysql, query, (unsigned long)strlen(query))) {
		cout << "mysql_real_query failure!" << endl;

	}
	res = mysql_store_result(&mysql);
	if (NULL == res) {	//没有字段的记录
		ret.len = 0;
		return ret;
	}

	while (row = mysql_fetch_row(res)) {
		char *name = row[0];
		ret = Hcacu->markov_H(name);
		r.H -= ret.H;
		r.H_c -= ret.H_c;
		r.rongyu = (r.H - Hinf) / r.H;	//每次都更新冗余度，直到r[lenc-1].H是真的H，此时冗余度也是H对应的冗余了。
		r.I = r.H - Hinf;
	}
	//printf("insert datalist %s success \n", c);
	return r;

}
int Do_mysql::caculate(char * src) {
	int p, count = 0;
	double joint_probability = 0, cond_probability = 0;
	double symbol_info = 0, cond_symbol_info = 0;
	int totalnum;
	int len = strlen(src);
	totalnum = longBytes - len + 1;
	count = this->select_list(src);	//查询数据库出现次数
	if (count == 0)
		return 0;//没有数据就不存入数据库
	joint_probability = double(count) / totalnum;
	symbol_info = log10(1 / joint_probability) / log10(2);
	printf("joint probability of %s is %lf	symbol info is %lf\n", src, joint_probability, symbol_info);
	if (len > 1) {
		char temp = src[len - 1];
		src[len - 1] = '\0';
		count = this->select_list(src);
		cond_probability = joint_probability / (double(count) / (totalnum + 1));
		if (cond_probability > 1)
			cond_probability = 1;
		cond_symbol_info = symbol_info - ((log10(double(totalnum + 1) / count) / log10(2)));
		src[len - 1] = temp;
		src[len] = '\0';

		printf("cond probability of %s is %lf	cond symbol info is %lf\n\n", src, cond_probability, cond_symbol_info);

	}
	this->insert_result(src, joint_probability, cond_probability, symbol_info, cond_symbol_info, len);
	return 0;
}

result_H Do_mysql::markov_H(char * src) {
	char c_cp[] = { "cp" };
	char c_jp[] = { "jp" };
	int len = strlen(src);
	int i;
	result_H ret;
	/*联合熵 ∑∑p(序列)*log2(p(序列))*/

	double jp = this->select_name(src, c_jp);
	if (jp) {
		ret.H = jp*log2(jp);
	}
	if (len > 1) {
		char si[Markovlenth], sj[Markovlenth];
		for (i = 0; i < len - 1; i++) {
			si[i] = src[i];
			sj[i] = src[i + 1];
		}
		si[i] = sj[i] = '\0';		//定义SI SJ 用来计算极限熵和联合熵

		double cp = this->select_name(src, c_cp);
		if (cp) {
			ret.H_c = jp*log2(cp);
		}

		double inf_j = this->select_name(sj, c_cp);
		double inf_i = this->select_name(si, c_jp);


	}

	return ret;

}

double Do_mysql::select_name(char *name, char *data) {

	int errornum = 0, errorflag = 0;
	sprintf(query, "select (%s) from result_data where name = '%s'", data, name);

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