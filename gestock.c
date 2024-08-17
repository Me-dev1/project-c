#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définitions des constantes pour les tailles maximales des champs
#define MAX_NOM 50
#define MAX_DESC 255
#define MAX_USER 50
#define FILENAME "stock.csv"

// Structure de données pour représenter un produit
typedef struct Produit {
    int id; // Identifiant unique du produit
    char nom[MAX_NOM]; // Nom du produit
    char description[MAX_DESC]; // Description du produit
    char utilisateur[MAX_USER]; // Nom de l'utilisateur associé au produit
    float prix_unitaire; // Prix unitaire du produit
    int quantite; // Quantité en stock du produit
    int seuil_alerte; // Seuil d'alerte pour le stock
    char date_entree[11]; // Date d'entrée en stock (format YYYY-MM-DD)
    char date_sortie[11]; // Date de sortie du stock (format YYYY-MM-DD)
    struct Produit *suivant; // Pointeur vers le produit suivant dans la liste chaînée
} Produit;

// Prototypes des nouvelles fonctions
void rechercherParNom(Produit *tete, char *nom);
void rechercherParUtilisateur(Produit *tete, char *utilisateur);

// Prototypes des autres fonctions utilisées dans le programme
Produit *chargerProduits();
void sauvegarderProduits(Produit *tete);
Produit *ajouterProduit(Produit *tete, int *dernierId);
void afficherProduits(Produit *tete);
Produit *supprimerProduit(Produit *tete, int id);
void modifierProduit(Produit *tete, int id);
void rechercherProduit(Produit *tete);
void trierProduits(Produit **tete, int mode);
void libererListe(Produit *tete);
void printProductDetails(Produit *prod);

// Fonction principale du programme
int main() {
    char utilisateur[MAX_USER]; // Nom de l'utilisateur
    printf("Entrez votre nom pour commencer : ");
    fgets(utilisateur, MAX_USER, stdin); // Lire le nom de l'utilisateur
    utilisateur[strcspn(utilisateur, "\n")] = 0; // Retirer le caractère de nouvelle ligne

    Produit *listeProduits = chargerProduits(); // Charger les produits depuis le fichier
    int dernierId = 1; // Initialiser l'ID du prochain produit
    Produit *temp = listeProduits;
    while (temp) {
        if (temp->id >= dernierId) {
            dernierId = temp->id + 1; // Mettre à jour l'ID du prochain produit si nécessaire
        }
        temp = temp->suivant; // Passer au produit suivant
    }

    int choix; // Variable pour stocker le choix de l'utilisateur dans le menu
    do {
        // Affichage du menu
        printf("\nMenu:\n");
        printf("1. Ajouter un produit\n");
        printf("2. Supprimer un produit\n");
        printf("3. Modifier un produit\n");
        printf("4. Afficher tous les produits\n");
        printf("5. Rechercher un produit\n");
        printf("6. Rechercher un produit par nom du produit\n");
        printf("7. Rechercher un produit par nom d'utilisateur\n");
        printf("8. Trier les produits\n");
        printf("9. Sauvegarder les produits\n");
        printf("10. Quitter\n");
        printf("Entrez votre choix : ");
        scanf("%d", &choix); // Lire le choix de l'utilisateur
        getchar(); // Consommer le caractère de nouvelle ligne laissé par scanf

        char recherche[MAX_NOM]; // Variable pour stocker le nom ou l'utilisateur recherché
        switch (choix) {
            case 1:
                listeProduits = ajouterProduit(listeProduits, &dernierId); // Ajouter un produit
                break;
            case 2:
                printf("Entrez l'ID du produit à supprimer : ");
                int supprId;
                scanf("%d", &supprId); // Lire l'ID du produit à supprimer
                getchar();
                listeProduits = supprimerProduit(listeProduits, supprId); // Supprimer le produit
                break;
            case 3:
                printf("Entrez l'ID du produit à modifier : ");
                int modifId;
                scanf("%d", &modifId); // Lire l'ID du produit à modifier
                getchar();
                modifierProduit(listeProduits, modifId); // Modifier le produit
                break;
            case 4:
                afficherProduits(listeProduits); // Afficher tous les produits
                break;
            case 5:
                rechercherProduit(listeProduits); // Rechercher un produit par ID
                break;
            case 6:
                printf("Entrez le nom du produit à rechercher : ");
                fgets(recherche, MAX_NOM, stdin); // Lire le nom du produit à rechercher
                recherche[strcspn(recherche, "\n")] = 0; // Retirer le caractère de nouvelle ligne
                rechercherParNom(listeProduits, recherche); // Rechercher par nom
                break;
            case 7:
                printf("Entrez le nom d'utilisateur à rechercher : ");
                fgets(recherche, MAX_USER, stdin); // Lire le nom de l'utilisateur à rechercher
                recherche[strcspn(recherche, "\n")] = 0; // Retirer le caractère de nouvelle ligne
                rechercherParUtilisateur(listeProduits, recherche); // Rechercher par utilisateur
                break;
            case 8:
                printf("Trier par 1: Nom, 2: Prix: ");
                int mode;
                scanf("%d", &mode); // Lire le mode de tri (1 pour nom, 2 pour prix)
                getchar();
                trierProduits(&listeProduits, mode); // Trier les produits
                break;
            case 9:
                sauvegarderProduits(listeProduits); // Sauvegarder les produits dans le fichier
                printf("Produits sauvegardés avec succès.\n");
                break;
            case 10:
                printf("Au revoir!\n"); // Quitter le programme
                break;
            default:
                printf("Choix invalide.\n"); // Message d'erreur pour choix invalide
                break;
        }
    } while (choix != 10); // Boucle jusqu'à ce que l'utilisateur choisisse de quitter

    libererListe(listeProduits); // Libérer la mémoire allouée pour la liste des produits
    return 0;
}

