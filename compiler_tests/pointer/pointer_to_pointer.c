int f() {
    int var;
    int *ptr;
    int **pptr;

    var = 3000;
    ptr = &var;
    pptr = &ptr;

    return  (3000 == *ptr) &&
            (3000 == **pptr);
}
