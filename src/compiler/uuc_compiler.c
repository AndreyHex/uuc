#include "../include/uuc_token.h"
#include "../include/uuc_compiler.h"
#include "../include/uuc_lexer.h"
#include "../include/uuc_bytecode.h"
#include "../include/uuc_precedence.h"
#include "../include/uuc_string.h"
#include "../include/uuc_result.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    enum {
        GLOBAL,
        LOCAL
    } scope;
    uint16_t index;
} IdentifierIndex;

void compile_declaration(CompilerContext *context);
void compile_var_declaration(CompilerContext *context);
void compile_function(CompilerContext *context);
void compile_statement(CompilerContext *context);
void compile_expression_statement(CompilerContext *context);
void compile_expression(CompilerContext *context);
IdentifierIndex compile_identifier(CompilerContext *context, int declaration);
void compile_call(IdentifierIndex index, CompilerContext *context);
void compile_if(CompilerContext *context);
void compile_while(CompilerContext *context);
void compile_for(CompilerContext *context);
void compile_break(CompilerContext *context);
void compile_continue(CompilerContext *context);
void compiler_resolve_loop_jumps(uint32_t loop_start, uint32_t loop_end, CompilerContext *context);
void compile_precedence(int can_assign, uint8_t min_p, CompilerContext *context);
void compile_unary(int can_assign, CompilerContext *context);
void compile_binary(int can_assign, CompilerContext *context);
void compile_number(Token token, CompilerContext *context);
void compile_string(Token token, CompilerContext *context);
void compile_bool(Token token, CompilerContext *context);
void compile_grouping(CompilerContext *context);
void compile_block(CompilerContext *context);

// returns new local_size 
uint32_t compiler_pop_scopes(uint32_t to, CompilerContext *context);
// begins new scope
void compiler_scope_begin(CompilerContext *context);
// ends scope
void compiler_scope_end(CompilerContext *context);

void compiler_advance(CompilerContext *context);
// returns current token
Token compiler_peek(CompilerContext *context);
// returns previous token
Token compiler_prev(CompilerContext *context);
// checks token for math with current and advances
// or exits with error
void compiler_consume(TokenType token_type, CompilerContext *context);
// returns index in constant list from bytecode slice
uint64_t compiler_emit_constant(Value value, CompilerContext *context);
void compiler_emit_opcode(OpCode code, CompilerContext *context);

uint64_t compiler_emit_jump(OpCode jump_op, CompilerContext *context);
void compiler_update_jump(uint64_t index, uint16_t length, CompilerContext *context);

// matches token_type with current token in context
int compiler_match(TokenType token_type, CompilerContext *context);
// enters panic mode
void compiler_panic(CompilerContext *context);
// skips tokens until point of synchronization 
void compiler_synchronize(CompilerContext *context);

Token next(CompilerContext *context) {
    return next_token(&context->lexer_context);
}

UucResult compile_code(UucFunction **main, char *code) {
    LexerContext lexer_context = lexer_init_context(code);
    *main = uuc_create_function("main");
    CompilerContext context = { 
        .lexer_context  = lexer_context,
        .main = *main,
        .current = *main,
        .current_token = {0},
        .previous_token = {0},
        .panic = 0,
        .error = 0,
        .local_size = 0,
        .scope_depth = 0,
        .loop_depth = -1,
        .break_size = 0,
        .continue_size = 0,
    };
    compiler_advance(&context);

    while(!compiler_match(TOKEN_EOF, &context) && !compiler_match(TOKEN_ERROR, &context)) {
        compile_declaration(&context);
    }
    if(context.error > 0) return UUC_COMP_ERROR;
    return UUC_OK;
}

void compile_declaration(CompilerContext *context) {
    Token t = compiler_peek(context);
    switch(t.type) {
        case TOKEN_VAR: compile_var_declaration(context); break;
        case TOKEN_FN: compile_function(context); break;
        case TOKEN_CLASS: {
            printf("class of fn -- WIP --\n");
            break;
        }
        default: compile_statement(context); break;
    }
    if(context->panic) compiler_synchronize(context);
}

