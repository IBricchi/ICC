int f() {
    int arr[3][4] = {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12}
    };

    return  (arr[1][2] == 7) &&
            (arr[0][3] == 4);
}
