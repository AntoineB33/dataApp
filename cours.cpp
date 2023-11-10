#include <iostream>
#include <vector>

template <typename T>
class Pile {
private:
    int tailleMax;
    std::vector<T> elements;

public:
    // Constructeur
    Pile(int tailleMax) : tailleMax(tailleMax) {}

    // Méthode pour vérifier si un élément peut être ajouté à la pile
    bool peutAjouter() const {
        return elements.size() < tailleMax;
    }

    // Méthode pour ajouter un élément à la pile
    void ajouterElement(const T& element) {
        if (peutAjouter()) {
            elements.push_back(element);
            std::cout << "Élément ajouté à la pile : " << element << std::endl;
        } else {
            std::cerr << "La pile est pleine. Impossible d'ajouter plus d'éléments." << std::endl;
        }
    }
};

int main() {
    // Exemple d'utilisation avec une pile d'entiers de taille maximale 5
    Pile<int> maPile(5);

    // Ajout d'éléments à la pile
    for (int i = 1; i <= 7; ++i) {
        if (maPile.peutAjouter()) {
            maPile.ajouterElement(i);
        }
    }

    return 0;
}
