void asd(char* line);
void kol(char* line);
void clr(char* arr, int cnt);

main() {
    char* view = "view \0";
    char* execute = "execute \0";
    int i = 0;
    while (1) {
        char line [80];
        i = 0;

        interrupt(0x21, 0, "Super Shell:", 0, 0);
        interrupt(0x21, 1, line, 0, 0);
        interrupt(0x21, 0, "\r\n", 0, 0);
        while (i < 4) {
            if (line[i] != view[i]) break;
            i++;

        }

        if (i == 4) asd(line);
        else {
            i = 0;
            while (i < 7) {
                if (line[i] != execute[i]) break;
                i++;

            }

            if (i == 7) {
                kol(line);
            } else
                interrupt(0x21, 0, "Bad Command!!\r\n", 0, 0);

        }

        clr(line, 79);


    }
}

void asd(char* line) {
    char file_name[20];
    char buffer[13312];
    int i = 5;
    while (1) {
        file_name[i - 5] = line[i];
        if (line[i] == '\0') break;
        ++i;
    }
    if (i > 6) {
        interrupt(0x21, 3, file_name, buffer, 0); /*read the file into buffer*/
        interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
    }
    clr(file_name, 20);
    clr(buffer, 13311);

}

void kol(char* line) {
    char file_name[20];
    int i = 8;
    while (1) {
        file_name[i - 8] = line[i];
        if (line[i] == '\0') break;
        ++i;
    }
    if (i > 9)
        interrupt(0x21, 4, file_name, 0x2000, 0); /*read the file into buffer*/
    clr(file_name, 20);

}

void clr(char* arr, int cnt) {
    while (cnt > 0) {
        arr[cnt] = '\0';
        --cnt;
    }

}