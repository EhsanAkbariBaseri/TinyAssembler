/* Tiny assembler
 * A very simple featured assembler written in C
 * Author : Ehsan Akbari
 * Source Github : https://github.com/EhsanAkbariBaseri/TinyAssembler
 * Version : 1.0.0
 * */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_PARAMS 4
/* The maximum parameters an instruction can take */
#define LINE_SIZE 256
#define INT_MAX_LEN 9
/* Since the maximum value an int can take is from billion order */
#define SYS_CALL_PRINT 4
#define SYS_CALL_EXIT 1

/* To print out Integers */
#define SYS_CALL_PRINT_INT 5


FILE* fp;

typedef enum token_t {
    INSTRUCTION,
    PARAMETER,
    INTEGER,
    FLOAT,
    REGISTER,
    ERROR,
    ENDFILE
} token_t;

struct register_t {
    char*name;
    int bytes;
    void* data;
};

typedef struct operand_t {
    token_t type;
    void *value;
    struct register_t reg;
    /* Is set only if the parameter is a register */
} operand_t;

typedef void instruction_function(operand_t[]);

typedef enum instruction_names {
	MOVE,
    INT
} instruction_names;

typedef struct instruction_t {
	char *str;
	instruction_names name;
    instruction_function *func;
} instruction_t;

/* Instruction functions */
void mov(operand_t[]);
void interrupt(operand_t[]);

instruction_t instrs[] = {{"mov",MOVE,mov},{"int",INT,interrupt}};


/* Utility functions */
char * reverse(const char* s);
char * toString(int x);
char* getLine(FILE*fp);
token_t getToken();
char *leftTrim(char *source);
bool isInstruction(char*);
void parse();
operand_t paramToOperand(char*);
instruction_t lookUpInstructions(instruction_names name);
bool isRegister(char*);
struct register_t lookUpRegister(char*);
size_t trim(char *out, size_t len, const char *str);
bool isInteger(char *str);
bool isFloat(char *str);
        struct register_t *lookUpRegisterM(char* rgname);

/* Global Variables */
token_t token_type;
instruction_names current_instr;
operand_t *params;
char *err_msg;
char current_line[LINE_SIZE];

typedef void* reg_t;

static struct register_t registers[] = {{"ah",1,NULL},{"al",1,NULL},{"ax",2,NULL},{"eax",4,NULL},{"ebx",4,NULL},{"bx",2,NULL},{"bl",1,NULL},{"bh",4,NULL},{"ecx",4,NULL},{"cx",2,NULL},{"cl",1,NULL},{"ch",4,NULL},{"edx",4,NULL},{"dx",2,NULL},{"dl",1,NULL},{"dh",4,NULL}};

void setRegister(struct register_t reg, void* const value) {
    memcpy(reg.data,value,reg.bytes);
}

void getRegister(struct register_t reg,void* data) {
    memcpy(data,reg.data,reg.bytes);
}

void initRegisters() {
    lookUpRegisterM("eax")->data = malloc(4);
    lookUpRegisterM("al")->data = registers[3].data;
    lookUpRegisterM("ah")->data = registers[1].data+8;
    lookUpRegisterM("ax")->data = lookUpRegister("al").data;
    lookUpRegisterM("ebx")->data = malloc(4);
    lookUpRegisterM("bl")->data = registers[3].data;
    lookUpRegisterM("bh")->data = registers[1].data+8;
    lookUpRegisterM("bx")->data = lookUpRegister("bl").data;
    lookUpRegisterM("ecx")->data = malloc(4);
    lookUpRegisterM("cl")->data = registers[3].data;
    lookUpRegisterM("ch")->data = registers[1].data+8;
    lookUpRegisterM("cx")->data = lookUpRegister("cl").data;
    lookUpRegisterM("edx")->data = malloc(4);
    lookUpRegisterM("dl")->data = registers[3].data;
    lookUpRegisterM("dh")->data = registers[1].data+8;
    lookUpRegisterM("dx")->data = lookUpRegister("dl").data;
}

int main(int argc,char *argv[]) {
//    if(argc!=2) {
//    	printf("Usage : asm <file_name> "
//    	       "\nIn which file name is the file you want to emulate\n");
//    	exit(0);
    //}
//    if((fp=fopen("/home/ehsan/Desktop/Assembler/test.asm","r"))==NULL){
//    	perror("asm");
//    	exit(-1);
//    }

    fp = stdin;

    initRegisters(); /* Never forget to run this at the beginning of the program! or you'll get into Segmentation Fault */
    parse();
}
char* getLine(FILE*fp) {
	char *line = (char*) malloc(LINE_SIZE);
	fgets(line,LINE_SIZE,fp);
	return line;
}

