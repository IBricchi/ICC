int g()
{
    char a = '}';
    a = 'a';
    char b = 'b';
    char c = '\n';
    
    return  (a != b) &&
            (a == 'a') &&
            ('a' == a) &&
            (++a == 'b') &&
            (c == '\n');
}
