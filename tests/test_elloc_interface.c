#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include <criterion/logging.h>
#include "../src/elloc.h"

ParameterizedTestParameters(test_elloc_interface, test_with_int) {
    static const int params[10] = {1, 2, 3, 321, 123, 24129, -12312, 939, 0, 83274};
    size_t nb_params = 10;

    return cr_make_param_array(int, params, nb_params);
}

ParameterizedTest(int *param, test_elloc_interface, test_with_int) {
    int *ptr = (int *) elloc(4);

    *ptr = *param;

    cr_expect_eq(*ptr, *param);

    efree((char *) ptr);
}

Test(test_elloc_interface, test_with_string) {
    static const char *params[5] = {"bar", "foo", "Idk", "Ayo", "abc"};
    for (int i = 0; i < 5; i++) {
        size_t len = strlen(params[i]) + 1;
    
        char *ptr = elloc(len);
    
        cr_assert_not_null(ptr);
    
        strcpy(ptr, params[i]);
    
        cr_expect_str_eq(ptr, params[i]);
    
        efree(ptr);
    }
}

Test(test_elloc_interface, test_for_independent_writes) {
    char *fir = elloc(11);
    char *sec = elloc(21);

    char *fir_str = "AAAAAAAAAA";
    char *sec_str = "BBBBBBBBBBBBBBBBBBBB";

    strcpy(fir, fir_str);
    strcpy(sec, sec_str);

    cr_expect_str_eq(fir, fir_str);
    cr_expect_str_eq(sec, sec_str);

    efree(fir);
    efree(sec);
}

Test(test_elloc_interface, test_for_different_sizes) {
    char big_str[1001];

    for (int i = 0; i < 1000; i++)
        big_str[i] = 'A';

    big_str[1000] = '\0';

    for (int i = 1; i < 1000; i++) {
        char *str = elloc(i + 1);

        for(int j = 0; j < i; j++)
            str[j] = 'A';

        str[i] = '\0';
        big_str[i] = '\0';

        cr_expect_str_eq(str, big_str);

        big_str[i] = 'A';
        efree(str);
    }
}

Test(test_elloc_interface, test_zero_elloc) {
    cr_assert_eq(elloc(0),  NULL);
}

Test(test_elloc_interface, test_negative_elloc) {
    cr_assert_eq(elloc(-1),  NULL);
}

Test(test_elloc_interface, test_sequential_efree) {
    char *a = elloc(11);
    char *b = elloc(11);
    char *c = elloc(11);

    char *str_a = "AAAAAAAAAA";
    char *str_b = "BBBBBBBBBB";
    char *str_c = "CCCCCCCCCC";

    strcpy(a, str_a);
    strcpy(b, str_b);
    strcpy(c, str_c);

    cr_expect_str_eq(a, str_a);
    cr_expect_str_eq(b, str_b);
    cr_expect_str_eq(c, str_c);

    efree(a);
    efree(b);
    efree(c);
}

Test(test_elloc_interface, test_reverse_sequential_efree) {
    char *a = elloc(11);
    char *b = elloc(11);
    char *c = elloc(11);

    efree(c);
    efree(b);
    efree(a);
}

Test(test_elloc_interface, test_non_sequential_efree) {
    char *a = elloc(11);
    char *str_a = "AAAAAAAAAA";
    strcpy(a, str_a);
    cr_expect_str_eq(a, str_a);
    efree(a);

    char *b = elloc(11);
    char *str_b = "BBBBBBBBBB";
    strcpy(b, str_b);
    cr_expect_str_eq(b, str_b);
    efree(b);

    char *c = elloc(11);
    char *str_c = "CCCCCCCCCC";
    strcpy(c, str_c);
    cr_expect_str_eq(c, str_c);
    efree(c);
}

Test(test_elloc_interface, test_many_ellocs) {
    char *ptr[1000];
    char *str = "AAAAAAAAAA";

    for(int i = 0; i < 1000; i++) {
        ptr[i] = elloc(11);
        strcpy(ptr[i], str);
    }
    
    for(int i = 0; i < 1000; i++) 
        cr_expect_str_eq(ptr[i], str);
    
    for(int i = 0; i < 1000; i++) 
        efree(ptr[i]);
}

Test(test_elloc_interface, test_double_efree) {
    char *a = elloc(5);

    cr_assert_eq(efree(a), 0);
    cr_assert_eq(efree(a), 2);
}

Test(test_elloc_interface, test_null_efree) {
    cr_assert_eq(efree( NULL), 1);
}


Test(test_elloc_interface, test_out_of_bounds_efree) {
    char *a = elloc(5);

    cr_assert_eq(efree(a + 100), 1);
    cr_assert_eq(efree(a - 100), 1);
}

Test(test_elloc_interface, test_reuse_memory) {
    char *a = elloc(4);

    strcpy(a, "bar");

    cr_expect_str_eq(a, "bar");
    
    cr_assert_eq(efree(a), 0);

    char *b = elloc(4);

    strcpy(b, "foo");

    cr_expect_str_eq(b, "foo");

    cr_assert_eq(efree(b), 0);
}

Test(test_elloc_interface, test_alignment) {
    for (int i = 1; i < 1000; i++) {
        char *str = elloc(i);

        int alignment = (long long) str % alignof(max_align_t);
        
        cr_assert_eq(alignment, 0);

        efree(str);
    }
}