//
//  testfts3.c
//  fst3
//
//  Created by vv on 16/2/19.
//  Copyright © 2016年 vv. All rights reserved.
//

#include <stdio.h>
#include "sqlite3.c"
#include "sqlite3ext.h"


//void t() {}

static char *querysql_prefix = "SELECT * FROM Book WHERE Book MATCH '\"baren\"'";
static char *querysql_suffix = "SELECT * FROM Book WHERE Book MATCH '\"akatosh\"'";
static char *querysql_infix = "SELECT * FROM Book WHERE Book MATCH '\"end\"'";

int maintest() {
    
    int i, j;
    int rows, cols;
    sqlite3 *db;
    char *errMsg = NULL;
    char **result;
    const sqlite3_tokenizer_module *ptr;
    char token_name[] = "character";
    
    if (sqlite3_open_v2("example.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
        return 0;
    }
    // remove existing table
    sqlite3_exec(db, "DROP TABLE Book;", 0, 0, &errMsg);
    // unicode61
    //sqlite3_exec(db, "CREATE VIRTUAL TABLE Book USING fts3(name TEXT NOT NULL, author TEXT, tokenize=simple);", 0, 0, &errMsg);
    sqlite3_exec(db, "CREATE VIRTUAL TABLE Book USING fts3(name TEXT NOT NULL, author TEXT, tokenize=unicode61);", 0, 0, &errMsg);

    // insert test data
    sqlite3_exec(db, "INSERT INTO Book VALUES('Biography of Barenziah, v1', 'Stern Gamboge');", 0, 0, &errMsg);
    sqlite3_exec(db, "INSERT INTO Book VALUES('Biography of Barenziah, v2', 'Stern Gamboge');", 0, 0, &errMsg);
    sqlite3_exec(db, "INSERT INTO Book VALUES('Biography of Barenziah, v3', 'Stern Gamboge');", 0, 0, &errMsg);
    sqlite3_exec(db, "INSERT INTO Book VALUES('The Alduin-Akatosh Dichotomy', 'Alexandre Simon');", 0, 0, &errMsg);
    sqlite3_exec(db, "INSERT INTO Book VALUES('The Legendary Sancre Tor', 'Matera Chapel');", 0, 0, &errMsg);
    sqlite3_exec(db, "INSERT INTO Book VALUES('The Legendary Scourge, v3', 'Marobar Sul');", 0, 0, &errMsg);
    // test prefix matches
    
    sqlite3_get_table(db, querysql_prefix, &result, &rows, &cols, &errMsg);

    printf("query results for %s\n", querysql_prefix);
    for (i = 0; i <= rows; i++) {
        for (j = 0; j< cols; j++) {
            printf("%s\t", result[i*cols + j]);
        }
        printf("\n");
    }
    sqlite3_free_table(result);
    
    printf("\n");
    printf("hi,,,");
    
    getchar();
    return 0;
}