class Ingredient:
    def __init__(self, nom, unite_defaut, categorie="Autre"):
        self.nom = nom
        self.unite = unite_defaut
        self.categorie = categorie

class Recette:
    def __init__(self):
        self.base_de_donnees = {}
        self._charger_ingredients_par_defaut()

    def ajouter_ingredient(self, nom, unite, categorie="Autre"):
        nouvel_ingredient = Ingredient(nom, unite, categorie)
        self.base_de_donnees[nom.lower()] = nouvel_ingredient

    def obtenir_unite(self, nom):
        nom_minuscule = nom.lower()
        if nom_minuscule in self.base_de_donnees:
            return self.base_de_donnees[nom_minuscule].unite
        return "unité(s)" # Si l'ingrédient n'est pas dans le catalogue

    def _charger_ingredients_par_defaut(self):
        self.ajouter_ingredient("farine", "g", "Épicerie")
        self.ajouter_ingredient("sucre", "g", "Épicerie")
        self.ajouter_ingredient("pâtes", "g", "Épicerie")
        self.ajouter_ingredient("lait", "L", "Frais")
        self.ajouter_ingredient("oeuf", "pièce(s)", "Frais")
        self.ajouter_ingredient("beurre", "g", "Frais")
        self.ajouter_ingredient("tomate", "pièce(s)", "Fruits & Légumes")