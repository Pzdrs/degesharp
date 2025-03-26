%{
#include "ds.tab.h"
void _(const char* message);
%}

%%
;                             {_("SEMICOLON ");}
jakoby                        {_("DECLARE ");}
dyghej                        {_("PRINT ");}
cokdyz                        {_("IF ");}
jinak                         {_("ELSE ");}
([1-9][0-9_]*|0)              {yylval = atoi(yytext); _("NUMBER ");}
[a-zA-Z_][a-zA-Z0-9_]*        {_("IDENTIFIER ");}
"=="                          {_("EQUALS ");};
"!="                          {_("NOT_EQUALS ");};
">="                          {_("GREAT_OR_EQUALS ");};
"<="                          {_("LESS_OR_EQUALS ");};
">"                           {_("GREATER_THAN ");};
"<"                           {_("LESS_THAN ");};

=                             {_("ASSIGN ");};

"-"                           {_("MINUSOP ");}
"{"                           {_("LBR ");}
"}"                           {_("RBR ");}
"+"                           {_("PLUSOP ");}
"*"                           {_("MULTOP ");}
"/"                           {_("DIVOP ");}
"("                           {_("LB ");}
")"                           {_("RB ");}

%%
int main() {
      yylex();
}
int yywrap() {}

void _(const char* message) {
      printf("%s", message);
}