// Fonction pour charger les produits depuis un fichier CSV
Produit *chargerProduits() {
    FILE *file = fopen(FILENAME, "r"); // Ouvrir le fichier en lecture
    if (!file) {
        printf("Fichier non trouvé, démarrage avec une liste vide.\n");
        return NULL; // Si le fichier n'existe pas, retourner une liste vide
    }

    Produit *tete = NULL, *courant = NULL; // Initialiser la tête et le courant de la liste
    while (!feof(file)) {
        Produit *nouveau = malloc(sizeof(Produit)); // Allouer de la mémoire pour un nouveau produit
        if (fscanf(file, "%d,%49[^,],%254[^,],%49[^,],%f,%d,%d,%10[^,],%10[^\n]",
                   &nouveau->id, nouveau->nom, nouveau->description, nouveau->utilisateur,
                   &nouveau->prix_unitaire, &nouveau->quantite, &nouveau->seuil_alerte,
                   nouveau->date_entree, nouveau->date_sortie) == 9) {
            nouveau->suivant = NULL; // Initialiser le pointeur suivant à NULL
            if (!tete) {
                tete = nouveau; // Si la liste est vide, le nouveau produit devient la tête
                courant = nouveau; // Le courant est aussi le nouveau produit
            } else {
                courant->suivant = nouveau; // Sinon, ajouter le nouveau produit à la fin de la liste
                courant = nouveau; // Mettre à jour le courant
            }
        } else {
            free(nouveau); // Libérer la mémoire allouée si la lecture échoue
            break; // Si la lecture échoue, arrêter la boucle
        }
    }
    fclose(file); // Fermer le fichier
    return tete; // Retourner la tête de la liste
}

// Fonction pour sauvegarder les produits dans un fichier CSV
void sauvegarderProduits(Produit *tete) {
    FILE *file = fopen(FILENAME, "w"); // Ouvrir le fichier en écriture
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return; // Si le fichier ne peut pas être ouvert, retourner
    }

    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    while (courant != NULL) {
        // Écrire les informations du produit dans le fichier
        fprintf(file, "%d,%s,%s,%s,%.2f,%d,%d,%s,%s\n",
                courant->id, courant->nom, courant->description, courant->utilisateur,
                courant->prix_unitaire, courant->quantite, courant->seuil_alerte,
                courant->date_entree, courant->date_sortie);
        courant = courant->suivant; // Passer au produit suivant
    }

    fclose(file); // Fermer le fichier
}

