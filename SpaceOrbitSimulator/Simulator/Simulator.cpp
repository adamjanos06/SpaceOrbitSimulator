#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//#include <cctype>
//#include <cmath>
//#include <limits>

std::string toLower(const std::string& s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

void clearPreviousLine()
{
    std::cout << "\x1b[1A\x1b[2K";
}


std::string promptExact(const std::string &question, const std::vector<std::string> &allowed)
{
    std::cout << question;

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::string low = toLower(input);

        bool ok = false;
        for (auto& a : allowed)
        {
            if (low == toLower(a))
            {
                ok = true;
                break;
            }
        }

        if (ok)
            return input;
        else
        {
            clearPreviousLine();
            std::cout << question;
        }
    }
}

int promptInt(const std::string &question, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max())
{
    while (true)
    {
        std::cout << question;
        std::string line;
        std::getline(std::cin, line);
        try {
            int val = std::stoi(line);
            if (val < minVal || val > maxVal) {
                clearPreviousLine();
                continue;
            }
            return val;
        }
        catch (...) {
            clearPreviousLine();
            continue;
        }
    }
}

void debugMode(const std::string &name, const std::string &str = "No pro or con")
{
    if (toLower(name) == "debug")
        std::cout << "//" << str << "//\n";
}

int main()
{
    std::cout << "\n\t\t\t\t\t*-*-*-*-* ASTRA SIMULATOR *-*-*-*-*\n";
    
    // NAME
    std::string rocketName;
    std::cout << "Name your Rocket (eg. Apollo11): ";
    std::getline(std::cin, rocketName);
    
    // MISSION
    std::string mission;
    debugMode(rocketName);
    std::cout << "Name your Mission (eg. cargo, satellite, orbit): ";
    getline(std::cin, mission);
    
    // LAUNCH SITE
    debugMode(rocketName, "1 = +% storm, 2 = +% fuel useage");
    std::string launchSiteChoice = promptExact("Pick a Launch site (1 for Florida, 2 for Russia): ", {"1", "2"});
    std::string launchSite = (toLower(launchSiteChoice) == "1") ? "Florida" : "Russia";

    // MATERIAL
    debugMode(rocketName, "2 = -% failure");
    std::string materialChoice = promptExact("Pick your rocket's material (1 for stainless steel, 2 for titanium): ", { "1", "2" });
    std::string material = (toLower(materialChoice) == "1") ? "stainless steel" : "titanium";

    // STAGE COUNT
    int stageCount = promptInt("Enter your stage count (1-5): ", 1, 5);

    // STAGE SEPARATION
    std::string stageSepChoice = promptExact("Manual or automatic stage separation? (m/a): ", { "m", "a" });
    bool isAutomaticSeparation = (toLower(stageSepChoice) == "a");

    
    std::cout << "\n\nYour Rocket's details:\n";
    std::cout << rocketName << "\n";
    std::cout << "Mission: " << mission << "\n";
    std::cout << "Launch site: " << launchSite << "\n";
    std::cout << "Material: " << material << "\n";
    std::cout << "Stages: " << stageCount << "\n";
    std::cout << "Separation: " << (isAutomaticSeparation ? "automatic" : "manual") << "\n\n";
    
    std::cout << "\n*-*-*-*-* End of summary *-*-*-*-*\n";

    return 0;
}