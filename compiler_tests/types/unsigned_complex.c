int f() {
    unsigned a = 5;
    unsigned int b = 10;

    unsigned c = 0xFFFFFFFF; // Not -1

    return  (a == 5) &&
            (b == 10) &&
            (c != -1);
}
