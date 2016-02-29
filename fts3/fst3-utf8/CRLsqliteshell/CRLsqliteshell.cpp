#include "stdafx.h"

#include "./sqlite-amalgamation-3090100/sqlite3.h"

#include "stdio.h"
#include <vcclr.h>
using namespace System;
using namespace System::Text;

static Encoding^ GBKEndoding()
{
	return Encoding::GetEncoding("gb18030");
}
static Encoding^ Utf8Endoding()
{
	return Encoding::GetEncoding("utf-8");
}
static String^ gbk2utf8(String^ s) {
	return Utf8Endoding()->GetString(Encoding::Convert(GBKEndoding(), Utf8Endoding(), GBKEndoding()->GetBytes(s)));
}
static String ^ utf82gbk(String^ s) {
	return GBKEndoding()->GetString(Encoding::Convert(Utf8Endoding(), GBKEndoding(), Utf8Endoding()->GetBytes(s)));
}

char *newArray(String ^ s) {
	array<unsigned char, 1> ^a = System::Text::UTF8Encoding().GetBytes(s);
	char *p = (char *)malloc(a->Length + 1);
	for (int i = 0; i < a->Length; i++) {
		p[i] = a[i];
	}
	p[a->Length] = 0;
	return p;
}


#define ASSERT(value) if (!(value)) { exit(0); /* __asm {int 3}*/}

#define NULL 0

static int sqPrepare(
	sqlite3 *db,            // Database handle
	const char *zSql,       // SQL statement, UTF-8 encoded
	int nByte,              // set -1, if zSql end with null, else to set it as nbytes of the buf  // Maximum length of zSql in bytes.
	sqlite3_stmt **ppStmt,  // OUT: Statement handle
	const char **pzTail     // OUT: Pointer to unused portion of zSql
	){
	int rc;
	rc = sqlite3_prepare_v2(db, zSql, -1,
		ppStmt, NULL
		);
	if (rc != SQLITE_OK || *ppStmt == NULL) {
		printf("Error in sqPrepare():  %s", sqlite3_errmsg(db));
		ASSERT(0);
	}
	return rc;
}

static int sqExec(
	sqlite3 *db,                // The database on which the SQL executes
	const char *zSql,           // The SQL to be executed
	sqlite3_callback xCallback, // Invoke this callback routine
	void *pArg,                 // First argument to xCallback()
	char **pzErrMsg             // Write error messages here
	){
	int rc;
	rc = sqlite3_exec(db, zSql, xCallback, 0, pzErrMsg);
	if (rc != SQLITE_OK){
		printf("SQL error: %s\n", *pzErrMsg);
		sqlite3_free(*pzErrMsg);
		ASSERT(0);
	}
	return rc;
}

// 某个utf8 字符占几个字节
// c: 必须指向utf8 字符串
static int utf8len(char *c) {
	unsigned char c1 = c[0];
	int len = -1;
	if ((c1 & 0x80) == 0) {  // 0b10000000
		len = 1;
	}
	else if ((c1 & 0xF0) == 0xF0) {  // 0b11110000
		len = 4;
	}
	else if ((c1 & 0xE0) == 0xE0) {  // 0b11100000
		len = 3;
	}
	else if ((c1 & 0xC0) == 0xC0) {  // 0b11000000 
		len = 2;
	}
	else {
		return -1;
	}
	return len;
}

/*
** Assuming z points to the first byte of a UTF-8 character,
** advance z to point to the first byte of the next UTF-8 character.
*/
// 计算字符个数
// 实现参考sqlite3 的lengthFunc 函数
static int utf8strlen(char *str) {
	int len;
	const unsigned char *z = (const unsigned char *)str;
	if (z == 0) {
		return -1;
	}
	len = 0;
	while (*z){
		len++;
		//SQLITE_SKIP_UTF8(z);
		if ((*(z++)) >= 0xc0) {
			while ((*z & 0xc0) == 0x80){ z++; }
		}
	}
	return len;
}