// Fonction pour ajouter un produit à la liste
Produit *ajouterProduit(Produit *tete, int *dernierId) {
    Produit *nouveau = malloc(sizeof(Produit)); // Allouer de la mémoire pour un nouveau produit
    if (!nouveau) {
        printf("Erreur d'allocation de mémoire.\n");
        return tete; // Si l'allocation échoue, retourner la liste non modifiée
    }

    nouveau->id = (*dernierId)++; // Attribuer un ID unique au nouveau produit
    printf("Entrez le nom du produit : ");
    fgets(nouveau->nom, MAX_NOM, stdin); // Lire le nom du produit
    nouveau->nom[strcspn(nouveau->nom, "\n")] = 0; // Retirer le caractère de nouvelle ligne

    printf("Entrez la description du produit : ");
    fgets(nouveau->description, MAX_DESC, stdin); // Lire la description du produit
    nouveau->description[strcspn(nouveau->description, "\n")] = 0; // Retirer le caractère de nouvelle ligne

    printf("Entrez le prix unitaire du produit : ");
    scanf("%f", &nouveau->prix_unitaire); // Lire le prix unitaire du produit

    printf("Entrez la quantité du produit : ");
    scanf("%d", &nouveau->quantite); // Lire la quantité du produit

    printf("Entrez le seuil d'alerte du produit : ");
    scanf("%d", &nouveau->seuil_alerte); // Lire le seuil d'alerte du produit
    getchar(); // Consommer le caractère de nouvelle ligne laissé par scanf

    printf("Entrez la date d'entrée (YYYY-MM-DD) : ");
    fgets(nouveau->date_entree, 11, stdin); // Lire la date d'entrée du produit
    nouveau->date_entree[strcspn(nouveau->date_entree, "\n")] = 0; // Retirer le caractère de nouvelle ligne

    printf("Entrez la date de sortie (YYYY-MM-DD) : ");
    fgets(nouveau->date_sortie, 11, stdin); // Lire la date de sortie du produit
    nouveau->date_sortie[strcspn(nouveau->date_sortie, "\n")] = 0; // Retirer le caractère de nouvelle ligne

    strcpy(nouveau->utilisateur, ""); // Initialiser le nom d'utilisateur à une chaîne vide
    nouveau->suivant = tete; // Ajouter le nouveau produit au début de la liste
    return nouveau; // Retourner la nouvelle tête de la liste
}

// Fonction pour afficher tous les produits de la liste
void afficherProduits(Produit *tete) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    while (courant != NULL) {
        // Afficher les détails du produit courant
        printf("ID: %d\nNom: %s\nDescription: %s\nUtilisateur: %s\nPrix unitaire: %.2f\nQuantité: %d\nSeuil d'alerte: %d\nDate d'entrée: %s\nDate de sortie: %s\n\n",
               courant->id, courant->nom, courant->description, courant->utilisateur,
               courant->prix_unitaire, courant->quantite, courant->seuil_alerte,
               courant->date_entree, courant->date_sortie);
        courant = courant->suivant; // Passer au produit suivant
    }
}

// Fonction pour supprimer un produit de la liste par son ID
Produit *supprimerProduit(Produit *tete, int id) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    Produit *precedent = NULL; // Initialiser le précédent à NULL
    while (courant != NULL) {
        if (courant->id == id) { // Si l'ID du produit courant correspond à l'ID recherché
            if (precedent == NULL) {
                tete = courant->suivant; // Supprimer le premier produit de la liste
            } else {
                precedent->suivant = courant->suivant; // Relier le précédent au suivant du courant
            }
            free(courant); // Libérer la mémoire du produit supprimé
            printf("Produit supprimé avec succès.\n");
            return tete; // Retourner la nouvelle tête de la liste
        }
        precedent = courant; // Mettre à jour le précédent
        courant = courant->suivant; // Passer au produit suivant
    }
    printf("Produit avec ID %d non trouvé.\n", id); // Message si le produit n'est pas trouvé
    return tete; // Retourner la tête de la liste non modifiée
}