void compile_function(CompilerContext *context) {
    compiler_consume(TOKEN_FN, context);

    Token t = compiler_peek(context);
    IdentifierIndex i = compile_identifier(context, 1);

    UucFunction *fun = uuc_create_function_t(t);
    UucFunction *prev = context->current;
    context->current = fun;

    compiler_scope_begin(context);

    compiler_consume(TOKEN_LPAREN, context);
    uint8_t arity = 0;
    while(!compiler_match(TOKEN_RPAREN, context)) {
        compile_identifier(context, 1);
        arity++;
        if(compiler_match(TOKEN_COMMA, context)) compiler_consume(TOKEN_COMMA, context);
        else break;
    }
    compiler_consume(TOKEN_RPAREN, context);
    fun->arity = arity;

    // body
    compile_block(context);

    compiler_emit_opcode(OP_NULL, context);
    compiler_emit_opcode(OP_RETURN, context);

    context->current = prev;
    context->scope_depth--;

    compiler_emit_constant((Value){.type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)fun }}, context);
    if(i.scope == GLOBAL) {
        compiler_emit_opcode(OP_DEFINE_GLOBAL, context);
        compiler_emit_opcode(i.index, context);
    }
}

void compiler_synchronize(CompilerContext *context) {
    Token p = compiler_peek(context);
    context->panic = 0;
    while(p.type != TOKEN_EOF) {
        if(p.type == TOKEN_SEMICOLON) {
            compiler_advance(context);
            return;
        }
        switch (p.type) {
            case TOKEN_WHILE:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_BREAK:
            case TOKEN_CONTINUE:
            case TOKEN_VAR: return;
            default:;
        }
        compiler_advance(context);
        p = compiler_peek(context);
    }
}

void compile_var_declaration(CompilerContext *context) {
    compiler_consume(TOKEN_VAR, context);

    IdentifierIndex index = compile_identifier(context, 1);

    if(compiler_match(TOKEN_EQUAL, context)) {
        compiler_consume(TOKEN_EQUAL, context);
        compile_expression(context);
    } else {
        compiler_emit_constant(uuc_val_null(), context);
    }
    compiler_consume(TOKEN_SEMICOLON, context);

    if(index.scope == LOCAL) return;
    compiler_emit_opcode(OP_DEFINE_GLOBAL, context);
    compiler_emit_opcode(index.index, context);
}

IdentifierIndex compile_identifier(CompilerContext *context, int declaration) {
    Token t = compiler_peek(context);
    compiler_consume(TOKEN_IDENTIFIER, context);
    if(context->scope_depth > 0) {
        if(declaration) {
            context->locals[context->local_size++] = (CompilerLocal){
                .name = t,
                .depth = context->scope_depth,
            };
            return (IdentifierIndex){ .scope = LOCAL, .index = context->local_size - 1};
        } else {
            // lookup for existing local
            for(int i = context->local_size - 1; i >= 0; i--) {
                Token l = context->locals[i].name;
                if(t.length == l.length && memcmp(t.start, l.start, t.length) == 0) {
                    return (IdentifierIndex){ .scope = LOCAL, .index = i};
                }
            }
        }
    }
    UucString *s = uuc_copy_string(t.start, t.length);
    uint16_t index = slice_register_name(uuc_val_string_obj(s), &context->current->bytecode);
    return (IdentifierIndex){ .scope = GLOBAL, .index = index};
}

void compile_statement(CompilerContext *context) {
    Token t = compiler_peek(context);
    switch(t.type) {
        case TOKEN_IF: compile_if(context); break;
        case TOKEN_WHILE: compile_while(context); break;
        case TOKEN_FOR: compile_for(context); break;
        case TOKEN_LBRACE: compile_block(context); break;
        case TOKEN_BREAK: compile_break(context); break;
        case TOKEN_CONTINUE: compile_continue(context); break;
        case TOKEN_RETURN: {
            compiler_consume(TOKEN_RETURN, context);
            compile_expression(context);
            compiler_consume(TOKEN_SEMICOLON, context);
            compiler_emit_opcode(OP_RETURN, context);
            break; 
        }
        default: compile_expression_statement(context);
    }
}

