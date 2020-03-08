#include <stdio.h>


int main(){
    char *tab[10]={"alicja", "wa" ,"krainie", "czarow"};
    char **slowo=tab;
    printf("%s",slowo[1]);
    printf("%s",slowo[0][1]);
    printf("%d",slowo[0][1]);


}
