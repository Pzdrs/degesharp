D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]

%{
#include "ds.tab.h"
#include "inc/debug.h"

int check_type();

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

// https://stackoverflow.com/a/36328890/12498149 - multiline comment regexp
%}

%%
"//".*                              { _("COMMENT"); }
"/*"[^*]*"*"([^/][^*]*"*"[^/])*"/"  { _("COMMENT_BLOCK"); }

jakoby                        { _("DECLARE"); return DECLARE; }
dyghej                        { _("PRINT");}
cokdyz                        { _("IF");}
jinak                         { _("ELSE");}
a			                  { _("AND");             return AND_OP; }
nebo			              { _("OR");              return OR_OP; }
je                            { _("EQUAL");           return EQ_OP; }
neni                          { _("NOT_EQUAL");       return NE_OP; }
nene                          { _("NOT");             return NOT_OP; }
true                          { _("TRUE_LITERAL"); yylval.bool_val = 1; return TRUE_LITERAL; }
false                         { _("FALSE_LITERAL"); yylval.bool_val = 0; return FALSE_LITERAL; }

{L}({L}|{D})*		           { return check_type(); }

([1-9][0-9_]*|0)              { _("CONSTANT_INT"); yylval.num_val = atoi(yytext); return CONSTANT;}
0[xX]{H}+                     { _("CONSTANT_HEX"); yylval.num_val = strtol(yytext, NULL, 0); return CONSTANT; }

\"(\\.|[^\\"])*\"	          { _("STRING_LITERAL"); yylval.str_val = process_string(yytext); return STRING_LITERAL; }

"++"			            { _("INCREMENT");       return INC_OP; }
"--"			            { _("DECREMENT");       return DEC_OP; }

">="                          { _("GREAT_OR_EQUAL");  return GE_OP; }
"<="                          { _("LESS_OR_EQUAL");   return LE_OP; }

";"                           { _("SEMICOLON");     return ';'; }
":"                           { _("COLON");        return ':'; }

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
"?"                           { _("QUESTION_MARK");        return '?'; }
.   {}
%%

#define MATCH_TYPE(name, token) if (strcmp(yytext, name) == 0) { _(#token); return token; }

int check_type() {
    /*
    When no keyword is matched, we check for a valid type.
    If the type is valid, we return the corresponding token.

    If the type is not valid, we return IDENTIFIER.
    */
    MATCH_TYPE("str", TYPE_STRING);
    MATCH_TYPE("int", TYPE_INT);
    MATCH_TYPE("bool", TYPE_BOOL);

    printf("IDENTIFIER(%s) ", yytext);
    yylval.str_val = strdup(yytext);
    return IDENTIFIER;
}