// utf8 编码规则
/*
1字节 0xxxxxxx
2字节 110xxxxx 10xxxxxx 0xC0 0x80
3字节 1110xxxx 10xxxxxx 10xxxxxx
4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
// 假定z 指向第一个utf8 字符，函数执行完以后z 指向下一个字符
static char *nextc(char *z) {
	if (z == 0) { return 0; }
	if (*z == 0) {
		return 0;
	}
	++z;
	while ((*z & 0xC0) == 0x80) { ++z; }  // 只要最高位是10 开头就继续移动指针
	return z;
}

static char *at(char *z, int pos) {
	char *t = z;
	int i;
	for (i = 0; i < pos; i++) {
		t = nextc(t);
	}
	return t;
}

static int utf8eq(char *c1, char *c2) {
	int i;
	if (c1 == 0 || c2 == 0 || *c1 == 0 || *c2 == 0) {
		return -1;
	}
	int len1 = utf8len(c1);
	int len2 = utf8len(c2);
	if (len1 != len2) {
		return 0;
	}
	else {
		for (i = 0; i < len1; i++) {
			if (c1[i] != c2[i]) {
				return 0;
			}
		}
	}
	return 1;
}


int tableExistQ(sqlite3 *db, char *tablename) {
	int i, j;
	int rc, idx, ncol, nbyte;
	char *errMsg = NULL;
	char **result;

	sqlite3_stmt *pStmt;
	char buf[512] = { 0 };

	sprintf(buf, "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '%s'; ", tablename);

	//rc = sqPrepare(db, "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name =  'enrondata1';", -1, &pStmt, NULL);
	rc = sqPrepare(db, buf, -1, &pStmt, NULL);
	/*
	rc = sqPrepare(db, "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name =  @p1;", -1, &pStmt, NULL);
	idx = sqlite3_bind_parameter_index(pStmt, "@p1");
	sqlite3_bind_text(pStmt, idx, tablename, strlen(tablename) + 1, SQLITE_STATIC);*/
	//rc = sqlite3_step(pStmt);
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		//char *data = sqlite3_column_text(pStmt, 0);
		int data = sqlite3_column_int(pStmt, 0);
		nbyte = sqlite3_column_bytes(pStmt, 0);  // 1nd col
		if (nbyte != 0) {
			//printf("%d\n", data);
			if (data != 0) {
				return 1;
			}
		}
		else {

		}
	}
	return 0;
}

