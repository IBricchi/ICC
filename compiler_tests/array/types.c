int g() {
    char cArr[10];
    unsigned uArr[11];
    float fArr[12];
    double dArr[13];

    cArr[2] = '*';
    uArr[3] = 999;
    fArr[4] = 7.67f;
    dArr[5] = 3.12345;

    return  (cArr[2] == '*') &&
            (uArr[3] == 999) &&
            (fArr[4] == 7.67f) &&
            (dArr[5] = 3.12345);
}
