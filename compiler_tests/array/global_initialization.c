int arr[3] = {1,2,3};

int f() {
    arr[2] = 9;

    return  (arr[1] == 2) &&
            (arr[2] == 9);
}
