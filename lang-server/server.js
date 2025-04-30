const { createConnection, TextDocuments, ProposedFeatures, CompletionItem, CompletionItemKind,
    Hover, MarkupContent, MarkupKind, TextDocumentPositionParams, Position, CompletionItemTag,
    InsertTextFormat } = require('vscode-languageserver/node');
const { TextDocument } = require('vscode-languageserver-textdocument');

const connection = createConnection(ProposedFeatures.all);
console.log('DegeSharp server connection created');

const documents = new TextDocuments(TextDocument);

const keywords = [
    'jakoby', 'cokdyz', 'jinak', 'loop', 'tadymas', 'true', 'false', 'str', 'int', 'bool'
];


let symbolTable = {
    variables: new Map(),
    functions: new Map()
};

const snippets = [
    {
        label: 'if',
        insertText: 'if (${1:condition}) {\n\t${2}\n}',
        insertTextFormat: InsertTextFormat.Snippet,
        kind: CompletionItemKind.Snippet,
        detail: 'if statement',
        documentation: {
            kind: MarkupKind.Markdown,
            value: 'Create an if statement'
        }
    },
    {
        label: 'ifelse',
        insertText: 'if (${1:condition}) {\n\t${2}\n} else {\n\t${3}\n}',
        insertTextFormat: InsertTextFormat.Snippet,
        kind: CompletionItemKind.Snippet,
        detail: 'if-else statement',
        documentation: {
            kind: MarkupKind.Markdown,
            value: 'Create an if-else statement'
        }
    },
    {
        label: 'for',
        insertText: 'for (let ${1:i}: number = 0; ${1:i} < ${2:count}; ${1:i}++) {\n\t${3}\n}',
        insertTextFormat: InsertTextFormat.Snippet,
        kind: CompletionItemKind.Snippet,
        detail: 'for loop',
        documentation: {
            kind: MarkupKind.Markdown,
            value: 'Create a for loop'
        }
    },
    {
        label: 'function',
        insertText: 'function ${1:name}(${2:params})-> ${3:returnType} {\n\t${4}\n}',
        insertTextFormat: InsertTextFormat.Snippet,
        kind: CompletionItemKind.Snippet,
        detail: 'function declaration',
        documentation: {
            kind: MarkupKind.Markdown,
            value: 'Create a function'
        }
    },
    {
        label: 'while',
        insertText: 'while (${1:condition}) {\n\t${2}\n}',
        insertTextFormat: InsertTextFormat.Snippet,
        kind: CompletionItemKind.Snippet,
        detail: 'while loop',
        documentation: {
            kind: MarkupKind.Markdown,
            value: 'Create a while loop'
        }
    }
];

connection.onInitialize(() => {
    console.log('Initializing DegeSharp server');
    return {
        capabilities: {
            textDocumentSync: documents.syncKind,
            completionProvider: {
                resolveProvider: true,
                triggerCharacters: ['.', ':', '(']
            },
            hoverProvider: true
        }
    };
});

// Parse document to build symbol table
function parseDocument(document) {
    const text = document.getText();
    const lines = text.split(/\r?\n/);

    symbolTable.variables = new Map();
    symbolTable.functions = new Map();

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];

        const varDeclaration = line.match(/let\s+(\w+)\s*:\s*(string|number|bool)/);
        if (varDeclaration) {
            symbolTable.variables.set(varDeclaration[1], {
                name: varDeclaration[1],
                type: varDeclaration[2],
                line: i
            });
        }

        const funcDeclaration = line.match(/function\s+(\w+)\s*\(/);
        if (funcDeclaration) {
            const paramsPart = line.substring(line.indexOf('(') + 1, line.indexOf(')'));
            const params = paramsPart.split(',').map(p => p.trim()).filter(p => p.length > 0);

            symbolTable.functions.set(funcDeclaration[1], {
                name: funcDeclaration[1],
                params: params,
                line: i
            });
        }
    }
}

documents.onDidChangeContent(change => {
    console.log(`Document changed: ${change.document.uri}`);
    parseDocument(change.document);
});

