<div align="center">

# 🌱 Smart Farm — Système IoT & Supervision de Ferme Intelligente

[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![HTML5](https://img.shields.io/badge/HTML5-Dashboard-E34F26?style=for-the-badge&logo=html5&logoColor=white)](https://developer.mozilla.org/)
[![IoT](https://img.shields.io/badge/IoT-Sensors_%26_Automation-38B2AC?style=for-the-badge)](https://en.wikipedia.org/wiki/Internet_of_things)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

Système de supervision agricole connecté associant un serveur performant en C++ (gestion des sockets et des flux de données capteurs) à un tableau de bord web pour le monitoring climatique, l'irrigation automatisée et le suivi des stocks.

[Fonctionnalités](#-fonctionnalités) • [Architecture](#-architecture) • [Exécution](#-exécution)

</div>

---

## 🌟 Fonctionnalités

- 💧 **Supervision de l'Irrigation :** Suivi en temps réel de la réserve d'eau (niveau actuel vs capacité maximale de 1000L) et déclenchement intelligent de l'arrosage.
- 🌡️ **Monitoring Climatique :** Télémétrie en direct de la température (°C) et de l'humidité relative (%) pour l'optimisation des cultures sous serre.
- 🌾 **Gestion des Stocks d'Alimentation :** Calcul du stock restant (en kg) et estimation de l'autonomie restante en jours d'activité.
- 📄 **Génération de Bilans & Rapports d'Activité :** Production automatisée de fiches d'état complètes (`rapport_ferme.txt`).
- ⚡ **Serveur Réseau C++ Léger & Robuste :** Architecture serveur socket TCP/HTTP ultra-rapide sans dépendance logicielle lourde.

---

## 🛠️ Architecture du Système

```
[ Capteurs IoT & Sondes ] ──(Télémétrie)──> [ Serveur Socket C++ (serveur.cpp) ]
                                                     │
                                                     ├──> [ Journalisation & Rapports (rapport_ferme.txt) ]
                                                     │
                                                     └──> [ Dashboard Web Utilisateur (index.html) ]
```

- **Moteur Serveur :** C++ (Sockets Berkeley / Winsock, gestion des protocoles de communication)
- **Interface Utilisateur :** HTML5, CSS3 Moderne, JavaScript (Visualisation des jauges et indicateurs)

---

## 📂 Contenu du Répertoire

```bash
projet-ferme-intelligente/
├── serveur.cpp          # Code source du serveur de télémétrie en C++
├── serveur.exe          # Binaire compilé prêt à l'emploi (Windows)
├── index.html           # Interface web de visualisation et monitoring
└── rapport_ferme.txt    # Exemple de rapport d'activité généré par le système
```

---

## 🚀 Compilation & Exécution

### Option 1 : Exécution directe (Binaire précompilé Windows)
1. Double-cliquez sur `serveur.exe` (ou exécutez-le dans un terminal) :
   ```cmd
   serveur.exe
   ```
2. Ouvrez `index.html` dans n'importe quel navigateur moderne pour observer les données télémétriques en direct.

### Option 2 : Compilation depuis le code source (g++ / MinGW / Clang)
Sur Windows (MinGW) :
```bash
g++ -std=c++17 serveur.cpp -o serveur.exe -lws2_32
./serveur.exe
```

Sur Linux / macOS :
```bash
g++ -std=c++17 serveur.cpp -o serveur
./serveur
```

---

## 📊 Exemple d'Extrait de Rapport Généré

```text
=== RAPPORT D'ACTIVITE FERME ===
1. ETAT IRRIGATION
   Niveau actuel : 50 Litres / Capacite max : 1000 Litres
2. ETAT CLIMATIQUE
   Temperature : 22 °C | Humidite : 60 %
3. STOCK ALIMENTATION
   Stock restant : 500 kg | Autonomie : 29 Jours
```

---

## 👤 Auteur

- **Yassir EL MANSSOURI** - [@yassir-el-manssouri](https://github.com/yassir-el-manssouri) | [LinkedIn](https://www.linkedin.com/in/yassir-el-manssouri/)
- Étudiant / Ingénieur à l'École Marocaine des Sciences de l'Ingénieur (EMSI).

---


