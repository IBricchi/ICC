
int f();

int main()
{
    int x[8];
    char y[12];
    return !(f()==sizeof(x)+sizeof(y));
}
