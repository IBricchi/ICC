struct a{
    int a;
    char b;
};

struct b{
    int arr[10];
};

struct x{
    int y;
    struct a z;
    struct b zu;
};

int f()
{
    struct a small;
    struct b arrSmall;
    struct x big;

    return  (sizeof(small) == 4 + 1) &&
            (sizeof(arrSmall) == 4*10) &&
            (sizeof(big) == 4 + (4 + 1) + (4*10));
}
