#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class JsonHelper {
public:
    static double getValeur(const string& json, const string& cle) {
        size_t pos = json.find("\"" + cle + "\"");
        if (pos == string::npos) return 0;

        pos = json.find(":", pos);
        if (pos == string::npos) return 0;

        size_t debut = pos + 1;
        size_t fin = json.find_first_of(",}", debut);
        
        string valeurStr = json.substr(debut, fin - debut);
        
        try {
            return stod(valeurStr);
        } catch (...) {
            return 0.0;
        }
    }
};

class ModuleFerme {
protected:
    string nomModule;

public:
    ModuleFerme(string n) : nomModule(n) {}
    virtual ~ModuleFerme() {}

    virtual string getStatusJson() const = 0;
    
    string getNom() const { return nomModule; }
};

class Reservoir : public ModuleFerme {
private:
    double niveauActuel;
    double capaciteMax;

public:
    Reservoir(double cap) : ModuleFerme("Irrigation"), niveauActuel(cap), capaciteMax(cap) {}

    bool operator-=(double quantite) {
        if (quantite > 0 && quantite <= niveauActuel) {
            niveauActuel -= quantite;
            return true;
        }
        return false;
    }

    void operator+=(double quantite) {
        niveauActuel += quantite;
        if (niveauActuel > capaciteMax) {
            niveauActuel = capaciteMax;
        }
    }

    friend ostream& operator<<(ostream& os, const Reservoir& r) {
        os << "[Module Eau] " << r.niveauActuel << "/" << r.capaciteMax << " L";
        return os;
    }

    string getStatusJson() const override {
        stringstream ss;
        ss << "\"eauNiveau\":" << niveauActuel << ", \"eauCapacite\":" << capaciteMax;
        return ss.str();
    }
    
    void remplirTotalement() { niveauActuel = capaciteMax; }
    double getNiveau() const { return niveauActuel; }
    double getCapacite() const { return capaciteMax; }
};

class Serre : public ModuleFerme {
private:
    double temperature;
    double humidite;

public:
    Serre() : ModuleFerme("Climat"), temperature(20.0), humidite(55.0) {}

    bool setClimat(double t, double h) {
        if (t >= -10 && t <= 60 && h >= 0 && h <= 100) {
            this->temperature = t;
            this->humidite = h;
            return true;
        }
        return false;
    }

    friend ostream& operator<<(ostream& os, const Serre& s) {
        os << "[Module Serre] " << s.temperature << "C, " << s.humidite << "% Hum";
        return os;
    }

    string getStatusJson() const override {
        stringstream ss;
        ss << "\"temperature\":" << temperature << ", \"humidite\":" << humidite;
        return ss.str();
    }
    
    double getTemp() const { return temperature; }
    double getHum() const { return humidite; }
};

class Silo : public ModuleFerme {
private:
    double stockGrain;
    double consommationJour;
    int joursOperation;

public:
    Silo(double stockInit) : ModuleFerme("Alimentation"), 
        stockGrain(stockInit), consommationJour(50.0), joursOperation(0) {}

    bool operator-=(double q) {
        if (stockGrain >= q) {
            stockGrain -= q;
            joursOperation++;
            return true;
        }
        return false;
    }

    void operator+=(double q) {
        stockGrain += q;
    }

    string getStatusJson() const override {
        stringstream ss;
        int autonomieRestante = (stockGrain > 0) ? (int)(stockGrain / consommationJour) : 0;
        
        ss << "\"stockNourriture\":" << stockGrain << ",";
        ss << "\"joursEcoules\":" << joursOperation << ",";
        ss << "\"consommationQuotidienne\":" << consommationJour << ",";
        ss << "\"autonomie\":" << autonomieRestante;
        return ss.str();
    }

    double getStock() const { return stockGrain; }
    double getConso() const { return consommationJour; }
    int getJours() const { return joursOperation; }
};

