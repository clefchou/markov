#include <Windows.h>
#include "common.h"
#include "domysql.h"
#define Markovlenth  10		//这里设置需要的阶数
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
	
		dm->longBytes = longBytes;
		
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
		dm->update_list(buffer);
		fseek(fp, 1, SEEK_CUR);
	}
	fclose(fp);
	return 0;
}

int string_markov() {			//序列查找并计算概率和条件概率并写入数据库
	char src[6];
	for (int i = 1; i <= Markovlenth+1; i++) {
		dm->markov(i);	
	}
	return 0;
}



result_H markov(char * src) {
	char c_cp[] = { "cp" };
	char c_jp[] = { "jp" };
	int len = strlen(src);
	int i;
	result_H ret;
	/*联合熵 ∑∑p(序列)*log2(p(序列))*/
	
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
		si[i] = sj[i] = '\0';		//定义SI SJ 用来计算极限熵和联合熵
		
		double cp = dm->select_name(src, c_cp);
		if (cp) {
			ret.H_c = jp*log2(cp);
		}
		
		double inf_j = dm->select_name(sj, c_cp);
		double inf_i = dm->select_name(si, c_jp);
	
		
	}

	return ret;

}
void H_markov(int len, vector<result_H> &r) {			//序列生成并计算概率和条件概率并写入数据库

			result_H ret = dm->markov(len, 1);
			int lenc = len;
			r[lenc-1].H = ret.H;
			r[lenc - 1].H_c = ret.H_c;
			r[lenc - 1].rongyu = ret.rongyu;	//每次都更新冗余度，直到r[lenc-1].H是真的H，此时冗余度也是H对应的冗余了。
			r[lenc - 1].I = ret.I;
}


int main() {

	clock_t start_time, finish_time;
	double total_time;
	start_time = clock();
	char route[30] = "sample.txt";	
	vector<result_H> result;
	for (int i = 0; i < Markovlenth; i++) {
		result_H r;
		
		result.push_back(r);
	}

	replacefile("\sample.txt");

	for (int len = 1; len < Markovlenth+1; len++) {	//扫描文章中的数据并将其存入数据库
		data_insert(len);
		printf("data update complete: %d\n", len);
	}
	printf("data update complete! \n");
	
	string_markov();

	for (int len = 1; len < Markovlenth + 1; len++) {	//计算熵，冗余度等等
		H_markov(len,result);

	}

	FILE *logfile = fopen("logfile.txt", "w");	//创建文件，记录熵和冗余度，概率什么的都存在数据库里
	for (int i = 0; i < Markovlenth; i++) {
		fprintf(logfile,"H = %lf	  H_c = %lf    冗余 = %lf	I = %lf\n", result[i].H,result[i].H_c,result[i].rongyu,result[i].I);
	}
	fclose(logfile);
	

	finish_time = clock();
	FILE *timelog = fopen("timelog.txt", "w");
	total_time = ((double)(finish_time - start_time)) / CLOCKS_PER_SEC;
	fprintf(timelog, "%f seconds\n", total_time);
	return 0;
}