#include <Windows.h>
#include "common.h"
#include "domysql.h"
#define Markovlenth  3		//����������Ҫ�Ľ���
#define Hinf 1.4
static string content = {};
static int longBytes = 0;
Do_mysql *dm = new Do_mysql();
int replacefile(char f[]) {
	FILE * fp = fopen(f, "rb");
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 0, SEEK_END);
	longBytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	FILE * fp1 = fopen("sourcefile.txt", "wb");
	for (int i = 0; i < longBytes; i++) {
		char t;
		char buffer[] = { ' ' };
		fscanf(fp, "%c", &t);
		if (t > 122 || t < 97) {
			if (t > 90 || t < 65) {
				//fseek(fp1, -1, SEEK_CUR);
				fwrite(buffer, sizeof(buffer), 1, fp1);
			}
			else {
				t = t + 32;
				char temp[] = { t };
				fwrite(temp, sizeof(temp), 1, fp1);
			}
		}
		else {
			char temp[] = { t };
		
			fwrite(temp, sizeof(temp), 1, fp1);
		}
	
		
	}
	fclose(fp);
	fclose(fp1);
	return 0;
}
void string_to_char(string c, char *buf) {
	int i;
	for ( i = 0; i < c.length(); i++) {
		buf[i] = c[i];
	}
	buf[i] = '\0';


}
int data_insert(int len) {
	char buffer[Markovlenth];
	FILE * fp = fopen("sourcefile.txt", "r+b");
	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i <= (longBytes - len); i++) {
		fgets(buffer, len + 1, fp);
		fseek(fp, -len, SEEK_CUR);
		/*
		int ret = dm->select_list(buffer);		
		if (ret == -1)
			dm->insert_list(buffer);
		else*/
			dm->update_list(buffer);
		/*p = strncmp(buffer, src, 5);
		if (p == 0)
		count += 1;*/
		fseek(fp, 1, SEEK_CUR);
	}
	fclose(fp);
	return 0;
}


int caculate(char * src) {
	int p,count = 0;
	double joint_probability = 0, cond_probability = 0;
	double symbol_info = 0, cond_symbol_info = 0;
	int totalnum;
	int len = strlen(src);
	totalnum = longBytes - len + 1;
	count = dm->select_list(src);	//��ѯ���ݿ���ִ���
	if (count == 0)
		return 0;//û�����ݾͲ��������ݿ�
	joint_probability = double(count)/ totalnum;
	symbol_info = log10(1 / joint_probability)/log10(2);
	printf("joint probability of %s is %lf	symbol info is %lf\n", src, joint_probability,symbol_info);
	if (len > 1) {
		char temp = src[len - 1];
		src[len - 1] = '\0';
		count = dm->select_list(src);
		cond_probability = joint_probability / (double(count) / (totalnum + 1));
		if (cond_probability > 1)
			cond_probability = 1;
		cond_symbol_info = symbol_info - ((log10(double(totalnum + 1) / count)/log10(2)));
		src[len - 1] = temp;
		src[len] = '\0';

		printf("cond probability of %s is %lf	cond symbol info is %lf\n\n", src, cond_probability, cond_symbol_info);

	}
	dm->insert_result(src, joint_probability, cond_probability, symbol_info, cond_symbol_info,len);
	return 0;
}


int string_generate(int len) {			//��������
	char src[6];
		for (int i = 0; i < 26; i++) {
			char t = 'a' + i;
			content = content + t;
			if (len > 1)
				string_generate(len - 1);	//������峤�ȱ�1���͵�������-1���ȵ����У�Ŀ���Ǵ�a-z,aa-zzһֱ���ɵ���Ҫ�ĳ���
			else {
				//markov(content);
				
				string_to_char(content, src);	
				
					
				dm->insert_list(src);
				printf("%s\n", src);
			}
				content = content.substr(0, content.length() - 1);
			
}
	return 0;
}

