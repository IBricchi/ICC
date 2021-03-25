int f() {
    int arr[10][15][20];
    arr[1][2][3] = 123;
    arr[5][1][10] = 789;

    double dArr[10][15][20];

    return  (arr[1][2][3] == 123) &&
            (arr[5][1][10] == 789) &&
            (sizeof(arr) == 10*15*20*4) &&
            (sizeof(dArr) == 10*15*20*8);
}
