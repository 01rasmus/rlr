#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stb_ds.h>
#include "token.h"

typedef struct rlr_sl_token_str_to_type_t {
    const char* string;
    rlr_sl_token_type_t type;
} rlr_sl_token_str_to_type_t;

/*
    this array contains strings that matches
    the actual strings that is equialent
    to a static token. Like "struct" or "*".

    these are sorted by length and then
    by alphabetical order, starting with
    the longest
*/
static rlr_sl_token_str_to_type_t rlr_sl_token_static_matches[] = {
    { .string = "double", .type = RLR_SL_TOKEN_TYPE_DOUBLE },
    { .string = "mat2x2", .type = RLR_SL_TOKEN_TYPE_MAT2X2 },
    { .string = "mat2x3", .type = RLR_SL_TOKEN_TYPE_MAT2X3 },
    { .string = "mat2x4", .type = RLR_SL_TOKEN_TYPE_MAT2X4 },
    { .string = "mat3x2", .type = RLR_SL_TOKEN_TYPE_MAT3X2 },
    { .string = "mat3x3", .type = RLR_SL_TOKEN_TYPE_MAT3X3 },
    { .string = "mat3x4", .type = RLR_SL_TOKEN_TYPE_MAT3X4 },
    { .string = "mat4x2", .type = RLR_SL_TOKEN_TYPE_MAT4X2 },
    { .string = "mat4x3", .type = RLR_SL_TOKEN_TYPE_MAT4X3 },
    { .string = "mat4x4", .type = RLR_SL_TOKEN_TYPE_MAT4X4 },
    { .string = "return", .type = RLR_SL_TOKEN_KEYWORD_RETURN },
    { .string = "struct", .type = RLR_SL_TOKEN_KEYWORD_STRUCT },
    { .string = "bvec2", .type = RLR_SL_TOKEN_TYPE_BVEC2 },
    { .string = "bvec3", .type = RLR_SL_TOKEN_TYPE_BVEC3 },
    { .string = "bvec4", .type = RLR_SL_TOKEN_TYPE_BVEC4 },
    { .string = "const", .type = RLR_SL_TOKEN_KEYWORD_CONST },
    { .string = "dvec2", .type = RLR_SL_TOKEN_TYPE_DVEC2 },
    { .string = "dvec3", .type = RLR_SL_TOKEN_TYPE_DVEC3 },
    { .string = "dvec4", .type = RLR_SL_TOKEN_TYPE_DVEC4 },
    { .string = "false", .type = RLR_SL_TOKEN_LITERAL_FALSE },
    { .string = "float", .type = RLR_SL_TOKEN_TYPE_FLOAT },
    { .string = "float", .type = RLR_SL_TOKEN_TYPE_FLOAT },
    { .string = "ivec2", .type = RLR_SL_TOKEN_TYPE_IVEC2 },
    { .string = "ivec3", .type = RLR_SL_TOKEN_TYPE_IVEC3 },
    { .string = "ivec4", .type = RLR_SL_TOKEN_TYPE_IVEC4 },
    { .string = "uvec2", .type = RLR_SL_TOKEN_TYPE_UVEC2 },
    { .string = "uvec3", .type = RLR_SL_TOKEN_TYPE_UVEC3 },
    { .string = "uvec4", .type = RLR_SL_TOKEN_TYPE_UVEC4 },
    { .string = "while", .type = RLR_SL_TOKEN_KEYWORD_WHILE },
    { .string = "bool", .type = RLR_SL_TOKEN_TYPE_BOOL },
    { .string = "bool", .type = RLR_SL_TOKEN_TYPE_BOOL },
    { .string = "else", .type = RLR_SL_TOKEN_KEYWORD_ELSE },
    { .string = "mat2", .type = RLR_SL_TOKEN_TYPE_MAT2 },
    { .string = "mat3", .type = RLR_SL_TOKEN_TYPE_MAT3 },
    { .string = "mat4", .type = RLR_SL_TOKEN_TYPE_MAT4 },
    { .string = "true", .type = RLR_SL_TOKEN_LITERAL_TRUE },
    { .string = "uint", .type = RLR_SL_TOKEN_TYPE_UINT },
    { .string = "vec2", .type = RLR_SL_TOKEN_TYPE_VEC2 },
    { .string = "vec3", .type = RLR_SL_TOKEN_TYPE_VEC3 },
    { .string = "vec4", .type = RLR_SL_TOKEN_TYPE_VEC4 },
    { .string = "for", .type = RLR_SL_TOKEN_KEYWORD_FOR },
    { .string = "int", .type = RLR_SL_TOKEN_TYPE_INT },
    { .string = "if", .type = RLR_SL_TOKEN_KEYWORD_IF },

    //single character tokens
    { .string = "-", .type = RLR_SL_TOKEN_SYMBOL_DASH },
    { .string = ",", .type = RLR_SL_TOKEN_SYMBOL_COMMA },
    { .string = ";", .type = RLR_SL_TOKEN_SYMBOL_SEMICOLON },
    { .string = ":", .type = RLR_SL_TOKEN_SYMBOL_COLON },
    { .string = "!", .type = RLR_SL_TOKEN_SYMBOL_EXCLAMATION },
    { .string = "(", .type = RLR_SL_TOKEN_SYMBOL_PARENTHESIS_OPENED },
    { .string = ")", .type = RLR_SL_TOKEN_SYMBOL_PARENTHESIS_CLOSED },
    { .string = "[", .type = RLR_SL_TOKEN_SYMBOL_BRACKET_OPENED },
    { .string = "]", .type = RLR_SL_TOKEN_SYMBOL_BRACKET_CLOSED },
    { .string = "{", .type = RLR_SL_TOKEN_SYMBOL_CURLY_BRACKET_OPENED },
    { .string = "}", .type = RLR_SL_TOKEN_SYMBOL_CURLY_BRACKET_CLOSED },
    { .string = "*", .type = RLR_SL_TOKEN_SYMBOL_STAR },
    { .string = "/", .type = RLR_SL_TOKEN_SYMBOL_FORWARD_SLASH },
    { .string = "&", .type = RLR_SL_TOKEN_SYMBOL_AND },
    { .string = "%", .type = RLR_SL_TOKEN_SYMBOL_PERCENT },
    { .string = "+", .type = RLR_SL_TOKEN_SYMBOL_PLUS },
    { .string = "<", .type = RLR_SL_TOKEN_SYMBOL_ARROW_LEFT },
    { .string = "=", .type = RLR_SL_TOKEN_SYMBOL_EQUAL },
    { .string = ">", .type = RLR_SL_TOKEN_SYMBOL_ARROW_RIGHT },
    { .string = "|", .type = RLR_SL_TOKEN_SYMBOL_PIPE },
    { .string = "~", .type = RLR_SL_TOKEN_SYMBOL_TILDE },
};

