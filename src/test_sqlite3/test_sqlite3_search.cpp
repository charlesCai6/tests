//四、数据查询：
//    数据查询是每个关系型数据库都会提供的最基本功能，下面的代码示例将给出如何通过SQLite API获取数据。
//    1). 创建测试数据表。
//    2). 插入一条测试数据到该数据表以便于后面的查询。
//    3). 执行SELECT语句检索数据。
//    4). 删除测试表。

#include <sqlite3.h>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;

void doTest()
{
    sqlite3* conn = NULL;
    //1. 打开数据库
    int result = sqlite3_open("./mytest.db",&conn);
    if (result != SQLITE_OK) {
        sqlite3_close(conn);
        return;
    }
    const char* createTableSQL =
        "CREATE TABLE TESTTABLE (int_col INT, float_col REAL, string_col TEXT)";
    sqlite3_stmt* stmt = NULL;
    int len = strlen(createTableSQL);
    //2. 准备创建数据表，如果创建失败，需要用sqlite3_finalize释放sqlite3_stmt对象，以防止内存泄露。
    if (sqlite3_prepare_v2(conn,createTableSQL,len,&stmt,NULL) != SQLITE_OK) {
        if (stmt)
            sqlite3_finalize(stmt);
        sqlite3_close(conn);
        return;
    }
    //3. 通过sqlite3_step命令执行创建表的语句。对于DDL和DML语句而言，sqlite3_step执行正确的返回值
    //只有SQLITE_DONE，对于SELECT查询而言，如果有数据返回SQLITE_ROW，当到达结果集末尾时则返回
    //SQLITE_DONE。
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(conn);
        return;
    }
    //4. 释放创建表语句对象的资源。
    sqlite3_finalize(stmt);
    printf("Succeed to create test table now.\n");

    //5. 为后面的查询操作插入测试数据。
    sqlite3_stmt* stmt2 = NULL;
    const char* insertSQL = "INSERT INTO TESTTABLE VALUES(20,21.0,'this is a test.')";
    if (sqlite3_prepare_v2(conn,insertSQL,strlen(insertSQL),&stmt2,NULL) != SQLITE_OK) {
        if (stmt2)
            sqlite3_finalize(stmt2);
        sqlite3_close(conn);
        return;
    }
    if (sqlite3_step(stmt2) != SQLITE_DONE) {
        sqlite3_finalize(stmt2);
        sqlite3_close(conn);
        return;
    }
    printf("Succeed to insert test data.\n");
    sqlite3_finalize(stmt2);

    //6. 执行SELECT语句查询数据。
    const char* selectSQL = "SELECT * FROM TESTTABLE";
    sqlite3_stmt* stmt3 = NULL;
    if (sqlite3_prepare_v2(conn,selectSQL,strlen(selectSQL),&stmt3,NULL) != SQLITE_OK) {
        if (stmt3)
            sqlite3_finalize(stmt3);
        sqlite3_close(conn);
        return;
    }
    int fieldCount = sqlite3_column_count(stmt3);
    do {
        int r = sqlite3_step(stmt3);
        if (r == SQLITE_ROW) {
            for (int i = 0; i < fieldCount; ++i) {
                //这里需要先判断当前记录当前字段的类型，再根据返回的类型使用不同的API函数
                //获取实际的数据值。
                int vtype = sqlite3_column_type(stmt3,i);
                if (vtype == SQLITE_INTEGER) {
                    int v = sqlite3_column_int(stmt3,i);
                    printf("The INTEGER value is %d.\n",v);
                } else if (vtype == SQLITE_FLOAT) {
                    double v = sqlite3_column_double(stmt3,i);
                    printf("The DOUBLE value is %f.\n",v);
                } else if (vtype == SQLITE_TEXT) {
                    const char* v = (const char*)sqlite3_column_text(stmt3,i);
                    printf("The TEXT value is %s.\n",v);
                } else if (vtype == SQLITE_NULL) {
                    printf("This value is NULL.\n");
                }
            }
        } else if (r == SQLITE_DONE) {
            printf("Select Finished.\n");
            break;
        } else {
            printf("Failed to SELECT.\n");
            sqlite3_finalize(stmt3);
            sqlite3_close(conn);
            return;
        }
    } while (true);
    sqlite3_finalize(stmt3);

    //7. 为了方便下一次测试运行，我们这里需要删除该函数创建的数据表，否则在下次运行时将无法
    //创建该表，因为它已经存在。
    const char* dropSQL = "DROP TABLE TESTTABLE";
    sqlite3_stmt* stmt4 = NULL;
    if (sqlite3_prepare_v2(conn,dropSQL,strlen(dropSQL),&stmt4,NULL) != SQLITE_OK) {
        if (stmt4)
            sqlite3_finalize(stmt4);
        sqlite3_close(conn);
        return;
    }
    if (sqlite3_step(stmt4) == SQLITE_DONE) {
        printf("The test table has been dropped.\n");
    }
    sqlite3_finalize(stmt4);
    sqlite3_close(conn);
}

int main()
{
    doTest();
    return 0;
}
//输出结果如下：
//Succeed to create test table now.
//Succeed to insert test data.
//The INTEGER value is 20.
//The DOUBLE value is 21.000000.
//The TEXT value is this is a test..
//Select Finished.
//The test table has been dropped.
