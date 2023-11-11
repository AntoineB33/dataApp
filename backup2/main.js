const { app, BrowserWindow, globalShortcut } = require('electron');

function createWindow() {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 1920, // Set your desired initial width
    height: 1080, // Set your desired initial height
    fullscreen: true, // Start in fullscreen mode
    autoHideMenuBar: true, // Hide the menu bar at the top
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    }
  });

  // Load your HTML file.
  mainWindow.loadFile('index.html');

  // Capture console messages and display them in the terminal
  mainWindow.webContents.on('console-message', (event, level, message, line, sourceId) => {
    console.log(`[${level}] (line ${line}): ${message}`);
  });

  // Register a global shortcut to quit the application when 'Esc' is pressed.
  globalShortcut.register('Esc', () => {
    app.quit();
  });
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
    // Handle IPC messages from the renderer process
    ipcMain.on('send-args-to-html', (event, args) => {
      mainWindow.webContents.send('receive-args', args);
    });
  }
});