int string_markov(int len) {			//�������ɲ�������ʺ��������ʲ�д�����ݿ�
	char src[6];
	for (int i = 0; i < 26; i++) {
		char t = 'a' + i;
		content = content + t;
		if (len > 1)
			string_markov(len - 1);
		else {
			string_to_char(content, src);
			caculate(src);			//�����ַ���src�ĸ��ʣ��������ʵȵȲ��������ݿ�
		}
		content = content.substr(0, content.length() - 1);
	}
	return 0;
}



result_H markov(char * src) {
	char c_cp[] = { "cp" };
	char c_jp[] = { "jp" };
	int len = strlen(src);
	int i;
	result_H ret;
	/*������ �ơ�p(����)*log2(p(����))*/
	
	double jp = dm->select_name(src, c_jp);
	if (jp) {
		ret.H = jp*log2(jp);
	}
	if (len > 1) {
		char si[Markovlenth], sj[Markovlenth];
		for ( i = 0; i < len - 1; i++) {
			si[i] = src[i];
			sj[i] = src[i + 1];
		}
		si[i] = sj[i] = '\0';		//����SI SJ �������㼫���غ�������
		
		double cp = dm->select_name(src, c_cp);
		if (cp) {
			ret.H_c = jp*log2(cp);
		}
		
		double inf_j = dm->select_name(sj, c_cp);
		double inf_i = dm->select_name(si, c_jp);
	
		
	}

	return ret;

}
int H_markov(int len, vector<result_H> &r) {			//�������ɲ�������ʺ��������ʲ�д�����ݿ�
	char src[6];

	for (int i = 0; i < 26; i++) {
		char t = 'a' + i;
		content = content + t;
		if (len > 1)
			H_markov(len - 1, r);
		else {
			string_to_char(content, src);
			result_H ret = markov(src);
			int lenc = strlen(src);
			r[lenc-1].H -= ret.H;
			r[lenc - 1].H_c -= ret.H_c;
			r[lenc - 1].rongyu = (r[lenc - 1].H - Hinf) / r[lenc - 1].H;	//ÿ�ζ���������ȣ�ֱ��r[lenc-1].H�����H����ʱ�����Ҳ��H��Ӧ�������ˡ�
			r[lenc - 1].I = r[lenc - 1].H - Hinf;
			printf("%lf\n", r[lenc - 1].H);
		}
		
		content = content.substr(0, content.length() - 1);

	}
	return 0;
}


int main() {
	char route[30] = "sample.txt";	
	vector<result_H> result;
	for (int i = 0; i < Markovlenth; i++) {
		result_H r;
		r.H = r.H_c = r.rongyu = r.I = 0;
		result.push_back(r);
	}
	//sock = mysql_real_connect(&my_connection, "127.0.0.1", "root", "zqtehyhg", "markov", 3306, 0, 0);
	//dm->connect();

	replacefile("\sample.txt");

	for (int len = 1; len < Markovlenth+1; len++) {	//���ɴ�a�� zzzzz������
		string_generate(len);
	}

	for (int len = 1; len < Markovlenth+1; len++) {	//ɨ�������е����ݲ�����������ݿ�
		data_insert(len);
		printf("data update complete: %d\n", len);
	}
	printf("data update complete! \n");
	
	for (int len = 1; len < Markovlenth + 1; len++) {	//�������ɲ�������ʺ��������ʲ�д�����ݿ�
		string_markov(len);
	}
	
	for (int len = 1; len < Markovlenth + 1; len++) {	//�����أ�����ȵȵ�
		H_markov(len,result);

	}

	FILE *logfile = fopen("logfile.txt", "w");	//�����ļ�����¼�غ�����ȣ�����ʲô�Ķ��������ݿ���
	for (int i = 0; i < Markovlenth; i++) {
		fprintf(logfile,"H = %lf	  H_c = %lf    ���� = %lf	I = %lf\n", result[i].H,result[i].H_c,result[i].rongyu,result[i].I);
	}
	fclose(logfile);

	//printf("//");
	
	return 0;
}