connection.onCompletion((textDocumentPosition) => {
    console.log(`Completion request at: ${textDocumentPosition.position.line}:${textDocumentPosition.position.character}`);
    const document = documents.get(textDocumentPosition.textDocument.uri);
    if (!document) return [];

    const position = textDocumentPosition.position;
    const line = document.getText({
        start: { line: position.line, character: 0 },
        end: { line: position.line, character: Number.MAX_VALUE }
    });

    const beforeCursor = line.substring(0, position.character);

    // Check different contextual conditions
    const isAfterColon = /let\s+\w+\s*:\s*$/.test(beforeCursor);
    const isInsidePrintParens = /print\s*\(\s*$/.test(beforeCursor) ||
        /print\s*\([^)]*,\s*$/.test(beforeCursor);

    const shouldSuggestSnippets = position.character === 0 ||
        /^\s*$/.test(beforeCursor) ||
        /[{;]\s*$/.test(beforeCursor);

    const completions = [];

    if (shouldSuggestSnippets) {
        snippets.forEach(snippet => {
            completions.push(snippet);
        });
    }

    if (isAfterColon) {
        ['string', 'number', 'bool'].forEach(type => {
            completions.push({
                label: type,
                kind: CompletionItemKind.TypeParameter,
                detail: `Type: ${type}`
            });
        });
    } else if (isInsidePrintParens) {

        symbolTable.variables.forEach(variable => {
            completions.push({
                label: variable.name,
                kind: CompletionItemKind.Variable,
                detail: `${variable.type} variable`,
                documentation: {
                    kind: MarkupKind.Markdown,
                    value: `Variable of type ${variable.type} defined on line ${variable.line + 1}`
                }
            });
        });

        completions.push(
            {
                label: 'true',
                kind: CompletionItemKind.Keyword,
                detail: 'Boolean literal: true'
            },
            {
                label: 'false',
                kind: CompletionItemKind.Keyword,
                detail: 'Boolean literal: false'
            },
            {
                label: '""',
                kind: CompletionItemKind.Text,
                detail: 'Empty string literal',
                insertText: '""',
                command: {
                    title: 'Cursor between quotes',
                    command: 'editor.action.triggerSuggest'
                }
            },
            {
                label: '0',
                kind: CompletionItemKind.Value,
                detail: 'Number literal: 0'
            }
        );

        symbolTable.functions.forEach(func => {
            completions.push({
                label: func.name,
                kind: CompletionItemKind.Function,
                detail: `function ${func.name}(${func.params.join(', ')})`,
                documentation: {
                    kind: MarkupKind.Markdown,
                    value: `Function defined on line ${func.line + 1}`
                },
                insertText: `${func.name}($1)`,
                insertTextFormat: InsertTextFormat.Snippet
            });
        });
    } else {
        // Default context - add all suggestions

        keywords.forEach(keyword => {
            completions.push({
                label: keyword,
                kind: CompletionItemKind.Keyword,
                detail: `Keyword: ${keyword}`,
                documentation: {
                    kind: MarkupKind.Markdown,
                    value: getKeywordDocumentation(keyword)
                }
            });
        });

        symbolTable.variables.forEach(variable => {
            completions.push({
                label: variable.name,
                kind: CompletionItemKind.Variable,
                detail: `${variable.type} variable`,
                documentation: {
                    kind: MarkupKind.Markdown,
                    value: `Variable of type ${variable.type} defined on line ${variable.line + 1}`
                }
            });
        });

        symbolTable.functions.forEach(func => {
            completions.push({
                label: func.name,
                kind: CompletionItemKind.Function,
                detail: `function ${func.name}(${func.params.join(', ')})`,
                documentation: {
                    kind: MarkupKind.Markdown,
                    value: `Function defined on line ${func.line + 1}`
                }
            });
        });

        ['string', 'number', 'bool'].forEach(type => {
            completions.push({
                label: type,
                kind: CompletionItemKind.TypeParameter,
                detail: `Type: ${type}`
            });
        });
    }

    console.log(`Returning ${completions.length} completions`);
    return completions;
});

connection.onCompletionResolve((item) => {
    if (item.kind === CompletionItemKind.Snippet) {
        const snippet = snippets.find(s => s.label === item.label);
        if (snippet) {
            item.documentation = {
                kind: MarkupKind.Markdown,
                value: `${snippet.documentation.value}\n\n**Example usage:**\n\`\`\`\n${snippet.insertText.replace(/\$\{\d+:([^}]*)\}/g, '$1')}\n\`\`\``
            };
        }
    }
    return item;
});

// Handle hover requests
connection.onHover((params) => {
    console.log(`Hover request at: ${params.position.line}:${params.position.character}`);
    const document = documents.get(params.textDocument.uri);
    if (!document) return null;

    const position = params.position;
    const wordRange = getWordRangeAtPosition(document, position);
    if (!wordRange) return null;

    const word = document.getText(wordRange);
    console.log(`Hover word: ${word}`);

    if (keywords.includes(word)) {
        return {
            contents: {
                kind: MarkupKind.Markdown,
                value: getKeywordDocumentation(word)
            }
        };
    }

    if (symbolTable.variables.has(word)) {
        const variable = symbolTable.variables.get(word);
        return {
            contents: {
                kind: MarkupKind.Markdown,
                value: `### Variable: ${variable.name}\nType: \`${variable.type}\`\nDefined on line ${variable.line + 1}`
            }
        };
    }

    if (symbolTable.functions.has(word)) {
        const func = symbolTable.functions.get(word);
        return {
            contents: {
                kind: MarkupKind.Markdown,
                value: `### Function: ${func.name}\nParameters: \`${func.params.join(', ') || 'none'}\`\nDefined on line ${func.line + 1}`
            }
        };
    }

    const snippetMatch = snippets.find(s => s.label === word);
    if (snippetMatch) {
        return {
            contents: {
                kind: MarkupKind.Markdown,
                value: `### Snippet: ${snippetMatch.label}\n${snippetMatch.documentation.value}\n\n**Template:**\n\`\`\`\n${snippetMatch.insertText.replace(/\$\{\d+:([^}]*)\}/g, '$1')}\n\`\`\``
            }
        };
    }

    return null;
});

function getWordRangeAtPosition(document, position) {
    const line = document.getText({
        start: { line: position.line, character: 0 },
        end: { line: position.line, character: Number.MAX_VALUE }
    });

    let startChar = position.character;
    while (startChar > 0 && /[\w]/.test(line[startChar - 1])) {
        startChar--;
    }

    let endChar = position.character;
    while (endChar < line.length && /[\w]/.test(line[endChar])) {
        endChar++;
    }

    if (startChar === endChar) return null;

    return {
        start: { line: position.line, character: startChar },
        end: { line: position.line, character: endChar }
    };
}

function getKeywordDocumentation(keyword) {
    switch (keyword) {
        case 'let':
            return '**let** - Used to let a new variable\n\nExample: `let x: number = 10;`';
        case 'print':
            return '**print** - Prints a value to console\n\nExample: `print(x);`';
        case 'if':
            return '**if** - Conditional statement\n\nExample: `if (x == 10) { print(x); }`';
        case 'else':
            return '**else** - Alternative branch for if statement\n\nExample: `if (x == 10) { print(x); } else { print("not 10"); }`';
        case 'for':
            return '**for** - Loop construct\n\nExample: `for (let i: number = 0; i < 10; i++) { print(i); }`';
        case 'function':
            return '**function** - Define a new function\n\nExample: `function add(a: number, b: number) -> number { return a + b; }`';
        case 'return':
            return '**return** - Return a value from a function\n\nExample: `return x + y;`';
        case 'while':
            return '**while** - Loop construct\n\nExample: `while (x > 0) { x--; }`';
        case 'true':
            return '**true** - Boolean literal representing true';
        case 'false':
            return '**false** - Boolean literal representing false';
        case 'string':
            return '**string** - String data type';
        case 'number':
            return '**number** - Numeric data type';
        case 'bool':
            return '**bool** - Boolean data type (true/false)';
        default:
            return `**${keyword}** - Language keyword`;
    }
}

connection.onDidChangeWatchedFiles(_change => {
    console.log('Watched files changed');
});

documents.listen(connection);

connection.listen();

console.log('DegeSharp server is ready!');