/**/
void test2() {
	int i, j;
	int rows, cols;
	sqlite3 *db;
	char *errMsg = NULL;
	char **result;

	sqlite3_stmt *pStmt;
	int rc;
	int ncol;
	int nbyte;

	// create the database
	//if (sqlite3_open_v2("example.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
	if (sqlite3_open_v2("E:\\ZY_XY.dll", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
		ASSERT(0);
	}

	int existQ = tableExistQ(db, "enrondata1");
	if (existQ) {

	}
	else {

		sqlite3_exec(db, "DROP TABLE enrondata1; ", 0, 0, &errMsg);

		// create table with sqlite FTS4 support
		sqlite3_exec(db, "CREATE VIRTUAL TABLE enrondata1 USING fts4(content TEXT);", 0, 0, &errMsg);

		// insert test data
		sqlite3_exec(db, "INSERT INTO enrondata1 (content ) VALUES('SQLite is a linux software...');", 0, 0, &errMsg);
		//sqlite3_exec(db, "SELECT * FROM enrondata1 WHERE content MATCH 'linux';", callbackDB, "first param for callback func", &errMsg);

		sqlite3_exec(db, "INSERT INTO enrondata1 (content ) VALUES('我 是 学 拖拉机 专业 的');", 0, 0, &errMsg);
	}


	//rc = sqPrepare(db, "SELECT * FROM enrondata1 WHERE content MATCH 'linux';", -1, &pStmt, NULL);

	rc = sqPrepare(db, "SELECT * FROM enrondata1 WHERE content MATCH '拖拉机';", -1, &pStmt, NULL);


	//rc = sqlite3_step(pStmt);
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		char *data = (char *)sqlite3_column_text(pStmt, 0);
		nbyte = sqlite3_column_bytes(pStmt, 0);  // 1nd col
		if (nbyte != 0) {
			printf("%s\n", data);
		}
		else {

		}
	}


	// V table
	int vexistQ = tableExistQ(db, "ATestV");
	if (vexistQ) {
		sqlite3_exec(db, "DROP TABLE ATestV; ", 0, 0, &errMsg);
	}
	//else {
	char *createSql = "CREATE TABLE ATestV ( \
		ATestid INTEGER NOT NULL PRIMARY KEY, \
		Maintitle TEXT, \
		A VARCHAR(400), \
		B VARCHAR(400), \
		C VARCHAR(400), \
		D VARCHAR(400), \
		E VARCHAR(400), \
		F VARCHAR(400), \
		Answer VARCHAR(1) NOT NULL CHECK(Answer in('A', 'B', 'C', 'D', 'E', 'F', '?')), \
		ItemNum INTEGER NOT NULL CHECK(ItemNum>2 and ItemNum<7) \
		, CItem VARCHAR(30) DEFAULT '', CAnswer VARCHAR(30) DEFAULT '', MaintitleLen INTEGER); ";
	sqlite3_exec(db, createSql, 0, 0, &errMsg);
	//}

	rc = sqPrepare(db, "SELECT * FROM atest where atestid = 25000002;", -1, &pStmt, NULL);

	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		int ATestid = sqlite3_column_int(pStmt, 0);
		char *tmp = (char *)sqlite3_column_text(pStmt, 1);
		//String ^ Maintitle = gcnew String(tmp);
		//String ^MaintitleGBK = utf82gbk(Maintitle);
		nbyte = sqlite3_column_bytes(pStmt, 1);  // 2nd col
		if (nbyte != 0) {
			printf("%s\n", tmp);
		}
		else {

		}
	}

	
	sqlite3_close_v2(db);

	printf("hi2,,,");
	getchar();
	
}


