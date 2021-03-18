typedef float float_t1;
typedef float_t1 float_t2;

int g() {
    typedef double yes;

    float_t1 a = 5.5f;
    float_t2 b = 5.5f;

    yes x = 45.1;
    yes y = 45.1;
    
    return  (a == b) &&
            (x == y);
}
