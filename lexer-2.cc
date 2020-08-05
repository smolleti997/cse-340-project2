
#include <cctype>
#include <istream>
#include <vector>
#include <stack>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <iostream>
#include "lexer.h"
#include "inputbuf.h"
#include "inputbuf.cc"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE", "EQUAL", "COLON", "COMMA",
     "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR" 
     // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

struct NodeValue{   //values of symbol table 
char* name;
char* scope;
int access;
};

struct symbolTable{   //symbol table
NodeValue* val;
symbolTable* prev;
symbolTable* next;
};


struct symbolTable* Node; // node is created
struct symbolTable* lastNode;
struct symbolTable* newNode;
char* sScope = "::";
int saccess = 0;

struct store {
string lhs;
string rhs;
char* scope1;
char* scope2;
};
vector< struct store> storeList;

void printAssignment(Token tok1, Token tok2){
struct symbolTable* temp = newNode;
struct symbolTable* temp1 = newNode;
char* scope1 = (char*)malloc(sizeof(char)*20);      
char* scope2 = (char*)malloc(sizeof(char)*20);

while(temp != NULL){
   if(temp->val->name == tok1.lexeme){
   if(temp->val->access == 2 && sScope != temp->val->scope){
        temp= temp->prev;
        continue;
    }else{
        scope1 = temp->val->scope;
        break;
    }
   }
   temp= temp->prev;
   
}
while(temp1 != NULL){
   if(temp1->val-> name == tok2.lexeme){
    if(temp1->val->access == 2 && sScope != temp1->val->scope){
    temp1 = temp1->prev;
    continue;
        
    }else{
        scope2 = temp1->val->scope;
        break;
    }
   }
   
   temp1 = temp1->prev;
   
}
if(strlen(scope1) == 0) {
 scope1 = new char[2];
 scope1[0] = '?';
 scope1[1] = '\0';
}
if(strlen(scope2) == 0){
scope2 = new char[2];
scope2[0] = '?';
scope2[1] = '\0';
} 
struct store temp_node;
temp_node.lhs = tok1.lexeme;
temp_node.rhs = tok2.lexeme;
temp_node.scope1 = scope1;
temp_node.scope2 = scope2;
storeList.push_back(temp_node);
cout << temp_node.scope1;
if(temp_node.scope1 != "::") 
        cout << "."; 
    cout<< temp_node.lhs << " = " << temp_node.scope2;
    if(temp_node.scope2 != "::") 
        cout<<".";
    cout<< temp_node.rhs << endl;

}

void syntaxError()
{
    cout<<"Syntax Error"<<endl;
    exit(1);
}

void deleteNode(char* lexeme){
  if(Node == NULL){
  return;
  }
   while(newNode->val->scope == lexeme){   
   lastNode->next = NULL;
   newNode ->prev = NULL;
   if(lastNode->prev != NULL){
   lastNode=lastNode->prev;
   newNode=lastNode->next;
   newNode->next=NULL;
   return;
   }
   else if(lastNode == newNode){
   lastNode = NULL;
   newNode = NULL;
   return;
   }
   else{
   lastNode->next = NULL;
   newNode ->prev = NULL;
   newNode=lastNode;
   lastNode->next =NULL;
   }
   }
    sScope = newNode->val->scope;
   
}

void addNodeInTheStart(char* lexeme)
{
 Node = (symbolTable*) malloc (sizeof(symbolTable));
 struct NodeValue* newval = (NodeValue*) malloc (sizeof(NodeValue));
 Node ->val = newval;
 Node-> next = NULL;   
 Node-> prev = NULL;

 int len = strlen(lexeme) ; 
 Node->val->name = new char[len+1];
 strcpy(Node->val->name,lexeme);
 Node->val->name[len] = '\0'; 
 Node->val->scope = sScope;
 Node->val->access = saccess; 
 
 newNode = Node;
 lastNode = Node; 
}