void compile_if(CompilerContext *context) {
    compiler_consume(TOKEN_IF, context);
    compiler_consume(TOKEN_LPAREN, context);
    compile_expression(context);
    compiler_consume(TOKEN_RPAREN, context);

    uint64_t index = compiler_emit_jump(OP_JUMP_IF_FALSE, context);
    uint32_t start_if = context->current->bytecode.size;
    compile_statement(context);
    if(context->panic) return;
    
    uint64_t else_index = 0;
    if(compiler_match(TOKEN_ELSE, context)) {
        compiler_consume(TOKEN_ELSE, context);
        else_index = compiler_emit_jump(OP_JUMP, context);
    }
    uint32_t end_if = context->current->bytecode.size;
    compiler_update_jump(index, end_if - start_if, context);
    if(else_index) {
        uint32_t start_else = context->current->bytecode.size;
        compile_statement(context);
        if(context->panic) return;
        uint32_t else_len = context->current->bytecode.size - start_else; 
        compiler_update_jump(else_index, else_len, context);
    }
}

void compile_while(CompilerContext *context) {
    compiler_consume(TOKEN_WHILE, context);
    compiler_consume(TOKEN_LPAREN, context);

    uint32_t loo_depth = context->loop_depth;
    context->loop_depth = context->scope_depth;

    uint32_t start = context->current->bytecode.size;
    compile_expression(context);
    compiler_consume(TOKEN_RPAREN, context);

    uint64_t cond_jump = compiler_emit_jump(OP_JUMP_IF_FALSE, context);
    uint32_t start_body = context->current->bytecode.size;

    compile_statement(context);
    if(context->panic) return;
    uint32_t start_jump = compiler_emit_jump(OP_JUMP_BACK, context);
    uint32_t end = context->current->bytecode.size;
    compiler_update_jump(cond_jump, end - start_body, context);
    compiler_update_jump(start_jump, end - start, context);
    context->loop_depth = loo_depth;
    compiler_resolve_loop_jumps(start, end, context);
}

void compile_for(CompilerContext *context) {
    compiler_consume(TOKEN_FOR, context);
    compiler_consume(TOKEN_LPAREN, context);

    uint32_t loo_depth = context->loop_depth;
    context->loop_depth = context->scope_depth;
    compiler_scope_begin(context);

    // decl
    if(compiler_match(TOKEN_SEMICOLON, context)) {
        compiler_consume(TOKEN_SEMICOLON, context);
    } else if(compiler_match(TOKEN_VAR, context)) {
        compile_var_declaration(context);
    }
    else compile_expression_statement(context);
    uint32_t after_decl = context->current->bytecode.size;

    // condition
    int cond_jump = -1;
    int cond_jump_over_increment = -1;
    uint32_t after_condition = 0; 
    if(compiler_match(TOKEN_SEMICOLON, context)) {
        compiler_consume(TOKEN_SEMICOLON, context);
        after_condition = uuc_cur_bcode(context)->size;
    } else {
        compile_expression(context);
        compiler_consume(TOKEN_SEMICOLON, context);
        cond_jump = compiler_emit_jump(OP_JUMP_IF_FALSE, context);
        after_condition = uuc_cur_bcode(context)->size;
        cond_jump_over_increment = compiler_emit_jump(OP_JUMP, context);
    }

    // increment
    int increment_jump = -1;
    uint32_t before_increment = uuc_cur_bcode(context)->size;
    if(!compiler_match(TOKEN_RPAREN, context)) compile_expression(context);
    if(cond_jump >= 0) {
        increment_jump = compiler_emit_jump(OP_JUMP_BACK, context);
    }
    compiler_consume(TOKEN_RPAREN, context);

    // body
    uint32_t body_start = uuc_cur_bcode(context)->size;
    compile_statement(context);
    if(context->panic) return;
    uint32_t loop_jump = compiler_emit_jump(OP_JUMP_BACK, context);
    uint32_t body_end = uuc_cur_bcode(context)->size;

    if(cond_jump >= 0) {
        compiler_update_jump(cond_jump, body_end - after_condition, context);
    }
    if(cond_jump_over_increment >= 0) {
        compiler_update_jump(cond_jump_over_increment, body_start - before_increment, context);
    }
    if(increment_jump >= 0) {
        compiler_update_jump(increment_jump, body_start - after_decl, context);
    }
    compiler_update_jump(loop_jump, body_end - before_increment, context);

    compiler_scope_end(context);
    context->loop_depth = loo_depth;
    compiler_resolve_loop_jumps(before_increment, body_end, context);
}