char* leftTrim(char *source) {
	int j = 0;
	while(!isalnum(*source)) {
	     ++source;
	     ++j;
	}
	return source;
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

token_t getToken() {
	if(feof(fp)){
	    return ENDFILE;	
	}
    char *that_line = getLine(fp);
    if(feof(fp)){
        return ENDFILE;
    }
    that_line[strlen(that_line)-1] = 0;
    while(!strcmp(that_line,"\n") || !strcmp(that_line,""))
        that_line=getLine(fp);
    strcpy(current_line,that_line);
    char *temp = leftTrim(that_line);
    if(isInstruction(temp)) {
        return INSTRUCTION;
    }
    else {
        err_msg = "Unrecognized Instruction\n";
        return ERROR;
    }
}

void parse() {
	token_t cu_token;
	while((cu_token= getToken())!=ENDFILE){
		if(cu_token==INSTRUCTION) {
            int i = 0; // Keep track of number of parameters
            operand_t params[MAX_PARAMS],temp;
			char*param = strtok(current_line," ");
            /* Remove the instruction name from current line */
            while((param = strtok(NULL,","))!=NULL && i < 4) {
                temp = paramToOperand(param);
                params[i] = temp;
                i++;
            }
            lookUpInstructions(current_instr).func(params);
		}
		else if (cu_token==ERROR) {
		    printf(err_msg);
		}
	}		
}

/* Interrupt functions */

void mov(operand_t params[]) {
    if(params[0].type == REGISTER && params[1].type == REGISTER) {
        printf("I came to here!");
        if(params[0].reg.bytes == params[1].reg.bytes )
            setRegister(params[0].reg,params[1].value);
        else {
            printf("Incompatible registers");
            return;
        }
    } else if(params[1].type == INTEGER) {
        setRegister(params[0].reg,params[1].value);
        int h = 0;
        getRegister(params[0].reg,&h);
        printf("%s => %d\n",params[0].reg.name,h);
    } else if (params[1].type == FLOAT) {
        if(params[0].reg.bytes==4) {
            setRegister(params[0].reg, params[1].value);
            float h = 0;
            getRegister(params[0].reg, &h);
            printf("%s => %f\n", params[0].reg.name, h);
        }
        else
            printf("This register is too small for storing floating-point numbers\n");
    }
}

void interrupt(operand_t params[]) {
    /* Get the command type */
    int eax = 0,ebx = 0,ecx = 0,edx = 0;
    getRegister(lookUpRegister("eax"),&eax);
    if (*(int*)params[0].value==80) {
        switch (eax) {
            case SYS_CALL_PRINT_INT:
                getRegister(lookUpRegister("ebx"),&ebx);
                printf("%d\n",ecx);
                break;
            default:
                printf("Unrecognized interrupt specified");
                break;
        }
    }
}

instruction_t lookUpInstructions(instruction_names name) {
    for (int i = 0; i < sizeof(instrs)/sizeof(instruction_t); ++i) {
        if(instrs[i].name == name)
            return instrs[i];
    }
    // TODO : Add error checking here
}

bool isInstruction(char *s) {
    char*instruct = strtok(s," \n");
    for(int i = 0;i< sizeof(instrs)/ sizeof(instruction_t);i++) {
        if(!(strcasecmp(s,instrs[i].str))) {
            current_instr  = instrs[i].name;
            return true;
        }
    }
    return false;
}

operand_t paramToOperand(char* p) {
    int *value_i = (int*) malloc(sizeof(int));
    float *value_f = (float*) malloc(sizeof(float));
    operand_t temp;
    if(isRegister(p)) {
        temp.reg = lookUpRegister(p);
        temp.type = REGISTER;
        //getRegister(temp.reg,temp.value);
        return temp;
    } else if (isInteger(p)) {
        temp.type = INTEGER;
        *value_i = atoi(p);
        temp.value = value_i;
        return temp;
    } else if (isFloat(p)) {
        temp.type = FLOAT;
        *value_f = atof(p);
        temp.value = value_f;
        return temp;
    }
    // TODO: Add error checking here
}

bool isRegister(char* rname) {
    for (int i = 0; i < sizeof(registers)/ sizeof(struct register_t); ++i) {
        if(!strcasecmp(rname,registers[i].name))
            return true;
    }
    return false;
}

struct register_t lookUpRegister(char* rgname) {
    for (int i = 0; i < sizeof(registers)/ sizeof(struct register_t); ++i) {
        if(!strcasecmp(rgname,registers[i].name))
            return registers[i];
    }
    // TODO : Add error checking here
}

/* It returns it modifyable */
struct register_t *lookUpRegisterM(char* rgname) {
    for (int i = 0; i < sizeof(registers)/ sizeof(struct register_t); ++i) {
        if(!strcasecmp(rgname,registers[i].name))
            return &registers[i];
    }
    // TODO : Add error checking here
}

// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t trim(char *out, size_t len, const char *str)
{
    if(len == 0)
        return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while(isspace(*str)) str++;

    if(*str == 0)  // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len-1 ? (end - str) : len-1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

bool isFloat(char *str) {
    char *current = (char*) malloc(strlen(str)+2);
    bool dot_encountered = false;
    trim(current,strlen(str)+2,str);
    if(*current== '-' || *current=='+') {
        ++current;
        while (*current != NULL) {
            if (!isdigit(*current) && *current!='.')
                return false;
            else if(*current=='.' && !dot_encountered)
                dot_encountered = true;
            else if(*current!='.')
                return false;
            ++current;
        }
        return true;
    } else if(isdigit(*current)) {
        while (*current != NULL) {
            if (!isdigit(*current) && *current!='.')
                return false;
            else if(*current=='.' && !dot_encountered)
                dot_encountered = true;
            else if(*current!='.' && !isdigit(*current))
                return false;
            else {
                ++current;
                continue;
            }
            ++current;
        }
        return true;
    }
    return false;
}

bool isInteger(char *str) {
    char *current = (char*) malloc(strlen(str)+2);
    bool dot_encountered = false;
    trim(current,strlen(str)+2,str);
    if(*current== '-' || *current=='+') {
        ++current;
        while (*current != NULL) {
            if (!isdigit(*current))
                return false;
            ++current;
        }
        return true;
    } else if(isdigit(*current)) {
        while (*current != NULL) {
            if (!isdigit(*current))
                return false;
            ++current;
        }
        return true;
    }
    return false;
}