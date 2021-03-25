void g(int *value) {
    *value = *value +1;
}

int f() {
    int a = 4;
    g(&a);

    return a == 5;
}
