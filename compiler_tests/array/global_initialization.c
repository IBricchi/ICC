int arr[3] = {1,2,3};
char cArr[2] = {'a','b'};
float fArr[1] = {45e3f};
double dArr[3] = {12.1, 34e-1, 45.3};

int f() {
    arr[2] = 9;

    return  (arr[1] == 2) &&
            (arr[2] == 9) &&
            (cArr[1] == 'b') &&
            (fArr[0] == 45e3f) &&
            (dArr[2] == 45.3);
}
