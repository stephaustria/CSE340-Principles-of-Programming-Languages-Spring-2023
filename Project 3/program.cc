/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSE 340 - Gordon Spring 2023                                                                                    //
// Stephen Austria and Zoe Alinea                                                                                  //
// ASU ID: 1220159551                                                                                              //
// IMPORTANT Note: This project was already partially (around half) completed by Zoe Alinea AND Stephen Austria,   //
// PRIOR to the announcement on April 10th stating that the project would be changed to individual. Parts of the   //
// parser functions were already completed as a pair. And an outline for the plans for our code was already made   //
// After the announcement the troubleshooting was done individually.                                               //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"
#include <cassert>

using namespace std;


LexicalAnalyzer* lexer = new LexicalAnalyzer();
std::vector<InstructionNode*> instructionList;
std::string addr[1000];
InstructionNode* beginning;
InstructionNode* curr;

int tokIndex(std::string str);

int tokIndex(std::string str) {
    int a = 0;
    while (a < 1000) {
        if (addr[a] == str) {
            return a;
        }
        a++;
    }
    return -1;
}

int tokMemIndex(int target);

int tokMemIndex(int target) {
    int a = 0;
    while (a < 1000) {
        if (mem[a] == target) {
            return a;
        }
        a++;
    }
    return -1;
}

bool primary(int index);

bool primary(int index) {

    Token tokn;
    tokn = lexer->peek(index);
    return tokn.token_type == ID || tokn.token_type == NUM;
}

bool relOp(int index);

bool relOp(int index) {
    Token tokn;
    tokn = lexer->peek(index);
    return (tokn.token_type == GREATER) || (tokn.token_type == LESS) || (tokn.token_type == NOTEQUAL);
}


bool op(int index);

bool op(int index) {
    Token tokn = lexer->peek(index);
    return (tokn.token_type == PLUS) || (tokn.token_type == MINUS) || (tokn.token_type == MULT) || (tokn.token_type == DIV);
}

bool conditions(int index);

bool conditions(int index) {
    bool is_Primary;
    is_Primary = primary(index);

    bool is_Relop;
    is_Relop = relOp(index + 1);

    bool is_Secondary;
    is_Secondary = primary(index + 2);

    return is_Primary && is_Relop && is_Secondary;

}

bool expressions(int index) {
    bool isPrimaryExpr;
    isPrimaryExpr = primary(index);
    bool isOpExpr;
    isOpExpr = op(index + 1);
    bool isSecondaryExpr;
    isSecondaryExpr = primary(index + 2);

    return isPrimaryExpr && isOpExpr && isSecondaryExpr;
}

std::vector<Token> conditionalParser();

std::vector<Token> conditionalParser() {

    std::vector<Token> aCondition;

    if (conditions(1)) {
        Token primary1;
        primary1 = lexer->GetToken();

        aCondition.push_back(primary1);

        Token relop;
        relop = lexer->GetToken();

        aCondition.push_back(relop);

        Token primary2;
        primary2 = lexer->GetToken();

        aCondition.push_back(primary2);

    }

    return aCondition;

}

std::vector<Token> expressionsParser();

std::vector<Token> expressionsParser() {

    std::vector<Token> anExpression;

    if (expressions (1)) {

        Token primary1;
        primary1 = lexer->GetToken();

        anExpression.push_back(primary1);

        Token op;
        op = lexer->GetToken();

        anExpression.push_back(op);

        Token primary2;
        primary2 = lexer->GetToken();

        anExpression.push_back(primary2);

    }
    return anExpression;
}


std::vector<Token> assignParser();

