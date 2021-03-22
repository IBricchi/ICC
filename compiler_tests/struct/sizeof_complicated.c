struct a{
    int a;
    char b;
};

struct x{
    int y;
    // int arr[10];
    struct a z;
};

int f()
{
    struct a small;
    struct x big;
    return  (sizeof(small) == 4 + 1) &&
            (sizeof(big) == 4 + 4 + 1);
}
