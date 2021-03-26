int a[5];

int f() {
    for(int i = 0; i < 5; i++){
        a[i] = i;
    }
    return a[4];
}

int g(){
    return a[4];
}