std::vector<Token> assignParser() {

    std::vector<Token> anAssignment;

    InstructionNode* assignmentInstruction;
    assignmentInstruction = new InstructionNode();
    assignmentInstruction->type = ASSIGN;

    Token id;
    id = lexer->GetToken();
    anAssignment.push_back(id);

    Token equal;
    equal = lexer->GetToken();
    anAssignment.push_back(equal);

    Token primary;
    primary = lexer->GetToken();
    anAssignment.push_back(primary);

    int leftHandIdx;
    leftHandIdx = tokIndex(anAssignment[0].lexeme);
    assignmentInstruction->assign_inst.left_hand_side_index = leftHandIdx;

    int op1Idx;
    Token op1Tok;
    op1Tok = anAssignment[2];

    if (op1Tok.token_type == ID) {
        op1Idx = tokIndex(op1Tok.lexeme);
    }
    else {
        int op1V;
        op1V = std::stoi(op1Tok.lexeme);

        mem[next_available] = op1V;
        addr[next_available] = op1Tok.lexeme;
        next_available = next_available + 1;

        op1Idx = tokIndex(op1Tok.lexeme);
    }

    assignmentInstruction->assign_inst.operand1_index = op1Idx;

    if (lexer->peek(1).token_type != SEMICOLON) {
        Token op;
        op = lexer->GetToken();

        anAssignment.push_back(op);

        Token primary2;
        primary2 = lexer->GetToken();
        anAssignment.push_back(primary2);

        ArithmeticOperatorType arithType;
        Token op1Token = anAssignment[3];

        switch (op1Token.token_type) {
        case PLUS:
            arithType = OPERATOR_PLUS;
            break;
        case MINUS:
            arithType = OPERATOR_MINUS;
            break;
        case MULT:
            arithType = OPERATOR_MULT;
            break;
        case DIV:
            arithType = OPERATOR_DIV;
            break;
        default:
            OPERATOR_NONE;
            break;
        }

        int op2Idx;

        Token op2Tok;
        op2Tok = anAssignment[4];

        if (op2Tok.token_type != ID) {
            int op2Val = std::stoi(op2Tok.lexeme);

            mem[next_available] = op2Val;
            addr[next_available] = op2Tok.lexeme;
            next_available = next_available + 1;
        }
        op2Idx = tokIndex(op2Tok.lexeme);

        assignmentInstruction->assign_inst.op = arithType;
        assignmentInstruction->assign_inst.operand2_index = op2Idx;

    }
    else {
        assignmentInstruction->assign_inst.op = OPERATOR_NONE;
    }

    lexer->GetToken();

    curr->next = assignmentInstruction;
    assignmentInstruction->next = NULL;
    curr = assignmentInstruction;

    return anAssignment;
}

void output();

void output() {

    Token outputToken;
    outputToken = lexer->GetToken();
    if (outputToken.token_type != OUTPUT) {
        return;
    }

    Token idToken;
    idToken = lexer->GetToken();
    if (idToken.token_type != ID) {
        return;
    }

    InstructionNode* output;
    output = new InstructionNode();

    output->type = OUT;
    output->output_inst.var_index = tokIndex(idToken.lexeme);

    curr->next = output;
    curr = output;

    Token semicolonToken;
    semicolonToken = lexer->GetToken();
    if (semicolonToken.token_type != SEMICOLON) {
        return;
    }
}

void input();

void input() {

    Token inputToken;
    inputToken = lexer->GetToken();
    if (inputToken.token_type != INPUT) {
        return;
    }

    Token idToken;
    idToken = lexer->GetToken();
    if (idToken.token_type != ID) {
        return;
    }

    InstructionNode* input;
    input = new InstructionNode();

    input->type = IN;
    input->input_inst.var_index = tokIndex(idToken.lexeme);
    curr->next = input;
    curr = input;

    Token semicolonToken;
    semicolonToken = lexer->GetToken();
    if (semicolonToken.token_type != SEMICOLON) {
        return;
    }

}
void body();
void statement();
void pSwitch();
void pFor();
void pInputs();
void idList();
void varSection();
void program();

void pWhile();