void compiler_resolve_loop_jumps(uint32_t loop_start, uint32_t loop_end, CompilerContext *context) {
    uint32_t depth =  context->scope_depth;
    while(context->continue_size > 0 &&
          context->continue_jumps[context->continue_size - 1].depth > depth) {
        CompilerJump jump = context->continue_jumps[context->continue_size - 1];
        compiler_update_jump(jump.index, jump.pos - loop_start, context);
        context->continue_size--;
    }
    while(context->break_size > 0 &&
          context->break_jumps[context->break_size - 1].depth > depth) {
        CompilerJump jump = context->break_jumps[context->break_size - 1];
        compiler_update_jump(jump.index, loop_end - jump.pos, context);
        context->break_size--;
    }
}

void compile_break(CompilerContext *context) {
    compiler_consume(TOKEN_BREAK, context);
    if(context->loop_depth < 0) {
        LOG_ERROR("Cannot 'break' outside of loop.\n");
        compiler_panic(context);
        return;
    }
    compiler_consume(TOKEN_SEMICOLON, context);
    compiler_pop_scopes(context->loop_depth, context);
    uint32_t index = compiler_emit_jump(OP_JUMP, context);
    uint32_t pos = uuc_cur_bcode(context)->size;
    uint32_t depth = context->scope_depth;
    context->break_jumps[context->break_size++] = (CompilerJump){
        .index = index,
        .pos = pos,
        .depth = depth
    };
}

void compile_continue(CompilerContext *context) {
    compiler_consume(TOKEN_CONTINUE, context);
    if(context->loop_depth < 0) {
        LOG_ERROR("Cannot 'continue' outside of loop.\n");
        compiler_panic(context);
        return;
    }
    compiler_consume(TOKEN_SEMICOLON, context);
    compiler_pop_scopes(context->loop_depth, context);
    uint32_t index = compiler_emit_jump(OP_JUMP_BACK, context);
    uint32_t pos = uuc_cur_bcode(context)->size;
    uint32_t depth = context->scope_depth;
    context->continue_jumps[context->continue_size++] = (CompilerJump){
        .index = index,
        .pos = pos,
        .depth = depth
    };
}

void compile_block(CompilerContext *context) {
    compiler_scope_begin(context);
    compiler_consume(TOKEN_LBRACE, context);
    Token t = compiler_peek(context);
    while(t.type != TOKEN_RBRACE && t.type != TOKEN_EOF) {
        compile_declaration(context);
        t = compiler_peek(context);
    }
    compiler_consume(TOKEN_RBRACE, context);
    compiler_scope_end(context);
}

void compiler_scope_begin(CompilerContext *context) {
    context->scope_depth++;
}

void compiler_scope_end(CompilerContext *context) {
    context->scope_depth--;
    uint32_t depth = context->scope_depth;
    context->local_size = compiler_pop_scopes(depth, context);
}

uint32_t compiler_pop_scopes(uint32_t to, CompilerContext *context) {
    uint32_t size = context->local_size;
    while(size > 0 && 
          context->locals[size - 1].depth > to) {
        compiler_emit_opcode(OP_POP, context);
        size--;
    }
    return size;
}

void compile_expression_statement(CompilerContext *context) {
    compile_expression( context);
    compiler_consume(TOKEN_SEMICOLON, context);
}

