#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mysql.h>
#include <string.h>

#define QLEN 1130
#define PAAMOUNT 10
#define PALEN 51
#define CONTLEN 51

/* 功能表
1.插入用户    // 唯一，非空约束
    INSERT INTO user VALUES('%nickname%','%email%','%password%','%reg_time%','%log_time%');
2.插入笔记    // 用户自定义约束
    INSERT INTO note VALUES('%title%','%username%','%keyword%','%ncontent%','%pub_time%','%mod_time%',%visiable%，%cid%);
3.删除分类    // 外键约束
    DELETE FROM category WHERE cname='%cname%';
4.连接查询:    // %belong.username% 加入的小组的组长和小组描述
    SELECT belong.groupname,usergroup.username,usergroup.gdescription FROM belong JOIN usergroup ON belong.groupname=usergroup.gname WHERE belong.username='%belong.username%';
5.嵌套查询:    // 跟 %username% 一个组的用户
    SELECT username FROM belong WHERE groupname IN (SELECT groupname FROM belong WHERE username='%username%' UNION SELECT gname FROM usergroup WHERE username='%username%') AND username<>'%username%';
6.分组查询    // 笔记评论数
    SELECT note.title,COUNT(comment.cid) FROM note LEFT JOIN comment ON note.title = comment.note AND note.username = '%note.username%' GROUP BY note.title;
*/
void printTips();    // 功能说明

