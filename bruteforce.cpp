// not owning, strictly ordered array with set_size elements, 0 <= .ids[i] < .array_size
// 0 <= .set_size <= .array_size
void Subset_first(struct Subset* s, int set_size);
bool Subset_next_same_size(Subset *s);
bool Subset_next(Subset *s);
struct Subset {
    Subset(int *buffer, int iset_size, int iarray_size) { ids = buffer; array_size = iarray_size; Subset_first(this, iset_size); }
    int *ids;
    int set_size;
    int array_size;
};

void Subset_first(Subset* s, int set_size) {
    s->set_size = set_size;
    for (int i = 0; i < set_size; ++i) s->ids[i] = i;
}

// returns false on wrap to first
bool Subset_next_same_size(Subset *s) {
    int i    = s->set_size - 1;
    int last = s->array_size - 1;
    while (i >= 0 && s->ids[i] == last) { --i; --last; } // Speed: можно избавитьcя от этого цикла, если поддерживать индекс последнего не дошедшего итератора
    if (i >= 0) {
        // выставляем всё подряд начиная с s->ids[i] += 1
        for (int j = s->ids[i] + 1; i < s->set_size; ++i, ++j) s->ids[i] = j;
        return true;
    }

    Subset_first(s, s->set_size);
    return false;
}

// returns false on wrap to first (empty) set
bool Subset_next(Subset *s) {
    if (Subset_next_same_size(s)) return true;

    if (s->set_size < s->array_size) {
        s->ids[s->set_size] = s->set_size;
        s->set_size += 1;
        return true;
    }

    Subset_first(s, 0);
    return false;
}

// not owning string consisting only of chars .allowed_chars
// это не индексы, потому что я надеюсь что так будет быстрее работать, а в переборе это важно=) TODO: benchmark this statement
void All_Strings_first(struct All_Strings *s, int size);
bool All_Strings_next_same_size(All_Strings *s);
bool All_Strings_next(All_Strings *s);
struct All_Strings {
    All_Strings(char *buffer, int isize, int imax_size, char const* iallowed_chars) {
        current  = buffer;
        max_size = imax_size;
        allowed_chars = iallowed_chars;
        for (int i = 0; i < sizeof allowed_char_to_index; ++i) {
            allowed_char_to_index[i] = 0;
        }

        n_allowed_chars = 0;
        for (char const* c = iallowed_chars; *c; ++c) {
            allowed_char_to_index[*c] = n_allowed_chars++;
        }

        All_Strings_first(this, isize);
    }
    char *current;
    int size;
    int max_size;

    char const* allowed_chars;
    int n_allowed_chars;
    unsigned char allowed_char_to_index[256];

    unsigned char next_idx(char c) {
        return allowed_char_to_index[c] + 1;
    }
};

void All_Strings_first(All_Strings *s, int size) {
    s->size = size;
    if (s->n_allowed_chars > 0) {
        for (int i = 0; i < size; ++i) s->current[i] = s->allowed_chars[0];
    }
}

// returns true on wrap to first sring of same length
bool All_Strings_next_same_size(All_Strings *s) {
    int i;
    for (i = 0; i < s->size && s->next_idx(s->current[i]) == s->n_allowed_chars; ++i) { // Speed: Knuth's counter
        s->current[i] = s->allowed_chars[0];
    }

    if (i == s->size) return false;

    s->current[i] = s->allowed_chars[s->next_idx(s->current[i])];
    return true;
}

// returns true on wrap to first empty string
bool All_Strings_next(All_Strings *s) {
    if (All_Strings_next_same_size(s)) return true;

    if (s->size < s->max_size) {
        s->current[s->size++] = s->allowed_chars[0];
        return true;
    }

    s->size = 0;
    return false;
}

// tests
#ifdef TESTS
#define static_array_size(Arr) (sizeof Arr / sizeof Arr[0])
#include <stdio.h>

char const* const vals[] = {"A", "B", "C"};

void print_subset_vals(Subset *s) {
    printf("{");
    for (int i = 0; i < s->set_size; ++i) printf("%s", vals[s->ids[i]]);
    printf("}\n");
}

void Subset_tests() {
    printf("# %s #\n", __FUNCTION__);
    static int tmpbuffer[static_array_size(vals)];

    int cnt[2] = {0, 0};
    for (int size = 0; size <= static_array_size(vals); ++size) {
        Subset subset = {tmpbuffer, size, static_array_size(vals)};
        do {
            print_subset_vals(&subset);
            cnt[0] += 1;
        } while (Subset_next_same_size(&subset));
    }

    Subset subset = {tmpbuffer, 0, static_array_size(vals)};
    do {
        print_subset_vals(&subset);
        cnt[1] += 1;
    } while (Subset_next(&subset));

    printf("%s:", __FUNCTION__);
    for (int i = 0; i < 2; ++i) {
        printf(" %s", cnt[i] == (1ULL << static_array_size(vals)) ? "SUCCESS" : "FAILURE");
    }
}

#define MAX_LEN 6

char const allowed_chars[] = "\\ \"ab";

void All_Strings_tests() {
    printf("# %s #\n", __FUNCTION__);
    static char buffer[MAX_LEN + 1];

    unsigned long long cnt[2] = {0, 0};
    for (int size = 0; size <= MAX_LEN; ++size) {
        All_Strings all_strings = {buffer, size, MAX_LEN, allowed_chars};
        do {
            buffer[size] = 0;
            printf("'%s'\n", buffer);
            cnt[0] += 1;
        } while (All_Strings_next_same_size(&all_strings));
    }

    All_Strings all_strings = {buffer, 0, MAX_LEN, allowed_chars};
    do {
        buffer[all_strings.size] = 0;
        printf("'%s'\n", buffer);
        cnt[1] += 1;
    } while (All_Strings_next(&all_strings));

    unsigned long long truecnt = 0;
    for (int i = 0; i <= MAX_LEN; ++i) {
        unsigned long long p = 1;
        for (int j = 0; j < i; ++j) p *= sizeof allowed_chars - 1;
        truecnt += p;
    }

    printf("%s:", __FUNCTION__);
    for (int i = 0; i < 2; ++i) {
        printf(" %s", cnt[i] == truecnt ? "SUCCESS" : "FAILURE");
    }
}

int main() { /*Subset_tests();*/ All_Strings_tests(); }
#endif
