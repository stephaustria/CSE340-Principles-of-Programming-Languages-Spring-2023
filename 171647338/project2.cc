/*
* Copyright (C) Mohsen Zohrevandi, 2017
*               Rida Bazzi 2019
* Do not share this file with anyone
*
* Stephen Austria 1220159551
* Zoe Alinea 1219539724
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <map>
#include <algorithm>
using namespace std;

LexicalAnalyzer la;
vector<Token> tokens;
vector<Token> terminals;
vector<Token> nonTerminals;
map<string, vector<string> > firstSets;
map<string, vector<string> > followSets;
void idList();
void rightHandSide();
void parseRule();
void ruleList();

//error handling
void syntax_error()
{
    cout << "SYNTAX ERROR !!!" << endl;
    exit(0);
}

Token expect(TokenType expects) {
    Token tok = la.GetToken();
    if (tok.token_type != expects) {
        syntax_error();
    }
    return tok;

}

// parser
//struct 
struct Rule {
    Token leftHand;
    vector<Token> rightHand;
};

vector<Rule> rules;
vector<Rule> usefulRules;

//parsing the idlist
void idList(Rule r) {

    Token tok = la.peek(1);
    TokenType tTokenType = tok.token_type;

    if (tTokenType != ID) {
        syntax_error();
    }
    else {
        Token tokn = la.GetToken();
        r.rightHand.push_back(tokn);
        tokens.push_back(tokn);
    }

    tok = la.peek(1);

    if (tok.token_type == ID) {
        idList(r);
        return;
    }
    else if (tok.token_type == STAR) {
        rules.push_back(r);
    }
    else {
        syntax_error();
    }

}


//parsing righthandside
void rightHandSide(Rule r) {

    Token tok = la.peek(1);
    TokenType tTokenType = tok.token_type;

    if (tTokenType == ID) {
        idList(r);
        return;
    }
    else if (tTokenType == STAR) {
        rules.push_back(r);
    }
    else {
        syntax_error();
    }

}

//parsing rule
void parseRule()
{
    Rule r;
    Token tok = la.GetToken();
    TokenType tTokenType = tok.token_type;

    if (tTokenType != ID) {
        syntax_error();
    }

    r.leftHand = tok;
    tokens.push_back(tok);
    expect(ARROW);
    rightHandSide(r);
    expect(STAR);
}

//parsing rule list
void ruleList() {

    parseRule();
    Token tok = la.peek(1);
    TokenType tTokenType = tok.token_type;

    if (tTokenType == ID) {
        ruleList();
    }
    else if (tTokenType == HASH) {
        return;
    }
    else {
        syntax_error();
    }
}

bool ins(vector<Token> vec, Token tok)
{
    Token a;
    for (Token i : vec) {
        a = i;
        if (a.lexeme == tok.lexeme)
            return true;
    }
    return false;
}
// read grammar
void ReadGrammar()
{
    vector<Token> aNonTerm;
    LexicalAnalyzer();

    ruleList();
    expect(HASH);

    int i = 0;
    size_t ruleSize = rules.size();
     while (i < rules.size()) {

        Token tempLeftHand;
        tempLeftHand = rules.at(i).leftHand;

        if (!ins(aNonTerm, tempLeftHand)) {
            aNonTerm.push_back(tempLeftHand);
    }
    else {
        string temp;
        }
        i++;
    }

    for (int i = 0; i < rules.size(); i++) {

        Token tokn = rules.at(i).leftHand;
        if (!ins(aNonTerm, tokn) && ins(nonTerminals, tokn)) {
            int a;
        }
        else if (ins(aNonTerm, tokn) && !ins(nonTerminals, tokn)) {
            nonTerminals.push_back(tokn);
        }

        int j = 0;
        while (j < rules.at(i).rightHand.size()) {
            tokn = rules.at(i).rightHand.at(j);
            if (ins(aNonTerm, tokn)) {
                if (!ins(nonTerminals, tokn)) {

                    nonTerminals.push_back(tokn);
                }
                else {

                string temp;
                }
            }
            else if (!ins(terminals, tokn)) {
                terminals.push_back(tokn);
            }
            j++;
        }
    }

}

// Task 1
void printTerminalsAndNoneTerminals()
{
    for (Token i : terminals) {
        cout << i.lexeme;
    }
    for (Token i : nonTerminals) {
        cout << i.lexeme;
    }
}

// Task 2
void RemoveUselessSymbols(bool out)
{
    map<string, bool> genT;

    genT["#"] = true;
    for (Token i : terminals) {

        genT[i.lexeme] = true;
    }
    genT["$"] = false;
    for (Token i : nonTerminals) {

        genT[i.lexeme] = false;
    }

    bool changes = true;

    while (changes) {
        changes = false;

        for (Rule i : rules) {

            Rule r = i;

            bool rEmpty;
            rEmpty = r.rightHand.empty();

            string lLex;
            lLex = r.leftHand.lexeme;

            if (rEmpty && !genT[lLex]) {
                genT[lLex] = true;
                changes = true;
            }
            else {
                bool aGen = true;
                vector<Token> rRight = r.rightHand;

                for (Token j : rRight) {

                    string jLex;
                    jLex = j.lexeme;

                    if (!genT[jLex]) {
                        aGen = false;
                        break;
                    }
                }
                if (aGen && !genT[lLex]) {
                    genT[lLex] = true;
                    changes = true;
                }
            }
        }
    }

    vector<Rule> genR;
    for (Rule i : rules) {
        Rule r = i;

        string lLex;
        lLex = r.leftHand.lexeme;

        if (genT[lLex]) {

            bool aGen = true;
            vector<Token> rRight = r.rightHand;

            for (Token j : rRight) {

                string jLex;
                jLex = j.lexeme;

                if (!genT[jLex]) {
                    aGen = false;
                    break;
                }
            }
            if (aGen) {
                genR.push_back(r);
            }
        }
    }

    map<string, bool> reach;

    for (Rule i : genR) {
        Rule r = i;

        string lLex;
        lLex = r.leftHand.lexeme;

        vector<Token> rRight;
        rRight = r.rightHand;

        reach[lLex] = false;

        for (Token j : rRight) {
            string jLex = j.lexeme;
            reach[jLex] = false;
        }
    }

    string leftRules = rules[0].leftHand.lexeme;
    reach[leftRules] = true;

    changes = true;
    while (changes) {
        changes = false;
        for (Rule i : genR) {
            Rule r = i;

            string lLex;
            lLex = r.leftHand.lexeme;

            vector<Token> rRight;
            rRight = r.rightHand;

            if (reach[lLex]) {
                for (Token j : rRight) {
                    string jLex = j.lexeme;
                    if (!reach[jLex]) {
                        reach[jLex] = true;
                        changes = true;
                    }
                }
            }
        }
    }
    vector<Rule> reachMe;
    for (Rule i : genR) {

        Rule r = i;
        string lLex;
        lLex = r.leftHand.lexeme;

        if (reach[lLex]) {
            Rule temp = r;
            reachMe.push_back(r);
            usefulRules.push_back(temp);
        }
    }
    if (out) {
        for (Rule i : reachMe) {
            Rule r = i;

            string lLex;
            lLex = r.leftHand.lexeme;

            cout << lLex << " -> ";

            bool rEmpty;
            rEmpty = r.rightHand.empty();

            vector<Token> rRight;
            rRight = r.rightHand;

            if (rEmpty) {
                cout << "#";
            }
            for (Token j : rRight) {
                string jLex = j.lexeme;
                cout << jLex << " ";
            }
            cout << "\n";
        }
    }
}

// Task 3
void CalculateFirstSets(bool out)
{
    bool change = true;
    firstSets["#"].push_back("#");

    for(Token a : terminals) {
        string adder = a.lexeme;
        firstSets[adder].push_back(adder);
    }

    while(change) {
        change = false;
        int iterator = 0;

        while(iterator < rules.size()) {
            Rule currentRule = rules.at(iterator);
            string cLL = currentRule.leftHand.lexeme;
            vector<Token> cRR = currentRule.rightHand;
            
            auto res = find(firstSets[cLL].begin(), firstSets[cLL].end(), "#"); //if looking for epsilon USING FIND HERE
            if(res == end(firstSets[cLL]) && cRR.empty()) {
                firstSets[cLL].insert(firstSets[cLL].begin(), "#");
                change = true;
                break;
            }

            int iterator2 = 0;
            while(iterator2 < cRR.size()) {
                auto search = find(firstSets[cLL].begin(), firstSets[cLL].end(), cRR.at(0).lexeme); 
                if(search == end(firstSets[cLL]) && !ins(nonTerminals, cRR.at(0))) { 
                        firstSets[cLL].push_back(cRR.at(0).lexeme);
                        change = true;
                        break;
                }
                else {
                    vector<string> store = firstSets[cRR.at(0).lexeme];
                    
                    int iterator3 = 0;
                    while(iterator3 < store.size()) {
                        if(store.at(iterator3) == "#") {
                            store.erase(store.begin() + iterator3);
                        }
                        iterator3 = iterator3 + 1;
                    }

                    int iterator4 = 0;
                    while(iterator4 < store.size()) {
                        auto search2 = find(firstSets[cLL].begin(), firstSets[cLL].end(), store.at(iterator4)); 
                        if(search2 == end(firstSets[cLL])) {
                            firstSets[cLL].push_back(store.at(iterator4));
                            change = true;
                            break;
                        }
                        iterator4 = iterator4 + 1;
                    }
                }
                // 4
                bool epsilon = false;
                int iterator5 = 0;
                while(iterator5 < cRR.size()) {
                    auto res = find(firstSets[cRR.at(iterator5).lexeme].begin(), firstSets[cRR.at(iterator5).lexeme].end(), "#"); 
                    if(res != end(firstSets[cRR.at(iterator5).lexeme])) {
                        if(iterator5 + 1 != cRR.size()) {
                            epsilon = false;
                        }
                        else if(iterator5 + 1 == cRR.size()) {
                            epsilon = true;
                        }
                        if(iterator5 + 1 < cRR.size()) {
                            vector<string> store = firstSets[cRR.at(iterator5 + 1).lexeme];
                            
                            int iterator6 = 0;
                            while(iterator6 < store.size()) {
                                if(store.at(iterator6) == "#") {
                                    store.erase(store.begin() + iterator6);
                                }
                                iterator6 = iterator6 + 1;
                            }

                            int iterator7 = 0;
                            while(iterator7 < store.size()) {
                                auto search2 = find(firstSets[cLL].begin(), firstSets[cLL].end(), store.at(iterator7)); 
                                if(search2 == end(firstSets[cLL])) {
                                    firstSets[cLL].push_back(store.at(iterator7));
                                    change = true;
                                    break;
                                }
                                iterator7 = iterator7 + 1;
                            }
                        }
                    }
                    else {
                        break;
                    }
                    iterator5 = iterator5 + 1;
                }

                // 5
                auto res = find(firstSets[cLL].begin(), firstSets[cLL].end(), "#"); 
                if(res == end(firstSets[cLL]) && epsilon) {
                    firstSets[cLL].insert(firstSets[cLL].begin(), "#");
                    change = true;
                    break;
                }
                iterator2 = iterator2 + 1;
            }
            iterator = iterator + 1;
        }
    }

    //Sorting
    for(Token i : nonTerminals) {

        vector<int> firstV;
        vector<int> secondV;
        int start; int end2;

        for(string k : firstSets[i.lexeme]) {
            int otherIterator = 0;
            while(otherIterator < terminals.size()) {
                if(k == terminals.at(otherIterator).lexeme) {
                    firstV.push_back(otherIterator);
                }
                otherIterator = otherIterator + 1;
            }
        }

        start = 40000;
        end2 = -1;

        while(0 < firstV.size()) {
            int secondIterator = 0;
            while(secondIterator < firstV.size()) {
                if(start > firstV.at(secondIterator)) {
                    start = firstV.at(secondIterator);
                    end2 = secondIterator;
                }
                secondIterator = secondIterator + 1;
            }
            firstV.erase(firstV.begin() + end2);
            secondV.push_back(start);
            start = 40000;
        }
        string atToken = i.lexeme;
        auto search3 = find(firstSets[atToken].begin(), firstSets[atToken].end(), "#"); 
        if(search3 != end(firstSets[atToken])) {
            firstSets[atToken].clear();
            firstSets[atToken].push_back("#");

            int sortIterator = 0;
            while(sortIterator < secondV.size()) {
                firstSets[atToken].push_back(terminals.at(secondV.at(sortIterator)).lexeme);
                sortIterator = sortIterator + 1;
            }
        }
        else {
            firstSets[atToken].clear();
            int elseIterator = 0;
            while(elseIterator < secondV.size()) {
                firstSets[atToken].push_back(terminals.at(secondV.at(elseIterator)).lexeme);
                elseIterator = elseIterator + 1;
            }
        }
    }
    //Printing
    if(out) {
        for(Token v : nonTerminals) {
            cout << "FIRST(" << v.lexeme << ") = { ";
            for(string q : firstSets[v.lexeme]) {
                if(q == firstSets[v.lexeme][0]) {
                    cout << q;
                }
                else if(q != firstSets[v.lexeme][0]) {
                    cout << ", " << q;
                }
            }
            cout << " }\n";
        }
    }
}

int main(int argc, char* argv[])
{
    int task;


    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }


    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */


    task = atoi(argv[1]);

    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // as described in project 2 presentation file

    switch (task) {
    case 1: printTerminalsAndNoneTerminals();
        break;


    case 2: RemoveUselessSymbols(true);
        break;


    case 3: CalculateFirstSets(true);
        break;


    case 4: //CalculateFollowSets(true);
        break;


    case 5: //CheckIfGrammarHasPredictiveParser();
        break;


    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}