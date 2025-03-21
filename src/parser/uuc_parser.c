#include "../include/uuc_token.h"
#include "../include/uuc_parser.h"
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

void parse_declaration(ParserContext *context);
void parse_var_declaration(ParserContext *context);
void parse_statement(ParserContext *context);
void parse_expression_statement(ParserContext *context);
void parse_expression(ParserContext *context);
IdentifierIndex parse_identifier(ParserContext *context, int declaration);
void parse_if(ParserContext *context);
void parse_while(ParserContext *context);
void parse_for(ParserContext *context);
void parse_break(ParserContext *context);
void parse_continue(ParserContext *context);
void parser_resolve_loop_jumps(uint32_t loop_start, uint32_t loop_end, ParserContext *context);
void parse_precedence(int can_assign, uint8_t min_p, ParserContext *context);
void parse_unary(int can_assign, ParserContext *context);
void parse_binary(int can_assign, ParserContext *context);
void parse_number(Token token, ParserContext *context);
void parse_string(Token token, ParserContext *context);
void parse_bool(Token token, ParserContext *context);
void parse_grouping(ParserContext *context);
void parse_block(ParserContext *context);

// returns new local_size 
uint32_t parser_pop_scopes(uint32_t to, ParserContext *context);
// begins new scope
void parser_scope_begin(ParserContext *context);
// ends scope
void parser_scope_end(ParserContext *context);

void parser_advance(ParserContext *context);
// returns current token
Token parser_peek(ParserContext *context);
// returns previous token
Token parser_prev(ParserContext *context);
// checks token for math with current and advances
// or exits with error
void parser_consume(TokenType token_type, ParserContext *context);
// returns index in constant list from bytecode slice
uint64_t parser_emit_constant(Value value, ParserContext *context);
void parser_emit_opcode(OpCode code, ParserContext *context);

uint64_t parser_emit_jump(OpCode jump_op, ParserContext *context);
void parser_update_jump(uint64_t index, uint16_t length, ParserContext *context);

// matches token_type with current token in context
int parser_match(TokenType token_type, ParserContext *context);
// enters panic mode
void parser_panic(ParserContext *context);
// skips tokens until point of synchronization 
void parser_synchronize(ParserContext *context);

Token next(ParserContext *context) {
    return next_token(&context->lexer_context);
}

UucResult parse_code(Slice *slice, char *code) {
    LexerContext lexer_context = lexer_init_context(code);
    ParserContext context = { 
        .lexer_context  = lexer_context,
        .bytecode = slice_init(32),
        .current_token = {0},
        .previous_token = {0},
        .panic = 0,
        .error = 0,
        .local_size = 0,
        .scope_depth = 0,
    };
    parser_advance(&context);

    while(!parser_match(TOKEN_EOF, &context) && !parser_match(TOKEN_ERROR, &context)) {
        parse_declaration(&context);
    }
    if(context.error > 0) return UUC_COMP_ERROR;
    *slice = context.bytecode;
    return UUC_OK;
}

void parse_declaration(ParserContext *context) {
    Token t = parser_peek(context);
    switch(t.type) {
        case TOKEN_VAR: parse_var_declaration(context); break;
        case TOKEN_FN: 
        case TOKEN_CLASS: {
            printf("class of fn -- WIP --\n");
            break;
        }
        default: parse_statement(context); break;
    }
    if(context->panic) parser_synchronize(context);
}

void parser_synchronize(ParserContext *context) {
    Token p = parser_peek(context);
    while(p.type != TOKEN_EOF) {
        if(p.type == TOKEN_SEMICOLON) {
            parser_advance(context);
            return;
        }
        switch (p.type) {
            case TOKEN_VAR: return;
            default:;
        }
        parser_advance(context);
    }
}

