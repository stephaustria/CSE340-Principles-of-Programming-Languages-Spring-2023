/*
 * Copyright (C) Rida Bazzi
 *
 * Do not share this file with anyone
 * 
 * Stephen Austria, ASU ID:1220159551
 * Zoe Alinea, ASU ID: 1219539724
 * CSE 340 Gordon 
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM","BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
        << reserved[(int)this->token_type] << " , "
        << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c, tmp1, tmp2, tmp3;
    int base08Target;
    base08Target = 0;
    int target_0;
    target_0 = 0;
    char arr[100];

    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            target_0 = 1;
        }
        else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
                if (c == '8')
                {
                    base08Target = 1;
                }
                else if (c == '9') {
                    base08Target = 1;
                }
                
            }

            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }

        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!

        input.GetChar(tmp1);
        if (tmp1 == 'x') //Check for 'x' first
        {
            input.GetChar(tmp2);
            if (tmp2 == '0')
            {
                input.GetChar(tmp3);

                if (base08Target == 0 && tmp3 == '8') //checking for BASE08
                {
                    tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                    tmp.token_type = BASE08NUM;
                    tmp.line_no = line_no;
                    return tmp; //return as a BASE08 token
                }
                else
                {
                    input.UngetChar(tmp3);
                }

                input.UngetChar(tmp2);
            }
            else if (tmp2 == '1')
            {
                input.GetChar(tmp3);
                if (tmp3 == '6') //Checking for BASE16
                {
                    tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                    tmp.token_type = BASE16NUM;
                    tmp.line_no = line_no;
                    return tmp; //return as a BASE16 token
                }
                else
                {
                    input.UngetChar(tmp3);
                }
                input.UngetChar(tmp2);
            }
            else
            {
                input.UngetChar(tmp2);
            }

            input.UngetChar(tmp1);
        }

        else if (tmp1 == '.')
        {
            int flagNum = 0;
            string letters = "";
            input.GetChar(tmp2);

            if (isdigit(tmp2))
            {
                while (isdigit(tmp2) && !input.EndOfInput())
                {
                    if (tmp2 != '0')
                    {
                        flagNum = 1;
                    }
                    letters = letters + tmp2;
                    input.GetChar(tmp2);

                }
                if (!input.EndOfInput())
                {
                    input.UngetChar(tmp2);
                }
            }
            else
            {
                input.UngetChar(tmp2);
            }
            if (flagNum == 0 && target_0 == 1)
            {
                input.UngetString(letters);
            }
            else
            {
                input.UngetString(letters);
                input.GetChar(tmp2);
                if (isdigit(tmp2))
                {
                    tmp.lexeme = tmp.lexeme + '.';
                    while ((isdigit(tmp2)) && !input.EndOfInput())
                    {
                        tmp.lexeme += tmp2;
                        input.GetChar(tmp2);
                    }
                    if (!input.EndOfInput()) {
                        input.UngetChar(tmp2);
                    }
                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(tmp2);
                }
            }

            input.UngetChar(tmp1);
        }

        else if (tmp1 == 'A') //Checking for 'A' character 
        {
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];


            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }

        }
        else if (tmp1 == 'B') { //Checking for 'B' character
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];


            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }
        }
        else if (tmp1 == 'C') { //Checking for 'C' character 
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];

            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }
        }
        else if (tmp1 == 'D') { //Checking for 'D' character
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];


            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }
        }
        else if (tmp1 == 'E') { //Checking for 'E' Character
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];


            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }
        }
        else if (tmp1 == 'F') { //Checking for 'F' character
            int ct = 0, tCt = 0;
            char arr[100];
            arr[ct] = tmp1;
            while (isdigit(arr[ct]) || (arr[ct] == 'A' || arr[ct] == 'B' || arr[ct] == 'C' || arr[ct] == 'D' || arr[ct] == 'E' || arr[ct] == 'F'))
            {
                ct = ct + 1;
                input.GetChar(arr[ct]);
            }
            tCt = ct;
            tmp1 = arr[ct];


            if (tmp1 == 'x')
            {
                input.GetChar(tmp2);
                if (tmp2 == '1')
                {
                    input.GetChar(tmp3);
                    if (tmp3 == '6')
                    {
                        for (int ct = 0; ct < tCt; ct++) {
                            tmp.lexeme = tmp.lexeme + arr[ct];
                        }
                        tmp.lexeme = tmp.lexeme + tmp1 + tmp2 + tmp3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;

                    }
                    else
                    {
                        input.UngetChar(tmp3);
                        input.UngetChar(tmp2);
                    }
                }
                else
                {
                    input.UngetChar(tmp2);
                }
                input.UngetChar(tmp1);
            }

            ct = ct - 1;
            while (ct >= 0)
            {
                input.UngetChar(arr[ct]);
                ct = ct - 1;
            }
        }

        else

        {
            input.UngetChar(tmp1);
        }
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
    case '.':
        tmp.token_type = DOT;
        return tmp;
    case '+':
        tmp.token_type = PLUS;
        return tmp;
    case '-':
        tmp.token_type = MINUS;
        return tmp;
    case '/':
        tmp.token_type = DIV;
        return tmp;
    case '*':
        tmp.token_type = MULT;
        return tmp;
    case '=':
        tmp.token_type = EQUAL;
        return tmp;
    case ':':
        tmp.token_type = COLON;
        return tmp;
    case ',':
        tmp.token_type = COMMA;
        return tmp;
    case ';':
        tmp.token_type = SEMICOLON;
        return tmp;
    case '[':
        tmp.token_type = LBRAC;
        return tmp;
    case ']':
        tmp.token_type = RBRAC;
        return tmp;
    case '(':
        tmp.token_type = LPAREN;
        return tmp;
    case ')':
        tmp.token_type = RPAREN;
        return tmp;
    case '<':
        input.GetChar(c);
        if (c == '=') {
            tmp.token_type = LTEQ;
        }
        else if (c == '>') {
            tmp.token_type = NOTEQUAL;
        }
        else {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            tmp.token_type = LESS;
        }
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=') {
            tmp.token_type = GTEQ;
        }
        else {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            tmp.token_type = GREATER;
        }
        return tmp;
    default:
        if (isdigit(c)) {
            input.UngetChar(c);
            return ScanNumber();
        }
        else if (isalpha(c)) {
            input.UngetChar(c);
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
            tmp.token_type = END_OF_FILE;
        else
            tmp.token_type = ERROR;

        return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
