#include <stdio.h>

#define DEBUGFS_CT "/sys/kernel/debug/abc/ct"
#define DEBUGFS_SIG "/sys/kernel/debug/abc/sig"

void my_printf(FILE* fp) {
	char current;
	while((current = fgetc(fp)) != EOF) putchar(current);
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("I accept 3 arguments.\n");
		return 0;
	}
	if(strcmp(argv[1], "0") == 0) {
		FILE *fl;
		fl = fopen(DEBUGFS_CT, "w+");
		fprintf(fl, argv[2]);
		my_printf(fl);
		fclose(fl);
	}
	else if(strcmp(argv[1], "1") == 0) {
		FILE *fl;
		fl = fopen(DEBUGFS_SIG, "w+");
		fprintf(fl, argv[2]);
		my_printf(fl);
		fclose(fl);
	}
	return 0;
}
