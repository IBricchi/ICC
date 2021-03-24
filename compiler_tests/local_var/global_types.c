int a = 22;
float b = 121.5f;
double c = 234.567;
char d = '7';
unsigned e = 11;

int g()
{
    a = a+1;
    b = b+1.0f;
    c = c+1.0;
    d = d+1;
    e = e+1;

    return  (a == 23) && 
            (b == 122.5f) &&
            (c == 235.567) &&
            (d == '8') &&
            (e == 12) ;
}
