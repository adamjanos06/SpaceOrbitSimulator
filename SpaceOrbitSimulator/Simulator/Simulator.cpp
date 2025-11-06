#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
//#include <cctype>
#include <cmath>
//#include <limits>

const double PI = 3.141592653589793;
const double g0 = 9.80665;
const double AIR_DENSITY = 1.225; // kg/m^3 at sea level
const double ESCAPE_VELOCITY = 11186;
const double SOLID_FUEL_DENSITY = 1.675; // typically 1.5 - 1.85 g/mL

struct EngineSpec {
    std::string name;
    double thrustSea; // thrust in Newtons
    double thrustVac;
    double ispSea; // specific impulse in seconds
    double ispVac;
    double throatArea; // m^2
};

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

void clearScreen() {
    std::cout << "\x1b[2J\x1b[H";
}

void debugMode(const std::string& name, const std::string& str = "No pro or con")
{
    if (toLower(name) == "debug")
        std::cout << "//" << str << "//\n";
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

double promptDouble(const std::string& question, double minVal = std::numeric_limits<double>::min(), double maxVal = std::numeric_limits<double>::max())
{
    while (true)
    {
        std::cout << question;
        std::string line;
        std::getline(std::cin, line);
        try {
            double val = std::stod(line);
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

void promptValidatedMasses(double& dryMass, double& fuelMass, double& payloadMass)
{
    const int DRY_MIN = 500, DRY_MAX = 200000;
    const int FUEL_MIN = 2000, FUEL_MAX = 3000000;
    const int PAYLOAD_MIN = 50, PAYLOAD_MAX = 150000;

    while (true)
    {
        std::cout << '\n';
        dryMass = static_cast<double>(promptInt("Dry mass (empty rocket) in kg (eg. 22000): ", DRY_MIN, DRY_MAX));
        fuelMass = static_cast<double>(promptInt("Total fuel mass in kg (eg. 395000): ", FUEL_MIN, FUEL_MAX));
        payloadMass = static_cast<double>(promptInt("Payload mass in kg (eg. 22800): ", PAYLOAD_MIN, PAYLOAD_MAX));

        double totalMass = dryMass + fuelMass + payloadMass;
        double fuelRatio = fuelMass / totalMass;
        double payloadRatio = payloadMass / totalMass;

        bool ok = true;
        bool fuelLow = false, fuelHigh = false, payloadHigh = false;
        if (fuelRatio < 0.70) { ok = false; fuelLow = true; }
        if (fuelRatio > 0.90) { ok = false; fuelHigh = true; }
        if (payloadRatio > 0.06) { ok = false; payloadHigh = true; }

        if (ok)
            break;
        else
        {
            int errorCount = 0;
            if (fuelLow || fuelHigh) ++errorCount;
            if (payloadHigh) ++errorCount;

            std::cout << "\n";
            if (fuelLow)
                std::cout << "[!] Fuel ratio too LOW (" << (fuelRatio * 100.0) << "%). Needs 70%-90% of total.\n";
            else if (fuelHigh)
                std::cout << "[!] Fuel ratio too HIGH (" << (fuelRatio * 100.0) << "%). Needs 70%-90% of total.\n";
            if (payloadHigh)
                std::cout << "[!] Payload ratio too HIGH (" << (payloadRatio * 100.0) << "%). Must be <= 6% of total.\n";
            
            std::cout << "Press Enter to try again...";
            
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            int clearCount = errorCount + 6;
            for (int i = 0; i < clearCount; ++i) {
                clearPreviousLine();
            }
        }
    }
}

int main()
{
    while (true)
    {
        clearScreen();
        std::cout << "\n\t\t\t\t\t*=*=*=*=* ASTRA SIMULATOR *=*=*=*=*\n";

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
        std::string launchSiteChoice = promptExact("Pick a Launch site (1 for Florida, 2 for Russia): ", { "1", "2" });
        std::string launchSite = (toLower(launchSiteChoice) == "1") ? "Florida" : "Russia";

        // MATERIAL
        debugMode(rocketName, "2 = -% failure");
        std::string materialChoice = promptExact("Pick your rocket's material (1 for stainless steel, 2 for titanium): ", { "1", "2" });
        std::string material = (toLower(materialChoice) == "1") ? "stainless steel" : "titanium";

        // STAGE COUNT
        int stageCount = promptInt("Enter your stage count (1-5): ", 1, 5);

        // STAGE SEPARATION
        bool isAutomaticSeparation;
        if (stageCount > 1)
        {
            std::string stageSepChoice = promptExact("Manual or automatic stage separation? (m/a): ", { "m", "a" });
            isAutomaticSeparation = (toLower(stageSepChoice) == "a");
        }


        std::cout << "\n--- PHYSICAL PROPERTIES ---\n";

        // HEIGHT
        double height = promptInt("Total rocket height in meters (eg. 70): ", 1, 120);

        // DIAMETER
        double diameter = promptDouble("Rocket diameter in meters (eg. 3.7): ", 1.0, 9.0);

        double dryMass = 0.0, fuelMass = 0.0, payloadMass = 0.0;
        promptValidatedMasses(dryMass, fuelMass, payloadMass);

        // ENGINE
        std::vector<EngineSpec> engines = {
            { "F1 (Apollo, Saturn V)", 6770000.0, 7770000.0, 263.0, 304.0, 0.67 },
            { "Merlin (SpaceX, Flacon9)", 845000.0, 981000, 282.0, 311.0, 0.042 },
            { "RD-191 (Russian, Angara)", 1920000, 2090000, 310, 337, 0.046 }
        };

        std::cout << "\nAvailable engines:\n";
        for (size_t i = 0; i < engines.size(); ++i) {
            std::cout << " " << (i + 1) << ") " << engines[i].name
                << " - Thrust at Sea level: " << engines[i].thrustSea / 1e3 << " kN"
                << ", Thrust in Vacuum: " << engines[i].thrustVac / 1e3 << " kN"
                << ", Isp at Sea level: " << engines[i].ispSea << " s"
                << ", Isp in Vacuum: " << engines[i].ispVac << " s"
                << ", Throat area: " << engines[i].throatArea << " m^2\n";
        }

        int choice = promptInt("\nChoose engine (1-3): ", 1, 3);
        EngineSpec engine = engines[choice - 1];


        // OUTPUT
        std::cout << "\n\n--- " << rocketName << "'s details ---\n";
        std::cout << "Mission: " << mission << "\n";
        std::cout << "Launch site: " << launchSite << "\n";
        std::cout << "Material: " << material << "\n";
        std::cout << "Stages: " << stageCount << "\n";
        if (stageCount > 1)
            std::cout << "Separation: " << (isAutomaticSeparation ? "automatic" : "manual") << "\n";

        std::cout << "\n--- Physical Properties ---\n";
        std::cout << "Height: " << height << "\n";
        std::cout << "Diameter: " << diameter << "\n";
        std::cout << "Dry mass: " << dryMass << "\n";
        std::cout << "Fuel mass: " << fuelMass << "\n";
        std::cout << "Payload mass: " << payloadMass << "\n";

        double totalMass = dryMass + fuelMass + payloadMass;
        std::cout << "Total mass: " << totalMass << " kg\n";
        std::cout << "Fuel ratio: " << (fuelMass / totalMass) * 100 << "%\n";
        std::cout << "Payload ratio: " << (payloadMass / totalMass) * 100 << "%\n";
        double radius = diameter / 2;
        double crossSection = PI * radius * radius;
        double volume = crossSection * height;

        std::cout << "\n--- Engine Performance ---\n";
        double VeSea = engine.ispSea * g0;
        std::cout << "Effective exhaust velocity (sea): " << VeSea << " m/s\n";
        double VeVac = engine.ispVac * g0;
        std::cout << "Effective exhaust velocity (vac): " << VeVac << " m/s\n";
        double mdotSea = engine.thrustSea / VeSea;
        std::cout << "Mass flow rate (sea): " << mdotSea << " kg/s\n";
        double mdotVac = engine.thrustVac / VeVac;
        std::cout << "Mass flow rate (vac): " << mdotVac << " kg/s\n";
        double TWR = engine.thrustSea / (totalMass * g0);
        std::cout << "Thrust-to-weight ratio (TWR): " << TWR << "\n";
        double burnTime = fuelMass / mdotSea;
        std::cout << "Burn Time: " << burnTime << "\n";

        std::cout << "\n*=*=*=*=* End of Summary *=*=*=*=*\n";


        //   S   I   M   U   L   A   T   I   O   N


        std::string proceed = promptExact("\nDo you wish to proceed to simulation? (Y/N): ", { "y", "n" });

        if (proceed == "n" || proceed == "N") {
            std::cout << "\nRestarting input...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        // double velocity = 0.0;
        // double deltaV = Ve * ln(totalMass/dryMass);
        // double acceleration = 0.0;

        return 0;
    }
}