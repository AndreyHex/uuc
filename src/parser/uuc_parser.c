#include "../include/uuc_token.h"
#include "../include/uuc_parser.h"
#include "../include/uuc_lexer.h"
#include "../include/uuc_bytecode.h"
#include "../include/uuc_precedence.h"
#include "../include/uuc_string.h"
#include "../include/uuc_result.h"
#include <stdint.h>
#include <stdlib.h>

void parse_declaration(ParserContext *context);
void parse_var_declaration(ParserContext *context);
void parse_statement(ParserContext *context);
void parse_assignment(ParserContext *context);
void parse_expression_statement(ParserContext *context);
void parse_expression(ParserContext *context);
uint16_t parse_identifier(ParserContext *context);
void parse_precedence(int can_assign, uint8_t min_p, ParserContext *context);
void parse_unary(int can_assign, ParserContext *context);
void parse_binary(int can_assign, ParserContext *context);
void parse_number(Token token, ParserContext *context);
void parse_string(Token token, ParserContext *context);
void parse_bool(Token token, ParserContext *context);
void parse_grouping(ParserContext *context);

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

// match token_type with current token in context
int parser_match(TokenType token_type, ParserContext *context);

void parser_synchronize(ParserContext *context);
void parser_panic(ParserContext *context);

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

    uint16_t index = parse_identifier(context);

    if(parser_match(TOKEN_EQUAL, context)) {
        parser_consume(TOKEN_EQUAL, context);
        parse_expression(context);
    } else {
        parser_emit_constant(uuc_val_null(), context);
    }
    parser_consume(TOKEN_SEMICOLON, context);

    parser_emit_opcode(OP_DEFINE_GLOBAL, context);
    parser_emit_opcode(index, context);
}

void parse_assignment(ParserContext *context) {
    uint16_t index = parse_identifier(context);
    parser_consume(TOKEN_EQUAL, context);
    parse_expression(context);
    // parser_emit_opcode(OP_SET_GLOBAL, context);
    parser_emit_opcode(index, context);
}

uint16_t parse_identifier(ParserContext *context) {
    Token t = parser_peek(context);
    parser_consume(TOKEN_IDENTIFIER, context);
    UucString *s = uuc_copy_string(t.start, t.length);
    return slice_register_constant(uuc_val_string_obj(s), &context->bytecode);
}

void parse_statement(ParserContext *context) {
    Token t = parser_peek(context);
    switch(t.type) {
        case TOKEN_IF: printf("if\n"); break;
        case TOKEN_WHILE: printf("while\n"); break;
        case TOKEN_FOR: printf("for\n"); break;
        case TOKEN_RBRACE: printf("{\n"); break;
        case TOKEN_RETURN: printf("return\n"); break;
        default: parse_expression_statement(context);
    }
}

void parse_expression_statement(ParserContext *context) {
    parse_expression( context);
    parser_consume(TOKEN_SEMICOLON, context);
    parser_emit_opcode(OP_POP, context);
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
            uint8_t index = parse_identifier(context);
            parser_emit_opcode(OP_GET_GLOBAL, context);
            parser_emit_opcode(index, context);
            break;
        }
        default: parse_unary(can_assign, context);
    }

    Token op = parser_peek(context);
    if(op.type != TOKEN_EQUAL) can_assign = 0;
    else if(can_assign == 0) {
        LOG_ERROR("Cannot assign to expression at %d:%d.\n", op.line, op.pos);
        parser_panic(context);
        return;
    }
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
    // prefix binding is stronger for '-' as example
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
        case TOKEN_EQUAL: parser_emit_opcode(OP_ASSIGN, context); break;
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

uint64_t parser_emit_constant(Value value, ParserContext *context) {
    return slice_push_constant(value, &context->bytecode);
}