void compile_expression(CompilerContext *context) {
    compile_precedence(1, 1, context);
}

void compile_precedence(int can_assign, uint8_t min_p, CompilerContext *context) {
    Token c = compiler_peek(context);
    switch(c.type) {
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE: {
            compile_number(c, context);
            compiler_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE: {
            compiler_emit_opcode(c.type == TOKEN_TRUE ? OP_TRUE : OP_FALSE , context);
            compiler_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_STRING: {
            compile_string(c, context);
            compiler_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_NULL: {
            compiler_emit_opcode(OP_NULL, context);
            compiler_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_LPAREN: compile_grouping(context); break;
        case TOKEN_IDENTIFIER: {
            IdentifierIndex index = compile_identifier(context, 0);
            Token p = compiler_peek(context);
            if(p.type == TOKEN_LPAREN) {
                compile_call(index, context);
                break;
            } else if(p.type == TOKEN_EQUAL && can_assign) {
                compiler_consume(TOKEN_EQUAL, context);
                compile_precedence(0, 1, context);
                OpCode code = index.scope == GLOBAL ? OP_SET_GLOBAL : OP_SET_LOCAL;
                compiler_emit_opcode(code, context);
                compiler_emit_opcode(index.index, context);
                return;
            }
            OpCode code = index.scope == GLOBAL ? OP_GET_GLOBAL : OP_GET_LOCAL;
            compiler_emit_opcode(code, context);
            compiler_emit_opcode(index.index, context);
            break;
        }
        default: compile_unary(can_assign, context);
    }

    Token op = compiler_peek(context);
    if(op.type == TOKEN_EQUAL && can_assign == 0) {
        LOG_ERROR("Cannot assign to expression at %d:%d.\n", op.line, op.pos);
        compiler_panic(context);
        return;
    }
    if(op.type != TOKEN_EQUAL) can_assign = 0;
    LOG_TRACE("compile_precedence token op: %s precedence: %d min_p: %d can_assign: %d position: %d:%d\n", token_name(op.type), precedence(op.type), min_p, can_assign, op.line, op.pos);
    while(precedence(op.type) > min_p) {
        compile_binary(can_assign, context);
        op = compiler_peek(context);
    }
}

void compile_call(IdentifierIndex index, CompilerContext *context) {
    compiler_consume(TOKEN_LPAREN, context);
    uint8_t args = 0;
    while(!compiler_match(TOKEN_RPAREN, context)) {
        compile_expression(context);
        args++;
        if(compiler_match(TOKEN_COMMA, context)) compiler_consume(TOKEN_COMMA, context);
        else break;
    }
    compiler_consume(TOKEN_RPAREN, context);
    OpCode code = index.scope == GLOBAL ? OP_GET_GLOBAL : OP_GET_LOCAL;
    compiler_emit_opcode(code, context);
    compiler_emit_opcode(index.index, context);
    compiler_emit_opcode(OP_CALL, context);
    compiler_emit_opcode(args, context);
    if(compiler_match(TOKEN_LPAREN, context)) compile_call(index, context);
}

void compile_unary(int can_assign, CompilerContext *context) {
    can_assign = 0;
    Token op = compiler_peek(context);
    compiler_advance(context);
    // prefix binding is stronger for '-' for example
    compile_precedence(can_assign, precedence(op.type) + 6, context);     
    switch(op.type) {
        case TOKEN_MINUS: compiler_emit_opcode(OP_NEGATE, context); break;
        case TOKEN_BANG: compiler_emit_opcode(OP_NOT, context); break;
        default: LOG_ERROR("Unsupported unary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void compile_binary(int can_assign, CompilerContext *context) {
    Token op = compiler_peek(context);
    LOG_TRACE("compile_binary token: %s precedence: %d can_assign: %d position: %d:%d\n", token_name(op.type), precedence(op.type), can_assign, op.line, op.pos);
    compiler_advance(context);
    compile_precedence(can_assign, precedence(op.type), context);
    switch(op.type) {
        case TOKEN_PLUS: compiler_emit_opcode(OP_ADD, context); break;
        case TOKEN_MINUS: compiler_emit_opcode(OP_SUBSTRACT, context); break;
        case TOKEN_STAR: compiler_emit_opcode(OP_MULTIPLY, context); break;
        case TOKEN_SLASH: compiler_emit_opcode(OP_DIVIDE, context); break;
        case TOKEN_EQUAL_EQUAL: compiler_emit_opcode(OP_EQ, context); break;
        case TOKEN_BANG_EQUAL: compiler_emit_opcode(OP_NE, context); break;
        case TOKEN_GREATER: compiler_emit_opcode(OP_GT, context); break;
        case TOKEN_GREATER_EQUAL: compiler_emit_opcode(OP_GTE, context); break;
        case TOKEN_LESS: compiler_emit_opcode(OP_LT, context); break;
        case TOKEN_LESS_EQUAL: compiler_emit_opcode(OP_LTE, context); break;
        case TOKEN_EQUAL: compiler_emit_opcode(OP_SET_GLOBAL, context); break;
        default: LOG_ERROR("Unsupported binary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void compile_grouping(CompilerContext *context) {
    compiler_consume(TOKEN_LPAREN, context);
    compile_expression(context);
    compiler_consume(TOKEN_RPAREN, context);
}

void compiler_advance(CompilerContext *context) {
    context->previous_token = context->current_token;
    context->current_token = next_token(&context->lexer_context);
}

void compiler_consume(TokenType token_type, CompilerContext *context) {
    if(!compiler_match(token_type, context)) {
        Token t = context->current_token;
        LOG_ERROR("Expected token type '%s' at %d:%d. Got '%s'\n", token_name(token_type), t.line, t.pos, token_name(t.type));
        compiler_panic(context);
        return;
    }
    compiler_advance(context);
}

void compile_number(Token token, CompilerContext *context) {
    if(token.type == TOKEN_INTEGER) {
        long val = strtol(token.start, NULL, 10);
        compiler_emit_constant((Value){ .type = TYPE_INT, .as = { .uuc_int = val } }, context);
    } else {
        double val = strtod(token.start, NULL);
        compiler_emit_constant((Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = val } }, context);
    }
}

void compile_string(Token token, CompilerContext *context) {
    UucString *str = uuc_copy_string(token.start + 1, token.length - 2);
    compiler_emit_constant((Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)str } }, context);
}

void compile_bool(Token token, CompilerContext *context) {
    if(token.type == TOKEN_TRUE) compiler_emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }, context);
    else compiler_emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }, context);
}

void compiler_panic(CompilerContext *context) {
    Token p = compiler_prev(context);
    LOG_ERROR("Panic on token '%s' %d:%d\n", token_name(p.type), p.line, p.pos);
    context->panic = 1;
    context->error++;
}

Token compiler_peek(CompilerContext *context) {
    return context->current_token;
}

Token compiler_prev(CompilerContext *context) {
    return context->previous_token;
}

int compiler_match(TokenType token_type, CompilerContext *context) {
    return context->current_token.type == token_type;
}

void compiler_emit_opcode(OpCode code, CompilerContext *context) {
    slice_push_code(code, &context->current->bytecode);
}

uint64_t compiler_emit_jump(OpCode jump_op, CompilerContext *context) {
    uint64_t index = slice_push_code(jump_op, &context->current->bytecode);
    slice_push_code(0x00, &context->current->bytecode);
    slice_push_code(0x00, &context->current->bytecode);
    return index;
}

void compiler_update_jump(uint64_t index, uint16_t length, CompilerContext *context) {
    LOG_TRACE("Updating jump. Index: %ld length: %d\n", index, length);
    uuc_cur_bcode(context)->codes[index + 1] = length >> 8;
    uuc_cur_bcode(context)->codes[index + 2] = length;
}

uint64_t compiler_emit_constant(Value value, CompilerContext *context) {
    return slice_push_constant(value, &context->current->bytecode);
}
