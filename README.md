# TinyAssembler
A very tiny assembler written in C

Whole the program is located in asm.c.

The file is well-documented but feel free to add some comments so that others can figure things out easily.

To use the source first clone to a directory and then `cd` to that directory and then : 
    
    cmake .
     
This configures a MakeFile. After that, run this : 
    
    make
   
Then run the program like this : 

    ./Assembler <input_asm_file>


Currently supported instructions : 

    mov
