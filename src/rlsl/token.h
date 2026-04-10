#pragma once
#include <stdbool.h>
#include "cursor.h"

typedef enum rlr_sl_token_type_t {
    //literals
    RLR_SL_TOKEN_LITERAL_IDENTIFIER = 0,
    RLR_SL_TOKEN_LITERAL_FALSE,
    RLR_SL_TOKEN_LITERAL_TRUE,
    RLR_SL_TOKEN_LITERAL_FLOAT,
    RLR_SL_TOKEN_LITERAL_INT,

    //base types
    RLR_SL_TOKEN_TYPE_BOOL,
    RLR_SL_TOKEN_TYPE_INT,
    RLR_SL_TOKEN_TYPE_UINT,
    RLR_SL_TOKEN_TYPE_FLOAT,
    RLR_SL_TOKEN_TYPE_DOUBLE,

    //vector types
    RLR_SL_TOKEN_TYPE_BVEC2,
    RLR_SL_TOKEN_TYPE_IVEC2,
    RLR_SL_TOKEN_TYPE_UVEC2,
    RLR_SL_TOKEN_TYPE_DVEC2,
    RLR_SL_TOKEN_TYPE_VEC2,
    RLR_SL_TOKEN_TYPE_BVEC3,
    RLR_SL_TOKEN_TYPE_IVEC3,
    RLR_SL_TOKEN_TYPE_UVEC3,
    RLR_SL_TOKEN_TYPE_DVEC3,
    RLR_SL_TOKEN_TYPE_VEC3,
    RLR_SL_TOKEN_TYPE_BVEC4,
    RLR_SL_TOKEN_TYPE_IVEC4,
    RLR_SL_TOKEN_TYPE_UVEC4,
    RLR_SL_TOKEN_TYPE_DVEC4,
    RLR_SL_TOKEN_TYPE_VEC4,

    //matrix types
    RLR_SL_TOKEN_TYPE_MAT2,
    RLR_SL_TOKEN_TYPE_MAT3,
    RLR_SL_TOKEN_TYPE_MAT4,
    RLR_SL_TOKEN_TYPE_MAT2X2,
    RLR_SL_TOKEN_TYPE_MAT2X3,
    RLR_SL_TOKEN_TYPE_MAT2X4,
    RLR_SL_TOKEN_TYPE_MAT3X2,
    RLR_SL_TOKEN_TYPE_MAT3X3,
    RLR_SL_TOKEN_TYPE_MAT3X4,
    RLR_SL_TOKEN_TYPE_MAT4X2,
    RLR_SL_TOKEN_TYPE_MAT4X3,
    RLR_SL_TOKEN_TYPE_MAT4X4,

    //keywords
    RLR_SL_TOKEN_KEYWORD_STRUCT,
    RLR_SL_TOKEN_KEYWORD_WHILE,
    RLR_SL_TOKEN_KEYWORD_FOR,
    RLR_SL_TOKEN_KEYWORD_IF,
    RLR_SL_TOKEN_KEYWORD_ELSE,
    RLR_SL_TOKEN_KEYWORD_RETURN,
    RLR_SL_TOKEN_KEYWORD_CONST,

    //symbols
    RLR_SL_TOKEN_SYMBOL_SPACE,
    RLR_SL_TOKEN_SYMBOL_SEMICOLON,
    RLR_SL_TOKEN_SYMBOL_COLON,
    RLR_SL_TOKEN_SYMBOL_STAR,
    RLR_SL_TOKEN_SYMBOL_DASH,
    RLR_SL_TOKEN_SYMBOL_PLUS,
    RLR_SL_TOKEN_SYMBOL_FORWARD_SLASH,
    RLR_SL_TOKEN_SYMBOL_TILDE,
    RLR_SL_TOKEN_SYMBOL_AND,
    RLR_SL_TOKEN_SYMBOL_PIPE,
    RLR_SL_TOKEN_SYMBOL_PERCENT,
    RLR_SL_TOKEN_SYMBOL_EXCLAMATION,
    RLR_SL_TOKEN_SYMBOL_EQUAL,
    RLR_SL_TOKEN_SYMBOL_COMMA,
    RLR_SL_TOKEN_SYMBOL_ARROW_LEFT,
    RLR_SL_TOKEN_SYMBOL_ARROW_RIGHT,
    RLR_SL_TOKEN_SYMBOL_CURLY_BRACKET_OPENED,
    RLR_SL_TOKEN_SYMBOL_CURLY_BRACKET_CLOSED,
    RLR_SL_TOKEN_SYMBOL_BRACKET_OPENED,
    RLR_SL_TOKEN_SYMBOL_BRACKET_CLOSED,
    RLR_SL_TOKEN_SYMBOL_PARENTHESIS_OPENED,
    RLR_SL_TOKEN_SYMBOL_PARENTHESIS_CLOSED,
} rlr_sl_token_type_t;

typedef struct rlr_sl_token_t {
    rlr_sl_token_type_t type;
    rlr_sl_cursor_t cursor_start;
    rlr_sl_cursor_t cursor_end;
    char* value;
} rlr_sl_token_t;

typedef struct rlr_sl_tokenizer_result_t {
    size_t token_count;
    rlr_sl_token_t* tokens;
    size_t error_count;
    char** errors;
} rlr_sl_tokenizer_result_t;

rlr_sl_tokenizer_result_t rlr_sl_token_tokenize_string(const char* source);
void rlr_sl_token_free(rlr_sl_token_t* token);

typedef bool (*rlr_sl_tokenizer_function_t)(rlr_sl_cursor_t*, rlr_sl_tokenizer_result_t*, const char*);

/*
    internal tokenizer functions
*/
bool _rlr_sl_token_tokenizer_parse_space(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source);
bool _rlr_sl_token_tokenizer_parse_static_tokens(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source);
bool _rlr_sl_token_tokenizer_parse_literal_float(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source);
bool _rlr_sl_token_tokenizer_parse_literal_int(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source);
bool _rlr_sl_token_tokenizer_parse_literal_identifier(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source);