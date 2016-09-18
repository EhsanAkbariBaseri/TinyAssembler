#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>


#define LINE_SIZE 256
#define INT_MAX_LEN 9
/* Since the maximum value an int can take is from billion order */

FILE* fp;

typedef enum token_t {
    INSTRUCTION,
    PARAMETER,
    NUMBER,
    REGISTER,
    ERROR,
    ENDFILE
} token_t;

typedef struct operand_t {
    token_t type;
    void *value;
} operand_t;

typedef void instruction_function(operand_t[]);

typedef enum register_names{
	AH,
	AL,
	EAX
} register_names;

typedef struct register_all_t {
	register_names name;
	char *str;
} register_all_t;

typedef enum instruction_names {
	MOVE
} instruction_names;

typedef struct instruction_t {
	char *str;
	instruction_names name;
    instruction_function func;
} instruction_t;

void mov(operand_t[]);

instruction_t instrs[] = {{"mov",MOVE,mov}};

char * reverse(const char* s);
char * toString(int x);
char* getLine(FILE*fp);
token_t get_token();
char *lefttrim(char* source);
bool isInstruction(char*);
void parse();

	
token_t token_type;
instruction_names current_instr;
register_names current_reg;
operand_t *params;
char *err_msg;

typedef void* reg_t;

static reg_t ah,al,eax;

void setRegister(reg_t reg, void* const value) {
    memcpy(reg,value,2);
}

void getRegister(reg_t reg,void* data) {
    memcpy(data,reg,2);
}

void initRegisters() {
	ah = malloc(2);
	al = malloc(2);
	eax = malloc(4);
}

void getRegister32(reg_t reg32,void *data) {
	if(reg32==eax) {
		int result;
     	char* num1_str = toString(*(int*)AL);
     	char* num2_str = toString(*(int*)AH);
     	char* holder = (char*) malloc(strlen(num1_str)+strlen(num2_str+1));
     	strcpy(holder,num1_str);
     	strcat(holder,num2_str);
         result = atoi(holder);
     	memcpy(data,&result,4);
	}
}

int main(int argc,char *argv[]) {
    char *l;
   /* if(argc!=2) {
    	printf("Usage : asm <file_name> "
    	       "\nIn which file name is the file you want to emulate\n");
    	exit(0);
    } 
    if((fp=fopen(argv[1],"r"))==NULL){
    	perror("asm");
    	exit(-1);
    }
    */
    fp = stdin;
    parse();
}
char* getLine(FILE*fp) {
	char *line = (char*) malloc(LINE_SIZE);
	fgets(line,LINE_SIZE,fp);
	return line;
}

char* lefttrim(char *source) {
	int j = 0;
	while(!isalnum(*source)) {
	     ++source;
	     ++j;
	}
	return source;
}

bool isInstruction(char *s) {
    char*instruct = strtok(s," \n");
	for(int i = 0;i<1;i++) {
		if(!(strcmp(s,instrs[i].str))) {
	     	current_instr  = instrs[i].name;
		     return true;	
	    }
    }
    return false;
}


char * toString(int x) {
    char *num_str = (char*) malloc(INT_MAX_LEN);
	int i = 0;
	while(x!=0) {
		int d = x%10;
		x/=10;
		num_str[i] = (int)'0'+d;
		i++;
	}
   return reverse(num_str);
}

char * reverse(const char* s) {
	int size = strlen(s);
    char *temp = (char*)malloc(size);
	temp[size-1]=0;
	for(int i=0;i<size;i++) {
		temp[size-i-1]=s[i];
	}
	return temp;
}

token_t get_token() {
	if(feof(fp)){
	    return ENDFILE;	
	}
     char *that_line = getLine(fp);
     while(!strcmp(that_line,"\n"))
          that_line=getLine(fp);
     char *temp = lefttrim(that_line);
     if(isInstruction(temp))

         return INSTRUCTION;
     else {
         err_msg = "Unrecognized Instruction\n";
         return ERROR;
     }
}

void parse() {
	token_t cu_token;
	while((cu_token=get_token())!=ENDFILE){
		if(cu_token==INSTRUCTION){
			if(current_instr==MOVE) {
				printf("mov encountered\n");
			}
		}
		else if (cu_token==ERROR) {
		    printf(err_msg);
		}
	}		
}

void mov(operand_t params[])