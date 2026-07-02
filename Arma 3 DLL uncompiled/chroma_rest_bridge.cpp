#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>

#pragma comment(lib, "winhttp.lib")

namespace {
    std::string g_instanceUri;

    std::string Trim(const std::string& value) {
        const char* ws = " \t\r\n";
        const size_t start = value.find_first_not_of(ws);
        if (start == std::string::npos) {
            return "";
        }
        const size_t end = value.find_last_not_of(ws);
        return value.substr(start, end - start + 1);
    }

    std::vector<std::string> Split(const std::string& value, char delimiter) {
        std::vector<std::string> parts;
        std::stringstream stream(value);
        std::string item;
        while (std::getline(stream, item, delimiter)) {
            parts.push_back(item);
        }
        return parts;
    }

    std::string ExtractJsonString(const std::string& body, const std::string& key) {
        const std::string pattern = "\"" + key + "\"";
        const size_t pos = body.find(pattern);
        if (pos == std::string::npos) {
            return "";
        }

        const size_t colon = body.find(':', pos);
        if (colon == std::string::npos) {
            return "";
        }

        const size_t firstQuote = body.find('"', colon + 1);
        if (firstQuote == std::string::npos) {
            return "";
        }
        const size_t secondQuote = body.find('"', firstQuote + 1);
        if (secondQuote == std::string::npos) {
            return "";
        }
        return body.substr(firstQuote + 1, secondQuote - firstQuote - 1);
    }

    bool ParseUri(const std::string& uri, std::string* host, INTERNET_PORT* port, std::string* path) {
        URL_COMPONENTS components = {};
        components.dwStructSize = sizeof(components);

        char hostBuffer[256] = {};
        char pathBuffer[1024] = {};
        components.lpszHostName = hostBuffer;
        components.dwHostNameLength = static_cast<DWORD>(sizeof(hostBuffer));
        components.lpszUrlPath = pathBuffer;
        components.dwUrlPathLength = static_cast<DWORD>(sizeof(pathBuffer));

        if (!WinHttpCrackUrlA(uri.c_str(), static_cast<DWORD>(uri.size()), 0, &components)) {
            return false;
        }

        if (host) {
            *host = hostBuffer;
        }
        if (port) {
            *port = components.nPort;
        }
        if (path) {
            *path = pathBuffer;
        }
        return true;
    }

    std::string SendJsonRequest(const std::string& host, INTERNET_PORT port, const std::string& path, const std::string& method, const std::string& body, std::string* responseOut) {
        HINTERNET session = WinHttpOpenA("a3chroma/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) {
            return "error:winhttp-session";
        }

        HINTERNET connection = WinHttpConnect(session, host.c_str(), port, 0);
        if (!connection) {
            WinHttpCloseHandle(session);
            return "error:winhttp-connection";
        }

        HINTERNET request = WinHttpOpenRequest(connection, method.c_str(), path.c_str(), NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        if (!request) {
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return "error:winhttp-request";
        }

        const wchar_t* headers = L"Content-Type: application/json\r\n";
        const BOOL sent = WinHttpSendRequest(request, headers, -1L,
            const_cast<char*>(body.c_str()), static_cast<DWORD>(body.size()),
            static_cast<DWORD>(body.size()), 0);
        if (!sent) {
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return "error:winhttp-send";
        }

        if (!WinHttpReceiveResponse(request, NULL)) {
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return "error:winhttp-response";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead = 0;
        while (WinHttpReadData(request, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }

        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        if (responseOut) {
            *responseOut = Trim(response);
        }
        return "ok";
    }

    std::string InitializeChroma() {
        const std::string body = "{\"title\":\"a3chroma\",\"description\":\"Arma 3 Chroma bridge\",\"author\":{\"name\":\"a3chroma\",\"contact\":\"https://github.com/tehsaint/a3chroma\"},\"device_supported\":[\"keyboard\"],\"category\":\"game\"}";
        std::string response;
        const std::string result = SendJsonRequest("127.0.0.1", 54235, "/razer/chromasdk", "POST", body, &response);
        if (result != "ok") {
            return result;
        }

        const std::string uri = ExtractJsonString(response, "uri");
        if (uri.empty()) {
            return "error:missing-uri";
        }

        g_instanceUri = uri;
        return "ok";
    }

    std::string TriggerKeyboardEffect() {
        if (g_instanceUri.empty()) {
            const std::string initResult = InitializeChroma();
            if (initResult != "ok") {
                return initResult;
            }
        }

        std::string host;
        INTERNET_PORT port = 0;
        std::string path;
        if (!ParseUri(g_instanceUri, &host, &port, &path)) {
            return "error:parse-uri";
        }

        if (path.empty() || path == "/") {
            path = "/chromasdk/keyboard";
        } else if (path.find("/keyboard") == std::string::npos && path.find("/chromasdk") != std::string::npos) {
            path += "/keyboard";
        } else if (path.find("/keyboard") == std::string::npos) {
            path = "/chromasdk/keyboard";
        }

        const std::string body = "{\"effect\":\"CHROMA_STATIC\",\"param\":{\"color\":16711680}}";
        std::string response;
        const std::string result = SendJsonRequest(host, port, path, "POST", body, &response);
        if (result != "ok") {
            return result;
        }
        return "ok";
    }
}

extern "C" {
    __declspec(dllexport) void __stdcall RVExtensionVersion(char* output, int outputSize) {
        const char* version = "a3chroma-rest-3.0";
        strncpy_s(output, outputSize, version, _TRUNCATE);
    }

    __declspec(dllexport) void __stdcall RVExtension(char* output, int outputSize, const char* function) {
        std::string input = function ? function : "";
        const std::vector<std::string> args = Split(input, '|');
        const std::string command = args.empty() ? "init" : args[0];

        if (command == "init") {
            const std::string response = InitializeChroma();
            strncpy_s(output, outputSize, response.c_str(), _TRUNCATE);
            return;
        }

        if (command == "flash_red_burst") {
            const std::string response = TriggerKeyboardEffect();
            strncpy_s(output, outputSize, response.c_str(), _TRUNCATE);
            return;
        }

        const std::string response = "error:unsupported-command";
        strncpy_s(output, outputSize, response.c_str(), _TRUNCATE);
    }
}
