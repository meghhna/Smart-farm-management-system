#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

using namespace std;

class Crop {
private:
    string name;
    double area;
    string season;
    double yieldPerAcre;
    static int cropCount;

protected:
    void writeCommonData(ostream& out) const {
        out << quoted(name) << ' ' << area << ' ' << quoted(season) << ' ' << yieldPerAcre;
    }

public:
    Crop() : name("Unnamed"), area(1.0), season("Unknown"), yieldPerAcre(0.0) {
        ++cropCount;
    }

    Crop(const string& n, double a, const string& s, double y)
        : name(n), area(a), season(s), yieldPerAcre(y) {
        if (n.empty() || a <= 0 || y < 0)
            throw invalid_argument("Crop name must not be empty, area must be positive, and yield must be non-negative.");
        ++cropCount;
    }

    virtual ~Crop() { --cropCount; }

    string getName() const { return name; }
    double getArea() const { return area; }
    string getSeason() const { return season; }
    double getYieldPerAcre() const { return yieldPerAcre; }

    virtual double totalYield() const {
        return area * yieldPerAcre;
    }

    virtual void display() const {
        cout << "\nCrop: " << name
             << "\nArea: " << area << " acres"
             << "\nSeason: " << season
             << "\nYield per acre: " << yieldPerAcre
             << "\nTotal yield: " << totalYield() << '\n';
    }

    // Each derived class writes its own type-specific data.
    virtual void save(ofstream& out) const = 0;

    bool operator<(const Crop& other) const {
        return totalYield() < other.totalYield();
    }

    static int getCropCount() { return cropCount; }
};

int Crop::cropCount = 0;

class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() = default;
};

class CerealCrop : public Crop, public Advisory {
private:
    string fertilizerType;

public:
    CerealCrop() : Crop(), fertilizerType("General") {}

    CerealCrop(const string& n, double a, const string& s, double y)
        : Crop(n, a, s, y), fertilizerType("General") {}

    CerealCrop(const string& n, double a, const string& s, double y,
               const string& fertilizer)
        : Crop(n, a, s, y), fertilizerType(fertilizer) {}

    void display() const override {
        cout << "\n[ Cereal Crop ]";
        Crop::display();
        cout << "Fertilizer: " << fertilizerType << '\n';
    }

    void giveAdvice() const override {
        cout << "Advice for " << getName()
             << ": Use a balanced fertilizer such as " << fertilizerType
             << ", rotate crops, and monitor weeds and pests regularly.\n";
    }

    void save(ofstream& out) const override {
        out << "CEREAL ";
        writeCommonData(out);
        out << ' ' << quoted(fertilizerType) << '\n';
    }
};

class VegetableCrop : public Crop, public Advisory {
private:
    int irrigationFrequency; // number of watering sessions per week

public:
    VegetableCrop() : Crop(), irrigationFrequency(3) {}

    VegetableCrop(const string& n, double a, const string& s, double y)
        : Crop(n, a, s, y), irrigationFrequency(3) {}

    VegetableCrop(const string& n, double a, const string& s, double y, int frequency)
        : Crop(n, a, s, y), irrigationFrequency(frequency) {
        if (frequency < 0)
            throw invalid_argument("Irrigation frequency cannot be negative.");
    }

    void display() const override {
        cout << "\n[ Vegetable Crop ]";
        Crop::display();
        cout << "Irrigation frequency: " << irrigationFrequency
             << " times per week\n";
    }

    void giveAdvice() const override {
        cout << "Advice for " << getName()
             << ": Water about " << irrigationFrequency
             << " times per week, adjust for rainfall and soil moisture, "
             << "and inspect leaves for pests.\n";
    }

    void save(ofstream& out) const override {
        out << "VEGETABLE ";
        writeCommonData(out);
        out << ' ' << irrigationFrequency << '\n';
    }
};

double readDouble(const string& prompt, double minimum, bool allowEqual = true) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        stringstream ss(line);
        double value;
        char extra;
        if (ss >> value && !(ss >> extra) &&
            (allowEqual ? value >= minimum : value > minimum)) {
            return value;
        }
        cout << "Invalid number. Please try again.\n";
    }
}

int readInt(const string& prompt, int minimum) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minimum)
            return value;
        cout << "Invalid whole number. Please try again.\n";
    }
}

string readText(const string& prompt) {
    while (true) {
        cout << prompt;
        string value;
        getline(cin, value);
        if (!value.empty())
            return value;
        cout << "This field cannot be empty.\n";
    }
}

