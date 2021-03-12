enum global{X,Y,Z=12};

int g()
{
    enum test{a,b=5,c,d};

    return  (a == 0 && b == 5 && c == 6 && d == 7) &&
            (X == 0 && Y == 1 && Z == 12)
            ;
}
