int iArr[4];
char cArr[5];
float fArr[6];
double dArr[7];
unsigned uArr[8];

int g() {
    iArr[2] = 10;
    cArr[3] = 'z';
    fArr[4] = 3.4f;
    dArr[5] = 34.3;
    uArr[6] = 55;
}

int f() {
    g();

    return  (iArr[2] == 10) &&
            (cArr[3] == 'z') &&
            (fArr[4] == 3.4f) &&
            (dArr[5] == 34.3) &&
            (uArr[6] == 55);
}
