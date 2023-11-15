#include "/usr/include/mysql/mysql.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcgi_stdio.h>

MYSQL *conn;
MYSQL_STMT *stmt;
my_ulonglong affected_rows;

// values for connecting
char *server = "localhost";
char *user = "root";
char *password = "Mathias123"; 
char *database = "buglogdb";

int handle_req() {
	conn = mysql_init(NULL);
	
	// connecting to db
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	// command for inserting to messages
	const char* msg_query = "INSERT INTO messages (sender, content, date) VALUES (?, ?, CURDATE())";
	stmt = mysql_stmt_init(conn);
	// preparing the insert
	if (mysql_stmt_prepare(stmt, msg_query, strlen(msg_query)) != 0) {
		printf("Error preparing query\n");
		return 1;
	}

	int id_msg = 0;
	const char* sender = "Mathias1";
	const char* content = "test message";

	MYSQL_BIND bp[3];

	// modifying fields
	bp[0].buffer_type = MYSQL_TYPE_STRING;
	bp[0].buffer = (char*)sender;
	bp[0].is_null = 0;
	bp[0].buffer_length = strlen(sender);

	bp[1].buffer_type = MYSQL_TYPE_STRING;
	bp[1].buffer = (char*)content;
	bp[1].is_null = 0;
	bp[1].buffer_length = strlen(content);

	// adding modifications to the prepared command
	if (mysql_stmt_bind_param(stmt, bp)) {
		printf("Failed to bind bp\n");
		return 1;
	}

	// executing insert
	if (mysql_stmt_execute(stmt)) {
		fprintf(stderr, "failed executing\n");
		return 1;
	}

	affected_rows = mysql_stmt_affected_rows(stmt);
	printf("Inserted rows successfully\nAffected row %lu\n", (unsigned long)affected_rows);

	// selecting from db
	if (mysql_query(conn, "SELECT * FROM messages")) {
		return 1;
	}

	MYSQL_RES* res = mysql_store_result(conn);
	MYSQL_ROW row;

	// printing contents of db
	if (res) {
		while ((row = mysql_fetch_row(res))) {
			printf("ID: %s, Sender: %s, Content: %s, Date: %s\n", row[0], row[1], row[2], row[3]);
		}
	}

	mysql_stmt_close(stmt); 
	mysql_close(conn);
	return 0;
}

int main(void) {
	while (FCGI_Accept() >= 0) {
		int r = handle_req();
	}

	return 0;
}
