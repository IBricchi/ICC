int a;
int b = 121;
a = 22;

int g()
{
    a++;
    b = b + 2;
    a = a - 2;
    return a == 21 && b == 123;
}