class GestionnaireFerme {
private:
    Reservoir* monReservoir;
    Serre* maSerre;
    Silo* monSilo;

public:
    GestionnaireFerme() {
        cout << ">>> Initialisation des systemes de la ferme..." << endl;
        monReservoir = new Reservoir(1000.0);
        maSerre = new Serre();
        monSilo = new Silo(500.0);
    }

    ~GestionnaireFerme() {
        delete monReservoir;
        delete maSerre;
        delete monSilo;
    }

    string getGlobalStats() {
        stringstream ss;
        ss << "{";
        ss << monReservoir->getStatusJson() << ",";
        ss << maSerre->getStatusJson() << ",";
        ss << monSilo->getStatusJson() << ",";
        
        double coutTotal = monSilo->getJours() * monSilo->getConso() * 3.5;
        ss << "\"coutTotal\":" << coutTotal;
        ss << "}";
        
        cout << "[LOG] Demande de status global recue." << endl;
        return ss.str();
    }

    string traiterArrosage(double quantite) {
        stringstream ss;
        ss << "{";
        
        if (*monReservoir -= quantite) {
            ss << "\"success\":true, \"message\":\"Arrosage OK\", \"niveau\":" << monReservoir->getNiveau();
            cout << "[ACTION] " << *monReservoir << " (-" << quantite << "L)" << endl;
        } else {
            ss << "\"success\":false, \"message\":\"Niveau eau insuffisant\"";
            cerr << "[ERREUR] Pas assez d'eau !" << endl;
        }
        ss << "}";
        return ss.str();
    }

    string traiterRemplissage() {
        monReservoir->remplirTotalement();
        cout << "[ACTION] Remplissage reservoir effectue." << endl;
        return "{\"success\":true, \"message\":\"Reservoir plein\"}";
    }

    string traiterClimat(double t, double h) {
        stringstream ss;
        if (maSerre->setClimat(t, h)) {
            ss << "{\"success\":true, \"message\":\"Climat mis a jour\"}";
            cout << "[ACTION] " << *maSerre << endl;
        } else {
            ss << "{\"success\":false, \"message\":\"Valeurs climat invalides\"}";
        }
        return ss.str();
    }

    string traiterDistribution() {
        if (*monSilo -= monSilo->getConso()) {
            cout << "[ACTION] Nourriture distribuee. Reste: " << monSilo->getStock() << "kg" << endl;
            return "{\"success\":true, \"message\":\"Distribution OK\"}";
        }
        return "{\"success\":false, \"message\":\"Stock vide !\"}";
    }

    string traiterReappro(double q) {
        *monSilo += q;
        cout << "[ACTION] Stock ajoute: +" << q << "kg" << endl;
        return "{\"success\":true, \"message\":\"Stock ajoute\"}";
    }

    string genererRapport() {
        ofstream fichier("rapport_ferme.txt");
        if (!fichier.is_open()) return "{\"success\":false}";

        time_t now = time(0);
        char* dt = ctime(&now);

        fichier << "=== RAPPORT D'ACTIVITE FERME ===" << endl;
        fichier << "Genere le : " << dt << endl;
        
        fichier << "1. ETAT IRRIGATION" << endl;
        fichier << "   Niveau actuel : " << monReservoir->getNiveau() << " Litres" << endl;
        fichier << "   Capacite max  : " << monReservoir->getCapacite() << " Litres" << endl << endl;
        
        fichier << "2. ETAT CLIMATIQUE" << endl;
        fichier << "   Temperature : " << maSerre->getTemp() << " C" << endl;
        fichier << "   Humidite    : " << maSerre->getHum() << " %" << endl << endl;
        
        fichier << "3. STOCK ALIMENTATION" << endl;
        fichier << "   Stock restant : " << monSilo->getStock() << " kg" << endl;
        fichier << "   Jours d'activite : " << monSilo->getJours() << endl;

        fichier.close();
        cout << "[Fichier] Rapport sauvegarde sur le disque." << endl;
        return "{\"success\":true}";
    }
};

