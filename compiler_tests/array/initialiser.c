int f() {
    int iArr[3] = {1,2,3};
    float fArr[3] = {1.3f, 4.3f, 56.3f};
    double dArr[1] = {34.5};
    char cArr[4] = {'a','b','c','d'};
    unsigned uArr[5] = {1,2,3,4,5};

    return  (iArr[1] == 2) &&
            (fArr[2] == 56.3f) &&
            (dArr[0] == 34.5) &&
            (cArr[3] == 'd') &&
            (uArr[3] == 4);
}
