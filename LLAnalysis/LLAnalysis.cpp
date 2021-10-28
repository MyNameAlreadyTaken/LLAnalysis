#define _CRT_SECURE_NO_WARNINGS
#include "LLAnalysis.h"

using namespace std;

struct {
    char name[21];
    int type;
    int addr;
} indent[1000];/*为标识符表*/


void setTable() {
	for (int i = 0; i < end(pretable) - begin(pretable); i++)
		table[pretable[i][0] - 128][pretable[i][1]] = pretable[i][2];
	return;
}

int getRuleNum(int X, int a) {
	for (int i = 0; i < end(pretable) - begin(pretable); i++) {
		if (pretable[i][0] == X && pretable[i][1] == a)
			return pretable[i][2];
	}
	return -1;
}

void writeToFile() {
    lastLine = 0;
    line = 1;
    row = 0;
    if ((f1 = fopen("res//in.txt", "r")) == NULL) {
        printf("connot open the file\n");
        exit(0);
        /*如不能打开输入文件，显示出错信息退出*/
    }

    if ((f2 = fopen("out.txt", "w")) == NULL) /*打开输出文件*/ {
        printf("connot open the file\n");
        exit(0);
    }
    while (!feof(f1)) {
        getsym();
        printf("%s %d\n", sym.name, sym.code);
        fprintf(f2, "%s %d\n", sym.name, sym.code);
    }
    rewind(f1);
    fclose(f1);
    fclose(f2);
}

void domain() {
    lastLine = 0;
    line = 1;
    row = 0;
	if ((f1 = fopen("res//in.txt", "r")) == NULL) {
		printf("connot open the file\n");
		exit(0);
		/*如不能打开输入文件，显示出错信息退出*/
	}
    rewind(f1);

	stack<int> mystack;
	mystack.push(35);
	mystack.push(128);
	getsym();
	while (!mystack.empty()) {
		int X = mystack.top();
		mystack.pop();
		if (X >= 128 && X <= 161) {
			int ruleNum = getRuleNum(X, sym.code);
			if (ruleNum != -1) {
                for (int i = end(rule[ruleNum]) - begin(rule[ruleNum]) - 1; i >= 0; i--) {
                    if (rule[ruleNum][i] != 0)
                        mystack.push(rule[ruleNum][i]);
                }
			}
			else {
				cout << "代码段不符合PASCAL语法。\n第" << sym.line <<"行出错。" << endl;
				exit(1);
			}
		}
		else if (X >= 0 && X <= 35){
			if (X != sym.code) {
                if (X == 28 || X == 26 || X == 32)
                    cout << "代码段不符合PASCAL语法。\n第" << lastLine << "行出错。" << endl;
                else
                    cout << "代码段不符合PASCAL语法。\n第" << sym.line << "行出错。" << endl;
				exit(1);
			}
			else {
				if (X == 35)
					break;
				else {
                    if (!feof(f1)) {
                        lastLine = sym.line;
                        getsym();
                    }
                    else
                        break;
				}
			}
		}
		else {
            cout << "代码段不符合PASCAL语法。\n第" << sym.line << "行出错，出现无法识别的字符。" << endl;
			exit(1);
		}
	}
	if (mystack.empty() && feof(f1))
		cout << "代码段符合PASCAL语法。" << endl;

	fclose(f1);
}

void getsym() {
    static char a[lenth1][10] = { "program","const","var","integer","long","procedure","if",
   "then","while","do","read","write","begin","end","odd"
    },
        d[lenth2][3] = { "+","-","*","/","=","<>","<","<=",">",">=", ".",",",";",":",":=","(",")" },
        str[21], ch = ' ';
    int i, n = 0;
    while (isspace(ch))
        ch = getchr();
    if (isalpha(ch)) {
        sym.line = line;
        n = 0;
        while (isalpha(ch) || isalnum(ch)) {
            if (isalpha(ch))
                ch = tolower(ch);
            str[n++] = ch;
            ch = getchr();
        }
        str[n] = '\0';
        for (i = 0; i < lenth1; i++)
            if (!strcmp(str, a[i])) break;
        if (i < lenth1) {
            strcpy(sym.name, a[i]);
            sym.code = i + 1;
        }
        else {
            for (i = 0; i < lenth; i++)
                if (!strcmp(str, indent[i].name)) break;
            if (i == lenth)
                strcpy(indent[i].name, str);
            strcpy(sym.name, indent[i].name);
            sym.code = 34;
        }
    }
    else if (isalnum(ch)) {
        val = 0; n = 0;
        while (isalnum(ch)) {
            val = val * 10 + ch - '0';
            sym.name[n++] = ch;
            ch = getchr();
        }
        sym.name[n] = '\0';
        sym.code = 33;
    }
    else {
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '.' || ch == ','
            || ch == ';' || ch == '(' || ch == ')') {
            str[0] = ch; str[1] = '\0';
            ch = getchr();
            for (i = 0; i < lenth2; i++)
                if (!strcmp(str, d[i])) {
                    strcpy(sym.name, str);
                    sym.code = i + 16;
                }
        }
        else {
            n = 0;
            if (ch == '>' || ch == ':') {
                str[n++] = ch;
                if ((ch = getchr()) == '=') {
                    str[n++] = ch;
                    ch = getchr();
                }
            }
            else
                if (ch == '<') {
                    str[n++] = ch; ch = getchr();
                    if (ch == '=' || ch == '>') {
                        str[n++] = ch;
                        ch = getchr();
                    }
                }
                else if (ch == -1) {
                    strcpy(sym.name, "#");
                    sym.code = 35;
                }
                else
                    error(1);
        }
        str[n] = '\0';
        for (i = 0; i < lenth2; i++)
            if (!strcmp(str, d[i])) {
                strcpy(sym.name, str);
                sym.code = i + 16;
            }
    }
}

char getchr() {
    char ch = fgetc(f1);
    if (ch == '\n') {
        row = 1;
        line++;
    }
    else
        if (ch != ' ' && ch != '\t')
            row++;
    return(ch);
}

void error(int n) {
    printf("There are %d-error\n", n);
    exit(0);
}

int main() {
	domain();
}