class SimpleServer {
private:
    SOCKET mainSocket;
    GestionnaireFerme& logiqueFerme;

    void sendResponse(SOCKET client, string content, string contentType = "application/json") {
        stringstream header;
        header << "HTTP/1.1 200 OK\r\n";
        header << "Content-Type: " << contentType << "\r\n";
        header << "Access-Control-Allow-Origin: *\r\n";
        header << "Content-Length: " << content.length() << "\r\n";
        header << "Connection: close\r\n\r\n";
        header << content;
        
        string fullResponse = header.str();
        send(client, fullResponse.c_str(), (int)fullResponse.length(), 0);
    }

public:
    SimpleServer(GestionnaireFerme& fermeRef) : logiqueFerme(fermeRef), mainSocket(INVALID_SOCKET) {}

    bool init(int port) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Erreur WSAStartup" << endl;
            return false;
        }

        mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (mainSocket == INVALID_SOCKET) {
            cerr << "Erreur creation socket" << endl;
            return false;
        }

        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = INADDR_ANY;
        service.sin_port = htons(port);

        if (bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
            cerr << "Erreur Bind" << endl;
            return false;
        }

        if (listen(mainSocket, 10) == SOCKET_ERROR) {
            cerr << "Erreur Listen" << endl;
            return false;
        }

        cout << ">>> Serveur en ecoute sur http://localhost:" << port << endl;
        return true;
    }

    void run() {
        while (true) {
            SOCKET clientSocket = accept(mainSocket, NULL, NULL);
            if (clientSocket != INVALID_SOCKET) {
                handleClient(clientSocket);
            }
        }
    }

    void handleClient(SOCKET client) {
        char buffer[4096] = {0};
        recv(client, buffer, 4096, 0);
        string request(buffer);
        
        string body = "";
        size_t bodyPos = request.find("\r\n\r\n");
        if (bodyPos != string::npos) {
            body = request.substr(bodyPos + 4);
        }

        string reponse;
        string type = "application/json";

        if (request.find("GET /api/stats") != string::npos) {
            reponse = logiqueFerme.getGlobalStats();
        } 
        else if (request.find("POST /api/arroser") != string::npos) {
            double q = JsonHelper::getValeur(body, "quantite");
            reponse = logiqueFerme.traiterArrosage(q);
        }
        else if (request.find("POST /api/remplir") != string::npos) {
            reponse = logiqueFerme.traiterRemplissage();
        }
        else if (request.find("POST /api/climat") != string::npos) {
            double t = JsonHelper::getValeur(body, "temperature");
            double h = JsonHelper::getValeur(body, "humidite");
            reponse = logiqueFerme.traiterClimat(t, h);
        }
        else if (request.find("POST /api/distribuer") != string::npos) {
            reponse = logiqueFerme.traiterDistribution();
        }
        else if (request.find("POST /api/reapprovisionner") != string::npos) {
            double q = JsonHelper::getValeur(body, "quantite");
            reponse = logiqueFerme.traiterReappro(q);
        }
        else if (request.find("POST /api/rapport") != string::npos) {
            reponse = logiqueFerme.genererRapport();
        }
        else if (request.find("GET / ") != string::npos || request.find("GET /index.html") != string::npos) {
            ifstream fichierHtml("index.html");
            if (fichierHtml) {
                stringstream bufferHtml;
                bufferHtml << fichierHtml.rdbuf();
                reponse = bufferHtml.str();
                type = "text/html";
            } else {
                reponse = "<h1>Erreur: index.html manquant</h1>";
                type = "text/html";
            }
        }
        else {
            reponse = "{}";
        }

        sendResponse(client, reponse, type);
        closesocket(client);
    }
};

int main() {
    GestionnaireFerme maFerme;
    SimpleServer serveur(maFerme);

    if (serveur.init(8080)) {
        serveur.run();
    } else {
        system("pause");
    }

    return 0;
}