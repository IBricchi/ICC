int f() {
    double d = 10.4;
    double a = 5.1;

    double c = 5.5 + 5.34;

    return (a+d == 15.5) &&
            !(3.3 == 3.34) &&
            (a+d != 15.4) &&
            !(4.5 != 4.5) &&
            (a+d < 18.9) &&
            (a+d <= 16.5) &&
            (a+d <= 15.5) &&
            !(15.5 <= 3.4) &&
            (19.1 > 2.3) &&
            !(45.3 > 45.3) &&
            (45.9 >= 16.5) &&
            (15.5 >= 15.5) &&
            !(1.2232 >= 3.4) &&
            (9.5 == 14.0 - 4.5) &&
            !(20.4 == 34.3 - 40.4) &&
            (15.0 * 3.0 == 45.0) &&
            !(15.1 * 3.4 == 45.1) &&
            (15.0 / 3.0 == 5.0) &&
            !(15.1 / 3.4 == 45.1) &&
            (-a == -5.1) &&
            (5.1 - 10.2 == -5.1) &&
            (-c != 10.84);
}
