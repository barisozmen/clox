#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "scanner.h"

static void test_scan_single_char_tokens(void **state) {
    (void) state;
    initScanner("( ) { } , . - + ; / *");

    assert_int_equal(scanToken().type, TOKEN_LEFT_PAREN);
    assert_int_equal(scanToken().type, TOKEN_RIGHT_PAREN);
    assert_int_equal(scanToken().type, TOKEN_LEFT_BRACE);
    assert_int_equal(scanToken().type, TOKEN_RIGHT_BRACE);
    assert_int_equal(scanToken().type, TOKEN_COMMA);
    assert_int_equal(scanToken().type, TOKEN_DOT);
    assert_int_equal(scanToken().type, TOKEN_MINUS);
    assert_int_equal(scanToken().type, TOKEN_PLUS);
    assert_int_equal(scanToken().type, TOKEN_SEMICOLON);
    assert_int_equal(scanToken().type, TOKEN_SLASH);
    assert_int_equal(scanToken().type, TOKEN_STAR);
    assert_int_equal(scanToken().type, TOKEN_EOF);
}

static void test_scan_two_char_tokens(void **state) {
    (void) state;
    initScanner("! != = == > >= < <=");

    assert_int_equal(scanToken().type, TOKEN_BANG);
    assert_int_equal(scanToken().type, TOKEN_BANG_EQUAL);
    assert_int_equal(scanToken().type, TOKEN_EQUAL);
    assert_int_equal(scanToken().type, TOKEN_EQUAL_EQUAL);
    assert_int_equal(scanToken().type, TOKEN_GREATER);
    assert_int_equal(scanToken().type, TOKEN_GREATER_EQUAL);
    assert_int_equal(scanToken().type, TOKEN_LESS);
    assert_int_equal(scanToken().type, TOKEN_LESS_EQUAL);
}

static void test_scan_integer_number(void **state) {
    (void) state;
    initScanner("123");

    Token token = scanToken();
    assert_int_equal(token.type, TOKEN_NUMBER);
    assert_int_equal(token.length, 3);
    assert_true(strncmp(token.start, "123", 3) == 0);
}

static void test_scan_decimal_number(void **state) {
    (void) state;
    initScanner("45.67");

    Token token = scanToken();
    assert_int_equal(token.type, TOKEN_NUMBER);
    assert_int_equal(token.length, 5);
    assert_true(strncmp(token.start, "45.67", 5) == 0);
}

static void test_scan_multiple_numbers(void **state) {
    (void) state;
    initScanner("123 45.67 0.5");

    Token token1 = scanToken();
    assert_int_equal(token1.type, TOKEN_NUMBER);
    assert_int_equal(token1.length, 3);

    Token token2 = scanToken();
    assert_int_equal(token2.type, TOKEN_NUMBER);
    assert_int_equal(token2.length, 5);

    Token token3 = scanToken();
    assert_int_equal(token3.type, TOKEN_NUMBER);
    assert_int_equal(token3.length, 3);
}

static void test_scan_string(void **state) {
    (void) state;
    initScanner("\"hello world\"");

    Token token = scanToken();
    assert_int_equal(token.type, TOKEN_STRING);
    assert_int_equal(token.length, 13);
}

static void test_scan_identifier(void **state) {
    (void) state;
    initScanner("variable _name test123");

    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
}

static void test_scan_keywords(void **state) {
    (void) state;
    initScanner("and class else false for fun if nil or print return super this true var while");

    assert_int_equal(scanToken().type, TOKEN_AND);
    assert_int_equal(scanToken().type, TOKEN_CLASS);
    assert_int_equal(scanToken().type, TOKEN_ELSE);
    assert_int_equal(scanToken().type, TOKEN_FALSE);
    assert_int_equal(scanToken().type, TOKEN_FOR);
    assert_int_equal(scanToken().type, TOKEN_FUN);
    assert_int_equal(scanToken().type, TOKEN_IF);
    assert_int_equal(scanToken().type, TOKEN_NIL);
    assert_int_equal(scanToken().type, TOKEN_OR);
    assert_int_equal(scanToken().type, TOKEN_PRINT);
    assert_int_equal(scanToken().type, TOKEN_RETURN);
    assert_int_equal(scanToken().type, TOKEN_SUPER);
    assert_int_equal(scanToken().type, TOKEN_THIS);
    assert_int_equal(scanToken().type, TOKEN_TRUE);
    assert_int_equal(scanToken().type, TOKEN_VAR);
    assert_int_equal(scanToken().type, TOKEN_WHILE);
}

static void test_scan_identifier_vs_keyword(void **state) {
    (void) state;
    initScanner("ifx for4 variable");

    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
    assert_int_equal(scanToken().type, TOKEN_IDENTIFIER);
}

static void test_scan_whitespace_handling(void **state) {
    (void) state;
    initScanner("  \t\r\n  123  \n\n  456  ");

    Token token1 = scanToken();
    assert_int_equal(token1.type, TOKEN_NUMBER);
    assert_int_equal(token1.line, 2);

    Token token2 = scanToken();
    assert_int_equal(token2.type, TOKEN_NUMBER);
    assert_int_equal(token2.line, 4);
}

static void test_scan_comment(void **state) {
    (void) state;
    initScanner("123 // this is a comment\n456");

    Token token1 = scanToken();
    assert_int_equal(token1.type, TOKEN_NUMBER);

    Token token2 = scanToken();
    assert_int_equal(token2.type, TOKEN_NUMBER);
    assert_int_equal(token2.line, 2);
}

static void test_scan_line_tracking(void **state) {
    (void) state;
    initScanner("123\n456\n789");

    Token token1 = scanToken();
    assert_int_equal(token1.line, 1);

    Token token2 = scanToken();
    assert_int_equal(token2.line, 2);

    Token token3 = scanToken();
    assert_int_equal(token3.line, 3);
}

static void test_scan_unexpected_character(void **state) {
    (void) state;
    initScanner("@");

    Token token = scanToken();
    assert_int_equal(token.type, TOKEN_ERROR);
}

static void test_scan_unterminated_string(void **state) {
    (void) state;
    initScanner("\"hello");

    Token token = scanToken();
    assert_int_equal(token.type, TOKEN_ERROR);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scan_single_char_tokens),
        cmocka_unit_test(test_scan_two_char_tokens),
        cmocka_unit_test(test_scan_integer_number),
        cmocka_unit_test(test_scan_decimal_number),
        cmocka_unit_test(test_scan_multiple_numbers),
        cmocka_unit_test(test_scan_string),
        cmocka_unit_test(test_scan_identifier),
        cmocka_unit_test(test_scan_keywords),
        cmocka_unit_test(test_scan_identifier_vs_keyword),
        cmocka_unit_test(test_scan_whitespace_handling),
        cmocka_unit_test(test_scan_comment),
        cmocka_unit_test(test_scan_line_tracking),
        cmocka_unit_test(test_scan_unexpected_character),
        cmocka_unit_test(test_scan_unterminated_string),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