void pWhile() {

    Token nxtToken = lexer->peek(1);

    if (nxtToken.token_type == WHILE) {
        lexer->GetToken();
        std::vector<Token> condition;
        condition = conditionalParser();
        Token condi0 = condition[0];

        if (condition.size() != 0) {

            InstructionNode* jmp;
            jmp = new InstructionNode();

            jmp->type = CJMP;
            jmp->cjmp_inst.operand1_index = tokIndex(condi0.lexeme);


            ConditionalOperatorType conditionalOpT;
            Token condi1;
            condi1 = condition[1];

            switch (condi1.token_type) {
            case GREATER:
                conditionalOpT = CONDITION_GREATER;
                break;
            case LESS:
                conditionalOpT = CONDITION_LESS;
                break;
            default:
                conditionalOpT = CONDITION_NOTEQUAL;
                break;
            }


            int conditionalOpTIdx;
            Token condi2;
            condi2 = condition[2];

            if (condi2.token_type == ID) {
                conditionalOpTIdx = tokIndex(condi2.lexeme);
               
            }
            else {
                int cop2Val = std::stoi(condi2.lexeme);
                mem[next_available] = cop2Val;
                addr[next_available] = condi2.lexeme;
                next_available = next_available + 1;

                conditionalOpTIdx = next_available - 1;
            }

            jmp->cjmp_inst.condition_op = conditionalOpT;
            jmp->cjmp_inst.operand2_index = conditionalOpTIdx;

            curr->next = jmp;
            jmp->next = NULL;
            curr = jmp;

            body();

            curr->next = jmp;

            InstructionNode* noop;
            noop = new InstructionNode();

            noop->type = NOOP;

            jmp->cjmp_inst.target = noop;

            noop->next = NULL;

            curr = noop;

        }
    }

}

void pIf();

void pIf() {

    lexer->GetToken();

    std::vector<Token> condition;
    condition = conditionalParser();

    Token condi0;
    condi0 = condition[0];

    if (condition.size() != 0) {
        InstructionNode* jmp;
        jmp = new InstructionNode();

        jmp->type = CJMP;
        jmp->cjmp_inst.operand1_index = tokIndex(condition[0].lexeme);

        ConditionalOperatorType cop;
        Token condi1 = condition[1];

        switch (condi1.token_type) {
        case GREATER:
            cop = CONDITION_GREATER;
            break;
        case LESS:
            cop = CONDITION_LESS;
            break;
        default:
            cop = CONDITION_NOTEQUAL;
            break;
        }

        Token condi2;
        condi2 = condition[2];

        jmp->cjmp_inst.condition_op = cop;
        jmp->cjmp_inst.operand2_index = tokIndex(condi2.lexeme);

        jmp->next = NULL;
        curr->next = jmp;

        curr = jmp;

        body();

        InstructionNode* noop;
        noop = new InstructionNode();

        noop->type = NOOP;
        noop->next = NULL;
        curr->next = noop;
        jmp->cjmp_inst.target = noop;

        curr = noop;

    }

}

void stateList();

void stateList() {
    Token nxtToken;
    nxtToken = lexer->peek(1);

    while (nxtToken.token_type != END_OF_FILE && nxtToken.token_type != RBRACE) {
        statement();
        nxtToken = lexer->peek(1);
    }
}


void statement() {
    Token nxtToken;
    nxtToken = lexer->peek(1);

    switch (nxtToken.token_type) {
    case WHILE:
        pWhile();
        break;
    case IF:
        pIf();
        break;
    case SWITCH:
        pSwitch();
        break;
    case FOR:
        pFor();
        break;
    case INPUT:
        input();
        break;
    case OUTPUT:
        output();
        break;
    default:
        assignParser();
        break;
    }
}


void body() {

    lexer->GetToken();

    stateList();

    lexer->GetToken();

}

void listCase(Token switchId);