String^ test3() {

	int i, j;
	int rows, cols;
	sqlite3 *db;
	char *errMsg = NULL;
	char **result;

	sqlite3_stmt *pStmt;
	int rc;
	int ncol;
	int nbyte;

	// create the database
	//if (sqlite3_open_v2("example.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
	if (sqlite3_open_v2("E:\\ZY_XY.dll", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
		ASSERT(0);
	}

	sqlite3_exec(db, "DROP TABLE enrondata1; ", 0, 0, &errMsg);
	int existQ = tableExistQ(db, "enrondata1");
	if (existQ) {

	}
	else {

		sqlite3_exec(db, "DROP TABLE enrondata1; ", 0, 0, &errMsg);

		// create table with sqlite FTS4 support
		//sqlite3_exec(db, "CREATE VIRTUAL TABLE enrondata1 USING fts4(content TEXT);", 0, 0, &errMsg);
		//sqlite3_exec(db, "CREATE TABLE enrondata1 USING fts4(content TEXT);", 0, 0, &errMsg);
		sqlite3_exec(db, "CREATE TABLE enrondata1 (content TEXT);", 0, 0, &errMsg);

		// insert test data
		sqlite3_exec(db, "INSERT INTO enrondata1 (content ) VALUES('SQLite is a linux software...');", 0, 0, &errMsg);
		//sqlite3_exec(db, "SELECT * FROM enrondata1 WHERE content MATCH 'linux';", callbackDB, "first param for callback func", &errMsg);

		sqlite3_exec(db, "INSERT INTO enrondata1 (content ) VALUES('我 是 学 拖拉机 专业 的');", 0, 0, &errMsg);
	}


	//rc = sqPrepare(db, "SELECT * FROM enrondata1 WHERE content MATCH 'linux';", -1, &pStmt, NULL);

	//rc = sqPrepare(db, "SELECT * FROM enrondata1 WHERE content MATCH '拖拉机';", -1, &pStmt, NULL);

	rc = sqPrepare(db, "SELECT * FROM enrondata1;", -1, &pStmt, NULL);

	//rc = sqlite3_step(pStmt);
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		char *data = (char *)sqlite3_column_text(pStmt, 0);
		nbyte = sqlite3_column_bytes(pStmt, 0);  // 1nd col
		if (nbyte != 0) {
			printf("%s\n", data);
		}
		else {

		}
	}

	// V table
	int vexistQ = tableExistQ(db, "ATestV");
	if (vexistQ) {
		//sqlite3_exec(db, "DROP TABLE ATestV; ", 0, 0, &errMsg);
	}
	//else {

	sqlite3_exec(db, "DROP TABLE ATestV; ", 0, 0, &errMsg);
	char *createSql = "CREATE VIRTUAL TABLE ATestV USING fts4  ( \
		Maintitle TEXT); ";

	sqlite3_exec(db, createSql, 0, 0, &errMsg);
	//}

	rc = sqPrepare(db, "SELECT * FROM atest where atestid = 25000002;", -1, &pStmt, NULL);

	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		int ATestid = sqlite3_column_int(pStmt, 0);
		char *tmp = (char *)sqlite3_column_text(pStmt, 1);
		String ^ sutf8 = gcnew String(tmp);
		char *utf8 = newArray(sutf8);
		//String ^sgbk = utf82gbk(sutf8);

		char buf[1024 * 2] = { 0 };

		//sprintf(buf, "INSERT INTO enrondata1 (content ) VALUES('%s');", tmp);
		sprintf(buf, "INSERT INTO ATestV (Maintitle) VALUES('%s');", tmp);
		sqlite3_exec(db, buf, 0, 0, &errMsg);

		nbyte = sqlite3_column_bytes(pStmt, 1);  // 2nd col
		if (nbyte != 0) {
			printf("%s\n", tmp);
		}
		else {

		}
	}


	sqlite3_close_v2(db);

	printf("hi2,,,");
	getchar();


	return gcnew String("a");
}

long sizef(FILE *pf) {
	long cur, siz;
	cur = ftell(pf);
	fseek(pf, 0, SEEK_END);
	siz = ftell(pf);
	fseek(pf, cur, SEEK_SET);
	return siz;
}


char *data(char *fname, long *out_siz_buf) {
	FILE *pf;
	char *buf = NULL;
	long siz, siz_r = 0;
	if ((pf = fopen(fname, "rb")) != 0) {
		siz = sizef(pf);
		buf = (char*)malloc(siz+1); if (buf == 0) { printf("malloc fail. in data()"); }
		memset(buf, 0, siz + 1);  // 后面附加一个0字节
		siz_r = fread(buf, 1, siz, pf); if (siz_r != siz) { printf("Error in data(). fread file siz not correct"); }
	}
	else {
		printf("can't open file '%s' in %s", fname, "data");
	}
	*out_siz_buf = siz_r;
	fclose(pf);
	return buf;
}

void dataWrite(char *fname, char *dat, int siz) {
	FILE *pf;
	if ((pf = fopen(fname, "wb")) != 0) {
		fwrite(dat, siz, 1, pf);
	}
	else {
		printf("can't open file '%s' in %s", fname, "dataWrite");
	}
	fclose(pf);
}

char *new2Mbytes() {
	char *p = (char*)malloc(1024*2+1);
	memset(p, 0, 1024 * 2 + 1);
	return p;
}

