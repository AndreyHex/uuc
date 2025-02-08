#include "../include/token.h"
#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/bytecode.h"
#include "../include/precedence.h"
#include <stdint.h>
#include <stdlib.h>

void parse_expression_statement(ParserContext *context);
void parse_expression(ParserContext *context);
void parse_unary(ParserContext *context);
void parse_binary(ParserContext *context);
void parse_number(Token token, ParserContext *context);
void parse_grouping(ParserContext *context);
void parse_precedence(uint8_t min_bp, ParserContext *context);

void parser_advance(ParserContext *context);
// returns current token
Token parser_peek(ParserContext *context);
// returns previous token
Token parser_prev(ParserContext *context);

// checks token for math with current and advances
// or exits with error
void parser_consume(TokenType token_type, ParserContext *context);

void emit_constant(double value, ParserContext *context);
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
    parse_precedence(0, context);
}

void parse_precedence(uint8_t min_bp, ParserContext *context) {
    Token number = parser_peek(context);
    parser_advance(context);
    Token op = parser_peek(context);

    Binding bp = binding_power(op.type);

    if(bp.l < min_bp) {
        parse_number(op, context);
        return; // ??
    }

    parser_advance(context);
    parse_precedence(bp.r, context);

    parse_number(op, context);
    if(op.type == PLUS) {
        emit_opcode(OP_ADD, context);
    } else {
        LOG_ERROR("UNSUPPORTED OPERATION ---  WIP --- \n");
    }
}

void parse_unary(ParserContext *context) {
    Token t = parser_peek(context);
    parser_advance(context);
    parse_precedence(0 ,context);
    if(t.type == MINUS) {
        emit_opcode(OP_NEGATE, context);
    }
    // else `!`
}

void parse_binary(ParserContext *context) {
    Token t = parser_peek(context);
    parser_advance(context);
    parse_precedence(0 ,context);
    if(t.type == PLUS) {
        emit_opcode(OP_ADD, context);
    }
}

void parse_grouping(ParserContext *context) {
    parser_consume(RIGHT_PAREN, context);
    parse_expression_statement(context);
    parser_consume(LEFT_PAREN, context);
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
    double val = strtod(token.start, NULL);
    emit_constant(val, context);
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
    slice_push_code(OP_ADD, &context->bytecode);
}

void emit_constant(double value, ParserContext *context) {
    slice_push_constant(value, &context->bytecode);
}