void addNodeInTheEnd(char* lexeme)
{
 lastNode = Node; // create another pointer to struct temp1 which will point to top of the Node
 while (lastNode->next != NULL){ 
 lastNode = lastNode-> next;
 }
 newNode = (symbolTable*) malloc (sizeof(symbolTable));
 struct NodeValue* newval = (NodeValue*) malloc(sizeof(NodeValue));
 newNode -> val = newval;
 newNode -> next = NULL;
 newNode -> prev = lastNode;
 lastNode ->next = newNode; 
 int len = strlen(lexeme) ;
 newNode -> val-> name = new char[len+1];
 strcpy(newNode ->val->name, lexeme);
 newNode -> val -> name[len] = '\0';
 newNode -> val -> scope = sScope;
 newNode -> val -> access = saccess;
}

 void addNode(char* lexeme){ 
 if(Node == NULL){ 
    addNodeInTheStart(lexeme);
 }
 else { 
    addNodeInTheEnd(lexeme);
 }
 
}
//*************************************************


LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}
 LexicalAnalyzer lexer;
 Token t1;
 Token t2;
 Token t3;

void LexicalAnalyzer::parse_program(void){
 t1 = lexer.GetToken();
 // since FIRST(program ) = ID
 if(t1.token_type == ID) {
   t2 = lexer.GetToken();
   if(t2.token_type == COMMA || t2.token_type == SEMICOLON){
   lexer.UngetToken(t2);
   lexer.UngetToken(t1);
   // Program is global_vars scope
   lexer.parse_global_vars();
   lexer.parse_scope();
   
   }
   else if(t2.token_type == LBRACE){
   lexer.UngetToken(t2);
   lexer.UngetToken(t1);
   // global_vars is epsilon
   lexer.parse_scope();
   }
   else {
   syntaxError();
   }
 } 
 else{
    syntaxError();
  }
}

int LexicalAnalyzer::parse_scope(void){
t1 = lexer.GetToken();
 if(t1.token_type == ID){
 string slexeme = t1.lexeme; 
   sScope = const_cast<char *>(slexeme.c_str());  
   t1= lexer.GetToken();   
  if(t1.token_type == LBRACE){   
   lexer.parse_public_vars();
   lexer.parse_private_vars();
   lexer.parse_stmt_list();
   t1 = lexer.GetToken();
    if(t1.token_type == RBRACE){
    deleteNode(sScope);
    t1=lexer.GetToken();
    // scope is id lbrace public_vars private_vars stmt_List rbrace
       if(t1.token_type ==END_OF_FILE){
       deleteNode(sScope);
       }
       else{
       UngetToken(t1);
       }
    return 0; 
     
    }
    else {
   syntaxError();
    }
   }
  else{
  cout<< "Syntax Error"<<endl;
  exit(1);
  }
 }
 else{
syntaxError();
 }
return 0; 
}

int LexicalAnalyzer::parse_global_vars(void){
t1 = lexer.GetToken();
if(t1.token_type == ID){
 lexer.UngetToken(t1);
 lexer.parse_var_list();
 t1 = lexer.GetToken();
   if(t1.token_type ==SEMICOLON){
   // global_vars is var_list semicolon
     }
   else {
   syntaxError();
   }   
 }
else {
syntaxError();
}
return 0;
}

int LexicalAnalyzer::parse_private_vars(void){
 t1=lexer.GetToken();
 if(t1.token_type == PRIVATE){
 saccess = 2;
  t1 = lexer.GetToken();
   if(t1.token_type == COLON){
    t1 =lexer.GetToken();
    if(t1.token_type == ID){
     lexer.UngetToken(t1);
     lexer.parse_var_list();
     t1 = lexer.GetToken();
     if(t1.token_type == SEMICOLON){
     // private_vars is private colon var_list semicolon
     }
     else{
     syntaxError();
     }
     }
    else{
    syntaxError();
    }
   }
   else{
   syntaxError();
   }
  
 }
 // FOLLOW Od private_vars is id
 else if(t1.token_type == ID) {
 lexer.UngetToken(t1);
 // private_vars is epsilon

 }
 else{
 syntaxError();
 }
 return 0;
}

int LexicalAnalyzer::parse_public_vars(void){
t1 = lexer.GetToken();
 if(t1.token_type == PUBLIC){
  saccess = 1;
  t1 = lexer.GetToken();
   if(t1.token_type == COLON){
    t1 = lexer.GetToken(); 
    if(t1.token_type == ID){
    lexer.UngetToken(t1);
    lexer.parse_var_list();
    // public _vars is public colon var_list semicolon
    t1 = lexer.GetToken();
      if(t1.token_type == SEMICOLON){
      }
      else{
      syntaxError();
      }
     }
    else{
    syntaxError();
     }
    }
   else{
   syntaxError();
    }
   }
   // since FOLLOW of public_vars is private or id 
  else if(t1.token_type == PRIVATE || t1.token_type == ID){
  lexer.UngetToken(t1);
  // when public is epsilon
   }
  else{
  syntaxError();
  }
  return 0;
}