int main(int argc, char *argv[])
{
    MYSQL mysql;    // 数据库连接的句柄
    MYSQL_RES *res = NULL;    // 查询结果集，结构类型
    MYSQL_ROW row;    // 存放一行查询结果的字符串数组
    int i, func, flag = 1;
    char query[QLEN], helpq[QLEN],tmp[PAAMOUNT][PALEN], contmp[CONTLEN];
    char para[PAAMOUNT][PALEN + 2], content[CONTLEN + 2];
    char lastnote[PALEN + 2];
    mysql_init(&mysql);    // 初始化MYSQL结构
    if(!mysql_real_connect(&mysql, "127.0.0.1", "root", "", "noter", 0, NULL, 0))    // 连接数据库
	{
		printf("%s\n", mysql_error(&mysql));
		exit(1);
	}
	printf("Access to the noter successfully!\n\n");
	mysql_query(&mysql, "set names gb2312");    // 编码格式，支持中文
	while(flag)
    {
        printTips();
        printf("\nChoose function:");
        scanf("%d", &func);
        getchar();
        system("cls");
        switch(func)
        {
        case 1:
            printf("功能1: 插入用户\n\n");
            printf("Input nickname:");
            scanf("%s", tmp[0]);
            getchar();
            printf("Input email:");
            scanf("%s", tmp[1]);
            getchar();
            printf("Input password:");
            scanf("%s", tmp[2]);
            getchar();
            printf("Input register time:");
            scanf("%[^\n]", tmp[3]);
            getchar();
            printf("Input last log time:");
            scanf("%[^\n]", tmp[4]);
            getchar();
            for(i = 0; i < 5; i ++)
            {
                if(strcmp("null", tmp[i]))
                {
                    strcpy(para[i],"\'");
                    strcat(para[i], tmp[i]);
                    strcat(para[i],"\'");
                }
                else
                {
                    strcpy(para[i], tmp[i]);
                }
            }
            strcpy(query, "INSERT INTO user VALUES(");
            for(i = 0; i < 5; i ++)
            {
                strcat(query, para[i]);
                if(i != 4)
                {
                    strcat(query, ",");
                }
                else
                {
                    strcat(query, ");");
                }
            }
    //        printf("query:%s\n", query);
            if(mysql_query(&mysql,query))
            {
                printf("Insert failed: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
                continue;
            }
            printf("Insert success!\n");
            // 重显示
            strcpy(query,"SELECT * FROM user WHERE nickname=");
            strcat(query,para[0]);
            strcat(query,";");
            if(mysql_real_query(&mysql, query, strlen(query)))    // 查询
            {
                printf("Fail to query: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
                continue;
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(res == NULL)
            {
                printf("Sorry,system error....\n");
                exit(1);
            }
            printf("Please check:\n");
            if((long int)mysql_num_rows(res))
            {

                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    printf("\n--- >>> \n");
                    printf("| Nickname: %s\n", row[0]);
                    printf("| Email: %s\n", row[1]);
                    printf("| Password: %s\n", row[2]);
                    printf("| Register time: %s\n", row[3]);
                    printf("| Last log time: %s\n", row[4]);
                    printf("<<< --- \n");
                }
            }
            mysql_free_result(res);    // 释放结果集
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 2:
            printf("功能2: 插入笔记\n\n");
            printf("Input title:");
            scanf("%[^\n]", tmp[0]);
            getchar();
            printf("Input username:");
            scanf("%s", tmp[1]);
            getchar();
            printf("Input keyword:");
            scanf("%s", tmp[2]);
            getchar();
            printf("Input content:");
            scanf("%[^\n]", contmp);
            getchar();
            printf("Input publish time:");
            scanf("%[^\n]", tmp[4]);
            getchar();
            printf("Input last modify time:");
            scanf("%[^\n]", tmp[5]);
            getchar();
            printf("Visible(1 for yes, 0 for no):");
            scanf("%s", tmp[7]);
            getchar();
            if(strcmp("0",tmp[7]) && strcmp("1",tmp[7]))
            {
                printf("Invalid visible.\n");
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
                continue;
            }
            // 展示所有可选分类
            strcpy(helpq,"SELECT cid,cname FROM category WHERE username=\'");
            strcat(helpq,tmp[1]);
            strcat(helpq,"\';");
            if(mysql_real_query(&mysql, helpq, strlen(helpq)))    // 查询
            {
                exit(1);
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(!(long int)mysql_num_rows(res))
            {
                strcpy(tmp[6], "null");
            }
            else
            {
                printf("Here are all your categories:\n");
                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    printf("%-3s : %s\n", row[0], row[1]);
                }
                printf("\nPlease choose category ID:");
                scanf("%s", tmp[6]);
                getchar();
            }
            mysql_free_result(res);    // 释放结果集

            for(i = 0; i < 8; i ++)
            {
                if(i != 3)
                {
                    if(strcmp("null", tmp[i]))
                    {
                        if(i != 6 && i != 7)
                        {
                            strcpy(para[i],"\'");
                            strcat(para[i], tmp[i]);
                            strcat(para[i],"\'");
                        }
                        else
                        {
                            strcpy(para[i], tmp[i]);
                        }
                    }
                    else
                    {
                        strcpy(para[i], tmp[i]);
                    }
                }
                else
                {
                    if(strcmp("null", contmp))
                    {
                        strcpy(content,"\'");
                        strcat(content, contmp);
                        strcat(content,"\'");
                    }
                    else
                    {
                        strcpy(content, "null");
                    }
                }
            }
            strcpy(query, "INSERT INTO note VALUES(");
            for(i = 0; i < 8; i ++)
            {
                if(i != 3)
                {
                    strcat(query, para[i]);
                }
                else
                {
                    strcat(query, content);
                }
                if(i != 7)
                {
                    strcat(query, ",");
                }
                else
                {
                    strcat(query, ");");
                }
            }
//            printf("query:%s\n", query);
            if(mysql_query(&mysql,query))
            {
                printf("Insert failed: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
                continue;
            }
            printf("Insert success!\n");
            // 重显示
            strcpy(query,"SELECT * FROM note WHERE title=");
            strcat(query,para[0]);
            strcat(query,"AND username=");
            strcat(query,para[1]);
            strcat(query,";");
            if(mysql_real_query(&mysql, query, strlen(query)))    // 查询
            {
                printf("Fail to query: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
                continue;
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(res == NULL)
            {
                printf("Sorry,system error....\n");
                exit(1);
            }
            printf("Please check:\n");
            if((long int)mysql_num_rows(res))
            {

                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    printf("\n--- >>> \n");
                    printf("| Title: %s\n", row[0]);
                    printf("| User: %s\n", row[1]);
                    printf("| Keyword: %s\n", row[2]);
                    printf("| Content: %s\n", row[3]);
                    printf("| Publish time: %s\n", row[4]);
                    printf("| Last modify time: %s\n", row[5]);
                    printf("| Visible: %s\n", row[6]);
                    printf("| Category ID: %s\n", row[7]);
                    printf("<<< --- \n");
                }
            }
            mysql_free_result(res);    // 释放结果集
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 3:
            printf("功能3: 删除分类\n\n");
            printf("Input category name:");
            scanf("%s",para[0]);
            getchar();
            strcpy(query, "DELETE FROM category WHERE cname=\'");
            strcat(query, para[0]);
            strcat(query, "\';");
    //        printf("query:%s\n", query);
            if(mysql_query(&mysql,query))
            {
                printf("%s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
            }
            printf("Delete success!\n");
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 4:
            printf("功能4：查询某用户加入的小组信息\n\n");
            printf("Input user's nickname:");
            scanf("%s",para[0]);
            getchar();
            strcpy(query, "SELECT belong.groupname,usergroup.username,usergroup.gdescription FROM belong JOIN usergroup ON belong.groupname=usergroup.gname WHERE belong.username=\'");
            strcat(query, para[0]);
            strcat(query, "\';");
    //        printf("query:%s\n", query);
            if(mysql_real_query(&mysql, query, strlen(query)))    // 查询
            {
                printf("Fail to query: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(res == NULL)
            {
                printf("Sorry,system error...\n");
                exit(1);
            }
            printf("%ld group(s) in total.\n", (long int)mysql_num_rows(res));    // 结果数量
            if((long int)mysql_num_rows(res))
            {

                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    printf("\n--- >>> \n");
                    printf("| Group name: %s\n", row[0]);
                    printf("| Group leader: %s\n", row[1]);
                    printf("| Group description: %s\n", row[2]);
                    printf("<<< --- \n");
                }
            }
            mysql_free_result(res);    // 释放结果集
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 5:
            printf("功能5: 查询与某用户在同一个小组的用户\n\n");
            printf("Input user's nickname:");
            scanf("%s",para[0]);
            getchar();
            strcpy(query, "SELECT groupname,username FROM belong WHERE groupname IN (SELECT groupname FROM belong WHERE username=\'");
            strcat(query, para[0]);
            strcat(query, "\' UNION SELECT gname FROM usergroup WHERE username=\'");
            strcat(query, para[0]);
            strcat(query, "\') AND username<>\'");
            strcat(query, para[0]);
            strcat(query, "\';");
    //        printf("query:%s\n", query);
            if(mysql_real_query(&mysql, query, strlen(query)))    // 查询
            {
                printf("Fail to query: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(res == NULL)
            {
                printf("Sorry, system error...\n");
                exit(1);
            }
            printf("%ld user(s) in total.\n", (long int)mysql_num_rows(res));    // 结果数量
            if((long int)mysql_num_rows(res))
            {
                printf("\n--- >>> \n");
                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    if(strcmp(lastnote,row[0]))
                    {
                        printf("|%10s|\n", row[0]);
                        strcpy(lastnote,row[0]);
                    }
                    printf("\t%s\n", row[1]);
                }
                printf("<<< --- \n");
            }
            mysql_free_result(res);    // 释放结果集
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 6:
            printf("功能6: 查询某用户的至少有某数量评论的笔记\n\n");
            printf("Input user's nickname:");
            scanf("%s",para[0]);
            getchar();
            printf("Input minimum comment number:");
            scanf("%s",para[1]);
            getchar();
            strcpy(query, "SELECT note.title, COUNT(comment.cid) FROM note JOIN comment ON note.title = comment.note AND note.username =\'");
            strcat(query, para[0]);
            strcat(query, "\' GROUP BY note.title HAVING COUNT(comment.cid) >=");
            strcat(query, para[1]);
            strcat(query, ";");
//            printf("query:%s\n", query);
            if(mysql_real_query(&mysql, query, strlen(query)))    // 查询
            {
                printf("Fail to query: %s\n", mysql_error(&mysql));
                printf("Press any key to return back ... \n");
                getchar();
                system("cls");
            }
            res = mysql_store_result(&mysql);    // 取出查询结果
            if(res == NULL)
            {
                printf("Sorry,system error...\n");
                exit(1);
            }
            printf("\n");
            if((long int)mysql_num_rows(res))
            {
                while ((row = mysql_fetch_row(res)))    // 按行读取
                {
                    printf("《 %s 》: %s\n", row[0], row[1]);
                }
            }
            mysql_free_result(res);    // 释放结果集
            printf("Press any key to return back ... \n");
            getchar();
            system("cls");
            break;
        case 7:
            flag = 0;
            break;
        default:
            printf("Undefined choice.\n");
        }
    }
    mysql_close(&mysql);    // 关闭Mysql连接
    return 0;
}

void printTips()
{
    printf("\t\t\t     **>博客管理系统<**\n");
    printf("\t\t++-----------------------------------------++\n");
    printf("\t\t++------------------功能-------------------++\n");
    printf("\t\t|    1.插入用户                             |\n");
    printf("\t\t|    2.插入博客                             |\n");
    printf("\t\t|    3.删除分组                             |\n");
    printf("\t\t|    4.查询某用户加入的小组信息             |\n");
    printf("\t\t|    5.查询与某用户在同一个小组的用户       |\n");
    printf("\t\t|    6.查询某用户的评论数量大于m的博客      |\n");
    printf("\t\t|    7.退出                                 |\n");
    printf("\t\t++=========================================++\n");
    return;
}

