#include "parser.hpp"

void exitError(std::string const &error) {
    std::cout << error << std::endl;
    exit(EXIT_FAILURE);
}

int getDirective(std::string const &token) {
    static const std::array<std::pair<std::string, int>, 17> globalDirectives = {
            std::make_pair("port", Directives::PORT),
            std::make_pair("host", Directives::HOST),
            std::make_pair("server_name", Directives::SERVER_NAME),
            std::make_pair("error_page", Directives::ERROR_PAGE),
            std::make_pair("max_file_size", Directives::MAX_FILE_SIZE),
            std::make_pair("time_out", Directives::TIME_OUT),
            std::make_pair("location", Directives::LOCATION),
            std::make_pair("root", Directives::ROOT),
            std::make_pair("allowed_method", Directives::ALLOWED_METHODS),
            std::make_pair("index", Directives::INDEX),
            std::make_pair("cgi", Directives::CGI),
            std::make_pair("redirect", Directives::REDIRECT),
            std::make_pair("upload_pass", Directives::UPLOAD),
            std::make_pair("auto_index", Directives::AUTO_INDEX),
            std::make_pair("auth_basic", Directives::AUTH_BASIC),
            std::make_pair("]", Directives::LOCATION_END),
            std::make_pair("}", Directives::SERVER_END),
    };

    for (int i = 0; i < globalDirectives.size(); ++i)
        if (token == globalDirectives[i].first)
            return globalDirectives[i].second;
    return e_error::INVALID_DIRECTIVE;
}

template<typename T>
void fillGlobalDirectives(T &field, T const &value, std::string const& directive) {
    if (field != "")
        exitError("Error: Invalid directive: " + directive);
    field = value;
}

std::vector<ServerConfig> performParsing(std::string const& filename) {
    std::vector<ServerConfig> globalConfig;
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in);

    std::string line;
    int i = -1, j = -1;
    bool isLocation = false;
    if (ifs.is_open()) {
        while (std::getline(ifs, line)) {
            if (line.empty())
                continue;

            std::vector<std::string> tokens = Utility::split(line);
            int directive = getDirective(tokens[0]);
            if (tokens[0] == "server")
                directive = -1;

            switch (directive) {
                case -1:
                    globalConfig.push_back(ServerConfig());
                    ++i;
                    j = -1;
                    break;
                case Directives::PORT:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._port, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._port, tokens[1], tokens[0]);
                    break;
                case Directives::HOST:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._host, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._host, tokens[1], tokens[0]);
                    break;
                case Directives::SERVER_NAME:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._server_name, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._server_name, tokens[1], tokens[0]);
                    break;
                case Directives::ERROR_PAGE:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._error_page, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._error_page, tokens[1], tokens[0]);
                    break;
                case Directives::MAX_FILE_SIZE:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._max_file_size, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._max_file_size, tokens[1], tokens[0]);
                    break;
                case Directives::TIME_OUT:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._time_out, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._time_out, tokens[1], tokens[0]);
                    break;
                case Directives::ROOT:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._root, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._root, tokens[1], tokens[0]);
                    break;
                case Directives::ALLOWED_METHODS:
                    if (!isLocation)
                        for (int k = 1; k < tokens.size(); ++k) globalConfig[i]._allowed_method.insert(tokens[k]);
                    else
                        for (int k = 1; k < tokens.size(); ++k)
                            globalConfig[i]._location[j]._allowed_method.insert(tokens[k]);
                    break;
                case Directives::INDEX:
                    if (!isLocation)
                        std::copy(tokens.begin() + 1, tokens.end(), std::back_inserter(globalConfig[i]._index));
                    else std::copy(tokens.begin() + 1, tokens.end(), std::back_inserter(globalConfig[i]._location[j]._index));
                    break;
                case Directives::AUTO_INDEX:
                    if (!isLocation)
                        fillGlobalDirectives(globalConfig[i]._auto_index, tokens[1], tokens[0]);
                    else
                        fillGlobalDirectives(globalConfig[i]._location[j]._auto_index, tokens[1], tokens[0]);
                    break;

                case Directives::LOCATION:
                    if (tokens.size() != 3 || tokens[2] != "[")
                        exitError("erorr near token " + tokens[0]);
                    globalConfig[i]._loc_path = tokens[1];
                    globalConfig[i]._location.push_back(ServerConfig());
                    ++j;
                    isLocation = true;
                    break;
                    //                    if (tokens.size() == 3) globalConfig[tokens[0]] = tokens[1];
                    //                    else if (tokens.size() == 4) std::cout << "size is 3" << std::endl;
                    //                    else exitError("SYNTAX ERROR.");

                    //                case Directives::INVALID:
                    //                    exitError("SYNTAX ERROR.");
                case Directives::LOCATION_END:
                    isLocation = false;
                    break;
                case e_error::INVALID_DIRECTIVE:
                    exitError("Invalid Directive " + tokens[0]);
                default:
                    break;
                    // exitError("Invalid directive found in config file");
            }
            //            std::cout << tokens << std::endl;
        }
    } else
        exitError("wrong config file path");

    // std::cout << globalConfig << std::endl;
    return globalConfig;
}
