D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]

%{
#include "ds.tab.h"
#include "inc/debug.h"

char* process_string(const char* text) {
    if (!text) return NULL;

    int len = strlen(text);
    if (len < 2 || text[0] != '"' || text[len - 1] != '"') {
        fprintf(stderr, "Invalid input: expected string wrapped in quotes\n");
        return NULL;
    }

    // Allocate enough space: worst case, no escapes removed.
    char* result = malloc(len - 1);  // len-2 for removing quotes, +1 for '\0'
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Strip quotes and copy middle part
    char* src = (char*)text + 1;
    char* dest = result;
    char* end = (char*)text + len - 1;

    while (src < end) {
        if (*src == '\\' && (src + 1 < end)) {
            src++;
            switch (*src) {
                case 'n': *dest++ = '\n'; break;
                case 't': *dest++ = '\t'; break;
                case 'r': *dest++ = '\r'; break;
                case '\\': *dest++ = '\\'; break;
                case '"': *dest++ = '"'; break;
                default: *dest++ = *src; break;
            }
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';

    return result;
}

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
([1-9][0-9_]*|0)              { _("CONSTANT"); yylval.num = atoi(yytext); return CONSTANT;}
0[xX]{H}+                     { _("CONSTANT"); yylval.num = strtol(yytext, NULL, 0); return CONSTANT; }
{L}({L}|{D})*                 { _("IDENTIFIER"); yylval.str = strdup(yytext); return IDENTIFIER; }
\"(\\.|[^\\"])*\"	          { _("STRING_LITERAL"); yylval.str = process_string(yytext); return STRING_LITERAL; }

"++"			            { _("INCREMENT");       return INC_OP; }
"--"			            { _("DECREMENT");       return DEC_OP; }

">="                          { _("GREAT_OR_EQUAL");  return GE_OP; }
"<="                          { _("LESS_OR_EQUAL");   return LE_OP; }

";"                           { _("SEMICOLON");     return ';'; }

"="                           { _("ASSIGN");        return '='; }
"+="                          { _("ADD_ASSIGN");    return ADD_ASSIGN; }
"-="                          { _("SUB_ASSIGN");    return SUB_ASSIGN; }
"*="                          { _("MUL_ASSIGN");    return MUL_ASSIGN; }
"/="                          { _("DIV_ASSIGN");    return DIV_ASSIGN; }

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