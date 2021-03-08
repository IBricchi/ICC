int f()
{
    int a = 3;
    int b = 5;
    int c = a-- + b++;
    return c == --a + ++b;
}
