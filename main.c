#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_WORDS 7
#define MAX_LEN 20

typedef struct {
    char words[MAX_WORDS][MAX_LEN];
    int word_count;
    char result[MAX_LEN];
    char op;
} Puzzle;

char unique_letters[10];
int num_letters = 0;
int letter_values[26];
int used_digits[10];

long long get_word_value(const char* word) {
    long long val = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        val = val * 10 + letter_values[word[i] - 'A'];
    }
    return val;
}

int is_valid(Puzzle* p) {
    for (int i = 0; i < p->word_count; i++) {
        if (letter_values[p->words[i][0] - 'A'] == 0) return 0;
    }
    if (letter_values[p->result[0] - 'A'] == 0) return 0;
    long long left_side = get_word_value(p->words[0]);
    for (int i = 1; i < p->word_count; i++) {
        long long val = get_word_value(p->words[i]);
        if (p->op == '+') left_side += val;
        else if (p->op == '-') left_side -= val;
        else if (p->op == '*') left_side *= val;
        else if (p->op == '/') {
            if (val == 0) return 0;
            left_side /= val;
        }
    }
    long long right_side = get_word_value(p->result);
    return left_side == right_side;
}

int is_leading_char(char c, Puzzle* p) {
    for (int i = 0; i < p->word_count; i++) {
        if (p->words[i][0] == c) return 1;
    }
    if (p->result[0] == c) return 1;
    return 0;
}

int solve(Puzzle* p, int idx) {
    if (idx == num_letters) {
        return is_valid(p);
    }
    char c = unique_letters[idx];
    int start_digit = is_leading_char(c, p) ? 1 : 0;
    for (int d = start_digit; d <= 9; d++) {
        if (!used_digits[d]) {
            letter_values[c - 'A'] = d;
            used_digits[d] = 1;
            if (solve(p, idx + 1)) return 1;
            used_digits[d] = 0;
            letter_values[c - 'A'] = -1;
        }
    }
    return 0;
}

void parse_puzzle(char* line, Puzzle* p) {
    p->word_count = 0;
    char* ptr = line;
    if (strchr(line, '+')) p->op = '+';
    else if (strchr(line, '-')) p->op = '-';
    else if (strchr(line, '*')) p->op = '*';
    else if (strchr(line, '/')) p->op = '/';
    char temp[MAX_LEN];
    while (sscanf(ptr, "%s", temp) == 1) {
        if (strcmp(temp, "=") == 0) {
            sscanf(ptr + 2, "%s", p->result);
            break;
        }
        else if (temp[0] >= 'A' && temp[0] <= 'Z') {
            strcpy(p->words[p->word_count++], temp);
        }
        ptr = strstr(ptr, temp) + strlen(temp);
    }

    int seen[26] = { 0 };
    num_letters = 0;
    for (int i = 0; i < p->word_count; i++)
        for (int j = 0; p->words[i][j]; j++) seen[p->words[i][j] - 'A'] = 1;
    for (int j = 0; p->result[j]; j++) seen[p->result[j] - 'A'] = 1;
    for (int i = 0; i < 26; i++) {
        if (seen[i]) unique_letters[num_letters++] = (char)(i + 'A');
    }
}

int main() {
    FILE* file = fopen("puzzles.txt", "r");
    if (!file) {
        printf("File not found!\n");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        Puzzle p;
        parse_puzzle(line, &p);
        printf("\nPuzzle: %s\n", line);
        for (int i = 0; i < 26; i++) letter_values[i] = -1;
        for (int i = 0; i < 10; i++) used_digits[i] = 0;
        clock_t start = clock();
        if (solve(&p, 0)) {
            clock_t end = clock();
            double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
            for (int i = 0; i < p.word_count; i++) {
                printf("%lld ", get_word_value(p.words[i]));
                if (i < p.word_count - 1) printf("%c ", p.op);
            }
            printf("= %lld\n", get_word_value(p.result));
            printf("Time: %f seconds\n", time_spent);
        }
        else {
            printf("No solution found.\n");
        }
    }
    fclose(file);
    return 0;
}
