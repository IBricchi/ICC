int f() {
    double d = 0.123e3;
    double d1 = 32e-5;

    float f = 1e-2f;
    float f2 = 90E3f;

    return  (d == 0.123e3) &&
            (d1 == 32e-5) &&
            (f == 1e-2f) &&
            (f2 == 90E3f);
}