void listCase(Token switchId) {

    InstructionNode* endSMethod;
    endSMethod = new InstructionNode();
    endSMethod->type = NOOP;

    do {
        Token nxtToken;
        nxtToken = lexer->peek(1);

        if (nxtToken.token_type != DEFAULT) {

            lexer->GetToken();
            Token op2;
            op2 = lexer->GetToken();

            int op2Idx;

            int op2VInt;
            op2VInt = std::stoi(op2.lexeme);
            mem[next_available] = op2VInt;
            addr[next_available] = op2.lexeme;
            next_available = next_available + 1;

            op2Idx = tokIndex(op2.lexeme);

            lexer->GetToken();

            InstructionNode* jmp;
            jmp = new InstructionNode();
            jmp->type = CJMP;

            jmp->cjmp_inst.operand1_index = tokIndex(switchId.lexeme);
            jmp->cjmp_inst.operand2_index = op2Idx;
            jmp->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

            curr->next = jmp;
            curr = jmp;

            body();

            InstructionNode* noop;
            noop = new InstructionNode();

            noop->type = NOOP;
            noop->next = NULL;

            jmp->cjmp_inst.target = jmp->next;
            jmp->next = noop;
            curr->next = endSMethod;
            curr = noop;
            
        }
        else {
            lexer->GetToken();

            lexer->GetToken();

            body();
            curr->next = endSMethod;
        }

    } while (lexer->peek(1).token_type != RBRACE);

    curr = endSMethod;

}

void pSwitch() {

    assert(lexer->GetToken().token_type == SWITCH);

    Token switchID;
    switchID = lexer->GetToken();
    assert(switchID.token_type == ID);

    assert(lexer->GetToken().token_type == LBRACE);

    listCase(switchID);

    assert(lexer->GetToken().token_type == RBRACE);


}



void pFor() {

    lexer->GetToken();

    lexer->GetToken();

    assignParser();

    InstructionNode* assignment1st;
    assignment1st = curr;

    std::vector<Token> condition;
    condition = conditionalParser();

    lexer->GetToken();


    assignParser();

    InstructionNode* assignment2nd;
    assignment2nd = curr;

    lexer->GetToken();

    InstructionNode* jmp;
    jmp = new InstructionNode();

    jmp->type = CJMP;
    jmp->cjmp_inst.operand1_index = tokIndex(condition[0].lexeme);

    ConditionalOperatorType conditionOpT;
    Token condi1 = condition[1];

    switch (condi1.token_type) {
    case GREATER:
         conditionOpT = CONDITION_GREATER;
        break;
    case LESS:
        conditionOpT = CONDITION_LESS;
        break;
    default:
        conditionOpT = CONDITION_NOTEQUAL;
        break;
    }
    jmp->cjmp_inst.condition_op = conditionOpT;

    int conditionOpTIdx;
    Token condi2 = condition[2];

    if (condi2.token_type == ID) {

        conditionOpTIdx = tokIndex(condi2.lexeme);

    }
    else {
        int conditionOpT2Value;
        conditionOpT2Value = std::stoi(condi2.lexeme);
        mem[next_available] = conditionOpT2Value;
        addr[next_available] = condi2.lexeme;
        conditionOpTIdx = next_available;
        next_available = next_available + 1;
    }

    jmp->cjmp_inst.operand2_index = conditionOpTIdx;


    assignment1st->next = jmp;
    jmp->next = NULL;
    curr = jmp;

    body();

    curr->next = assignment2nd;
    assignment2nd->next = jmp;

    InstructionNode* noop;
    noop = new InstructionNode();

    noop->type = NOOP;
    noop->next = NULL;

    jmp->cjmp_inst.target = noop;
    curr = noop;

}

void pInputs() {

    for (auto token = lexer->GetToken(); token.token_type != END_OF_FILE; token = lexer->GetToken()) {
        inputs.push_back(std::stoi(token.lexeme));
    }

}

void idList() {

    next_available = 0;
    Token tokn;
    tokn = lexer->GetToken();

    while (tokn.token_type != SEMICOLON) {
        if (tokn.token_type == COMMA) {
            tokn = lexer->GetToken();
            continue;
        }
        addr[next_available] = tokn.lexeme;
        mem[next_available] = 0;
        next_available = next_available + 1;
        tokn = lexer->GetToken();
    }
}

void varSection() {

    idList();
}

void program() {

    InstructionNode* noop;
    noop = new InstructionNode;

    noop->type = NOOP;
    curr = noop;
    beginning = curr;

    varSection();
    body();
    pInputs();
}

struct InstructionNode* parse_generate_intermediate_representation() {

    program();

    return beginning;
}
