const path = require('path');
const { workspace, ExtensionContext } = require('vscode');
const { LanguageClient, TransportKind } = require('vscode-languageclient/node');

let client;

function activate(context) {
    console.log('DegeSharp Language Server activated!');

    const serverModule = context.asAbsolutePath(path.join('.', 'server.js'));
    const serverOptions = {
        run: {
            module: serverModule,
            transport: TransportKind.ipc
        },
        debug: {
            module: serverModule,
            transport: TransportKind.ipc,
            options: { execArgv: ['--nolazy', '--inspect=6009'] }
        }
    };

    const clientOptions = {
        documentSelector: [
            { scheme: 'file', language: 'degesharp' }
        ],
        synchronize: {
            fileEvents: workspace.createFileSystemWatcher('**/*.deges')
        }
    };

    client = new LanguageClient(
        'degesharpLanguageServer',
        'DegeSharp Language Server',
        serverOptions,
        clientOptions
    );

    const disposable = client.start();

    context.subscriptions.push(disposable);
}

// Handle extension deactivation
function deactivate() {
    if (!client) {
        return undefined;
    }
    return client.stop();
}

module.exports = {
    activate,
    deactivate
};