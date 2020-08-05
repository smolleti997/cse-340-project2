/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    PUBLIC, PRIVATE, EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, ID, ERROR // TODO: Add labels for new token types here
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();
    void parse_program(void);
    int parse_global_vars(void);
    int parse_scope(void);
    int parse_var_list(void);
    int parse_public_vars(void);
    int parse_private_vars(void);
    int parse_stmt_list(void);
    int parse_stmt(void);


  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool SkipComment();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    
};
    



#endif  //__LEXER__H__