// Fonction pour modifier les détails d'un produit par son ID
void modifierProduit(Produit *tete, int id) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    while (courant != NULL) {
        if (courant->id == id) { // Si l'ID du produit courant correspond à l'ID recherché
            printf("Modification du produit ID %d\n", id);
            printf("Nom actuel: %s, Nouveau nom: ", courant->nom);
            fgets(courant->nom, MAX_NOM, stdin); // Lire le nouveau nom du produit
            courant->nom[strcspn(courant->nom, "\n")] = 0; // Retirer le caractère de nouvelle ligne

            printf("Description actuelle: %s, Nouvelle description: ", courant->description);
            fgets(courant->description, MAX_DESC, stdin); // Lire la nouvelle description du produit
            courant->description[strcspn(courant->description, "\n")] = 0; // Retirer le caractère de nouvelle ligne

            printf("Nom d'utilisateur actuel: %s, Nouveau nom d'utilisateur: ", courant->utilisateur);
            fgets(courant->utilisateur, MAX_USER, stdin); // Lire le nouveau nom d'utilisateur
            courant->utilisateur[strcspn(courant->utilisateur, "\n")] = 0; // Retirer le caractère de nouvelle ligne

            printf("Prix unitaire actuel: %.2f, Nouveau prix unitaire: ", courant->prix_unitaire);
            scanf("%f", &courant->prix_unitaire); // Lire le nouveau prix unitaire du produit

            printf("Quantité actuelle: %d, Nouvelle quantité: ", courant->quantite);
            scanf("%d", &courant->quantite); // Lire la nouvelle quantité du produit

            printf("Seuil d'alerte actuel: %d, Nouveau seuil: ", courant->seuil_alerte);
            scanf("%d", &courant->seuil_alerte); // Lire le nouveau seuil d'alerte du produit
            getchar(); // Consommer le caractère de nouvelle ligne laissé par scanf

            printf("Date d'entrée actuelle: %s, Nouvelle date d'entrée (YYYY-MM-DD): ", courant->date_entree);
            fgets(courant->date_entree, 11, stdin); // Lire la nouvelle date d'entrée du produit
            courant->date_entree[strcspn(courant->date_entree, "\n")] = 0; // Retirer le caractère de nouvelle ligne

            printf("Date de sortie actuelle: %s, Nouvelle date de sortie (YYYY-MM-DD): ", courant->date_sortie);
            fgets(courant->date_sortie, 11, stdin); // Lire la nouvelle date de sortie du produit
            courant->date_sortie[strcspn(courant->date_sortie, "\n")] = 0; // Retirer le caractère de nouvelle ligne

            printf("Produit modifié avec succès.\n");
            return; // Sortir de la fonction après la modification
        }
        courant = courant->suivant; // Passer au produit suivant
    }
    printf("Produit avec ID %d non trouvé.\n", id); // Message si le produit n'est pas trouvé
}

// Fonction pour rechercher un produit par son ID
void rechercherProduit(Produit *tete) {
    int id; // Variable pour stocker l'ID recherché
    printf("Entrez l'ID du produit à rechercher: ");
    scanf("%d", &id); // Lire l'ID recherché
    getchar(); // Consommer le caractère de nouvelle ligne laissé par scanf

    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    while (courant != NULL) {
        if (courant->id == id) { // Si l'ID du produit courant correspond à l'ID recherché
            printProductDetails(courant); // Afficher les détails du produit trouvé
            return; // Sortir de la fonction après avoir trouvé le produit
        }
        courant = courant->suivant; // Passer au produit suivant
    }
    printf("Produit avec ID %d non trouvé.\n", id); // Message si le produit n'est pas trouvé
}

