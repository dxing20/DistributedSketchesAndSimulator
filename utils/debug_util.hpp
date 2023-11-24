// Purpose: Header file for togglable debug printing and execution

#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <fstream>
#include <map>
#include <string>
#include <set>

#define DEBUG true

// Define various debug types
enum class DebugType {
    ROUTING,
    FORWARDING,
};

class DebugManager {
public:
    // Constructor: Initialize file mappings and active debug types
    DebugManager() {
        fileMappings[DebugType::ROUTING] = "logs/routing.log";
        fileMappings[DebugType::FORWARDING] = "logs/forwarding.log";
        // Initialize more file mappings as needed

        // Initialize active debug types as needed !!!!!!here!!!!!! <<<<<<
        activeDebugTypes.insert(DebugType::ROUTING);
        activeDebugTypes.insert(DebugType::FORWARDING);
    }

    // Destructor: Close all open files
    ~DebugManager() {
        for (auto& stream : fileStreams) {
            if (stream.second.is_open()) {
                stream.second.close();
            }
        }
    }

    // Function to log a message
    void log(DebugType type, const std::string& message) {
        if (isTypeActive(type)) {
            // Open the file if not already open
            if (fileStreams[type].is_open() == false) {
                
                fileStreams[type].open(fileMappings[type], std::ios::out);
            }
            // Log the message
            fileStreams[type] << message;
        }
    }

    // Check if a debug type is active
    bool isTypeActive(DebugType type) {
        return activeDebugTypes.find(type) != activeDebugTypes.end();
    }

private:
    std::map<DebugType, std::string> fileMappings;
    std::map<DebugType, std::ofstream> fileStreams;
    std::set<DebugType> activeDebugTypes;
};

// Global instance of DebugManager
inline DebugManager debugManager;

// Debug print macro
#ifdef DEBUG
#define DEBUG_PRINTF(type, format, ...) \
    do { \
        if (DEBUG && debugManager.isTypeActive(type)) { \
            char buffer[256]; \
            snprintf(buffer, sizeof(buffer), format, __VA_ARGS__); \
            debugManager.log(type, buffer); \
        } \
    } while (0)
#define DEBUG_PRINT(type, msg) \
    do { \
        if (DEBUG && debugManager.isTypeActive(type)) { \
            debugManager.log(type, msg); \
        } \
    } while (0)
#else
#define DEBUG_PRINTF(type, format, ...) 
#define DEBUG_PRINT(type, msg)
#endif



// Optional code execution macro
#ifdef DEBUG
#define DEBUG_EXEC(type, code) \
    do { \
        if (DEBUG && debugManager.isTypeActive(type)) { \
            code; \
        } \
    } while (0)
#else
#define DEBUG_EXEC(type, code)
#endif

#endif // DEBUG_UTILS_H
