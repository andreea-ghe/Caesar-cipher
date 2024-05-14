#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>


void getDistribution(double *percentages) {
    // here we get the percentages (distribution in english) of all the letters
    setlocale(LC_NUMERIC, "C");
    FILE * file = fopen("../distribution", "r");

    char buffer[1000];
    int i = 0;
    while(fgets(buffer, sizeof(buffer), file) != NULL) {
        char *endptr;
        percentages[i] = strtod(buffer, &endptr);
        i ++;
    }

    fclose(file);
}

void getFrequency(char *text, int *freq) {
    // compute the frequency of every letter (both small and great letters have equal frequency)
    for (int i = 0; i < 52; i++)
        freq[i] = 0;

    for (int i = 0; i < strlen(text); i ++)
        if (text[i] >= 'a' && text[i] <= 'z')
            freq[(int)text[i] - 'a'] ++;
        else if (text[i] >= 'A' && text[i] <= 'Z')
            freq[(int)text[i] - 'A'] ++;
}

int nrLetters(char *text) {
    // we number the letters from the text, ignoring other characters
    int ans = 0;
    for (int i = 0; i < strlen(text); i ++)
        if (text[i] >= 'a' && text[i] <= 'z' || text[i] >= 'A' && text[i] <= 'Z')
            ans ++;
    return ans;
}

void histogram(char *text, double *percentages, double *c) {
    // here we compute the histogram
    int n = nrLetters(text);
    for (int i = 0; i < 26; i ++)
        c[i] = n * percentages[i] / 100;
}

double chiDistance(char *text, int *freq, double *c) {
    // we find the Chi distance
    double sum = 0;
    for (int i = 0; i < 26; i ++)
        sum += (double)((freq[i] - c[i]) * (freq[i] - c[i]) / c[i]);
    return sum;
}

void decoder(char *text, int * freq, double *c) {
    double ans = chiDistance(text, freq, c);
    int shifted = 0;

    // we find with how many position is the code shifted (k)
    int k = 0;
    while (k < 26) {
        // we shift all the letters by 1, 2, ...
        int aux = freq[0];
        for (int i = 0; i < 25; i ++)
            freq[i] = freq[i + 1];
        freq[25] = aux;

        // we find the best solution with minimum Chi distance
        double dist = chiDistance(text, freq, c);
        if (dist < ans) {
            ans = dist;
            shifted = k;
        }
        k++;
    }

    // after we find k we can decode the text by shifting all letters, we must take separate cases for lower and upper cases
    char lower_key[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    char upper_key[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    for (int i = 0; i < strlen(text); i ++)
        if (text[i] >= 'a' && text[i] <= 'z') {
            if ((int)text[i] - 97 - shifted < 0)
                text[i] = lower_key[25 + (int)text[i] - 97 - shifted];
            else
                text[i] = lower_key[(int)text[i] - 97 - shifted - 1];
        }
        else if(text[i] >= 'A' && text[i] <= 'Z') {
            if ((int)text[i] - 65 - shifted < 0)
                text[i] = upper_key[25 + (int)text[i] - 65 - shifted];
            else
                text[i] = upper_key[(int)text[i] - 65 - shifted - 1];
        }
}

int main() {

    char text[10001];
    printf("Please enter the message you want to decode: \n");
    fgets(text, sizeof(text), stdin);
    printf("%s", text);

    int freq[50];
    getFrequency(text, freq);

    double percentages[26];
    getDistribution(percentages);

    double c[50];
    histogram(text, percentages, c);

    decoder(text, freq, c);

    printf("\nHere is your decoded message:\n%s", text);
    return 0;
}