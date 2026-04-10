/*
    RL Render Shader Language
*/
#pragma once
#include <stddef.h>
#include "ast.h"

rlr_sl_ast_node_t* rlr_sl_compile(const char* source, char* out_errors, size_t* out_error_count, size_t out_errors_size);