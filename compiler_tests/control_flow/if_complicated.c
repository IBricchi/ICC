int f() {
    int x = 1;
    int y = 10;

    if (y > 5 && x == 0) {
        int z;
        x = -1;
    } else if (y > 8 || x == -123) {
        int a = 34;
        x = 11;
        y = x + 1;
        y++;
    } else {
        int u = 100;
        return u;
    }

    return y == 13;
}