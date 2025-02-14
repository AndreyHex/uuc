#include "../include/token.h"
#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/bytecode.h"
#include "../include/precedence.h"
#include "../include/uuc_string.h"
#include <stdint.h>
#include <stdlib.h>

void parse_expression_statement(ParserContext *context);
void parse_expression(ParserContext *context);
void parse_unary(ParserContext *context);
void parse_binary(ParserContext *context);
void parse_number(Token token, ParserContext *context);
void parse_string(Token token, ParserContext *context);
void parse_bool(Token token, ParserContext *context);
void parse_grouping(ParserContext *context);
void parse_precedence(uint8_t min_p, ParserContext *context);

void parser_advance(ParserContext *context);
// returns current token
Token parser_peek(ParserContext *context);
// returns previous token
Token parser_prev(ParserContext *context);

// checks token for math with current and advances
// or exits with error
void parser_consume(TokenType token_type, ParserContext *context);

void emit_constant(Value value, ParserContext *context);
void emit_opcode(OpCode code, ParserContext *context);

// match token_type with current token in context
int match(TokenType token_type, ParserContext *context);

Token next(ParserContext *context) {
    return next_token(&context->lexer_context);
}

Slice parse_code(char *code) {
    LexerContext lexer_context = lexer_init_context(code);
    ParserContext context = { 
        .lexer_context  = lexer_context,
        .bytecode = slice_init(32),
        .current_token = {0},
        .previous_token = {0},
    };
    parser_advance(&context);

    while(!match(TOKEN_EOF, &context) && !match(TOKEN_ERROR, &context)) {
        parse_expression_statement(&context);
    }
    return context.bytecode;
}

void parse_expression_statement(ParserContext *context) {
    parse_expression(context);
    parser_consume(SEMICOLON, context);
}

void parse_expression(ParserContext *context) {
    parse_precedence(1, context);
}

void parse_precedence(uint8_t min_p, ParserContext *context) {
    Token c = parser_peek(context);
    if(c.type == INTEGER || c.type == DOUBLE) {
        parse_number(c, context);
        parser_advance(context);
    } else if (c.type == STRING) {
        parse_string(c, context);
        parser_advance(context);
    } else if (c.type == TRUE || c.type == FALSE) {
        emit_opcode(c.type == TRUE ? OP_TRUE : OP_FALSE , context);
        parser_advance(context);
    } else if (c.type == TOKEN_NULL) {
        emit_opcode(OP_NULL, context);
        parser_advance(context);
    } else if (c.type == LEFT_PAREN) {
        parse_grouping(context);
    } else {
        parse_unary(context);
    }

    Token op = parser_peek(context);
    LOG_TRACE("parse_precedence token op: %s precedence: %d min_p: %d position: %d:%d\n", token_name(op.type), precedence(op.type), min_p, op.line, op.pos);
    while(precedence(op.type) > min_p) {
        parse_binary(context);
        op = parser_peek(context);
    }
}

void parse_unary(ParserContext *context) {
    Token op = parser_peek(context);
    parser_advance(context);
    // prefix binding is stronger for '-' as example
    parse_precedence(precedence(op.type) + 6, context);     switch(op.type) {
        case MINUS: emit_opcode(OP_NEGATE, context); break;
        case BANG: emit_opcode(OP_NOT, context); break;
        default: LOG_ERROR("Unsupported unary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void parse_binary(ParserContext *context) {
    Token op = parser_peek(context);
    LOG_TRACE("parse_binary token: %s precedence: %d position: %d:%d\n", token_name(op.type), precedence(op.type), op.line, op.pos);
    parser_advance(context);
    parse_precedence(precedence(op.type), context);
    switch(op.type) {
        case PLUS: emit_opcode(OP_ADD, context); break;
        case MINUS: emit_opcode(OP_SUBSTRACT, context); break;
        case STAR: emit_opcode(OP_MULTIPLY, context); break;
        case SLASH: emit_opcode(OP_DIVIDE, context); break;
        case EQUAL_EQUAL: emit_opcode(OP_EQ, context); break;
        case BANG_EQUAL: emit_opcode(OP_NE, context); break;
        case GREATER: emit_opcode(OP_GT, context); break;
        case GREATER_EQUAL: emit_opcode(OP_GTE, context); break;
        case LESS: emit_opcode(OP_LT, context); break;
        case LESS_EQUAL: emit_opcode(OP_LTE, context); break;
        default: LOG_ERROR("Unsupported binary operator '%s' at %d:%d\n", token_name(op.type), op.line, op.pos);
    }
}

void parse_grouping(ParserContext *context) {
    parser_consume(LEFT_PAREN, context);
    parse_expression(context);
    parser_consume(RIGHT_PAREN, context);
}

void parser_advance(ParserContext *context) {
    context->previous_token = context->current_token;
    context->current_token = next_token(&context->lexer_context);
}

void parser_consume(TokenType token_type, ParserContext *context) {
    if(!match(token_type, context)) {
        Token t = context->current_token;
        LOG_ERROR("Expected token type '%s' at %d:%d. Got '%s'\n", token_name(token_type), t.line, t.pos, token_name(t.type));
        exit(1);
    }
    parser_advance(context);
}

void parse_number(Token token, ParserContext *context) {
    if(token.type == INTEGER) {
        long val = strtol(token.start, NULL, 10);
        emit_constant((Value){ .type = TYPE_INT, .as = { .uuc_int = val } }, context);
    } else {
        double val = strtod(token.start, NULL);
        emit_constant((Value){ .type = TYPE_DOUBLE, .as = { .uuc_double = val } }, context);
    }
}

void parse_string(Token token, ParserContext *context) {
    UucString *str = uuc_copy_string(token.start + 1, token.length - 2);
    emit_constant((Value){ .type = TYPE_OBJ, .as = { .uuc_obj = (UucObj*)str } }, context);
}

void parse_bool(Token token, ParserContext *context) {
    if(token.type == TRUE) emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 1 } }, context);
    else emit_constant((Value){ .type = TYPE_BOOL, .as = { .uuc_bool = 0 } }, context);
}

Token parser_peek(ParserContext *context) {
    return context->current_token;
}

Token parser_prev(ParserContext *context) {
    return context->previous_token;
}

int match(TokenType token_type, ParserContext *context) {
    return context->current_token.type == token_type;
}

void emit_opcode(OpCode code, ParserContext *context) {
    slice_push_code(code, &context->bytecode);
}

void emit_constant(Value value, ParserContext *context) {
    slice_push_constant(value, &context->bytecode);
}
