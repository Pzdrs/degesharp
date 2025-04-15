D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]

%{
#include "ds.tab.h"
#include "inc/debug.h"
%}

%%
jakoby                        { _("DECLARE"); return DECLARE; }
dyghej                        { _("PRINT");}
cokdyz                        { _("IF");}
jinak                         { _("ELSE");}
a			                  { _("AND");             return AND_OP; }
nebo			              { _("OR");              return OR_OP; }
je                            { _("EQUAL");           return EQ_OP; }
neni                          { _("NOT_EQUAL");       return NE_OP; }
nene                          { _("NOT");             return '!'; }
([1-9][0-9_]*|0)              { _("INTEGER"); yylval.num = atoi(yytext); return INTEGER;}
0[xX]{H}+                     { _("HEX_INTEGER"); yylval.num = strtol(yytext, NULL, 0); return INTEGER; }
{L}({L}|{D})*                 { _("IDENTIFIER"); yylval.str = strdup(yytext); return IDENTIFIER; }

"++"			            { _("INCREMENT");       return INC_OP; }
"--"			            { _("DECREMENT");       return DEC_OP; }

">="                          { _("GREAT_OR_EQUAL");  return GE_OP; }
"<="                          { _("LESS_OR_EQUAL");   return LE_OP; }

";"                           { _("SEMICOLON");       return ';'; }
"="                           { _("ASSIGN");          return '='; }
">"                           { _("GREATER_THAN");    return '>'; }
"<"                           { _("LESS_THAN");       return '<'; }
"("                           { _("OPEN_PAREN");      return '('; }
")"                           { _("CLOSE_PAREN");     return ')'; }
"{"                           { _("OPEN_BRACE");      return '{'; }
"}"                           { _("CLOSE_BRACE");     return '}'; }
"+"                           { _("PLUS");            return '+'; }
"-"                           { _("MINUS");           return '-'; }
"*"                           { _("ASTERISK");        return '*'; }
"/"                           { _("SLASH");           return '/'; }
"^"                           { _("CARET");           return '^'; }

.   {}
%%