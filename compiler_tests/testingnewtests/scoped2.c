typedef int global_t; 

char test_block() {
    typedef char global_t;    
    global_t b = 'c';
    return b; 
}

int f() {
    global_t x = 5; 
    char c = test_block();
    return x;            
}