// 取一列的文本数据
void sq_col_txt_data(sqlite3_stmt *pStmt, int idx, char**ps, int psidx) {  // ps 指针数组，存返回结果
	char *dt = (char *)sqlite3_column_text(pStmt, idx);
	char *colname = (char *)sqlite3_column_name(pStmt, idx);
	int nbyte = sqlite3_column_bytes(pStmt, idx);  // 1nd col
	if (nbyte != 0) {
		dataWrite("doc.txt", dt, nbyte);
		int a = system("demo.exe");
		long len;
		ps[psidx] = data("data.txt", &len);
	} else {
		printf("warning: nbyte is 0. in function sq_col_data. idx=%d", idx);
		ps[psidx] = "";
	}
}

void InsertATestV(sqlite3 *db) {
	int i, j;
	int rows, cols;
	//sqlite3 *db;
	char *errMsg = NULL;
	char **result;

	sqlite3_stmt *pStmt;
	int rc;
	int ncol;
	int nbyte;
	//static char sbuf[1024] = {0};

	sqlite3_exec(db, "DROP TABLE ATestV; ", 0, 0, &errMsg);
	char *createSql = "CREATE VIRTUAL TABLE ATestV USING fts4  ( \
					  							ATestid INTEGER NOT NULL PRIMARY KEY, \
					  					  		Maintitle TEXT, \
												A VARCHAR(400), \
												B VARCHAR(400), \
												C VARCHAR(400), \
												D VARCHAR(400), \
												E VARCHAR(400), \
												F VARCHAR(400)  \
												)";

	sqlite3_exec(db, createSql, 0, 0, &errMsg);

	char* ps[8] = { 0 };


	//rc = sqPrepare(db, "SELECT * FROM atest where atestid = 25000002;", -1, &pStmt, NULL);
	rc = sqPrepare(db, "SELECT * FROM atest limit 10;", -1, &pStmt, NULL);
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		ps[0] = new2Mbytes();
		sprintf(ps[0], "%d", sqlite3_column_int(pStmt, 0));  // ATestid
		
		sq_col_txt_data(pStmt, 1, ps, 1);  // Maintitle
		sq_col_txt_data(pStmt, 2, ps, 2);  // A
		sq_col_txt_data(pStmt, 3, ps, 3);  // B
		sq_col_txt_data(pStmt, 4, ps, 4);  // C
		sq_col_txt_data(pStmt, 5, ps, 5);  // D
		sq_col_txt_data(pStmt, 6, ps, 6);  // E
		sq_col_txt_data(pStmt, 7, ps, 7);  // F

		char *sql = "INSERT INTO ATestV (ATestid, Maintitle, A, B, C, D, E, F) VALUES(%s, '%s', '%s', '%s', '%s', '%s', '%s', '%s');";
		char *buf = (char*)malloc(1024 * 16 + 1);
		memset(buf, 0, 1024 * 16 + 1);
		
		sprintf(buf, sql, ps[0], ps[1], ps[2], ps[3], ps[4], ps[5], ps[6], ps[7]);
		sqlite3_exec(db, buf, 0, 0, &errMsg);


		free(buf);
		if (strlen(ps[0]) > 0) free(ps[0]);
		if (strlen(ps[1]) > 0) free(ps[1]);
		if (strlen(ps[2]) > 0) free(ps[2]);
		if (strlen(ps[3]) > 0) free(ps[3]);
		if (strlen(ps[4]) > 0) free(ps[4]);
		if (strlen(ps[5]) > 0) free(ps[5]);
		if (strlen(ps[6]) > 0) free(ps[6]);
		if (strlen(ps[7]) > 0) free(ps[7]);

		
		/*
		char *dt = (char *)sqlite3_column_text(pStmt, 1);
		char *colname = (char *)sqlite3_column_name(pStmt, 1);
		nbyte = sqlite3_column_bytes(pStmt, 1);  // 1nd col
		if (nbyte != 0) {
			dataWrite("doc.txt", dt, nbyte);
			int a = system("demo.exe");
			long len;
			ps[1] = data("data.txt", &len);


			
			char *buf = (char*)malloc(len + 1024 + 1);
			memset(buf, 0, len + 1024 + 1);
			char *sql = "INSERT INTO ATestV (Maintitle) VALUES('%s');";
			//char *sql = "INSERT INTO ATestV (%s) VALUES('%s');";
			sprintf(buf, sql, txt);
			printf("@@@@@@%s@@@@@@", buf);
			sqlite3_exec(db, buf, 0, 0, &errMsg);

			free(buf);
			free(txt);
			
		}
		else {
			printf("Error: ATest Maintitle nbyte is 0");
		}*/
	}
}