int LexicalAnalyzer::parse_var_list(void){
  t1 = lexer.GetToken();
  char* lexeme = (char*) malloc (sizeof (t1.lexeme)); 
  memcpy(lexeme,(t1.lexeme).c_str(),sizeof(t1)); 
  addNode(lexeme); 
  
  symbolTable* temp1 = Node; 
  while(temp1 != NULL){
   temp1 = temp1 ->next; 
  }
  if(t1.token_type ==ID){
    t1 = lexer.GetToken();
     if(t1.token_type == COMMA){    
     
       lexer.parse_var_list();
       // var_list is id comma var_list
       return 0;        
       }


// FOLLOW of var_list has semicolon
     else if(t1.token_type == SEMICOLON){
       UngetToken(t1);
       return 0;
       }
     else{
      syntaxError();
     }
  }
  else {
   syntaxError();
  }
   return 0;
}

int LexicalAnalyzer::parse_stmt(void){

  t1 = lexer.GetToken();
  if(t1.token_type == ID){
    t2 = lexer.GetToken();
     if(t2.token_type == EQUAL){     
      t3 = lexer.GetToken();
       if(t3.token_type == ID) {
        printAssignment(t1, t3);
        t1 = lexer.GetToken();
          if(t1.token_type == SEMICOLON){
          return 0;
          
          // stmt is id equal id semicolon
          }
          else{
          syntaxError();
          }
       }
       else {
       syntaxError();
       }
      }
     else if(t2.token_type == LBRACE){
     
     // stmt can be scope
     // stmt is id lbrace public_vars private_vars stmt_list rbrace
   
     sScope = const_cast<char*>((t1.lexeme).c_str()); 
     lexer.UngetToken(t2);
     lexer.UngetToken(t1);
     lexer.parse_scope();    
     
     // stmt is scope 
     }
     
     else {
     syntaxError();
     }
  }
  
  else{
  syntaxError();
  }
  return 0;
}

int LexicalAnalyzer::parse_stmt_list(void){
 t1 = lexer.GetToken();
   if(t1.token_type == ID){
   // FIRST of stmt_list has stmt 
   // stmt has id 
     lexer.UngetToken(t1);
      lexer.parse_stmt();
       t2 = lexer.GetToken();
       if(t2.token_type == ID){
       lexer.UngetToken(t2);
       lexer.parse_stmt_list();
       // stmt is stmt stmt_list
       return 0;
       }
       else if(t2.token_type == RBRACE){       
       lexer.UngetToken(t2);       
       return 0;
       }  
       
       else {
       syntaxError();
       }
    }
    
    else {
    syntaxError();
    }
    return 0;
}
bool LexicalAnalyzer::SkipComment(){
 
  char c;
  bool comment_encountered = false;
    input.GetChar(c);
  if(input.EndOfInput()){
  // if end of input then bool value of variable is retained 
  input.UngetChar(c);
  return comment_encountered;
  }
     if(c=='/'){
    input.GetChar(c);
         if(c=='/'){
         // if '/'/' is encountered , it is recognised as comment 
         comment_encountered = true;
         
           while (c != '\n'){
           // comment is seen till '/''n' is observed
           comment_encountered = true;
           input.GetChar(c);
           }
         line_no +=1;
         SkipComment();      
         }
         else{
         comment_encountered = false;
         exit(1);
         }
      }
     else {
     input.UngetChar(c);    
     return comment_encountered;
     }
  return comment_encountered;
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
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
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
        if (IsKeyword(tmp.lexeme)){          
            tmp.token_type = FindKeywordIndex(tmp.lexeme);           
            }
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

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
    SkipComment();
    SkipSpace();
    
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
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
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        
        default:
            if (isdigit(c)){
              input.UngetChar(c);
              tmp.token_type = ERROR;
              return tmp;
             }            
            else if (isalpha(c)) {
            input.UngetChar(c);
            return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
            tmp.token_type = END_OF_FILE;
               
            else{
            tmp.token_type = ERROR;
            }

            return tmp;
    }
}

int main()
{
 
    lexer.parse_program(); 
    //printlist();
   
}