void addCrop(vector<unique_ptr<Crop>>& crops) {
    cout << "\n1. Cereal crop\n2. Vegetable crop\n";
    int type = readInt("Choose crop type: ", 1);
    if (type != 1 && type != 2) {
        cout << "Choose 1 or 2.\n";
        return;
    }

    string name = readText("Crop name: ");
    double area = readDouble("Cultivated area (acres): ", 0, false);
    string season = readText("Growing season: ");
    double yield = readDouble("Expected yield per acre: ", 0);

    if (type == 1) {
        string fertilizer = readText("Fertilizer type: ");
        crops.push_back(make_unique<CerealCrop>(name, area, season, yield, fertilizer));
    } else {
        int frequency = readInt("Irrigation sessions per week: ", 0);
        crops.push_back(make_unique<VegetableCrop>(name, area, season, yield, frequency));
    }
    cout << "Crop added successfully.\n";
}

void displayCrops(const vector<unique_ptr<Crop>>& crops) {
    if (crops.empty()) {
        cout << "No crop records available.\n";
        return;
    }
    cout << fixed << setprecision(2);
    for (const auto& crop : crops)
        crop->display();
    cout << "\nTotal registered crops: " << Crop::getCropCount() << '\n';
}

void showAdvice(const vector<unique_ptr<Crop>>& crops) {
    if (crops.empty()) {
        cout << "No crops available for advice.\n";
        return;
    }
    for (const auto& crop : crops) {
        // Runtime polymorphism through the abstract Advisory interface.
        const Advisory* advisory = dynamic_cast<const Advisory*>(crop.get());
        if (advisory)
            advisory->giveAdvice();
    }
}

void compareCrops(const vector<unique_ptr<Crop>>& crops) {
    if (crops.size() < 2) {
        cout << "Add at least two crops before comparing.\n";
        return;
    }
    size_t first = static_cast<size_t>(readInt("First crop number (starting at 1): ", 1) - 1);
    size_t second = static_cast<size_t>(readInt("Second crop number (starting at 1): ", 1) - 1);

    if (first >= crops.size() || second >= crops.size() || first == second) {
        cout << "Invalid crop numbers. Select two different existing records.\n";
        return;
    }

    const Crop& a = *crops[first];
    const Crop& b = *crops[second];
    if (a < b)
        cout << b.getName() << " has the higher total yield ("
             << b.totalYield() << ").\n";
    else if (b < a)
        cout << a.getName() << " has the higher total yield ("
             << a.totalYield() << ").\n";
    else
        cout << "Both crops have the same total yield.\n";
}

void saveCrops(const vector<unique_ptr<Crop>>& crops,
               const string& filename = "crops.txt") {
    ofstream out(filename);
    if (!out)
        throw runtime_error("Could not open " + filename + " for writing.");
    for (const auto& crop : crops)
        crop->save(out);
    cout << crops.size() << " crop record(s) saved to " << filename << ".\n";
}

void loadCrops(vector<unique_ptr<Crop>>& crops,
               const string& filename = "crops.txt") {
    ifstream in(filename);
    if (!in)
        throw runtime_error("Could not open " + filename + " for reading.");

    vector<unique_ptr<Crop>> loaded;
    string type;
    while (in >> type) {
        string name, season;
        double area, yield;
        if (!(in >> quoted(name) >> area >> quoted(season) >> yield))
            throw runtime_error("Invalid or incomplete crop data in file.");

        if (type == "CEREAL") {
            string fertilizer;
            if (!(in >> quoted(fertilizer)))
                throw runtime_error("Invalid cereal crop record.");
            loaded.push_back(make_unique<CerealCrop>(
                name, area, season, yield, fertilizer));
        } else if (type == "VEGETABLE") {
            int frequency;
            if (!(in >> frequency))
                throw runtime_error("Invalid vegetable crop record.");
            loaded.push_back(make_unique<VegetableCrop>(
                name, area, season, yield, frequency));
        } else {
            throw runtime_error("Unknown crop type in file: " + type);
        }
    }

    if (!in.eof())
        throw runtime_error("Error while reading crop data.");
    crops = move(loaded);
    cout << crops.size() << " crop record(s) loaded from " << filename << ".\n";
}

int main() {
    vector<unique_ptr<Crop>> crops;
    bool running = true;

    cout << "=====================================\n"
         << "      SMART FARM MANAGEMENT SYSTEM\n"
         << "=====================================\n";

    while (running) {
        cout << "\n1. Add crop\n"
             << "2. Display all crops and total yield\n"
             << "3. Show farming advice\n"
             << "4. Compare two crops by total yield\n"
             << "5. Save crop data to file\n"
             << "6. Load crop data from file\n"
             << "7. Show number of crops\n"
             << "0. Exit\n";

        int choice = readInt("Enter choice: ", 0);
        try {
            switch (choice) {
                case 1: addCrop(crops); break;
                case 2: displayCrops(crops); break;
                case 3: showAdvice(crops); break;
                case 4: compareCrops(crops); break;
                case 5: saveCrops(crops); break;
                case 6: loadCrops(crops); break;
                case 7: cout << "Current crop count: "
                             << Crop::getCropCount() << '\n'; break;
                case 0: running = false; break;
                default: cout << "Invalid menu option.\n";
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << '\n';
        }
    }

    cout << "Farm data session ended. Goodbye.\n";
    return 0;
}