int main(array<System::String ^> ^args){
	
	system("chcp 65001");  // 控制台黑黑的窗口 ->右键点击 ->改字体 才能正确打印utf8 汉字
	Console::WriteLine(L"严");
	//int a = system("demo.exe");

	/*
	long siz;
	char *o = data("data.txt", &siz);
	char *buf = (char*)malloc(siz + 1024 + 1);
	memset(buf, 0, siz + 1024 + 1);
	char *sql = "INSERT INTO ATestV (Maintitle) VALUES('%s');";
	sprintf(buf, sql, o);
	
	printf("\n#####%s$$$$$\n",o);
	*/

	int i, j;
	int rows, cols;
	sqlite3 *db;
	char *errMsg = NULL;
	char **result;

	sqlite3_stmt *pStmt;
	int rc;
	int ncol;
	int nbyte;

	if (sqlite3_open_v2("E:\\ZY_XY.dll", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
		ASSERT(0);
	}

	InsertATestV(db);

	/*

	sqlite3_exec(db, "DROP TABLE ATestV; ", 0, 0, &errMsg);
	char *createSql = "CREATE VIRTUAL TABLE ATestV USING fts4  ( \
					  		Maintitle TEXT); ";

	sqlite3_exec(db, createSql, 0, 0, &errMsg);

	

	sqlite3_exec(db, "INSERT INTO ATestV (Maintitle) VALUES('我 是 学 拖拉机 专业 的');", 0, 0, &errMsg);

	sqlite3_exec(db, "INSERT INTO ATestV (Maintitle) VALUES('我 是 学 手扶 拖拉机 专业 的');", 0, 0, &errMsg);

	sqlite3_exec(db, buf, 0, 0, &errMsg);

	rc = sqPrepare(db, "SELECT * FROM ATestV where Maintitle MATCH '手扶'", -1, &pStmt, NULL);

	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		ncol = sqlite3_column_count(pStmt);
		char *tmp = (char *)sqlite3_column_text(pStmt, 0);
		printf("%s\n", tmp);
	}*/

	sqlite3_close_v2(db);


	//test3();

	//unsigned char yan[] = { 0xE4, 0xB8, 0xA5, 0 }; // 严 e4 b8 a5
	//unsigned char yan2[] = "严";
	//int len = strlen((char*)yan);
	//printf("%s, %02x %02x %02x\n", yan, yan[0], yan[1], yan[2]);
	//printf("%s\n", "严");

	//array<unsigned char, 1> ^y = gcnew array<unsigned char, 1>(len);
	//printf("lenght: %d\n", y->Length);
	//
	//for (int i = 0; i < len; i++) {
	//	y[i] = yan[i];
	//}

	//String^ utf8 = System::Text::UTF8Encoding().GetString(y);
	//String^ gbk = utf82gbk(utf8);
	//array<unsigned char, 1> ^g = GBKEndoding()->GetBytes(gbk);
	//printf("%02x %02x\n", g[0], g[1]);  // 严 D1 CF

	////array<unsigned char, 1> ^a = System::Text::UTF8Encoding().GetBytes("严"); // Encoding.UTF8.GetBytes

	////test3();
 //   Console::WriteLine(L"Hello World");
    return 0;
}