// Fonction pour trier les produits par nom ou prix unitaire
void trierProduits(Produit **tete, int mode) {
    if (*tete == NULL) return; // Si la liste est vide, retourner

    int trie = 0; // Variable pour indiquer si la liste est triée
    Produit *courant; // Pointeur pour parcourir la liste
    while (!trie) {
        trie = 1; // Supposer que la liste est triée
        courant = *tete; // Initialiser le courant à la tête de la liste
        while (courant->suivant != NULL) {
            int comparer = 0; // Variable pour stocker le résultat de la comparaison
            if (mode == 1) { // Si le mode est 1, comparer par nom
                comparer = strcmp(courant->nom, courant->suivant->nom);
            } else if (mode == 2) { // Si le mode est 2, comparer par prix unitaire
                comparer = (courant->prix_unitaire > courant->suivant->prix_unitaire) - 
                           (courant->prix_unitaire < courant->suivant->prix_unitaire);
            }
            if (comparer > 0) { // Si les produits sont dans le mauvais ordre
                Produit temp = *courant; // Échanger les produits
                courant->id = courant->suivant->id;
                strcpy(courant->nom, courant->suivant->nom);
                strcpy(courant->description, courant->suivant->description);
                strcpy(courant->utilisateur, courant->suivant->utilisateur);
                courant->prix_unitaire = courant->suivant->prix_unitaire;
                courant->quantite = courant->suivant->quantite;
                courant->seuil_alerte = courant->suivant->seuil_alerte;
                strcpy(courant->date_entree, courant->suivant->date_entree);
                strcpy(courant->date_sortie, courant->suivant->date_sortie);

                courant->suivant->id = temp.id;
                strcpy(courant->suivant->nom, temp.nom);
                strcpy(courant->suivant->description, temp.description);
                strcpy(courant->suivant->utilisateur, temp.utilisateur);
                courant->suivant->prix_unitaire = temp.prix_unitaire;
                courant->suivant->quantite = temp.quantite;
                courant->suivant->seuil_alerte = temp.seuil_alerte;
                strcpy(courant->suivant->date_entree, temp.date_entree);
                strcpy(courant->suivant->date_sortie, temp.date_sortie);

                trie = 0; // Indiquer que la liste n'est pas encore triée
            }
            courant = courant->suivant; // Passer au produit suivant
        }
    }
    printf("Produits triés avec succès.\n");
}

// Fonction pour libérer la mémoire allouée pour la liste des produits
void libererListe(Produit *tete) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    while (courant != NULL) {
        Produit *suivant = courant->suivant; // Sauvegarder le pointeur vers le produit suivant
        free(courant); // Libérer la mémoire du produit courant
        courant = suivant; // Passer au produit suivant
    }
}

// Fonction pour afficher les détails d'un produit
void printProductDetails(Produit *prod) {
    // Afficher les détails du produit
    printf("ID: %d\nNom: %s\nDescription: %s\nUtilisateur: %s\nPrix unitaire: %.2f\nQuantité: %d\nSeuil d'alerte: %d\nDate d'entrée: %s\nDate de sortie: %s\n\n",
           prod->id, prod->nom, prod->description, prod->utilisateur,
           prod->prix_unitaire, prod->quantite, prod->seuil_alerte,
           prod->date_entree, prod->date_sortie);
}

// Fonction pour rechercher des produits par nom
void rechercherParNom(Produit *tete, char *nom) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    int trouvé = 0; // Variable pour indiquer si un produit a été trouvé
    while (courant != NULL) {
        if (strcmp(courant->nom, nom) == 0) { // Si le nom du produit courant correspond au nom recherché
            printProductDetails(courant); // Afficher les détails du produit trouvé
            trouvé = 1; // Indiquer qu'un produit a été trouvé
        }
        courant = courant->suivant; // Passer au produit suivant
    }
    if (!trouvé) { // Si aucun produit n'a été trouvé
        printf("Aucun produit trouvé avec le nom %s.\n", nom);
    }
}

// Fonction pour rechercher des produits par nom d'utilisateur
void rechercherParUtilisateur(Produit *tete, char *utilisateur) {
    Produit *courant = tete; // Initialiser le courant à la tête de la liste
    int trouvé = 0; // Variable pour indiquer si un produit a été trouvé
    while (courant != NULL) {
        if (strcmp(courant->utilisateur, utilisateur) == 0) { // Si le nom d'utilisateur du produit courant correspond au nom recherché
            printProductDetails(courant); // Afficher les détails du produit trouvé
            trouvé = 1; // Indiquer qu'un produit a été trouvé
        }
        courant = courant->suivant; // Passer au produit suivant
    }
    if (!trouvé) { // Si aucun produit n'a été trouvé
        printf("Aucun produit trouvé avec l'utilisateur %s.\n", utilisateur);
    }
}
