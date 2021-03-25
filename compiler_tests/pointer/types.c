int f() {
    char c = 'j';
    char *pChar = &c;

    float f = 4.5f;
    float *pFloat = &f;

    double d = 5.5;
    double *pDouble = &d;

    unsigned u = 11;
    unsigned *pUnsinged = &u;
    
    return  (*pChar == 'j') &&
            (*pFloat == 4.5f) &&
            (*pDouble == 5.5) &&
            (*pUnsinged == 11);
}