void parse_var_declaration(ParserContext *context) {
    parser_consume(TOKEN_VAR, context);

    IdentifierIndex index = parse_identifier(context, 1);

    if(parser_match(TOKEN_EQUAL, context)) {
        parser_consume(TOKEN_EQUAL, context);
        parse_expression(context);
    } else {
        parser_emit_constant(uuc_val_null(), context);
    }
    parser_consume(TOKEN_SEMICOLON, context);

    if(index.scope == LOCAL) return;
    parser_emit_opcode(OP_DEFINE_GLOBAL, context);
    parser_emit_opcode(index.index, context);
}

IdentifierIndex parse_identifier(ParserContext *context, int declaration) {
    Token t = parser_peek(context);
    parser_consume(TOKEN_IDENTIFIER, context);
    if(context->scope_depth > 0) {
        if(declaration) {
            context->locals[context->local_size++] = (ParserLocal){
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
    uint16_t index = slice_register_name(uuc_val_string_obj(s), &context->bytecode);
    return (IdentifierIndex){ .scope = GLOBAL, .index = index};
}

void parse_statement(ParserContext *context) {
    Token t = parser_peek(context);
    switch(t.type) {
        case TOKEN_IF: parse_if(context); break;
        case TOKEN_WHILE: parse_while(context); break;
        case TOKEN_FOR: parse_for(context); break;
        case TOKEN_LBRACE: parse_block(context); break;
        case TOKEN_BREAK: parse_break(context); break;
        case TOKEN_CONTINUE: parse_continue(context); break;
        case TOKEN_RETURN: printf("return\n"); break;
        default: parse_expression_statement(context);
    }
}

void parse_if(ParserContext *context) {
    parser_consume(TOKEN_IF, context);
    parser_consume(TOKEN_LPAREN, context);
    parse_expression(context);
    parser_consume(TOKEN_RPAREN, context);

    uint64_t index = parser_emit_jump(OP_JUMP_IF_FALSE, context);
    uint32_t start_if = context->bytecode.size;
    parse_statement(context);
    
    uint64_t else_index = 0;
    if(parser_match(TOKEN_ELSE, context)) {
        parser_consume(TOKEN_ELSE, context);
        else_index = parser_emit_jump(OP_JUMP, context);
    }
    uint32_t end_if = context->bytecode.size;
    parser_update_jump(index, end_if - start_if, context);
    if(else_index) {
        uint32_t start_else = context->bytecode.size;
        parse_statement(context);
        uint32_t else_len = context->bytecode.size - start_else; 
        parser_update_jump(else_index, else_len, context);
    }
}

void parse_while(ParserContext *context) {
    parser_consume(TOKEN_WHILE, context);
    parser_consume(TOKEN_LPAREN, context);

    uint32_t loo_depth = context->loop_depth;
    context->loop_depth = context->scope_depth;

    uint32_t start = context->bytecode.size;
    parse_expression(context);
    parser_consume(TOKEN_RPAREN, context);

    uint64_t cond_jump = parser_emit_jump(OP_JUMP_IF_FALSE, context);
    uint32_t start_body = context->bytecode.size;

    parse_statement(context);
    uint32_t start_jump = parser_emit_jump(OP_JUMP_BACK, context);
    uint32_t end = context->bytecode.size;
    parser_update_jump(cond_jump, end - start_body, context);
    parser_update_jump(start_jump, end - start, context);
    context->loop_depth = loo_depth;
    parser_resolve_loop_jumps(start, end, context);
}

void parse_for(ParserContext *context) {
    parser_consume(TOKEN_FOR, context);
    parser_consume(TOKEN_LPAREN, context);

    uint32_t loo_depth = context->loop_depth;
    context->loop_depth = context->scope_depth;
    parser_scope_begin(context);

    // decl
    if(parser_match(TOKEN_SEMICOLON, context)) {
        parser_consume(TOKEN_SEMICOLON, context);
    } else if(parser_match(TOKEN_VAR, context)) {
        parse_var_declaration(context);
    }
    else parse_expression_statement(context);
    uint32_t after_decl = context->bytecode.size;

    // condition
    int cond_jump = -1;
    int cond_jump_over_increment = -1;
    uint32_t after_condition = 0; 
    if(parser_match(TOKEN_SEMICOLON, context)) {
        parser_consume(TOKEN_SEMICOLON, context);
        after_condition = context->bytecode.size;
    } else {
        parse_expression(context);
        parser_consume(TOKEN_SEMICOLON, context);
        cond_jump = parser_emit_jump(OP_JUMP_IF_FALSE, context);
        after_condition = context->bytecode.size;
        cond_jump_over_increment = parser_emit_jump(OP_JUMP, context);
    }

    // increment
    int increment_jump = -1;
    uint32_t before_increment = context->bytecode.size;
    if(!parser_match(TOKEN_RPAREN, context)) parse_expression(context);
    if(cond_jump >= 0) {
        increment_jump = parser_emit_jump(OP_JUMP_BACK, context);
    }
    parser_consume(TOKEN_RPAREN, context);

    // body
    uint32_t body_start = context->bytecode.size;
    parse_statement(context);
    uint32_t loop_jump = parser_emit_jump(OP_JUMP_BACK, context);
    uint32_t body_end = context->bytecode.size;

    if(cond_jump >= 0) {
        parser_update_jump(cond_jump, body_end - after_condition, context);
    }
    if(cond_jump_over_increment >= 0) {
        parser_update_jump(cond_jump_over_increment, body_start - before_increment, context);
    }
    if(increment_jump >= 0) {
        parser_update_jump(increment_jump, body_start - after_decl, context);
    }
    parser_update_jump(loop_jump, body_end - before_increment, context);

    parser_scope_end(context);
    context->loop_depth = loo_depth;
    parser_resolve_loop_jumps(before_increment, body_end, context);
}

void parser_resolve_loop_jumps(uint32_t loop_start, uint32_t loop_end, ParserContext *context) {
    uint32_t depth =  context->scope_depth;
    while(context->continue_size > 0 &&
          context->continue_jumps[context->continue_size - 1].depth > depth) {
        ParserJump jump = context->continue_jumps[context->continue_size - 1];
        parser_update_jump(jump.index, jump.pos - loop_start, context);
        context->continue_size--;
    }
    while(context->break_size > 0 &&
          context->break_jumps[context->break_size - 1].depth > depth) {
        ParserJump jump = context->break_jumps[context->break_size - 1];
        parser_update_jump(jump.index, loop_end - jump.pos, context);
        context->break_size--;
    }
}

void parse_break(ParserContext *context) {
    if(context->scope_depth == 0) {
        LOG_ERROR("Cannot '' in global context.");
        parser_panic(context);
        return;
    }
    parser_consume(TOKEN_BREAK, context);
    parser_consume(TOKEN_SEMICOLON, context);
    parser_pop_scopes(context->loop_depth, context);
    uint32_t index = parser_emit_jump(OP_JUMP, context);
    uint32_t pos = context->bytecode.size;
    uint32_t depth = context->scope_depth;
    context->break_jumps[context->break_size++] = (ParserJump){
        .index = index,
        .pos = pos,
        .depth = depth
    };
}

void parse_continue(ParserContext *context) {
    if(context->scope_depth == 0) {
        LOG_ERROR("Cannot 'continue' in global context.");
        parser_panic(context);
        return;
    }
    parser_consume(TOKEN_CONTINUE, context);
    parser_consume(TOKEN_SEMICOLON, context);
    parser_pop_scopes(context->loop_depth, context);
    uint32_t index = parser_emit_jump(OP_JUMP_BACK, context);
    uint32_t pos = context->bytecode.size;
    uint32_t depth = context->scope_depth;
    context->continue_jumps[context->continue_size++] = (ParserJump){
        .index = index,
        .pos = pos,
        .depth = depth
    };
}

void parse_block(ParserContext *context) {
    parser_scope_begin(context);
    parser_consume(TOKEN_LBRACE, context);
    Token t = parser_peek(context);
    while(t.type != TOKEN_RBRACE && t.type != TOKEN_EOF) {
        parse_declaration(context);
        t = parser_peek(context);
    }
    parser_consume(TOKEN_RBRACE, context);
    parser_scope_end(context);
}

void parser_scope_begin(ParserContext *context) {
    context->scope_depth++;
}

void parser_scope_end(ParserContext *context) {
    context->scope_depth--;
    uint32_t depth = context->scope_depth;
    context->local_size = parser_pop_scopes(depth, context);
}

uint32_t parser_pop_scopes(uint32_t to, ParserContext *context) {
    uint32_t size = context->local_size;
    while(size > 0 && 
          context->locals[size - 1].depth > to) {
        parser_emit_opcode(OP_POP, context);
        size--;
    }
    return size;
}

void parse_expression_statement(ParserContext *context) {
    parse_expression( context);
    parser_consume(TOKEN_SEMICOLON, context);
}

void parse_expression(ParserContext *context) {
    parse_precedence(1, 1, context);
}

void parse_precedence(int can_assign, uint8_t min_p, ParserContext *context) {
    Token c = parser_peek(context);
    switch(c.type) {
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE: {
            parse_number(c, context);
            parser_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE: {
            parser_emit_opcode(c.type == TOKEN_TRUE ? OP_TRUE : OP_FALSE , context);
            parser_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_STRING: {
            parse_string(c, context);
            parser_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_NULL: {
            parser_emit_opcode(OP_NULL, context);
            parser_advance(context);
            can_assign = 0;
            break;
        }
        case TOKEN_LPAREN: parse_grouping(context); break;
        case TOKEN_IDENTIFIER: {
            IdentifierIndex index = parse_identifier(context, 0);
            if(parser_peek(context).type == TOKEN_EQUAL && can_assign) {
                parser_consume(TOKEN_EQUAL, context);
                parse_precedence(0, 1, context);
                OpCode code = index.scope == GLOBAL ? OP_SET_GLOBAL : OP_SET_LOCAL;
                parser_emit_opcode(code, context);
                parser_emit_opcode(index.index, context);
                return;
            }
            OpCode code = index.scope == GLOBAL ? OP_GET_GLOBAL : OP_GET_LOCAL;
            parser_emit_opcode(code, context);
            parser_emit_opcode(index.index, context);
            break;
        }
        default: parse_unary(can_assign, context);
    }

    Token op = parser_peek(context);
    if(op.type == TOKEN_EQUAL && can_assign == 0) {
        LOG_ERROR("Cannot assign to expression at %d:%d.\n", op.line, op.pos);
        parser_panic(context);
        return;
    }
    if(op.type != TOKEN_EQUAL) can_assign = 0;
    LOG_TRACE("parse_precedence token op: %s precedence: %d min_p: %d can_assign: %d position: %d:%d\n", token_name(op.type), precedence(op.type), min_p, can_assign, op.line, op.pos);
    while(precedence(op.type) > min_p) {
        parse_binary(can_assign, context);
        op = parser_peek(context);
    }
}

void parse_unary(int can_assign, ParserContext *context) {
    can_assign = 0;
    Token op = parser_peek(context);
    parser_advance(context);
    // prefix binding is stronger for '-' for example
    parse_precedence(can_assign, precedence(op.type) + 6, context);     
    switch(op.type) {
        case TOKEN_MINUS: parser_emit_opcode(OP_NEGATE, context); break;
        case TOKEN_BANG: parser_emit_opcode(OP_NOT, context); break;
        default: LOG_ERROR("Unsupported unary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void parse_binary(int can_assign, ParserContext *context) {
    Token op = parser_peek(context);
    LOG_TRACE("parse_binary token: %s precedence: %d can_assign: %d position: %d:%d\n", token_name(op.type), precedence(op.type), can_assign, op.line, op.pos);
    parser_advance(context);
    parse_precedence(can_assign, precedence(op.type), context);
    switch(op.type) {
        case TOKEN_PLUS: parser_emit_opcode(OP_ADD, context); break;
        case TOKEN_MINUS: parser_emit_opcode(OP_SUBSTRACT, context); break;
        case TOKEN_STAR: parser_emit_opcode(OP_MULTIPLY, context); break;
        case TOKEN_SLASH: parser_emit_opcode(OP_DIVIDE, context); break;
        case TOKEN_EQUAL_EQUAL: parser_emit_opcode(OP_EQ, context); break;
        case TOKEN_BANG_EQUAL: parser_emit_opcode(OP_NE, context); break;
        case TOKEN_GREATER: parser_emit_opcode(OP_GT, context); break;
        case TOKEN_GREATER_EQUAL: parser_emit_opcode(OP_GTE, context); break;
        case TOKEN_LESS: parser_emit_opcode(OP_LT, context); break;
        case TOKEN_LESS_EQUAL: parser_emit_opcode(OP_LTE, context); break;
        case TOKEN_EQUAL: parser_emit_opcode(OP_SET_GLOBAL, context); break;
        default: LOG_ERROR("Unsupported binary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void parse_grouping(ParserContext *context) {
    parser_consume(TOKEN_LPAREN, context);
    parse_expression(context);
    parser_consume(TOKEN_RPAREN, context);
}

void parser_advance(ParserContext *context) {
    context->previous_token = context->current_token;
    context->current_token = next_token(&context->lexer_context);
}

void parser_consume(TokenType token_type, ParserContext *context) {
    if(!parser_match(token_type, context)) {
        Token t = context->current_token;
        LOG_ERROR("Expected token type '%s' at %d:%d. Got '%s'\n", token_name(token_type), t.line, t.pos, token_name(t.type));
        parser_panic(context);
        return;
    }
    parser_advance(context);
}

void parse_number(Token token, ParserContext *context) {
    if(token.type == TOKEN_INTEGER) {
        long val = strtol(token.start, NULL, 10);
        parser_emit_constant((Value){ .type = TYPE_INT, .as = { .uuc_int = val } }, context);
    } else {
        double val = strtod(token.start, NULL);
        parser_emit_constant((Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = val } }, context);
    }
}

void parse_string(Token token, ParserContext *context) {
    UucString *str = uuc_copy_string(token.start + 1, token.length - 2);
    parser_emit_constant((Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)str } }, context);
}

void parse_bool(Token token, ParserContext *context) {
    if(token.type == TOKEN_TRUE) parser_emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }, context);
    else parser_emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }, context);
}

void parser_panic(ParserContext *context) {
    context->panic = 1;
    context->error++;
}

Token parser_peek(ParserContext *context) {
    return context->current_token;
}

Token parser_prev(ParserContext *context) {
    return context->previous_token;
}

int parser_match(TokenType token_type, ParserContext *context) {
    return context->current_token.type == token_type;
}

void parser_emit_opcode(OpCode code, ParserContext *context) {
    slice_push_code(code, &context->bytecode);
}

uint64_t parser_emit_jump(OpCode jump_op, ParserContext *context) {
    uint64_t index = slice_push_code(jump_op, &context->bytecode);
    slice_push_code(0x00, &context->bytecode);
    slice_push_code(0x00, &context->bytecode);
    return index;
}

void parser_update_jump(uint64_t index, uint16_t length, ParserContext *context) {
    LOG_TRACE("Updating jump. Index: %ld length: %d\n", index, length);
    context->bytecode.codes[index + 1] = length >> 8;
    context->bytecode.codes[index + 2] = length;
}

uint64_t parser_emit_constant(Value value, ParserContext *context) {
    return slice_push_constant(value, &context->bytecode);
}
