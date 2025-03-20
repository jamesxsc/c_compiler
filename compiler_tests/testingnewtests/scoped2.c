typedef int global_t; 

int test_block(int a) {
    typedef char global_t;    
    global_t b = (global_t)(a + 10); 
    return b; 
}

int f() {
    global_t x = 5; 
    x = x + test_block(x); 
    return x;            
}

