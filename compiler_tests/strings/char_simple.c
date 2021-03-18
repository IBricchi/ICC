int g()
{
    char a = '}';
    a = 'a';
    char b = 'b';
    
    return  (a != b) &&
            (a == 'a') &&
            ('a' == a) &&
            (++a == 'b');
}