void rlr_sl_token_free(rlr_sl_token_t* token) {
    switch(token->type) {
        case RLR_SL_TOKEN_LITERAL_IDENTIFIER:
        case RLR_SL_TOKEN_LITERAL_FLOAT:
        case RLR_SL_TOKEN_LITERAL_INT:
            free(token->value);
            break;
        default:
            break;
    }
}

/*
    list of tokenizer functions.
    the order of these functions are important
    and should not be changed
*/
static rlr_sl_tokenizer_function_t tokenizer_functions[] = {
    _rlr_sl_token_tokenizer_parse_space,
    _rlr_sl_token_tokenizer_parse_static_tokens,
    _rlr_sl_token_tokenizer_parse_literal_float,
    _rlr_sl_token_tokenizer_parse_literal_int,
    _rlr_sl_token_tokenizer_parse_literal_identifier
};

rlr_sl_tokenizer_result_t rlr_sl_token_tokenize_string(const char* source) {
    rlr_sl_tokenizer_result_t res = (rlr_sl_tokenizer_result_t){
        .error_count = 0,
        .token_count = 0,
        .errors = NULL,
        .tokens = NULL,
    };

    rlr_sl_cursor_t cursor = rlr_sl_cursor_create();
    size_t source_length = strlen(source);
    while(1) {
        bool added_token = false;
        for(int32_t i = 0; i < sizeof(tokenizer_functions) / sizeof(rlr_sl_tokenizer_function_t); i++) {
            rlr_sl_tokenizer_function_t tokenizer = tokenizer_functions[i];
            if(tokenizer(&cursor, &res, source)) {
                added_token = true;
                break;
            }
        }

        //we arrived at a character that is illegal.
        //we will add it as an error, and continue
        //to check if there are more errors further down
        if(!added_token) {

        }

        if(source_length <= cursor.index) {
            break;
        }
    }
    return res;
err:
    arrfree(res.errors);
    arrfree(res.tokens);
    res.errors = NULL;
    res.tokens = NULL;
    return res;
}

bool _rlr_sl_token_tokenizer_parse_space(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source) {
    rlr_sl_cursor_t cursor_current = *cursor;
    rlr_sl_cursor_t cursor_start = cursor_current;
    bool is_space = false;
    while(1) {
        const char character = source[cursor_current.index];
        bool current_is_space = isspace(character);
        if(current_is_space) {
            is_space = true;
        } else {
            break;
        }
        if(rlr_sl_cursor_advance(&cursor_current, source, 1) != 1) {
            break;
        }
    }
    if(is_space) {
        rlr_sl_token_t token = (rlr_sl_token_t){
            .type = RLR_SL_TOKEN_SYMBOL_SPACE,
            .cursor_start = cursor_start,
            .cursor_end = cursor_current,
        };
        (*cursor) = cursor_current;
        arrpush(res->tokens, token);
    }
    return is_space;
}

bool _rlr_sl_token_tokenizer_parse_static_tokens(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source) {
    rlr_sl_cursor_t cursor_current = *cursor;

    bool found_token = false;
    for(int32_t i = 0; i < sizeof(rlr_sl_token_static_matches) / sizeof(rlr_sl_token_str_to_type_t); i++) {
        rlr_sl_token_str_to_type_t* stt = &rlr_sl_token_static_matches[i];
        size_t token_string_length = strlen(stt->string);

        if(strcmp(stt->string, source + cursor_current.index) == 0) {
            if(rlr_sl_cursor_advance(&cursor_current, source, token_string_length) != token_string_length) {
                break;
            }
            rlr_sl_token_t token = (rlr_sl_token_t){
                .type = stt->type,
                .cursor_start = *cursor,
                .cursor_end = cursor_current,
            };
            found_token = true;
            arrpush(res->tokens, token);
            (*cursor) = cursor_current;
            break;
        }
    }
    return found_token;
}

bool _rlr_sl_token_tokenizer_parse_literal_float(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source) {

}

bool _rlr_sl_token_tokenizer_parse_literal_int(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source) {

}

bool _rlr_sl_token_tokenizer_parse_literal_identifier(rlr_sl_cursor_t* cursor, rlr_sl_tokenizer_result_t* res, const char* source) {

}