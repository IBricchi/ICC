float f() {
    return 11.1f;
}

double d() {
    return 22.2;
}

char c() {
    return '!';
}

unsigned u() {
    return 11;
}

int* p() {
    int a = 99;
    return &a;
}

int g() {
    return  (f() == 11.1f) &&
            (d() == 22.2) &&
            (c() == '!') &&
            (u() == 11) &&
            (*p() == 99);
}
